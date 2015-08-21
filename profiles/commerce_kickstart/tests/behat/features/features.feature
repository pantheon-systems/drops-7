Feature: Features should be in default state
  In order for the site to be reliable when installing
  As an administrator
  The features should be in default state without conflicts

  @api
  Scenario: Check the features
    When I am logged in as a user with the "administrator" role
    And I go to "/admin/structure/features"
    Then I should not see "Conflicts with"

  @api
  Scenario: I can add links to Menu User Menu without overriding feature
    When I am logged in as a user with the "administrator" role
      And I go to "admin/structure/menu/manage/menu-user-menu/add"
    Then I fill in the following:
      | Menu link title   | Testing override        |
      | Path              | https://www.drupal.org  |
      | Weight            | -10                     |
      And I press "Save"
    When I am on "/admin/structure/features/commerce_kickstart_user"
      Then I should not see "Overridden"
