@settings
Feature: Article Settings allow the user to hide/show an article's published date
In order to show or hide an article's published date
An authenticated user with the proper role
Should be able to set Article Settings options

# ARTICLE SETTINGS PERMS ARE PART OF 'ADMINISTER EXPRESS SETTINGS' - SITE EDITORS DON'T GET TO DO THIS.

Scenario Outline: A user with the proper role can access the Article Settings options
 Given I am logged in as a user with the <role> role
 When I go to "admin/settings/news/article-settings"
 Then I should see <message>
    
Examples:
    | role            | message                          |
    | developer       | "Article Published Date Display" |
    | administrator   | "Article Published Date Display" |
    | site_owner      | "Article Published Date Display" |
    | content_editor  | "Access denied" |
    | edit_my_content | "Access denied" |
    | site_editor      | "Access denied" |
    | edit_only        | "Access denied" |
    | access_manager   | "Access denied" |
    | configuration_manager | "Article Published Date Display" |
    

Scenario: An anonymous user cannot access the Article Settings options
  When I am on "admin/settings/news/article-settings"
  Then I should see "Access denied"
  
#CHANGING THE SETTINGS HIDES THE PUBLISHED DATE ON ARTICLE

Scenario: Changing the Article Settings does hide the publish date on article
 Given I am logged in as a user with the "site_owner" role
 And am on "admin/settings/news/article-settings"
 When I select "hide" from "date_display"
 And I press "edit-submit"
 Then I should see "Article settings have been saved."
 And I go to "node/add/article"
 And I fill in "edit-title" with "A New Article"
 And I fill in "Body" with "Here is more information."
 And I press "Save"
 Then I should see "A New Article"
 And I should not see a ".author-meta-date" element    
