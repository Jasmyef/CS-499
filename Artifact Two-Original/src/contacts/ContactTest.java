package contacts; 

import org.junit.jupiter.api.Test; import static org.junit.jupiter.api.Assertions.*; 

public class ContactTest { 

@Test 
void creatingValidContactStoresData() { 
    Contact contact = new Contact( 
            "C01", 
            "Jasmyne", 
            "Fisher", 
            "8041234567", 
            "123 Sesame Street" 
    ); 
 
    assertEquals("C01", contact.getContactId()); 
    assertEquals("Jasmyne", contact.getFirstName()); 
    assertEquals("Fisher", contact.getLastName()); 
    assertEquals("8041234567", contact.getPhone()); 
    assertEquals("123 Sesame Street", contact.getAddress()); 
} 
 
@Test 
void contactIdRulesAreEnforced() { 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact(null, "Jay", "Fish", "8041234567", "123 Sesame Street")); 
 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("TOO_LONG_ID", "Jay", "Fish", "8041234567", "123 Sesame Street")); 
} 
 
@Test 
void firstNameRulesAreEnforced() { 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C02", null, "Rose", "5555555555", "456 Big Bird Lane")); 
 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C02", "FirstNameTooLong", "Rose", "5555555555", "456 Big Bird Lane")); 
} 
 
@Test 
void lastNameRulesAreEnforced() { 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C03", "Lily", null, "5555555555", "456 Big Bird Lane")); 
 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C03", "Lily", "LastNameTooLong", "5555555555", "456 Big Bird Lane")); 
} 
 
@Test 
void phoneMustBeTenDigitsOnly() { 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C04", "Poppy", "Lane", null, "123 Sesame Street")); 
 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C04", "Poppy", "Lane", "123456789", "123 Sesame Street")); 
 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C04", "Poppy", "Lane", "12345abcde", "123 Sesame Street")); 
} 
 
@Test 
void addressRulesAreEnforced() { 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C05", "Violet", "Petals", "5555555555", null)); 
 
    assertThrows(IllegalArgumentException.class, () -> 
            new Contact("C05", "Violet", "Petals", "5555555555", 
                    "This address is definitely longer than thirty characters")); 
} 
 
@Test 
void settersFollowTheSameRules() { 
    Contact contact = new Contact( 
            "C06", 
            "Lily", 
            "Rose", 
            "5555555555", 
            "456 Big Bird Lane" 
    ); 
 
    contact.setFirstName("Jay"); 
    contact.setLastName("Echo"); 
    contact.setPhone("1112223333"); 
    contact.setAddress("789 Cookie Monster Ave"); 
 
    assertEquals("Jay", contact.getFirstName()); 
    assertEquals("Echo", contact.getLastName()); 
    assertEquals("1112223333", contact.getPhone()); 
    assertEquals("789 Cookie Monster Ave", contact.getAddress()); 
 
    assertThrows(IllegalArgumentException.class, () -> contact.setFirstName(null)); 
    assertThrows(IllegalArgumentException.class, () -> contact.setLastName("LastNameTooLong")); 
    assertThrows(IllegalArgumentException.class, () -> contact.setPhone("short")); 
    assertThrows(IllegalArgumentException.class, () -> contact.setAddress(null)); 
} 
  

} 

 