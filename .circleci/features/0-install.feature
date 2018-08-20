Feature: Installer
  In order to know that we can install the site via drush
  As a website user
  I need to be able to install a Drupal site

  Scenario: Installer is ready
    Given I have wiped the site
    And I have reinstalled "CI Drops-7 [{site-name}.{env}]"
    And I visit "/"
    Then I should see "Welcome to CI Drops-7"
