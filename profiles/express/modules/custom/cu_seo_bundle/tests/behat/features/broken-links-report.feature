@cu_seo_bundle

Feature: Broken Links Report
    Scenario Outline: An authenticated user with a role should be able to access the Broken links report page
        Given I am logged in as a user with the <role> role
        When I go to "admin/reports/linkchecker"
        Then I should see <message>

        Examples:
            | role            | message        |
            | edit_my_content | "Broken links" |
            | edit_only       | "Broken links" |
            | content_editor  | "Broken links" |
            | site_editor     | "Broken links" |
            | site_owner      | "Broken links" |
            | administrator   | "Broken links" |
            | developer       | "Broken links" |

    Scenario: An anonymous user should not be able to access the Broken links report page
        When I am on "admin/reports/linkchecker"
        Then I should see "Access denied"