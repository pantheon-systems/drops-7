Feature: Check for settings on the cache configuration page
  In order to know that the site was installed from the pantheon profile
  As a website administrator
  I need to know that Pantheon's initial settings were applied by the installer

  @api
  Scenario: Check to see if the page cache setting is set
    Given I am logged in as a user with the "administrator" role
    And I am on "/admin/config/development/performance"
    Then the "900" option from "page_cache_maximum_age" should be selected
