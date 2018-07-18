@design
Feature: Design Themes change the themeing of the site while maintaining the branded look
In order to set my site apart from others in the University
As an authenticated user with the proper role
I should be able to change the site theme

Scenario Outline: Access - only Devs, Admins, SOs and ConMgrs can access the Site Theme page
Given I am logged in as a user with the <role> role
When I go to "admin/theme"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Choose a theme" |
| administrator    | "Choose a theme" |
| site_owner       | "Choose a theme" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Choose a theme" |


Scenario: An anonymous user should not be able to set site name
When I go to "admin/theme"
Then I should see "Access denied"
  
Scenario: Functionality - All available themes should be available
  Given  I am logged in as a user with the "site_owner" role
  And am on "admin/theme"
  Then I should see "Modern"
  And I should see "Highlight"
    And I should see "Ivory"
    And I should see "Layers"
    And I should see "Minimal"
    And I should see "Rise"
    And I should see "Shadow"
    And I should see "Simple"
    And I should see "Spirit"
    And I should see "Swatch"
    And I should see "Tradition"

# @broken
#Scenario: Functionality - An Admin-level user can change the site theme
# Given  I am logged in as a user with the "site_owner" role
# And am on "admin/theme"
# And I click the ".btn-info" element NEED BETTER SELECTOR
# Then I should see "Active theme has been set."
   
Scenario: Access - As a site_owner I should not see jquery theme settings
  Given  I am logged in as a user with the "site_owner" role
    And am on "admin/theme/config/cumodern"
  Then I should not see "jQuery Update"
