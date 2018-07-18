@settings
Feature: Google Analytics Account ID
In order to link my Web Express site with Google Analytics
An authenticated user with the proper role
Should be able to set the Google Analytics Account ID

#SOME ROLES CAN SET THE GOOGLE ANALYTICS ID

Scenario Outline: Only Devs, Admins, SOs and CMs can access Google Analytics ID
Given I am logged in as a user with the <role> role
When I go to "admin/settings/site-configuration/google-analytics"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Google Analytics" |
| administrator    | "Google Analytics" |
| site_owner       | "Google Analytics" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Google Analytics" |


Scenario: An anonymous user should not be able to set the Google Analytics Account ID
  When I am on "admin/settings/site-configuration/google-analytics"
  Then I should see "Access denied"
  

Scenario: When Google Analytics ID is populated, it shows up on SEO dashboard
  Given I am logged in as a user with the "site_owner" role
  And am on "admin/settings/site-configuration/google-analytics"
  When I fill in "edit-ga-account" with "UA-987654-1"
  And I press "Save"
  Then I should see "The configuration options have been saved"
  And I go to "admin/dashboard/seo"
  Then I should see "You have a custom google analytics account assigned to your website."
  

Scenario Outline: Most roles cannot access the Google Analytics General Settings page
Given I am logged in as a user with the <role> role
And am on "admin/config/system/googleanalytics"
Then I should see "Access denied"

 Examples:
    | role            |
    | administrator   | 
    | site_owner      |
    | content_editor |
    | edit_my_content  | 
    | site_editor      | 
    | edit_only        | 
    | access_manager   | 

