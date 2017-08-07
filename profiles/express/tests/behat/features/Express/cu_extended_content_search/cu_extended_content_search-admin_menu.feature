Feature: CU Extended Content Search Menu
  When I log into the website
  As an content editor, site owner, administrator or developer
  I should be able to see the correct menu and shortcuts.

  @api @extended_search
  Scenario: As a developer I should see the complete menu
    Given  CU - I am logged in as a user with the "developer" role
    When I go to "user"
    Then I should see the link "Content"
      And I should see the link "Structure"
      And I should see the link "Appearance"
      And I should see the link "Users"
      And I should see the link "Modules"
      And I should see the link "Configuration"
      And I should see the link "Reports"
      And I should see the link "Settings"
      And I should see the link "Design"
      And I should see the link "Help"

  @api @extended_search
  Scenario Outline: As a site_owner or an administrator I should see a partial menu
    Given  CU - I am logged in as a user with the <role> role
    When I go to "user"
    Then I should see the link "Content"
      And I should see the link "Structure"
      And I should see the link "Users"
      And I should see the link "Settings"
      And I should see the link "Design"
      And I should see the link "Help"

    Examples:
    | role |
    | administrator |
    | site_owner |

  @api @extended_search
  Scenario: As a content_editor I should see a limited menu
    Given  CU - I am logged in as a user with the "content_editor" role
    When I go to "user"
    Then I should see the link "Express"
      And I should see the link "Shortcuts"
      And I should see the link "Help"
      And I should see the link "Log out"

  @api @extended_search
  Scenario Outline: As an authenticated user with a role I should see a partial menu
    Given  CU - I am logged in as a user with the <role> role
    When I go to "user"
    Then I should see the link "Add content"
    And I should see the link "Find content"
     ## Add this back in when FIT-1231 is fixed
     #And I should see the link "Blocks"
    And I should see the link "Edit shortcuts"

    Examples:
      | role |
      | developer |
      | administrator |
      | site_owner |
      | content_editor |

  @api @extended_search
  Scenario: As a developer I should see extra links in the shortcuts menu
    Given  CU - I am logged in as a user with the "developer" role
    When I go to "user"
    Then I should see the link "Theme"

