Feature: When I login to Express, I am redirected to my dashboard that correctly includes all components on the User tab.

  @api @dashboards @879
  Scenario: A Site Owner should see Who's Online, System Status, and username blocks.
    Given  CU - I am logged in as a user with the "site_owner" role
    Then I should see the text "Dashboard"
      And I should see the text "Who's online"
      And I should see the text "System Status"
