# WHEN I LOGON I SHOULD BE REDIRECTED TO THE DASHBOARD

@dashboard
Feature: The Web Express Dashboard
When I login to a Web Express website
As an authenticated user
I am redirected to my dashboard

  Scenario Outline: An authenticated user should see Who's Online, System Status, and username blocks.
    Given  I am logged in as a user with the <role> role
    Then I should be on "admin/dashboard/user"
    Then I should see "Dashboard"
    And I should see "Who's online"
    And I should see "User since:"
    And I should see "Roles:"
    And I should see "System Status"
    And I should see a ".status-message" element

    Examples:
      | role            |
      | developer       |
      | administrator   |
      | site_owner      |
      | content_editor  |
      | edit_my_content |
