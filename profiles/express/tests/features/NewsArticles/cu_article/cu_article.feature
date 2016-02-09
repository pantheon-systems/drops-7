Feature: Article Content Type
  When I log into the website
  As an content editor, site owner, administrator or developer
  I should be able to create, edit, and delete page content

  @api
  Scenario Outline: An authenticated user should be able to access the form for adding page content
    Given I am logged in as a user with the <role> role
    When I go to "node/add/article"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api
  Scenario: An anonymous user should not be able to access the form for adding page content
    Given I am an anonymous user
    When I go to "node/add/article"
    Then I should see "Access denied"

  # @Todo: Add check for external URL
  @api
  Scenario: An authenticated user should be able to create article node
    Given I am logged in as a user with the "content_editor" role
      And am on "node/add/article"
      And fill in "Title" with "New article"
      And fill in "Body" with "Demo body content"
      #And fill in "field_external_url" with "www.google.com"
      #And fill in "Tags" with "Tag1, Tag with lots of parts"
    When I press the "Save" button
    Then the "h1" element should contain "New Article"
      And I should see the text "Demo body content"
      #And I should see the link "Tag1"
      #And I should see the link "Tag with lots of parts"
    #   And I should see the following success messages:
    #   | messages |
    #   | The taxonomy term has been linked to this page. |
    #   | An Article List Page has been created for the tags on the article node you just created/updated. |
