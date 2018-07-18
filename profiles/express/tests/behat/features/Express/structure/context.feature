@structure
Feature: Context
  In order to exercise control over my regions, pages and blocks
  As an authenticated user
  I should be able to set conditions and reactions with Context

  Scenario Outline: An authenticated user should be able to access the form for adding a Context
    Given I am logged in as a user with the <role> role
    When I go to "admin/structure/context"
    Then I should see <message>

    Examples:
      | role                  | message                                              |
      | edit_my_content       | "Access denied"                                      |
      | content_editor        | "Context allows you to manage contextual conditions" |
      | site_owner            | "Context allows you to manage contextual conditions" |
      | administrator         | "Context allows you to manage contextual conditions" |
      | developer             | "Context allows you to manage contextual conditions" |
      | configuration_manager | "Access denied"                                      |
      | site_editor           | "Context allows you to manage contextual conditions" |
      | edit_only             | "Access denied"                                      |
      | access_manager        | "Access denied"                                      |

  Scenario Outline: Available Contexts are limited for all but Devs
    Given I am logged in as a user with the <role> role
    When I go to "admin/structure/context"
    Then I should see "homepage"
    And I should see "sitewide"
    And I should see "sitewide-except-homepage"
    And I should not see "express_layout_blocks"
    And I should not see "pc2tr40fz12bx"
    And I should not see "search_results"

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | site_editor    |

  @context @contextconditions
  Scenario: Context Conditions drop-down should be properly populated
    Given  I am logged in as a user with the "site_owner" role
    And am on "admin/structure/context/add"
    When I select "Context (any)" from "edit-conditions-selector"
    And I select "Context (all)" from "edit-conditions-selector"
    And I select "Default context" from "edit-conditions-selector"
    And I select "Layout" from "edit-conditions-selector"
    And I select "Menu" from "edit-conditions-selector"
    And I select "Node type" from "edit-conditions-selector"
    And I select "Taxonomy" from "edit-conditions-selector"
    And I select "Path" from "edit-conditions-selector"
    And I select "Query string" from "edit-conditions-selector"
    And I select "Sitewide context" from "edit-conditions-selector"
    And I select "Sitewide public" from "edit-conditions-selector"

  @context @contextreactions
  Scenario: Context Reactions drop-down should be properly populated
    Given  I am logged in as a user with the "site_owner" role
    And am on "admin/structure/context/add"
    When I select "Backstretch" from "edit-reactions-selector"
    And I select "Blocks" from "edit-reactions-selector"
    And I select "Breadcrumb" from "edit-reactions-selector"
    And I select "Columns Override" from "edit-reactions-selector"
    And I select "Menu" from "edit-reactions-selector"
    And I select "Regions" from "edit-reactions-selector"
    And I select "Page Title Image" from "edit-reactions-selector"

