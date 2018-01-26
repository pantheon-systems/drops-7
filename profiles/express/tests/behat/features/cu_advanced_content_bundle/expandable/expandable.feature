Feature: Quicktabs

@api @expandable
Scenario Outline: Content Editors, Site Owners, Administrators, Developers should be able to add Expandable from the block/add screen
  Given  CU - I am logged in as a user with the <role> role
  When I am at "block/add/expandable"
  Then I should not see <message>

  Examples:
  | role           | message         |
  | content_editor | "Access denied" |
  | site_owner     | "Access denied" |
  | administrator  | "Access denied" |
  | developer      | "Access denied" |
