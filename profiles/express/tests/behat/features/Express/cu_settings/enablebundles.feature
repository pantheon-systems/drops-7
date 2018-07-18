@settings
Feature: Enabling Bundles
In order to add functionality to a Web Express site
An authenticated user with the proper role
Should be able to access the Bundle List pages

#SOME ROLES CAN ENABLE BUNDLES

Scenario Outline: Only Devs, Admins, SOs and ConMgrs can Enable Bundles
Given I am logged in as a user with the <role> role
When I go to "admin/settings/bundles/list"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Configure Bundles" |
| administrator    | "Configure Bundles" |
| site_owner       | "Configure Bundles" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Configure Bundles" |


Scenario: The Bundle List page displays three tabs
  Given I am logged in as a user with the "site_owner" role
  And am on "admin/settings/bundles/list"
  Then I should see "Configure Bundles"
  And I should see "Core"
  And I should see "Add-on"
  And I should see "Request"

# THE FOLLOWING TEST WHITESCREENS FOR DEVELOPERS IN TEST ENVIRONMENT; TAGGING AS BROKEN FOR NOW 
 @broken
Scenario Outline: Only Devs, Admins, SOs and ConMgrs can access the Bundle Add-on page
Given I am logged in as a user with the <role> role
And am on "admin/settings/bundles/list/addon"
Then I should see <message>

Examples:
| role             | message |
| developer        | "These are bundles that can be added" |
| administrator    | "These are bundles that can be added" |
| site_owner       | "These are bundles that can be added" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "These are bundles that can be added" |
    
# THE FOLLOWING TEST WHITESCREENS FOR DEVELOPERS IN TEST ENVIRONMENT; TAGGING AS BROKEN FOR NOW 
 @broken
Scenario Outline: Only Devs, Admins, SOs and ConMgrs can access the Bundle Request page
Given I am logged in as a user with the <role> role
And am on "admin/settings/bundles/list/request"
Then I should see <message>

Examples:
| role             | message |
| developer        | "These are bundles that must be requested"|
| administrator    | "These are bundles that must be requested"|
| site_owner       | "These are bundles that must be requested"|
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "These are bundles that must be requested"|
