@settings @redirects
Feature: URL redirects
  In order to create vanity URLs or fix broken pages
  An authenticated user with the proper role
  Should be able to create a URL redirect

  Scenario Outline: Devs, Admins, SOs and ConMgrs can Create URL Redirects
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/redirect"
    Then I should see <message>

    Examples:
      | role                  | message         |
      | developer             | "Redirects"     |
      | administrator         | "Redirects"     |
      | site_owner            | "Redirects"    |
      | content_editor        | "Access denied" |
      | edit_my_content       | "Access denied" |
      | site_editor           | "Redirects"     |
      | edit_only             | "Access denied" |
      | access_manager        | "Access denied" |
      | configuration_manager | "Redirects"     |

  Scenario: the URL redirects form is properly populated with functionality
    Given I am logged in as a user with the "site_owner" role
    And am on "admin/config/search/redirect"
    Then I should see "List"
    And I should see "Delete redirects"
    And I should see the link "Add redirect"
    And I should see "Filter redirects"
    And I should see the link "sort by From"
    And I should see the link "sort by To"
    And I should see the link "sort by Status"
    And I should see the link "sort by Type"
    And I should see the link "sort by Count"
    And I should see the link "sort by Last accessed"


  Scenario Outline: Devs, Admins, SOs and ConMgrs can create a URL redirect
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/redirect/add"
    Then I should see <message>

    Examples:
      | role                  | message                                       |
      | developer             | "Enter an internal Drupal path or path alias" |
      | administrator         | "Enter an internal Drupal path or path alias" |
      | site_owner            | "Enter an internal Drupal path or path alias" |
      | content_editor        | "Access denied"                               |
      | edit_my_content       | "Access denied"                               |
      | site_editor           | "Enter an internal Drupal path or path alias" |
      | edit_only             | "Access denied"                               |
      | access_manager        | "Access denied"                               |
      | configuration_manager | "Enter an internal Drupal path or path alias" |


  Scenario: the Add URL Redirect page is properly populated with functionality
    Given I am logged in as a user with the "site_owner" role
    When I go to "admin/config/search/redirect/add"
    Then I should see "From"
    And I should see "To"
    And the "edit-status" checkbox should be checked
    And I should see "Advanced options"


  Scenario Outline: Devs, Admins, SOs and ConMgrs can delete a URL redirect
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/redirect/delete"
    Then I should see <message>

    Examples:
      | role                  | message                                                         |
      | developer             | "Your site may contain redirects that have never been accessed" |
      | administrator         | "Your site may contain redirects that have never been accessed" |
      | site_owner            | "Your site may contain redirects that have never been accessed" |
      | content_editor        | "Access denied"                                                 |
      | edit_my_content       | "Access denied"                                                 |
      | site_editor           | "Your site may contain redirects that have never been accessed" |
      | edit_only             | "Access denied"                                                 |
      | access_manager        | "Access denied"                                                 |
      | configuration_manager | "Your site may contain redirects that have never been accessed" |


  Scenario Outline: Only Developers can access the URL Redirect Settings page
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/redirect/settings"
    Then I should see <message>

    Examples:
      | role                  | message                                            |
      | developer             | "Delete redirects that have not been accessed for" |
      | administrator         | "Access denied"                                    |
      | site_owner            | "Access denied"                                    |
      | content_editor        | "Access denied"                                    |
      | edit_my_content       | "Access denied"                                    |
      | site_editor           | "Redirects"                                        |
      | edit_only             | "Access denied"                                    |
      | access_manager        | "Access denied"                                    |
      | configuration_manager | "Access denied"                                    |

  Scenario: An anonymous user can not create, add or delete a URL redirect
    When I am on "admin/config/search/redirect"
    Then I should see "Access denied"
    And I go to "admin/config/search/redirect/add"
    Then I should see "Access denied"
    And I go to "admin/config/search/redirect/delete"
    Then I should see "Access denied"
    And I go to "admin/config/search/redirect/settings"
    Then I should see "Access denied"

  Scenario: A site owner cannot enter an absolute path on a node form for the alias field.
    Given I am logged in as a user with the "site_owner" role
    When I go to "node/add/page"
    And I fill in "Title" with "Path Alias Page"
    And I uncheck "Generate automatic URL alias"
    And I fill in "URL alias" with "https://www.colorado.edu/mysite/important"
    And I press "Save"
    Then I should see "Enter a custom path by which this content can be accessed. Do not enter the full url."

  Scenario Outline: Only a developer should be able to add math redirects.
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/match_redirect/add"
    Then I should see <message>

    Examples:
      | role                  | message                                                                                                    |
      | developer             | "By default if a pattern matches and there is content for that target url then it will not be redirected." |
      | administrator         | "By default if a pattern matches and there is content for that target url then it will not be redirected." |
      | site_owner            | "Access denied"                                                                                            |
      | content_editor        | "Access denied"                                                                                            |
      | edit_my_content       | "Access denied"                                                                                            |
      | site_editor           | "Access denied"                                                                                            |
      | edit_only             | "Access denied"                                                                                            |
      | access_manager        | "Access denied"                                                                                            |
      | configuration_manager | "Access denied"                                                                                            |

  Scenario: A developer should be able to add and edit match redirects.
    Given I am logged in as a user with the "developer" role
    When I go to "node/add/page"
    And I fill in "Title" with "Fruity Tooty"
    And I fill in "Body" with "Fruity Tooty Body text."
    And I press "Save"
    Then I should see "Basic page Fruity Tooty has been created."
    When I go to "node/add/page"
    And I fill in "Title" with "Veggie Tooty"
    And I fill in "Body" with "Veggie Tooty Body text."
    And I press "Save"
    Then I should see "Basic page Veggie Tooty has been created."
    When I go to "admin/config/search/match_redirect/add"
    Then I should see "By default if a pattern matches and there is content for that target url then it will not be redirected."
    When I fill in "Pattern" with "fruity*"
    And I fill in "Target" with "veggie-tooty"
    And I select "301" from "status_code"
    And I press "Save"
    Then I should see "Redirect saved."
    When I go to "fruity-tooty"
    # User sees page since "Allow content to be redirected?" isn't checked.
    Then I should see "Fruity Tooty Body text."
    When I go to "admin/config/search/redirect/wildcards"
    And I follow "Edit"
    Then I should see "Edit match redirect"
    When I check "Allow content to be redirected?"
    And I press "Save"
    Then I should see "Redirect saved."
    And I should see "Edit"
    And I should see "Delete"
    # User gets redirected even though content is there.
    When I go to "fruity-tooty"
    Then I should see "Veggie Tooty Body text."

  Scenario Outline: Less privileged users should not be able to edit match redirects.
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/match_redirect/add"
    Then I should see <message>

    Examples:
      | role                  | message         |
      | developer             | "Edit"          |
      | administrator         | "Edit"          |
      | site_owner            | "Access denied" |
      | content_editor        | "Access denied" |
      | edit_my_content       | "Access denied" |
      | site_editor           | "Access denied" |
      | edit_only             | "Access denied" |
      | access_manager        | "Access denied" |
      | configuration_manager | "Access denied" |
