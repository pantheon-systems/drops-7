Feature: File Content Type
  When I login to the website
  As a content editor, site owner, administrator or developer
  I should be able to add a file

  @api
  Scenario Outline: An authenticated user should be able to access the form for adding a file
    Given  CU - I am logged in as a user with the <role> role
    When I go to "node/add/file"
    Then I should not see <message>

    Examples:
      | role           | message         |
      | content_editor | "Access denied" |
      | site_owner     | "Access denied" |
      | administrator  | "Access denied" |
      | developer      | "Access denied" |

  @api
  Scenario: An anonymous user should not be able to access the form for adding a file
    Given I am an anonymous user
    When I go to "node/add/file"
    Then I should see "Access denied"


  @api @javascript
  Scenario: A content editor should be able to access the form for adding a file
    Given  CU - I am logged in as a user with the "content_editor" role
    When I go to "node/add/file"
      And  I fill in "Title" with "My File"
      And I fill in "body[und][0][value]" with "Sample Description"
      And I attach the file "ralphie.jpg" to "edit-field-file-attachment-und-0-upload"
      And I press the "Upload" button
      #And I wait for AJAX
    Then I should see "ralphie.jpg"
    When I press the "Save" button
    Then I should see "My File"
