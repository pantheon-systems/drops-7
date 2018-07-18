# FOR MORE TESTS OF WEB EXPRESS STRUCTURE:
# SEE ARTICLES FOR TAXONOMY TESTS
# SEE MENUS FOR MENUS TESTS

@structure
Feature: Structure section with links to context, menus, etc
When I go to the Admin/Structure page
As an authenticated user
I should be able to adjust the structure of my site

  Scenario Outline: Some users see four links
    Given I am logged in as a user with the <role> role
    When I go to "admin/structure"
    Then I should see the link "Context"
    And I should see the link "Menus"
    And I should see the link "Taxonomy"

    Examples:
      | role          |
      | administrator |
      | site_owner    |
      | site_editor   |
 # IN FLUX | content_editor  |

  Scenario Outline:  Some users should not be able to access Admin/Structure
    Given I am logged in as a user with the <role> role
    And I am on "admin/structure"
    Then I should see <message>

    Examples:
      | role                  | message                                     |
      | edit_my_content       | "Access denied"                             |
      | configuration_manager | "You do not have any administrative items." |
      | edit_only             | "You do not have any administrative items." |
      | access_manager        | "You do not have any administrative items." |

  Scenario: An anonymous user should not be able to access Admin/Structure
    When I am on "admin/structure"
    Then I should see "Access denied"
