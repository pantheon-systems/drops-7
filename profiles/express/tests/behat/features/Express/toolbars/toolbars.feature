# Checking the Express toolbar and Shortcut toolbar

@toolbars
 Feature: the Express and Shortcut toolbars
  When I log into the website
  As an authenticated user
  I should see the correct toolbar menus and shortcuts.
  
 Scenario Outline: All roles should see the blue and white toolbars
  Given I am logged in as a user with the <role> role
  When I go to "/"
  Then I should see the link "Express"
   And I should see the link "Dashboard"

  Examples:
    | role |
    | developer |
    | administrator |
    | site_owner |
    | content_editor |
    | edit_my_content |
    | site_editor      | 
    | edit_only        | 
    

Scenario Outline: All roles should see a blue toolbar with the same five links
 Given I am logged in as a user with the <role> role
 When I go to "/"
 Then I should see the link "Express"
 And I should see the link "Shortcuts"
 And I should see the link "My account"
 And I should see the link "Help"
 And I should see the link "Log out"

Examples:
     | role |
     | developer |
     | administrator |
     | site_owner |
     | content_editor |
     | edit_my_content |
     | site_editor      | 
     | edit_only        | 

 
# CHECKING THE EXPRESS MENU
  Scenario: As a developer I should see the complete Express menu
    Given I am logged in as a user with the "developer" role
    When I go to "admin"
    Then I should see the link "Dashboard"
      And I should see the link "Content"
      And I should see the link "Structure"
      And I should see the link "Appearance"
      And I should see the link "Users"
      And I should see the link "Modules"
      And I should see the link "Configuration"
      And I should see the link "Reports"
      And I should see the link "Design"
      And I should see the link "Settings"


  Scenario Outline: As an administrator or a site_owner I should see a partial Express menu
    Given I am logged in as a user with the <role> role
    When I go to "admin"
    Then I should see the link "Dashboard"
     And I should see the link "Content"
     And I should see the link "Structure"
     And I should see the link "Users"
     And I should see the link "Design"
     And I should see the link "Settings"

    Examples:
    | role |
    | administrator |
    | site_owner |


  Scenario: As a content_editor I should see a limited Express menu
   Given  I am logged in as a user with the "content_editor" role
   When I go to "admin"
   Then I should see the link "Dashboard"
    And I should see the link "Content"
    And I should see the link "Structure"
    And I should see the link "Settings"
      

  Scenario: As an edit_my_content I should see an extremely limited Express menu
   Given I am logged in as a user with the "edit_my_content" role
   When I go to "admin"
   Then I should see "Access Denied"
   And I should see the link "Dashboard"
   And I should see the link "Content"
    

 Scenario Outline: Most user roles should see the same Shortcuts menu
   Given I am logged in as a user with the <role> role
   When I am on "/"
   And I click the "a" element with "Shortcuts" for "title"
   Then I should see the link "Add content"
   And I should see the link "Find content"
   And I should see the link "Blocks"
   And I should see the link "Context"
   And I should see the link "Main Menu"
   And I should see the link "Edit shortcuts"

    Examples:
      | role |
      | developer |
      | administrator |
      | site_owner |
   #   | content_editor | HIDE TILL PERMS CAN BE WORKED OUT


  Scenario: An edit_my_content user should see a very limited Shortcuts menu
   Given I am logged in as a user with the "edit_my_content" role
   When I am on "/"
   And I click the "a" element with "Shortcuts" for "title"
   Then I should see the link "Find content"
