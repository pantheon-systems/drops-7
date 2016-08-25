Feature: Context for the Social Media Bundle

@api @context @contextreactions
Scenario Outline: A content_editor should see a limited number of context reactions
Given I am logged in as a user with the "content_editor" role
  And am on "admin/structure/context/add"
Then I select <reaction> from "edit-reactions-selector"

  Examples:
    | reaction |
    | "Social media share links" |
