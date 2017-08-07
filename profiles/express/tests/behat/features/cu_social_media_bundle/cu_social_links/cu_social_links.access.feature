Feature: Social Links Access Feature
  Test access to creation of social link beans.

  @api @social_media
  Scenario Outline: Certain user roles should be able to create Create Social Links block content.
  Given  CU - I am logged in as a user with the <role> role
  And I am on "block/add/social-links"
  Then I should see <message>

  Examples:
  | role            | message                      |
  | content_editor  | "Create Social Links block"  |
  | site_owner      | "Create Social Links block"  |
  | administrator   | "Create Social Links block"  |
  | developer       | "Create Social Links block"  |
  | edit_my_content | "Access Denied"              |

  @api @social_media
  Scenario: An anonymous user shouldn't be able to create Create Social Links block content.
  Given I am on "block/add/social-links"
  Then I should see "Access Denied"

  @api @social_media @max_execution_time
  Scenario Outline: Users should be able to view Social Links block content.
    Given  CU - I am logged in as a user with the <role> role
    When I create a "social_links" block with the label "Social Links Block"
    Then I should see <message>

    Examples:
      | role            | message               |
      | content_editor  | "Social Links Block"  |
      | site_owner      | "Social Links Block"  |
      | administrator   | "Social Links Block"  |
      | developer       | "Social Links Block"  |
      | edit_my_content | "Access Denied"       |

  @api @social_media
  Scenario: Anonymous users shouldn't be able to view Social Links block content.
    Given I am an anonymous user
    When I create a "social_links" block with the label "Social Links Block"
    Then I should see "Access Denied"
