
@settings
Feature: an XML Site Map improves Search Engine results
In order to communicate the site layout to search engines
An authenticated user with the proper role
Should be able to update the Web Express XML site map

#SOME ROLES CAN ACCESS SITE MAP SETTINGS

Scenario Outline: Devs, Admins and SOs can access the sitemap page
  Given I am logged in as a user with the <role> role
  And am on "admin/settings/seo/xmlsitemap"
  Then I should see "sitemap.xml"
  And I should see "Update Sitemap"
  And I should see "Include Menus"
  And the checkbox "edit-options-menu-footer-menu" should be checked
  And the checkbox "edit-options-main-menu" should be checked
  And the checkbox "edit-options-menu-secondary-menu" should be checked
  
Examples:
    | role            | 
    | developer       | 
    | administrator   | 
    | site_owner      | 
    | configuration_manager |
       
#SOME ROLES CAN UPDATE AND CHANGE SITE MAP SETTINGS
 @broken
Scenario: Functionality - The sitemap can be rebuilt
  Given I am logged in as a user with the "site_owner" role
  And am on "admin/settings/seo/xmlsitemap"
  And I press "Update Sitemap"
  And I wait for the ".messages.status" element to appear
  Then I should see "The sitemap links were rebuilt."
  And I press "Add Menus"
  And I wait for the ".messages.status" element to appear
  Then I should see "Menu options have been updated for sitemap"

# SOME ROLES CAN NOT ACCESS SITE MAP SETTINGS

Scenario Outline: CEs and EMCs should not be able to update the sitemap
Given I am logged in as a user with the <role> role
And am on "admin/settings/seo/xmlsitemap"
Then I should see "Access denied"

 Examples:
    | role            | 
    | content_editor |
    | edit_my_content  | 
    | site_editor      | 
    | edit_only        | 
    | access_manager   | 
 

Scenario: An anonymous user should not be able to update the sitemap
  When I am on "admin/settings/seo/xmlsitemap"
  Then I should see "Access denied"
