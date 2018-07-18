@settings
Feature: Site Description populates Meta tag "Description" on site homepage
In order to optimize search engine results with Meta Tag Description
Authenticated users with the proper role
Should be able to add a Site Description

Scenario Outline: Devs, Admins, SOs and ConMgrs can access the Site Description
Given I am logged in as a user with the <role> role
When I go to "admin/settings/site-configuration/site-description"
Then I should see <message>

Examples:
| role             | message |
| developer        | "Site Description" |
| administrator    | "Site Description" |
| site_owner       | "Site Description" |
| content_editor   | "Access denied" |
| edit_my_content  | "Access denied" |
| site_editor      | "Access denied" |
| edit_only        | "Access denied" |
| access_manager   | "Access denied" |
| configuration_manager | "Site Description" |


Scenario: An anonymous user should not be able to set site name
  When I am on "admin/settings/site-configuration/site-description"
  Then I should see "Access denied"

 @testing_frontpage
Scenario: When Site Description is populated, it shows up on the homepage
  Given I am logged in as a user with the "site_owner" role
  And am on "admin/settings/site-configuration/site-description"
  And fill in "site_description" with "We offer personalized career development"
  And I press "edit-submit"
  Then I should see "The configuration options have been saved"
  And I go to "/"
  Then the response should contain "content=\"We offer personalized career development\""


  
