Feature: Twitter Feed Access Feature
  Test access to creation of content, configuration of settings, and viewing of content.

  @api @social_media
  Scenario Outline: Certain user roles should be able to create Create Twitter Block block content.
  Given I am logged in as a user with the <role> role
  And I am on "block/add/twitter-block"
  Then I should see <message>

  Examples:
  | role            | message                       |
  | content_editor  | "Create Twitter Block block"  |
  | site_owner      | "Create Twitter Block block"  |
  | administrator   | "Create Twitter Block block"  |
  | developer       | "Create Twitter Block block"  |
  | edit_my_content | "Access Denied"               |

  @api @social_media
  Scenario: An anonymous user shouldn't be able to create Create Twitter Block block content.
  Given I am on "block/add/twitter-block"
  Then I should see "Access Denied"

  @api @social_media
  Scenario Outline: Users should be able to view Twitter Block block content.
  Given I am logged in as a user with the <role> role
  When I create a "twitter_block" block with the label "Twitter Block"
  Then I should see <message>

  Examples:
  | role            | message          |
  | content_editor  | "Twitter Block"  |
  | site_owner      | "Twitter Block"  |
  | administrator   | "Twitter Block"  |
  | developer       | "Twitter Block"  |
  | edit_my_content | "Access Denied"  |

  @api @social_media
  Scenario: Anonymous users shouldn't be able to view Twitter Block block content.
  Given I am an anonymous user
  When I create a "twitter_block" block with the label "Twitter Block"
  Then I should see "Access Denied"
