Feature: Facebook Activity Access Feature
  Test access to creation of social link beans.

  @api @social_media
  Scenario Outline: Certain user roles should be able to create Create Facebook Activity block content.
  Given  CU - I am logged in as a user with the <role> role
  And I am on "block/add/facebook-activity"
  Then I should see <message>

  Examples:
  | role            | message                           |
  | content_editor  | "Create Facebook Activity block"  |
  | site_owner      | "Create Facebook Activity block"  |
  | administrator   | "Create Facebook Activity block"  |
  | developer       | "Create Facebook Activity block"  |
  | edit_my_content | "Access Denied"                   |

  @api @social_media
  Scenario: An anonymous user shouldn't be able to create Create Facebook Activity block content.
  Given I am on "block/add/facebook-activity"
  Then I should see "Access Denied"

  @api @social_media
  Scenario Outline: Users should be able to view Facebook Activity block content.
  Given  CU - I am logged in as a user with the <role> role
  When I create a "facebook_activity" block with the label "Facebook Activity Block"
  Then I should see <message>

  Examples:
  | role            | message                    |
  | edit_my_content | "Access Denied"            |
  | content_editor  | "Facebook Activity Block"  |
  | site_owner      | "Facebook Activity Block"  |
  | administrator   | "Facebook Activity Block"  |
  | developer       | "Facebook Activity Block"  |

  @api @social_media
  Scenario: Anonymous users shouldn't be able to view Facebook Activity block content.
  Given I am an anonymous user
  When I create a "facebook_activity" block with the label "Facebook Activity Block"
  Then I should see "Access Denied"
