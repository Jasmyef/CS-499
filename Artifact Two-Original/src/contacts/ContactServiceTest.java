package contacts; 

  

import org.junit.jupiter.api.Test; 

import static org.junit.jupiter.api.Assertions.*; 

  

public class ContactServiceTest { 

  

    @Test 

    void uniqueIdContactIsAdded() { 

        ContactService service = new ContactService(); 

  

        Contact contact = new Contact( 

                "C01", 

                "Jasmyne", 

                "Fisher", 

                "8041234567", 

                "123 Sesame Street" 

        ); 

  

        boolean added = service.addContact(contact); 

  

        assertTrue(added); 

        assertNotNull(service.getContact("C01")); 

    } 

  

    @Test 

    void duplicateIdContactIsRejected() { 

        ContactService service = new ContactService(); 

  

        Contact first = new Contact( 

                "C02", 

                "Lily", 

                "Rose", 

                "5555555555", 

                "456 Big Bird Lane" 

        ); 

  

        Contact second = new Contact( 

                "C02", 

                "Daisy", 

                "Bloom", 

                "1112223333", 

                "789 Cookie Monster Ave" 

        ); 

  

        assertTrue(service.addContact(first)); 

        boolean secondAdd = service.addContact(second); 

  

        assertFalse(secondAdd); 

        assertEquals("Lily", service.getContact("C02").getFirstName()); 

    } 

  

    @Test 

    void deletingExistingContactWorks() { 

        ContactService service = new ContactService(); 

  

        Contact contact = new Contact( 

                "C03", 

                "Violet", 

                "Petals", 

                "5555555555", 

                "456 Big Bird Lane" 

        ); 

  

        service.addContact(contact); 

        boolean deleted = service.deleteContact("C03"); 

  

        assertTrue(deleted); 

        assertNull(service.getContact("C03")); 

    } 

  

    @Test 

    void deletingUnknownIdReturnsFalse() { 

        ContactService service = new ContactService(); 

  

        boolean deleted = service.deleteContact("NO_ID"); 

  

        assertFalse(deleted); 

    } 

  

    @Test 

    void updatingAllFieldsForExistingContact() { 

        ContactService service = new ContactService(); 

  

        Contact contact = new Contact( 

                "C04", 

                "Poppy", 

                "Lane", 

                "5555555555", 

                "123 Sesame Street" 

        ); 

  

        service.addContact(contact); 

  

        assertTrue(service.updateFirstName("C04", "Iris")); 

        assertTrue(service.updateLastName("C04", "Dawn")); 

        assertTrue(service.updatePhone("C04", "1112223333")); 

        assertTrue(service.updateAddress("C04", "789 Cookie Monster Ave")); 

  

        Contact updated = service.getContact("C04"); 

  

        assertAll( 

                () -> assertEquals("Iris", updated.getFirstName()), 

                () -> assertEquals("Dawn", updated.getLastName()), 

                () -> assertEquals("1112223333", updated.getPhone()), 

                () -> assertEquals("789 Cookie Monster Ave", updated.getAddress()) 

        ); 

    } 

  

    @Test 

    void updateMethodsReturnFalseIfContactMissing() { 

        ContactService service = new ContactService(); 

  

        assertFalse(service.updateFirstName("MISSING", "Test")); 

        assertFalse(service.updateLastName("MISSING", "Test")); 

        assertFalse(service.updatePhone("MISSING", "5555555555")); 

        assertFalse(service.updateAddress("MISSING", "123 Sesame Street")); 

    } 

} 

 

 

 
