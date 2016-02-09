Feature: Page Content Type
  When I log into the website
  As an content editor, site owner, administrator or developer
  I should be able to create, edit, and delete page content

  @api @page
  Scenario Outline: An authenticated user should be able to access the form for adding page content
    Given I am logged in as a user with the <role> role
    When I go to "node/add/page"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @page
  Scenario: An anonymous user should not be able to access the form for adding page content
    Given I am an anonymous user
    When I go to "node/add/page"
    Then I should see "Access denied"

  @api @page
  Scenario: An authenticated user should be able to create page node
    Given I am logged in as a user with the "content_editor" role
      And I am on "node/add/page"
      And for "Title" I enter "New page"
      And for "Body" I enter "Demo body content"
      And for "Menu link title" I enter "New Menu Item"
    # When I attach the file "../../../assets/ralphie.jpg" to "edit-field-photo-und-0-upload"
    #   And I press the "Upload" button
    #   And for "Alternate text" I enter "Ralphie running with people"
    #   And I press the "Insert" button
    When I press the "Save" button
    Then the "h1" element should contain "New Page"
      And I should see the text "Demo body content"
      # And I should see an image in the "Content" region
      # And I should see the image alt "Ralphie running with people" in the "Content" region
      And I should see the text "New Menu Item"

  @api @page
  Scenario: The provide menu link box should be checked on node creation but remain unchecked if user chooses to uncheck that box.
    Given I am logged in as a user with the "site_owner" role
    When I go to "node/add/page"
    And  I fill in "edit-title" with "New Page"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck the box "edit-menu-enabled"
    And I press the "Save" button
    And I click "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked