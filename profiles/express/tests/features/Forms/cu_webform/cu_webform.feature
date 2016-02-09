Feature: Webform feature

  @api @webform
  Scenario: The provide menu link box should be checked on node creation but remain unchecked if user chooses to uncheck that box.
    Given I am logged in as a user with the "site_owner" role
    When I go to "node/add/webform"
    And  I fill in "edit-title" with "New Webform"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck the box "edit-menu-enabled"
    And I press the "Save" button
    And I click "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked

  @api @webform @feedback @clean_install
  Scenario: A site owner should see a webform in the feedback form list of one exists
    Given I am logged in as a user with the "site_owner" role
    And am on "node/add/webform"
    And fill in "Title" with "Contact Form"
    When I press the "Save" button
    And I go to "admin/settings/site-configuration/feedback"
    Then I should not see "There are no published webforms available"

  @api @webform @feedback @clean_install
  Scenario: A site_owner should see no webforms after install on feedback form settings page
    Given I am logged in as a user with the "site_owner" role
    And am on "admin/settings/site-configuration/feedback"
    Then I should see "There are no published webforms available"

  @api @webform
  Scenario Outline: An site owner/administrator/developer should be able to access the settings feedback page
    Given I am logged in as a user with the <role> role
    When I go to "admin/settings/site-configuration/feedback"
    Then I should not see <message>

    Examples:
      | role           | message         |
      | site_owner     | "Access denied" |
      | administrator  | "Access denied" |
      | developer      | "Access denied" |
