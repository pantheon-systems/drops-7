Feature: Photo Gallery Access
  Test access to creation of content, configuration of settings, and viewing of content.

  @api @max_execution_time
  Scenario Outline: Certain user roles should be able to create Photo Gallery content.
    Given  CU - I am logged in as a user with the <role> role
      And I am on "node/add/photo-gallery"
    Then I should see <message>

    Examples:
    | role            | message                 |
    | content_editor  | "Create Photo Gallery"  |
    | site_owner      | "Create Photo Gallery"  |
    | administrator   | "Create Photo Gallery"  |
    | developer       | "Create Photo Gallery"  |
    | edit_my_content | "Access Denied"         |

  @api
  Scenario: An anonymous user shouldn't be able to create Photo Gallery content.
    Given I am on "node/add/photo-gallery"
    Then I should see "Access Denied"

  @api
  Scenario Outline: All users should be able to view a photo gallery node.
    Given  CU - I am logged in as a user with the <role> role
      And I create a "photo_gallery" node with the title "New Gallery"
    Then I should see <message>

    Examples:
      | role            | message        |
      | content_editor  | "New Gallery"  |
      | site_owner      | "New Gallery"  |
      | administrator   | "New Gallery"  |
      | developer       | "New Gallery"  |
      | edit_my_content | "New Gallery"  |

  @api @node_creation
  Scenario: An anonymous user should be able to view Photo Gallery content.
    Given I am an anonymous user
      And I create a "photo_gallery" node with the title "New Gallery"
    Then I should see "New Gallery"
