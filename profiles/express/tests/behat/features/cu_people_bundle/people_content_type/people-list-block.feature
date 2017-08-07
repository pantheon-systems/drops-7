Feature: People List Block

@api @people-list-block
Scenario Outline: An authenticated user should be able to access the form for adding a people list block
    Given  CU - I am logged in as a user with the <role> role
    When I go to "block/add/people-list-block"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

@api @people-list-block
Scenario: An anonymous user should not be able to access the form for adding person content
  Given I am an anonymous user
  When I go to "block/add/people-list-block"
  Then I should see "Access denied"

@api @people-list-block
Scenario: Content editors can create person nodes
  Given  CU - I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Staff"
    And fill in "Last Name" with "Person"
    And fill in "Job Type" with "Staff"
    And fill in "edit-field-person-title-und-0-value" with "My Job Title"
    And fill in "Department" with "Department One"
  When I press the "Save" button
  Then I should see "Person Staff Person has been created."

  Given  CU - I am logged in as a user with the "content_editor" role
    And am on "node/add/person"
    And fill in "First Name" with "Faculty"
    And fill in "Last Name" with "Person"
    And fill in "Job Type" with "Faculty"
    And fill in "edit-field-person-title-und-0-value" with "My Job Title"
      And fill in "Department" with "Department Two"
  When I press the "Save" button
  Then I should see "Person Faculty Person has been created."

  Given  CU - I am logged in as a user with the "content_editor" role
    And am on "block/add/people-list-block"
    And fill in "Title" with "People List Block"
    And fill in "Label" with "People List Block"
  When I press the "Save" button

  Then I should see "Staff Person"
    And I should see "Faculty Person"
