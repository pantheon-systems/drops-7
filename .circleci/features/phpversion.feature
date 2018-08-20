Feature: Check php version
  In order to know that pantheon.upstream.yml is working
  As a website user
  I need to know that I am running the correct version of php

  @api
  Scenario: Check the php version in the phpinfo output
    Given I am logged in as a user with the "administrator" role
    And I am on "/admin/reports/status/php"
    Then I should see "PHP Version 5.6."
