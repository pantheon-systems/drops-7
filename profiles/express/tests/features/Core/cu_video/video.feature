Feature: Video permissions check
When I login to the website
As a user
I should not be able to add new video nodes

@api
Scenario Outline: An authenticated user should not be able to add new video nodes
  Given I am logged in as a user with the <role> role
  When I go to "node/add/video"
  Then I should see <message>

  Examples:
  | role           | message         |
  | content_editor | "Access denied" |
  | site_owner     | "Access denied" |
  | administrator  | "Access denied" |
  | developer      | "Access denied" |
