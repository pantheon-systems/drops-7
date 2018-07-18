Feature: People

@people
Scenario Outline: An authenticated user should be able to access the form for adding person content
    Given  I am logged in as a user with the <role> role
    When I go to "node/add/person"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

@people
Scenario: An anonymous user should not be able to access the form for adding person content
  When I am on "node/add/person"
  Then I should see "Access denied"

@people
Scenario Outline: An authenticated user should be able to access the form for adding people list page content
    Given  I am logged in as a user with the <role> role
    When I go to "node/add/people-list-page"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

@people
Scenario: An anonymous user should not be able to access the form for adding people list page content
  When I am on "node/add/people-list-page"
  Then I should see "Access denied"

@people
Scenario: Content editors can create person nodes
  Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Staff"
    And fill in "Last Name" with "Person"
    And fill in "Job Type" with "Staff"
    And fill in "edit-field-person-title-und-0-value" with "My Job Title"
    And fill in "Department" with "Department One"
  When I press "Save"
  Then I should see "Person Staff Person has been created."

  # Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Faculty"
    And fill in "Last Name" with "Person"
    And fill in "Job Type" with "Faculty"
    And fill in "edit-field-person-title-und-0-value" with "My Job Title"
      And fill in "Department" with "Department Two"
  When I press "Save"
  Then I should see "Person Faculty Person has been created."

  # Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "People"
    And I select "Table" from "edit-field-people-list-display-und"
    And I select "Grid" from "edit-field-people-list-display-und"
    And I select "List" from "edit-field-people-list-display-und"
    And I select "Show" from "edit-field-people-pos-filter-show-und"
  When I press "Save"
    And I go to "people"
  Then I should see "Staff Person"
    And I should see "Faculty Person"
    And I should see a "select" element

  # Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Faculty People"
    And I check "Faculty"
  When I press "Save"
    And I go to "faculty-people"
  Then I should not see "Staff Person"
    And I should see "Faculty Person"

  # Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Staff People"
    And I check "Staff"
  When I press "Save"
    And I go to "staff-people"
  Then I should see "Staff Person"
    And I should not see "Faculty Person"

  # Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Department One People"
    And I check "Department One"
  When I press "Save"
    And I go to "department-one-people"
  Then I should see "Staff Person"
    And I should not see "Faculty Person"

  # Given  I am logged in as a user with the "content_editor" role
    And am on "node/add/people-list-page"
    And fill in "Title" with "Department Two People"
    And I check "Department Two"
  When I press "Save"
    And I go to "department-two-people"
  Then I should not see "Staff Person"
    And I should see "Faculty Person"

  @people @people-filters
  Scenario: Person nodes should accept more than 1 filter value per filter
    Given  I am logged in as a user with the "content_editor" role
      And am on "node/add/person"
      And fill in "First Name" with "Faculty"
      And fill in "Last Name" with "Person"
      And fill in "edit-field-person-filter-1-und" with "Filter 1 Term 1, Filter 1 Term 2"
      And fill in "edit-field-person-filter-2-und" with "Filter 2 Term 1, Filter 2 Term 2"
      And fill in "edit-field-person-filter-3-und" with "Filter 3 Term 1, Filter 3 Term 2"
    When I press "Save"
    Given I go to "admin/structure/taxonomy/people_filter_1"
    Then I should see "Filter 1 Term 1"
      And I should see "Filter 1 Term 2"
      And I should not see "Filter 1 Term 1, Filter 1 Term 2"
    Given I go to "admin/structure/taxonomy/people_filter_2"
    Then I should see "Filter 2 Term 1"
      And I should see "Filter 2 Term 2"
      And I should not see "Filter 2 Term 1, Filter 2 Term 2"
    Given I go to "admin/structure/taxonomy/people_filter_3"
    Then I should see "Filter 3 Term 1"
      And I should see "Filter 3 Term 2"
      And I should not see "Filter 3 Term 1, Filter 3 Term 2"

    @people @javascript @broken
    Scenario: Footer, Main Menu, and Secondary Menus should be available when creating a Person
      Given  I am logged in as a user with the "content_editor" role
        And I am on "node/add/person"
        And I fill in "First Name" with "John Doe"
        And I click the "#edit-menu-enabled" element
        And I select "<Footer Menu>" from "Parent item"
        And I select "<Secondary Menu>" from "Parent item"
        And I press "Save"
      Given I resize the window to a "mobile" resolution.
      When I click the ".mobile-menu-toggle a" element
      Then I should see "John Doe"
      And I resize the window to a "desktop" resolution.

  @people @people-filters
  Scenario: Adding a label to the filter terms should result in the label showing up on the people list page.
    Given  I am logged in as a user with the "site_owner" role
      And am on "node/add/person"
      And fill in "First Name" with "Staff"
      And fill in "Last Name" with "Person"
      And fill in "Job Type" with "Staff"
      And fill in "edit-field-person-title-und-0-value" with "My Job Title"
      And fill in "Department" with "Department One"
      And fill in "edit-field-person-filter-1-und" with "apples"
      And fill in "edit-field-person-filter-2-und" with "cats"
      And fill in "edit-field-person-filter-3-und" with "trees"
    When I press "Save"
      And am on "node/add/person"
      And fill in "First Name" with "Faculty"
      And fill in "Last Name" with "Person"
      And fill in "Job Type" with "Faculty"
      And fill in "edit-field-person-title-und-0-value" with "My Job Title"
      And fill in "Department" with "Department Two"
      And fill in "edit-field-person-filter-1-und" with "oranges"
      And fill in "edit-field-person-filter-2-und" with "dogs"
      And fill in "edit-field-person-filter-3-und" with "shrubs"
    When I press "Save"
      And am on "admin/settings/people/settings"
      And I fill in "Department Label" with "Group"
      And I fill in "Type Label" with "type 2"
      And I fill in "Filter One Label" with "filter one label"
      And I fill in "Filter Two Label" with "filter two label"
      And I fill in "Filter Three Label" with "filter three label"
      And I press "Save"
      And I am on "node/add/people-list-page"
      And I fill in "Title" with "People List Page"
      And I select "Show" from "edit-field-people-dept-filter-show-und"
      And I select "Show" from "edit-field-people-pos-filter-show-und"
      And I select "Show" from "edit-field-people-filter1-show-und"
      And I select "Show" from "edit-field-people-filter2-show-und"
      And I select "Show" from "edit-field-people-filter3-show-und"
      And I press "Save"
    Then I should see "Group"
      And I should see "type 2"
      And I should see "filter one label"
      And I should see "filter two label"
      And I should see "filter three label"
