@page @photoinsert @broken
Feature: A Basic Page can contain many types of photos
When I create a Basic Page
As an authenticated user
I should be able to upload and place a photo

# TO INCLUDE BODY CONTENT
# THIS NEXT LINE IS NECESSARY FOR FINDING THE BODY FIELD WHEN JAVASCRIPT TESTING IS ENABLED
# And I follow "Disable rich-text"
# And fill in "Body" with "Little cakes with frosting"

# PER TESTING WITH photofinder.feature, JAVASCRIPT CAN'T FIND THE ASSET TO UPLOAD

# THERE IS CURRENTLY NO KNOWN WAY TO PRESS THE INSERT BUTTON. 

#THIS TEST UPLOADS A GRAPHIC; SAVES, THEN VERIFIES THAT IT HAS BEEN UPLOADED AND THAT WE ARE ON CORRECT PAGE; NO INSERT; CHECKS FOR ALT TEXT

Scenario: A graphic can be uploaded by saving the page
 Given I am logged in as a user with the "site_owner" role
 And I am on "node/add/page"
 And fill in "edit-title" with "Castles"
 And I fill in "Body" with "The development of defensive architecture"
 And I fill in "edit-field-photo-und-0-alt" with "A ruined castle in the fog"
 And I attach the file "assets/castle.jpg" to "edit-field-photo-und-0-upload"
 And I press "edit-submit"
 And I follow "Edit"
 Then I should see "Edit Basic page Castles"
 # THIS NEXT LINE PROVES IT WAS UPLOADED
 And I should see "File information"
 And I should see "Click and drag the crosshair to target the most important portion of the image"
 And I should see "castle.jpg"
 And the "edit-menu-link-title" element should have "Castles" in the "value"
 And I should see "Insert"
 And I press "edit-submit"
 Then I should be on "/castles"
 And I should see "Castles"
 And I should see "The development of defensive architecture"
 #NEXT LINE SHOWS THAT IMAGE WAS NOT INSERTED INTO BODY
 And the response should not contain "alt=\"A ruined castle in the fog\""
  
 @javascript
#TEST TWO: THIS TEST UPLOADS A GRAPHIC USING THE 'UPLOAD' BUTTON; CHECKs FOR INSERT ELEMENT
Scenario: A graphic can be uploaded and inserted into a page; checking for an insert element with javascript
  Given I am logged in as a user with the "site_owner" role
  And I am on "node/add/page"
  And fill in "edit-title" with "Cupcakes"
  # THIS NEXT LINE IS NECESSARY FOR FINDING THE BODY FIELD WHEN JAVASCRIPT TESTING IS ENABLED
  And I follow "Disable rich-text"
 And fill in "Body" with "Little cakes with frosting"
   And I fill in "edit-field-photo-und-0-alt" with "Lavender and lemony goodness"
   And I attach the file "assets/cupcakes.jpg" to "edit-field-photo-und-0-upload"
  And I press "edit-field-photo-und-0-upload-button"
 Then I should see "File information"
 And I should see "Click and drag the crosshair to target the most important portion of the image"
 And I should see "cupcakes.jpg"
 And I should see "Insert"
And I should see an "Insert" element
 # INSERT IS NOT A LINK; CANNOT FOLLOW And I follow "Insert"
# CLICKING INSERT IS UNDEFINED STEP And I click "Insert"
   # INSERT IS A FORM SUBMIT BUTTON; SHOULD BE PRESSABLE
  # PRESSING INSERT BUTTON DOESN'T WORK; CAN'T FIND IT APPARENTLY
 # COULD NOT FIND THIS ELEMENT When I click the "input" element with "image_image" for "rel"
# hold off on this one When I click the "Insert" element
 And I press "edit-submit"
  Then I should be on "/cupcakes"
  And I should see "Cupcakes"
  And I should see "Little cakes with frosting"
   #NEXT LINE SHOWS THAT IMAGE WAS INDEED INSERTED INTO BODY
And the response should contain "alt=\"Lavender and lemony goodness\""
  
 @javascript
#TEST THREE: UPLOAD BY SAVING; clicking 'INSERT' element
Scenario: Upload a graphic by saving, then come back and insert it
  Given I am logged in as a user with the "site_owner" role
  And I am on "node/add/page"
  And fill in "edit-title" with "Mountains"
    # THIS NEXT LINE IS NECESSARY FOR FINDING THE BODY FIELD WHEN JAVASCRIPT TESTING IS ENABLED
  And I follow "Disable rich-text"
 And fill in "Body" with "Demo body content"
 And I fill in "edit-field-photo-und-0-alt" with "Pink clouds, blue mountains"
 And I attach the file "assets/mountains.jpg" to "edit-field-photo-und-0-upload"
  And I press "edit-submit"
 And I follow "Edit"
 Then I should see "Edit Basic page Mountains"
 # THIS NEXT LINE PROVES IT WAS UPLOADED
And I should see "File information"
 And I should see "Click and drag the crosshair to target the most important portion of the image"
 And I should see "mountains.jpg"
 And I should see "Insert"
# COULD NOT FIND THIS ELEMENT; END UP ON SEARCH PAGE  When I click the "input" element with "Insert" for "value"
When I click the "Insert" element
 And I press "edit-submit"
  Then I should be on "/mountains"
  And I should see "Mountains"
  And I should see "Demo body content"
 And the response should contain "alt=\"Pink clouds, blue mountains\""
  
  

#TEST FOUR: UPLOAD BY CLICKING UPLOAD; following INSERT
Scenario: Inserting a different size graphic than the default
  Given I am logged in as a user with the "site_owner" role
  And I am on "node/add/page"
  And fill in "edit-title" with "Dogs"
 And fill in "Body" with "Demo body content"
 And I attach the file "assets/dog.jpg" to "edit-field-photo-und-0-upload"
  And I fill in "edit-field-photo-und-0-alt" with "Red heeler with sunflower"
 And I press "edit-field-photo-und-0-upload-button"
 Then I should see "File information"
  And I should see "Click and drag the crosshair to target the most important portion of the image"
 And I should see "dog.jpg"
 # SYSTEM CANNOT FIND THE SELECT OPTIONS
 # And I select "image_hero" from "Style:"
 #  When I click the "input" element with "Insert" for "value"
 When I click the "Insert" element
 And I press "edit-submit"
  Then I should be on "/dogs"
  And I should see "Dogs"
  And I should see "Demo body content"
  And the response should contain "alt=\"Red heeler with sunflower\""  

