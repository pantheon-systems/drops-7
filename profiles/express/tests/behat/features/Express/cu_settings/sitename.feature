@settings
Feature: Site Name identifies the Web Express website; appears in top banner
In order to identify what Web Express site this is
An authenticated user with the proper role
Should be able to set the website name

Scenario Outline: Devs, Admins, SOs and ConMgrs can access Site Name
Given I am logged in as a user with the <role> role
When I go to "admin/settings/site-configuration/site-name"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Site Name" |
| administrator    | "Site Name" |
| site_owner       | "Site Name" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Site Name" |


Scenario: An anonymous user should not be able to set site name
 When I am on "admin/settings/site-configuration/site-name"
 Then I should see "Access denied"
 
 @testing_frontpage
Scenario: When Site Name is populated, it shows up on the homepage
  Given I am logged in as a user with the "site_owner" role
  And am on "admin/settings/site-configuration/site-name"
  And fill in "edit-site-name-1" with "New Site Title"
  And fill in "edit-site-name-2" with "Second Line"
  When I press "Save"
  Then I should see "The configuration options have been saved"
  And I go to "/"
  Then I should see a ".site-name-two-lines" element
  And I should see "New Site Title Second Line"
