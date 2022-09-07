Feature: Check for existance of Pantheon Solr module
  In order to know that the site was installed from Pantheon's upstream
  As a website user
  I need to know that Pantheon's Solr module is available

  @api
  Scenario: Check to see if Pantheon Solr is available
    Given I am logged in as a user with the "administrator" role
    And I am on "/admin/modules"
    Then I should see "Pantheon Apache Solr"
