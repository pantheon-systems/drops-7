Feature: Quicktabs

@api @quicktabs
Scenario Outline: Content Editors, Site Owners, Administrators, Developers should be able to add quicktabs from the block/add screen
  Given I am logged in as a user with the <role> role
  When I go to "block/add/quicktab"
  Then I should not see <message>

  Examples:
  | role           | message         |
  | content_editor | "Access denied" |
  | site_owner     | "Access denied" |
  | administrator  | "Access denied" |
  | developer      | "Access denied" |
