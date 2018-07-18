Feature: Article Content Type
  When I log into the website
  As an content editor, site owner, administrator or developer
  I should be able to create, edit, and delete page content


  Scenario Outline: An authenticated user should be able to access the form for adding page content
    Given  I am logged in as a user with the <role> role
    When I go to "node/add/article"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |


  Scenario: An anonymous user should not be able to access the form for adding page content
    When I am on "node/add/article"
    Then I should see "Access denied"

  @javascript
  Scenario: An authenticated user should be able to create article node
    Given  I am logged in as a user with the "content_editor" role
      And am on "node/add/article"
      And fill in "Title" with "New article"
      And I follow "Disable rich-text"
      And fill in "Body" with "Demo body content"
      And I follow "External Link"
      And fill in "edit-field-article-external-url-und-0-url" with "www.google.com"
      And I follow "Tags"
      And fill in "Tags" with "Tag1, Tag with lots of parts"
    When I press "Save"
    Then the "#page-title" element should contain "New Article"
      And I should see "Demo body content"
      And I should see "Tag1"
      And I should see "Tag with lots of parts"
      And I should see "The taxonomy term has been linked to this page."
      And I should see "An Article List Page has been created for the tags on the article node you just created/updated."
