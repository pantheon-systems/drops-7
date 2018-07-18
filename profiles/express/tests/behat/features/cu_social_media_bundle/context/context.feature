Feature: Context for the Social Media Bundle

@context @contextreactions
Scenario Outline: A content_editor should see a limited number of context reactions
Given  I am logged in as a user with the "content_editor" role
  And I go to "admin/structure/context/add"
When I select <reaction> from "edit-reactions-selector"

  Examples:
    | reaction |
    | "Social media share links" |
