Feature: FAQs

  @api @faqs
  Scenario: The provide menu link box should be checked on node creation but remain unchecked if user chooses to uncheck that box.
    Given I am logged in as a user with the "site_owner" role
    When I go to "node/add/faqs"
      And  I fill in "edit-title" with "New FAQ"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck the box "edit-menu-enabled"
      And I press the "Save" button
      And I click "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked