Feature: Manage Users
  To manage user accounts
  As an administrator user
  I should have an improved interface

  Background:
    Given I am logged in as a user with the "administrator" role
    Given users:
      | name     | mail            | status | roles               |
      | Joe User | joe@example.com | 1      | administrator       |
      | Jane User| jane@example.com| 1      | authenticated user  |
      | Inactive | nope@example.com| 0      | authenticated user  |

  @api
  Scenario: I can search by email
    When I am on "/admin/people"
      Then I fill in "E-mail" with "joe@example.com"
      And I press "Apply"
    Then I should see the link "Joe User"
      But I should not see the link "Jane User"

  @api
  Scenario: I can search by username
    When I am on "/admin/people"
    Then I fill in "Username" with "Jane User"
      And I press "Apply"
    Then I should see the link "Jane User"
      But I should not see the link "Joe User"

  @api
  Scenario: I can search by status
    When I am on "/admin/people"
    # Setting status 0 for "Given users" isn't actually marking blocked?
      And I click "edit" in the "Inactive" row
      And I select the radio button "Blocked" with the id "edit-status-0"
      And I press "Save"
    Then I select "No" from "Active"
      And I press "Apply"
    Then I should see the link "Inactive"
      But I should not see the link "Joe User"

  @api
  Scenario: I can search by role
    When I am on "/admin/people"
    Then I select "administrator" from "Role"
      And I press "Apply"
    Then I should see the link "Joe User"
      But I should not see the link "Jane User"
