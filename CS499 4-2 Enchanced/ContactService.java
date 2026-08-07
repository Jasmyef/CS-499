package contacts;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;
import java.util.Map;
import java.util.function.Consumer;

public class ContactService {

    // Enhancement (Algorithms & Data Structures):
    // Contacts are kept in a HashMap, keyed by contact ID, so lookups, adds, deletes,
    // and updates go straight to the right contact instead of scanning a whole list.
    private final Map<String, Contact> contactsById = new HashMap<>();

    // Enhancement (Databases):
    // Contacts used to live only in memory and disappeared the moment the program closed.
    // Now every change is also written to a small local SQLite database file, so contacts
    // are still there the next time the program starts.
    private final Connection connection;

    public ContactService() {
        this("contacts.db");
    }

    public ContactService(String databaseFile) {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + databaseFile);
            createTableIfNotExists();
            loadContactsFromDatabase();
        } catch (SQLException e) {
            throw new IllegalStateException("Could not open the contacts database", e);
        }
    }

    private void createTableIfNotExists() throws SQLException {
        String sql = "CREATE TABLE IF NOT EXISTS contacts (" +
                "contact_id TEXT PRIMARY KEY, " +
                "first_name TEXT NOT NULL, " +
                "last_name TEXT NOT NULL, " +
                "phone TEXT NOT NULL, " +
                "address TEXT NOT NULL)";
        try (Statement stmt = connection.createStatement()) {
            stmt.execute(sql);
        }
    }

    private void loadContactsFromDatabase() throws SQLException {
        String sql = "SELECT contact_id, first_name, last_name, phone, address FROM contacts";
        try (Statement stmt = connection.createStatement();
             ResultSet rs = stmt.executeQuery(sql)) {
            while (rs.next()) {
                Contact contact = new Contact(
                        rs.getString("contact_id"),
                        rs.getString("first_name"),
                        rs.getString("last_name"),
                        rs.getString("phone"),
                        rs.getString("address")
                );
                contactsById.put(contact.getContactId(), contact);
            }
        }
    }

    public boolean addContact(Contact contact) {
        if (contact == null) {
            throw new IllegalArgumentException("contact cannot be null");
        }

        String id = contact.getContactId();
        if (contactsById.containsKey(id)) {
            return false;
        }

        String sql = "INSERT INTO contacts (contact_id, first_name, last_name, phone, address) VALUES (?, ?, ?, ?, ?)";
        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, contact.getContactId());
            stmt.setString(2, contact.getFirstName());
            stmt.setString(3, contact.getLastName());
            stmt.setString(4, contact.getPhone());
            stmt.setString(5, contact.getAddress());
            stmt.executeUpdate();
        } catch (SQLException e) {
            throw new IllegalStateException("Could not save the new contact", e);
        }

        contactsById.put(id, contact);
        return true;
    }

    public boolean deleteContact(String contactId) {
        if (contactId == null || !contactsById.containsKey(contactId)) {
            return false;
        }

        String sql = "DELETE FROM contacts WHERE contact_id = ?";
        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, contactId);
            stmt.executeUpdate();
        } catch (SQLException e) {
            throw new IllegalStateException("Could not delete the contact", e);
        }

        contactsById.remove(contactId);
        return true;
    }

    public Contact getContact(String contactId) {
        return contactsById.get(contactId);
    }

    // The four update methods still each have their own name (so nothing calling them
    // has to change), but they all share one private helper underneath, instead of each
    // repeating the same "get it, check it, save it" logic on their own.
    public boolean updateFirstName(String contactId, String newFirstName) {
        return applyUpdate(contactId, "first_name", newFirstName, c -> c.setFirstName(newFirstName));
    }

    public boolean updateLastName(String contactId, String newLastName) {
        return applyUpdate(contactId, "last_name", newLastName, c -> c.setLastName(newLastName));
    }

    public boolean updatePhone(String contactId, String newPhone) {
        return applyUpdate(contactId, "phone", newPhone, c -> c.setPhone(newPhone));
    }

    public boolean updateAddress(String contactId, String newAddress) {
        return applyUpdate(contactId, "address", newAddress, c -> c.setAddress(newAddress));
    }

    private boolean applyUpdate(String contactId, String columnName, String newValue, Consumer<Contact> change) {
        Contact contact = getContact(contactId);
        if (contact == null) {
            return false;
        }

        // Contact's own setter validates the new value first (length checks, etc.)
        // before anything gets saved to the database.
        change.accept(contact);

        String sql = "UPDATE contacts SET " + columnName + " = ? WHERE contact_id = ?";
        try (PreparedStatement stmt = connection.prepareStatement(sql)) {
            stmt.setString(1, newValue);
            stmt.setString(2, contactId);
            stmt.executeUpdate();
        } catch (SQLException e) {
            throw new IllegalStateException("Could not save the update", e);
        }

        return true;
    }

    public void close() {
        try {
            connection.close();
        } catch (SQLException e) {
            // Nothing more to do at shutdown.
        }
    }
}
