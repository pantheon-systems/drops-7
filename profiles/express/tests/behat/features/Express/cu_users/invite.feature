@users @invite
Feature: User External Invite
Given I am an administrator
When I login to a Web Express site
I should be able to invite users to my site and manage invitations.

  Scenario Outline: Express roles have correct permissions to access user creation and invite pages.
    Given I am logged in as a user with the <role> role
    When I go to "admin/people"
    Then I should see <message>
    When I go to "admin/people/invite"
    Then I should see <message1>
    When I go to "admin/people/invite/operations"
    Then I should see <message2>
    When I go to "admin/people/create"
    Then I should see <message3>
    When I go to "admin/config/people/invite"
    Then I should see <message4>


    Examples:
      | role                  | message         | message1          | message2                | message3                                                    | message4                           |
      | developer             | "Users"         | "Invite New User" | "No invites available." | "This web page allows administrators to register new users" | "Number of days invites are valid" |
      | administrator         | "Users"         | "Invite New User" | "No invites available." | "Access denied"                                             | "Access denied"                    |
      | site_owner            | "Users"         | "Invite New User" | "No invites available." | "Access denied"                                             | "Access denied"                    |
      | content_editor        | "Access denied" | "Access denied"   | "Access denied"         | "Access denied"                                             | "Access denied"                    |
      | edit_my_content       | "Access denied" | "Access denied"   | "Access denied"         | "Access denied"                                             | "Access denied"                    |
      | site_editor           | "Access denied" | "Access denied"   | "Access denied"         | "Access denied"                                             | "Access denied"                    |
      | edit_only             | "Access denied" | "Access denied"   | "Access denied"         | "Access denied"                                             | "Access denied"                    |
      | access_manager        | "Users"         | "Invite New User" | "No invites available." | "Access denied"                                             | "Access denied"                    |
      | configuration_manager | "Users"         | "Access denied"   | "Access denied"         | "Access denied"                                             | "Access denied"                    |

  Scenario: An anonymous user should not be able to access user creation and invite pages.
    When I am on "admin/people"
    Then I should see "Access denied"
    When I am on "admin/people/invite"
    Then I should see "Access denied"
    When I am on "admin/people/invite/operations"
    Then I should see "Access denied"
    When I am on "admin/people/create"
    Then I should see "Access denied"
    When I go to "admin/config/people/invite"
    Then I should see "Access denied"

  Scenario: Functionality - Users landing page is properly populated with fields for finding and sorting users
    Given I am logged in as a user with the "site_owner" role
    When I go to "admin/people"
    Then I should see "Name"
    And I should see an "#edit-combine" element
    And I should see "Role"
    And I should see an "#edit-rid-op" element
    And I should see "Active"
    And I should see an "#edit-status" element
    And I should see an "#edit-submit-cu-people-administration-override-view" element
    And I should see a "#edit-reset" element
    # And I should see the link "sort by Username"
    And I should see the link "sort by Active"
    # HIDING FOR NOW And I should see the link "sort by Primary Affiliation"
    And I should see the link "sort by Member for"
    And I should see the link "sort by Last access"

  Scenario: Functionality - Invite page has input fields and roles for sending invites.
    Given I am logged in as a user with the "site_owner" role
    When I go to "admin/people/invite"
    Then I should see "Core Role"
    And I should see "Add-on Roles"
    And I should see "Email addresses"
    And I should see "Custom message"
    # Look for role names.
    And I should see "Content Editor"
    And I should see "Site Editor"
    And I should see "Site Owner"
    And I should see "Access Manager"
    And I should see "Configuration Manager"

  Scenario: Functionality - Sending an invitation
    Given I am logged in as a user with the "site_owner" role
    When I go to "admin/people/invite"
    And I fill in "edit-email" with "newname@example.com"
    And I check the "Content Editor" radio button
    And I press "edit-submit"
    Then I should see "Successfully invited new user!"

  Scenario: Functionality - Cancelling an invitation
    Given I am logged in as a user with the "site_owner" role
    When I go to "admin/people/invite/operations"
    Then I should see "newname@example.com"
    When I check "edit-table-0"
    And I press "edit-submit"
    Then I should see "Deleted 1 user invite."

  Scenario: The Access Manager and Configuration Manager Add-on roles can only be granted when inviting a Site Editor.
    Given I am logged in as a user with the "site_owner" role
    When I go to "admin/people/invite"
    And I check the "Content Editor" radio button
    And I check "Access Manager"
    And I fill in "Email addresses" with "ex@ample.com"
    And I fill in "Custom message" with "Howdy!"
    And I press "Send Invites"
    Then I should see "Successfully invited new user!"
