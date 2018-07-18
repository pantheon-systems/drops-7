@settings
Feature: Web Express bundles its features into three types: Core, Add-on and Request
In order to simplify the features of a site
An authenticated user with the proper role
Should see bundles of features in three categories

# NOTE MOST OF THESE TESTS CANNOT BE AUTOMATED BECAUSE THE BUNDLES THAT APPEAR IN PROFILE MODULE MANAGER ARE CONTROLLED BY
# THE ATLAS INSTANCE FOR EACH ENVIRONMENT. THEY ONLY EVER MATCH PROD WHEN ITS ALL BEEN REBUILT AS CLONE OF PROD
# MOST OF THEM HAVE BEEN DISABLED
#NOTE: PAGE ACCESS PERMISSIONS ARE TESTED IN ENABLINGBUNDLES.FEATURE

# CORE BUNDLES
Scenario Outline: Users with the proper role can access the Site Settings page
Given I am logged in as a user with the <role> role
  When I go to "admin/settings/bundles/list"
  Then I should see "Advanced Content"
  And I should see "Advanced Design"
 And I should see "Advanced Layout"
#  THE REST OF THESE CHANGE OVER TIME AND DON'T MATCH PROD
#  And I should see "Feeds"
#  And I should see "Forms"
#  And I should see "News and Articles"
#  And I should see "People"
#  And I should see "Photo Gallery"
#  And I should see "Search Engine Optimization"
#  And I should see "Social Media"
    
Examples:
    | role            | 
    | developer       | 
    | administrator   | 
    | site_owner      | 
    | configuration_manager |
    
    

Scenario Outline: Users with a restricted role cannot access the Site Settings page
Given I am logged in as a user with the <role> role
When I go to "admin/settings/bundles/list"
Then I should see "Access denied"
  
 Examples:
    | role            | 
    | edit_my_content  | 
    | site_editor      | 
    | edit_only        | 
    | access_manager   | 
  

Scenario: An anonymous user should not be able to access the Site Settings page
 When I go to "admin/settings/bundles/list"
 Then I should see "Access denied"

# ADD-ON BUNDLES
# NOTE THIS TEST CANNOT BE AUTOMATED BECAUSE THE BUNDLES THAT APPEAR IN PROFILE MODULE MANAGER ARE CONTROLLED BY
# THE ATLAS INSTANCE FOR EACH ENVIRONMENT. THEY ONLY EVER MATCH PROD WHEN ITS ALL BEEN REBUILT AS CLONE OF PROD

#Scenario Outline: Add-on bundles
#  Given I am logged in as a user with the <role> role
#  When I go to "admin/settings/bundles/list/addon"
#  Then I should see "Add-on"
#  And I should see "These are bundles that can be added at any time"
# Then I should see "Content Sequence"
#   And I should see "Collections"
#   And I should see "Localist Events"
#   And I should see "Forms"
#   And I should see "Publication"
    
#Examples:
#    | role            | 
#    | developer       | 
#    | administrator   | 
#    | site_owner      | 
    

# REQUESTED BUNDLES
# NOTE THIS TEST CANNOT BE AUTOMATED BECAUSE THE BUNDLES THAT APPEAR IN PROFILE MODULE MANAGER ARE CONTROLLED BY
# THE ATLAS INSTANCE FOR EACH ENVIRONMENT. THEY ONLY EVER MATCH PROD WHEN ITS ALL BEEN REBUILT AS CLONE OF PROD

#Scenario Outline: Requested bundles
#  Given I am logged in as a user with the <role> role
#  When I go to "admin/settings/bundles/list/request"
#  Then I should see "Request"
#  And I should see "These are bundles that must be requested"
#  Then I should see "Chemistry Title"
#  And I should see "Live Chat"
#  And I should see "Live Stream"
#  And I should see "Responsive Visibility"
#  And I should see "Video Hero Unit"
#  And I should see "Newsletter"
    
#Examples:
#    | role            | 
#    | developer       | 
#    | administrator   | 
#    | site_owner      | 
