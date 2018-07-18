@menus
Feature: Menus control the navigation structure of the site
When I go to the Admin/Menu page
As an Admin level user
I can add and edit the menus on my site
  
# The only users who can add or edit menus are Site Editor and up.

Scenario Outline: Access - An Admin level user should be able to add and edit all Express menus
  Given I am logged in as a user with the <role> role
  When I go to "admin/structure/menu"
  Then I should see "Menus"
  And I go to "admin/structure/menu/settings"
  Then I should see "Source for the Main links"
  And I go to "admin/structure/menu/add"
  Then I should see "Title"
    And I go to "admin/structure/menu/manage/menu-footer-menu"
 Then I should see "Footer Menu"
  And I go to "admin/structure/menu/manage/main-menu"
  Then I should see "Main Menu"
  And I go to "admin/structure/menu/manage/menu-mobile-menu"
  Then I should see "Mobile Menu"
  And I go to "admin/structure/menu/manage/menu-secondary-menu"
  Then I should see "Secondary Menu"

Examples:
| role           | 
| developer      | 
| administrator  | 
| site_owner     | 
| site_editor    | 


Scenario Outline: Access - A user with limited roles cannot add or edit menus
  Given I am logged in as a user with the <role> role
  When I go to "admin/structure/menu"
  Then I should see "Access denied"
  And I go to "admin/structure/menu/settings"
 Then I should see "Access denied"
  And I go to "admin/structure/menu/add"
  Then I should see "Access denied"
  And I go to "admin/structure/menu/manage/main-menu"
  Then I should see "Access denied"
  And I go to "admin/structure/menu/manage/menu-mobile-menu"
 Then I should see "Access denied"
  And I go to "admin/structure/menu/manage/menu-footer-menu"
 Then I should see "Access denied"
  And I go to "admin/structure/menu/manage/menu-secondary-menu"
  Then I should see "Access denied"
  
Examples:
| role                  |  
# @todo fix for this role.
# | content_editor        |
| edit_my_content       | 
| edit_only             | 
| access_manager        | 
| configuration_manager | 

Scenario: Access - An anonymous user should not be able to cannot add or edit menus
  When I am on "admin/structure/menu"
  Then I should see "Access denied"
  
# NOTE: THE MENUS SECTION HAS LINKS TO THE BLOCKS ADMIN PAGE AND CONTENT TYPES ADMIN PAGE
# WHICH ARE OFF LIMITS TO ALL BUT DEVELOPERS
Scenario Outline: Access - No one (but Devs) can access the Drupal System Block Admin page or Content Types page
Given I am logged in as a user with the <role> role
When I go to "admin/structure/block"
Then I should see "Access denied"
And I go to "admin/structure/types"
Then I should see "Access denied"
  
Examples:
| role                  |  
| administrator         | 
| site_owner            | 
| content_editor        | 
| edit_my_content       | 
| site_editor           | 
| edit_only             | 
| access_manager        | 
| configuration_manager | 

Scenario: Functionality -  The Menus landing page is properly populated with links and content
  Given I am logged in as a user with the "site_owner" role
  When I go to "admin/structure/menu"
  Then I should see the link "List menus"
  And I should see the link "Settings"
  And I should see the link "Add menu"
  And I should see "Footer Menu"
  And I should see "Main menu"
  And I should see "Mobile Menu"
  And I should see "Secondary Menu"
  
  Scenario: Functionality - The Menu Settings page is properly populated with functionality
  Given I am logged in as a user with the "site_owner" role
  When I go to "admin/structure/menu/settings"
  Then I should see "Source for the Main links"
  And I should see "Source for the Secondary links"
  And I should see "Secondary Menu Label"
  And I should see "Source for the mobile links"
  And I should see "Source for the footer links"
  
  Scenario Outline: Functionality - A menu item can be added to all menus
  Given I am logged in as a user with the "site_owner" role
  When I go to <path>
  And I fill in "edit-additem-title" with "Academics"
  And I press "edit-submit" 
  Then the "edit-link-title" field should contain "Academics"
  And I fill in "edit-link-path" with "https://www.colorado.edu/academics"
  And I should see "Icon"
  And I should see "Advanced menu item settings"
  And I press "Save"
  Then I should see "Your configuration has been saved."
  
  Examples:
  | path |
  | "admin/structure/menu/manage/menu-footer-menu" |
  | "admin/structure/menu/manage/main-menu" |
  | "admin/structure/menu/manage/menu-mobile-menu" |
  | "admin/structure/menu/manage/menu-secondary-menu" |
