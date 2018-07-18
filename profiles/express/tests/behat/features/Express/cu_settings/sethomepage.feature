@settings
Feature: Setting a New Homepage
In order to create a unique homepage experience
An authenticated user with the proper role
Should be able to change the default front page

# ACCESSING THE HOME PAGE SETTINGS

Scenario Outline: Devs, Admins, SOs and ConMgrs can access Home Page settings; CEs and EMCs cannot
  Given I am logged in as a user with the <role> role
  When I go to "admin/settings/adv-content/frontpage"
  Then I should see <message>

 Examples:
    | role            | message |
    | developer       | "Default front page" |
    | administrator   | "Default front page" |
    | site_owner      | "Default front page" |
    | content_editor  | "Access Denied" |
    | edit_my_content | "Access Denied" |
    | site_editor      | "Access denied" |
    | edit_only        | "Access denied" |
    | access_manager   | "Access denied" |
    | configuration_manager | "Default front page" |
    
    
# SETTING A NEW HOME PAGE
# create a basic page; use it for the new homepage; then change it back
@testing_frontpage
Scenario: A site-owner can create a Basic Page and use it for the new homepage
Given I am logged in as a user with the "site_owner" role
And I am on "node/add/page"
When I fill in "edit-title" with "New Home"
And I fill in "Body" with "Our special new home page"
And I uncheck "edit-menu-enabled"
And I press "Save"
Then the url should match "new-home"
And I go to "admin/settings/adv-content/frontpage"
# VERIFY THAT THE CURRENT HOMEPAGE IS CALLED 'HOME'
Then the "edit-site-frontpage" field should contain "home"
And I fill in "edit-site-frontpage" with "new-home"
When I press "Save"
Then I should see "The configuration options have been saved."
And I go to "/"
Then I should see "Our special new home page"
# CHANGE IT BACK TO CLEAR THE TESTS
And I go to "admin/settings/adv-content/frontpage"
And I fill in "edit-site-frontpage" with "home"
When I press "Save"
Then I should see "The configuration options have been saved."
