Feature: Article List Page

  @article-list-page
  Scenario: The provide menu link box should be checked on node creation but remain unchecked if user chooses to uncheck that box.
    Given  I am logged in as a user with the "site_owner" role
    When I go to "node/add/article-list-page"
      And  I fill in "edit-title" with "Article List Page"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck "edit-menu-enabled"
      And I press "Save"
      And I follow "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked
