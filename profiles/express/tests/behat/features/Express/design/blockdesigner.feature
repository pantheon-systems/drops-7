@design
Feature: Block Designer adds unique look to blocks while maintaining the branded theme
In order to set my site apart from others in the University
As an authenticated user with the proper role
I should be able to change the Block Design themes

Scenario Outline: Access - only Devs, Admins, SOs and ConMgrs can access the Block Designer page
Given I am logged in as a user with the <role> role
When I go to "admin/theme/block-designer"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Block Designer Themes give you a quick and easy way" |
| administrator    | "Block Designer Themes give you a quick and easy way" |
| site_owner       | "Block Designer Themes give you a quick and easy way" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Block Designer Themes give you a quick and easy way" |


Scenario: Access - An anonymous user should not be able to set site name
  When I am on "admin/theme/block-designer"
  Then I should see "Access denied"
  
Scenario Outline: Access - only Devs, Admins, SOs and ConMgrs can add a Block Design
Given I am logged in as a user with the <role> role
When I go to "admin/theme/block-designer/add"
Then I should see <message>

Examples:
| role             | message |
| developer        | "A name describing your block theme" |
| administrator    | "A name describing your block theme" |
| site_owner       | "A name describing your block theme" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
# | site_editor      | "Access denied" | CAN ADD AND USE BLOCK DESIGNER
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "A name describing your block theme" |

 Scenario: Access - An anonymous user should not be able to set site name
  When I am on "admin/theme/block-designer/add"
  Then I should see "Access denied"
 
Scenario: Functionality - A simple block theme can be created
  Given  I am logged in as a user with the "site_owner" role
   When I am on "admin/theme/block-designer/add"
  And I fill in "edit-block-theme-name" with "Simple Outline Block"
  And I select "outline" from "exbd_style"
  And I press "edit-submit"
  Then I should see "Block Designer Theme settings have been saved."
  And I should see "Simple Outline Block"
  
