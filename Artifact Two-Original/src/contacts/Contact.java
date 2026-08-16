package contacts; 

  

public class Contact { 

  

    private final String contactId; 

    private String firstName; 

    private String lastName; 

    private String phone; 

    private String address; 

  

    public Contact(String contactId, 

                   String firstName, 

                   String lastName, 

                   String phone, 

                   String address) { 

  

        this.contactId = validateId(contactId); 

        this.firstName = validateName(firstName, "firstName"); 

        this.lastName = validateName(lastName, "lastName"); 

        this.phone = validatePhone(phone); 

        this.address = validateAddress(address); 

    } 

  

    public String getContactId() { 

        return contactId; 

    } 

  

    public String getFirstName() { 

        return firstName; 

    } 

  

    public String getLastName() { 

        return lastName; 

    } 

  

    public String getPhone() { 

        return phone; 

    } 

  

    public String getAddress() { 

        return address; 

    } 

  

    public void setFirstName(String firstName) { 

        this.firstName = validateName(firstName, "firstName"); 

    } 

  

    public void setLastName(String lastName) { 

        this.lastName = validateName(lastName, "lastName"); 

    } 

  

    public void setPhone(String phone) { 

        this.phone = validatePhone(phone); 

    } 

  

    public void setAddress(String address) { 

        this.address = validateAddress(address); 

    } 

  

    private String validateId(String id) { 

        if (id == null || id.length() > 10) { 

            throw new IllegalArgumentException("contactId"); 

        } 

        return id; 

    } 

  

    private String validateName(String value, String field) { 

        if (value == null || value.length() > 10) { 

            throw new IllegalArgumentException(field); 

        } 

        return value; 

    } 

  

    private String validatePhone(String value) { 

        if (value == null || value.length() != 10) { 

            throw new IllegalArgumentException("phone"); 

        } 

        for (char c : value.toCharArray()) { 

            if (c < '0' || c > '9') { 

                throw new IllegalArgumentException("phone"); 

            } 

        } 

        return value; 

    } 

  

    private String validateAddress(String value) { 

        if (value == null || value.length() > 30) { 

            throw new IllegalArgumentException("address"); 

        } 

        return value; 

    } 

} 

 