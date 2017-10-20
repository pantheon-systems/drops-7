Feature: Photo Gallery Creation
  Create different configurations of Photo Galleries.

  @api @javascript @broken
  Scenario: Create a basic photo gallery.
    Given  CU - I am logged in as a user with the "content_editor" role
      And I am on "node/add/photo-gallery"
      And I fill in "edit-title" with "Test Photo Gallery"
      And I fill in "edit-field-photo-und-0-alt" with "alt one"
      And I fill in "edit-field-photo-und-0-title" with "title one"
      And I attach the file "ralphie.jpg" to "edit-field-photo-und-0-upload"
      And I press the "Upload" button
      And I wait 5 seconds
      #And I wait for AJAX
      And I fill in "edit-field-photo-und-1-alt" with "alt two"
      And I fill in "edit-field-photo-und-1-title" with "title two"
      And I attach the file "ralphie.jpg" to "edit-field-photo-und-1-upload"
      And I press the "Upload" button
      #And I wait for AJAX
      And I press the "Save" button
    Then I should see "Photo Gallery Test Photo Gallery has been created."
    When I click the "img" element with "alt one" for "alt"
      And I wait 5 seconds
    Then I should see "alt one"
    When I click the "#cboxNext" element
      #And I wait for AJAX
      #And I wait for AJAX
      #And I wait for AJAX
    Then I should see "alt two"
    When I click the "#cboxClose" element
    Then I should see "Test Photo Gallery"
