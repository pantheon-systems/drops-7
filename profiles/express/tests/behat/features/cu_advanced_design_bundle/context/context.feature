Feature: Context

@context @contextconditions @exclude_all_bundles
Scenario Outline: A content_editor should see a limited number of context conditions
  Given  I am logged in as a user with the "content_editor" role
    And am on "admin/structure/context/add"
  When I select <condition> from "edit-conditions-selector"

    Examples:
      | condition |
      | "Context (any)" |
      | "Context (all)" |
      | "Default context" |
      | "Layout" |
      | "Menu" |
      | "Node type" |
      | "Taxonomy" |
      | "Path" |
      | "Query string" |
      | "Sitewide context" |
      | "Sitewide public" |

@context @contextreactions @exclude_all_bundles
Scenario Outline: A content_editor should see a limited number of context reactions
  Given  I am logged in as a user with the "content_editor" role
    And am on "admin/structure/context/add"
  Then I select <reaction> from "edit-reactions-selector"

    Examples:
      | reaction |
      | "Backstretch" |
      | "Blocks" |
      | "Breadcrumb" |
      | "Columns Override" |
      | "Menu" |
      | "Regions" |
      | "Page Title Image" |
