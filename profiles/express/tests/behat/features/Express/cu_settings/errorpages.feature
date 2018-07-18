@settings
Feature: Error Pages
  In order to improve a reader's experience when Web Express pages go missing
  An authenticated user with the proper role
  Should be able to set unique 404 and 403 pages

  # ACCESSING THE ERROR PAGES
  Scenario Outline: Devs, Admins, SOs and ConMgrs can access Error Pages; CEs and EMCs cannot
    Given I am logged in as a user with the <role> role
    When I go to "admin/settings/adv-content/error"
    Then I should see <message>

    Examples:
      | role                  | message                                             |
      | developer             | "Allows you to set the default \"Not Found\" page." |
      | administrator         | "Allows you to set the default \"Not Found\" page." |
      | site_owner            | "Allows you to set the default \"Not Found\" page." |
      | content_editor        | "Access Denied"                                     |
      | edit_my_content       | "Access Denied"                                     |
      | site_editor           | "Access denied"                                     |
      | edit_only             | "Access denied"                                     |
      | access_manager        | "Access denied"                                     |
      | configuration_manager | "Allows you to set the default \"Not Found\" page." |

  # SETTING THE 404 PAGE
  # create a basic page; use it for 404 page
  Scenario: A site-owner can create a Basic Page and use it for the 404 page
    Given I am logged in as a user with the "site_owner" role
    And I am on "node/add/page"
    When fill in "Title" with "404 Page"
    And fill in "Body" with "The requested page is not available."
    And I uncheck "Provide a menu link"
    And I press "Save"
    Then the url should match "404-page"
    Then I go to "admin/settings/adv-content/error"
    And fill in "Default 404 page" with "404-page"
    When I press "Submit"
    Then I should see "The configuration options have been saved."
    And I go to "missing-page-test"
    Then I should see "404 Page"


  # SETTING THE 403 PAGE
  # Create a basic page; use it for 403 page.
  Scenario: A site-owner can create a Basic Page and use it for the 403 page
    Given I am logged in as a user with the "site_owner" role
    And I am on "node/add/page"
    When fill in "edit-title" with "Secret Page"
    And fill in "Body" with "You do not have permission to access this page"
    And I uncheck "edit-menu-enabled"
    And I press "Save"
    Then the url should match "secret-page"
    Then I go to "admin/settings/adv-content/error"
    And fill in "edit-site-403" with "secret-page"
    When I press "Submit"
    Then I should see "The configuration options have been saved"
    And I go to "admin/config/development/maintenance"
    Then I should see "Secret Page"
    # NEXT WE TURN THAT OFF SO FURTHER TESTS RETURN THE DEFAULT 403 PAGE.
    Then I go to "admin/settings/adv-content/error"
    And fill in "edit-site-403" with ""
    And I press "Submit"
    Then I should see "The configuration options have been saved"
