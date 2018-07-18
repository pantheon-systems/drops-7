@settings
Feature: Site Contact populates Site Information region
In order to provide contact information about the site
Authenticated users with the proper role
Should be able to add Contact Information

#SOME ROLES CAN SET THE SITE CONTACT

Scenario Outline: only Devs, Admins, SOs and ConMgrs can access Contact Information
Given I am logged in as a user with the <role> role
When I go to "admin/settings/site-configuration/contact"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Contact Information" |
| administrator    | "Contact Information" |
| site_owner       | "Contact Information" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Contact Information" |


Scenario: An anonymous user should not be able to set site name
  When I am on "admin/settings/site-configuration/contact"
  Then I should see "Access denied"

# Testing the frontpage affects the cu_campus_map_bundle tests.
 @testing_frontpage
Scenario: When Contact Information is populated, it shows up in the footer region
  Given I am logged in as a user with the "site_owner" role
  And am on "admin/settings/site-configuration/contact"
  And fill in "Contact Information" with "email@example.edu"
  And I press "edit-submit"
  Then I should see "The configuration options have been saved"
  And I go to "/"
  Then I should see "email@example.edu"
    
