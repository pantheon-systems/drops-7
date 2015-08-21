Feature: Login Commerce Kickstart
  In order to start using additional features of the site
  As an anonymous user
  I should be able to Login

  Scenario: View the Login page
    When I go to "/user/login"
    Then I should see "Login"
      And I should see the following <links>
        | links                    |
        | Forgot your password?    |
        | Create an account        |

  Scenario: Username validation: Invalid username
    When I go to "/user/login"
      And I fill in "Username" with "randomname"
      And I fill in "Password" with "invalidpassword"
      And I press "Log in"
    Then I should see "Sorry, unrecognized username or password."
      And the field "Username" should be outlined in red

  Scenario: User should be able to login and see the user profile
    When I go to "/user/login"
    And I fill in "Username" with "Sample Customer"
    And I fill in "Password" with "customer"
    And I press "Log in"
    Then I should see "HELLO, SAMPLE CUSTOMER"
    Then I should see the following <links>
      | links                   |
      | My account              |
      | Address Book            |
      | Update email/password   |
      | Order history           |
      | Manage shipping address |
      | Manage billing address  |
    And I should see the following <texts>
      | texts      |
      | Account information      |
      | Primary shipping address |
      | Primary billing address  |
      | Email address            |
      | Recent orders            |

  @api
  Scenario: Login and as admin and view user profile
    When I am logged in as a user with the "administrator" role
      And I go to "/user"
    Then I should see the following <links>
      | links                 |
      | My account            |
      | Address Book          |
      | Update email/password |
      | Connections           |
      | Order history         |
      But I should not see an "#breadcrumb" element

  @api @variables
  Scenario: Login with user breadcrumbs enabled
    When I am logged in as a user with the "administrator" role
      And user breadcrumbs are enabled
      And I go to "/user"
    Then I should see an "#breadcrumb" element
