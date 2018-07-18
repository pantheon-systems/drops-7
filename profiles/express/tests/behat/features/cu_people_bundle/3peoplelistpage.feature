 @people
Feature: People List Page Content Type
  In order to display a directory list of Person nodes
  As an authenticated user
  I should be able to create, edit, and delete People List Page content

  Scenario Outline: An authenticated user should be able to access the form for adding people list page content
    Given  I am logged in as a user with the <role> role
    When I go to "node/add/people-list-page"
    Then I should see <message>

    Examples:
      | role                  | message                   |
      | edit_my_content       | "Access denied"           |
      | content_editor        | "Create People List Page" |
      | site_owner            | "Create People List Page" |
      | administrator         | "Create People List Page" |
      | developer             | "Create People List Page" |
      | configuration_manager | "Access denied"           |
      | site_editor           | "Create People List Page" |
      | edit_only             | "Access denied"           |
      | access_manager        | "Access denied"           |


  Scenario: An anonymous user should not be able to access the form for adding people list page content
    When I am on "node/add/people-list-page"
    Then I should see "Access denied"


  Scenario: The Provide Menu Link box should be checked on node creation but remain unchecked if unchecked.
    Given I am logged in as a user with the "site_owner" role
    When I go to "node/add/people-list-page"
    And  I fill in "Title" with "New People List Page"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck "edit-menu-enabled"
    And I press "Save"
    And I follow "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked

   @javascript
  Scenario: The People List Page provides several display/format types
    Given I am logged in as a user with the "site_owner" role
    And am on "node/add/people-list-page"
    And I click the ".group-people-list-display.field-group-fieldset a.fieldset-title" element
    And I select "Table" from "edit-field-people-list-display-und"
    And I select "Grid" from "edit-field-people-list-display-und"
    And I select "List" from "edit-field-people-list-display-und"
    
## POPULATING DATA TABLE FOR PEOPLE LIST PAGES AND BLOCKS

  Scenario: Create Person 1 - Deshawn Michael StaffGeoMariDes
    Given I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Deshawn"
    And fill in "Last Name" with "StaffGeoMariDes"
    And fill in "edit-field-person-job-type-und" with "Staff"
    And fill in "edit-field-person-title-und-0-value" with "Director"
    And fill in "edit-field-person-department-und" with "Geophysics"
    And fill in "edit-field-person-email-und-0-email" with "deshawn@example.com"
    And fill in "edit-field-person-phone-und-0-value" with "303-123-4567"
    And fill in "edit-field-person-filter-1-und" with "Marietta"
    And fill in "edit-field-person-filter-2-und" with "Design"
    When I press "Save"
    Then I should see "Person Deshawn StaffGeoMariDes has been created."

  Scenario: Create Person 2 - Alejandro Cruz FacGeoHoneyLaw
    Given I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Alejandro"
    And fill in "Last Name" with "FacGeoHoneyLaw"
    And fill in "edit-field-person-job-type-und" with "Faculty"
    And fill in "edit-field-person-title-und-0-value" with "Manager"
    And fill in "edit-field-person-department-und" with "Geophysics"
    And fill in "edit-field-person-email-und-0-email" with "alejandro@example.com"
    And fill in "edit-field-person-phone-und-0-value" with "303-444-6789"
    And fill in "edit-field-person-filter-1-und" with "Honeywell"
    And fill in "edit-field-person-filter-2-und" with "Law"
    When I press "Save"
    Then I should see "Person Alejandro FacGeoHoneyLaw has been created."


  Scenario: Create Person 3 - Kendall Hull StaffTechHoneyLaw
    Given I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Kendall"
    And fill in "Last Name" with "StaffTechHoneyLaw"
    And fill in "edit-field-person-job-type-und" with "Staff"
    And fill in "edit-field-person-title-und-0-value" with "Supervisor"
    And fill in "edit-field-person-department-und" with "Technology"
    And fill in "edit-field-person-email-und-0-email" with "kendall@example.com"
    And fill in "edit-field-person-phone-und-0-value" with "303-333-5567"
    And fill in "edit-field-person-filter-1-und" with "Honeywell"
    And fill in "edit-field-person-filter-2-und" with "Law"
    When I press "Save"
    Then I should see "Person Kendall StaffTechHoneyLaw has been created."


  Scenario: Create Person 4 - Abdullah Lang FacTechMariDes
    Given I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Abdullah"
    And fill in "Last Name" with "FacTechMariDes"
    And fill in "edit-field-person-job-type-und" with "Faculty"
    And fill in "edit-field-person-title-und-0-value" with "Instructor"
    And fill in "edit-field-person-department-und" with "Technology"
    And fill in "edit-field-person-email-und-0-email" with "abdullah@example.com"
    And fill in "edit-field-person-phone-und-0-value" with "303-123-4567"
    And fill in "edit-field-person-filter-1-und" with "Marietta"
    And fill in "edit-field-person-filter-2-und" with "Design"
    When I press "Save"
    Then I should see "Person Abdullah FacTechMariDes has been created."

   @javascript
  Scenario: Adding taxonomy terms to Persons populates the the People List Page filters
    Given I am logged in as a user with the "site_owner" role
    And am on "node/add/people-list-page"
    And I click the ".group-people-list-filter.field-group-fieldset a.fieldset-title" element
    And I should see "Geophysics"
    And I should see "Technology"
    Then I should see "Faculty"
    Then I should see "Staff"
    And I should see "Honeywell"
    And I should see "Marietta"
    And I should see "Design"
    And I should see "Law"


  Scenario: A People List Page filters persons correctly
    Given I am logged in as a user with the "site_owner" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Our Faculty"
    And I check "Faculty"
    When I press "Save"
    Then I should be on "our-faculty"
    And I should see "Our Faculty"
    And I should see "Alejandro FacGeoHoneyLaw"
    And I should see "Abdullah FacTechMariDes"
    And I should not see "Kendall StaffTechHoneyLaw"
    And I should not see "Deshawn StaffGeoMariDes"


  Scenario: A People List Page can group people by chosen filter
    Given I am logged in as a user with the "site_owner" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Research Groups"
    And I check "Geophysics"
 # And I click the ".group-people-list-filter.field-group-fieldset a.fieldset-title" element
    And I select "people_filter_2" from "edit-field-people-group-by-und"
    And I press "Save"
    Then I should see "Design"
    And I should see "Deshawn StaffGeoMariDe"
    And I should see "Law"
    And I should see "Alejandro FacGeoHoneyLaw"


  Scenario: A People List Page can display all the chosen filters
    Given I am logged in as a user with the "site_owner" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Directory"
    And I select "Show" from "edit-field-people-dept-filter-show-und"
    And I select "Show" from "edit-field-people-pos-filter-show-und"
    And I select "Show" from "edit-field-people-filter1-show-und"
    And I select "Show" from "edit-field-people-filter2-show-und"
  # LEAVE DEFAULT And I select "Hide" from "edit-field-people-filter3-show-und"
    And I press "Save"
    Then I should see a ".people-list-filter" element
    #  DUPLICATE CHECK  And the response should contain "class=\"people-list-filter\""
    Then I should see "Appointment"
    And I should see "Division"
    And I should see "Research Group"
    And I should see "Area of Expertise"
    And I should not see "Committees"
      # THIS ONE IS AN ATLAS ERROR
    # TEST FINDS THIS EVEN THOUGH HIDDEN And I should not see "Leave This Field Blank"
    
