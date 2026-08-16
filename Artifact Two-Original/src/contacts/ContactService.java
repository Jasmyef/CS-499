package contacts; 

  

import java.util.ArrayList; 

import java.util.List; 

  

public class ContactService { 

  

    private final List<Contact> contactList = new ArrayList<>(); 

  

    public boolean addContact(Contact contact) { 

        if (contact == null) { 

            throw new IllegalArgumentException("contact cannot be null"); 

        } 

  

        String id = contact.getContactId(); 

        if (indexOfId(id) != -1) { 

            return false; 

        } 

  

        contactList.add(contact); 

        return true; 

    } 

  

    public boolean deleteContact(String contactId) { 

        if (contactId == null) { 

            return false; 

        } 

  

        int pos = indexOfId(contactId); 

        if (pos == -1) { 

            return false; 

        } 

  

        contactList.remove(pos); 

        return true; 

    } 

  

    public Contact getContact(String contactId) { 

        int pos = indexOfId(contactId); 

        if (pos == -1) { 

            return null; 

        } 

        return contactList.get(pos); 

    } 

  

    public boolean updateFirstName(String contactId, String newFirstName) { 

        Contact c = getContact(contactId); 

        if (c == null) return false; 

        c.setFirstName(newFirstName); 

        return true; 

    } 

  

    public boolean updateLastName(String contactId, String newLastName) { 

        Contact c = getContact(contactId); 

        if (c == null) return false; 

        c.setLastName(newLastName); 

        return true; 

    } 

  

    public boolean updatePhone(String contactId, String newPhone) { 

        Contact c = getContact(contactId); 

        if (c == null) return false; 

        c.setPhone(newPhone); 

        return true; 

    } 

  

    public boolean updateAddress(String contactId, String newAddress) { 

        Contact c = getContact(contactId); 

        if (c == null) return false; 

        c.setAddress(newAddress); 

        return true; 

    } 

  

    private int indexOfId(String id) { 

        for (int i = 0; i < contactList.size(); i++) { 

            if (contactList.get(i).getContactId().equals(id)) { 

                return i; 

            } 

        } 

        return -1; 

    } 

} 

 