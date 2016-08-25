Feature: CU Layout Access

  @api @layout
  Scenario Outline: The layout settings form should be available for certain roles.
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/content/express-layout"
    Then I should not see <message>

    Examples:
      | role            | message         |
      | content_editor  | "Select which regions are available to place blocks for each content type." |
      | edit_my_content | "Select which regions are available to place blocks for each content type." |
      | site_owner      | "Select which regions are available to place blocks for each content type." |
      | administrator   | "Select which regions are available to place blocks for each content type." |
      | developer       | "Access denied" |

