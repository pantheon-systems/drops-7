@settings
Feature: RSS Feeds
In order to allow other sites to display the site articles
An authenticated user with the proper role
Should be able to create an RSS feed of the site articles

#CREATE AN ARTICLE TO BE ON SAFE SIDE OF RSS BUILDER

Scenario: Create article node in case RSS Builder decides it needs one
  Given I am logged in as a user with the "site_owner" role
  And am on "node/add/article"
  When I fill in "Title" with "Read About Ducks"
  And I fill in "Body" with "Demo body content"
  And I press "Save"
  Then I should see "Read About Ducks"

#SOME ROLES CAN ACCESS RSS FEED SETTINGS

Scenario Outline: A user with proper role can view the RSS feeds
  Given I am logged in as a user with the <role> role
  And am on "admin/settings/feeds/rss/overview"
  Then I should see "Manage RSS Feeds"
  And I should see the link "Add a RSS Feed"
  
Examples:
    | role            | 
    | developer       | 
    | administrator   | 
    | site_owner      | 
    | content_editor  | 
    | site_editor      | 
#   | configuration_manager | HIDE FOR NOW; RETEST LATER

    
# SOME ROLES CAN NOT ACCESS RSS FEED SETTINGS

Scenario Outline: EMCs, EditOnly and AcsMgrs should not be able to view the RSS feeds
Given I am logged in as a user with the <role> role
When I go to "admin/settings/feeds/rss/overview"
Then I should see "Access denied"

 Examples:
    | role            | 
    | edit_my_content  | 
    | edit_only        | 
    | access_manager   | 
    

Scenario: An anonymous user should not be able to view the RSS feeds
  When I am on "admin/settings/feeds/rss/overview"
  Then I should see "Access denied"
  
#SOME ROLES CAN ACCESS THE RSS FEED BUILDER

Scenario Outline: A user with proper role can access the RSS feed builder page
  Given I am logged in as a user with the <role> role
  When I go to "admin/settings/feeds/rss/add"
 Then I should see "Build custom RSS feeds"
 And I should see "Categories"
 And I should see "Tags"
  
Examples:
    | role            | 
    | developer       | 
    | administrator   | 
    | site_owner      | 
    | content_editor  | 
    | site_editor      | 
#   | configuration_manager | HIDE FOR NOW; RETEST LATER

# SOME ROLES CAN NOT ACCESS THE RSS FEED BUILDER

Scenario Outline: EMCs, EditOnly and AcsMgrs should not be able to access the RSS feed builder page
Given I am logged in as a user with the <role> role
When I go to "admin/settings/feeds/rss/add"
Then I should see "Access denied"

 Examples:
    | role            | 
    | edit_my_content  | 
    | edit_only        | 
    | access_manager   | 
    

Scenario: An anonymous user should not be able to access the RSS feed builder page
  When I am on "admin/settings/feeds/rss/add"
  Then I should see "Access denied"
  
# BUILDING AN RSS FEED
# NOTE THIS PARTICULAR FEED CAN ONLY BE TESTED ONCE; USE OTHER FEEDS TO TEST OTHER ROLES

Scenario: One user can build a feed with the default feed/rss.xml
 Given I am logged in as a user with the "site_owner" role
 When I go to "admin/settings/feeds/rss/add"
 Then I should see "Build custom RSS feeds"
 And I fill in "edit-rss-title" with "Exciting News"
 And I press "edit-submit"
 Then I should see "Exciting News"
 And I should see "feed/rss.xml"
 
