@siteconfig
Feature: Site Naming
When I login to a Web Express website
As an authenticated user
I may or may not be able to change the Site Name settings

@testing_frontpage
Scenario: Changing the Site Name
  Given I am logged in as a user with the "site_owner" role
  When I go to "admin/settings/site-configuration/site-name"
  And I fill in "edit-site-name-1" with "My Web Express Site"
  And I fill in "edit-site-name-2" with "Minds to Match Our Mountains"
  And I press "Save configuration"
  Then I should see "The configuration options have been saved."
  When I go to "/"
  Then I should see "My Web Express Site"
  And I should see "Minds to Match Our Mountains"
