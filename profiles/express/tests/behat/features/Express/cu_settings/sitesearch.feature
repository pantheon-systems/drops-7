@settings
#NOTE: THIS FEATURE NEEDS A GA ACCOUNT FOR ADVANCED FUNCTIONALITY; HENCE THE SIMPLE TESTS BELOW
Feature: Site Search Settings
In order to improve search capabilities
An authenticated user with the proper role
Should be able to access the Site Search Settings


Scenario Outline: Only Devs, Admins, SOs and CMs can set Site Search Settings options
 Given I am logged in as a user with the <role> role
 And am on "admin/settings/search/search-settings"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Site Search Settings" |
| administrator    | "Site Search Settings" |
| site_owner       | "Site Search Settings" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Site Search Settings" |


Scenario: An anonymous user should not be able to set Site Search Settings
 When I am on "admin/settings/search/search-settings"
 Then I should see "Access denied"
  
