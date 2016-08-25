Feature: Events Bundle Access Feature
  Test access to creation of content, configuration of settings, and viewing of content.

  @api @events
  Scenario Outline: Certain user roles should be able to create Event Calendar Block content.
    Given I am logged in as a user with the <role> role
      And I am on "block/add/cu-events-calendar-block"
    Then I should see <message>

    Examples:
      | role            | message                               |
      | content_editor  | "Create Events Calendar Block block"  |
      | site_owner      | "Create Events Calendar Block block"  |
      | administrator   | "Create Events Calendar Block block"  |
      | developer       | "Create Events Calendar Block block"  |
      | edit_my_content | "Access Denied"                       |

  @api @events
  Scenario: An anonymous user shouldn't be able to create Event Calendar Block content.
    Given I am on "block/add/cu-events-calendar-block"
    Then I should see "Access Denied"

  @api @events @node_creation @max_execution_time
  Scenario Outline: All users should be able to view a Event Calendar Block.
    Given I am logged in as a user with the <role> role
      And I create a "cu_events_calendar_block" block with the label "New Event Calendar Block"
    Then I should see <message>

    Examples:
      | role            | message                     |
      | content_editor  | "New Event Calendar Block"  |
      | site_owner      | "New Event Calendar Block"  |
      | administrator   | "New Event Calendar Block"  |
      | developer       | "New Event Calendar Block"  |
      | edit_my_content | "Access Denied"             |

  @api @events @node_creation
  Scenario: An anonymous user should be able to view Event Calendar Block content.
    Given I am an anonymous user
      And I create a "cu_events_calendar_block" block with the label "New Event Calendar Block"
    Then I should see "Access Denied"

  @api @events
  Scenario Outline: Certain user roles should be able to create Event Calendar Block content.
    Given I am logged in as a user with the <role> role
    And I am on "block/add/events-calendar-grid"
    Then I should see <message>

    Examples:
      | role            | message                              |
      | content_editor  | "Create Events Calendar Grid block"  |
      | site_owner      | "Create Events Calendar Grid block"  |
      | administrator   | "Create Events Calendar Grid block"  |
      | developer       | "Create Events Calendar Grid block"  |
      | edit_my_content | "Access Denied"                      |

  @api @events
  Scenario: An anonymous user shouldn't be able to create Event Calendar Block content.
    Given I am on "block/add/events-calendar-grid"
    Then I should see "Access Denied"

  @api @events @node_creation @max_execution_time
  Scenario Outline: All users should be able to view a Event Calendar Block.
    Given I am logged in as a user with the <role> role
    And I create a "events_calendar_grid" block with the label "New Event Calendar Grid"
    Then I should see <message>

    Examples:
      | role            | message                    |
      | content_editor  | "New Event Calendar Grid"  |
      | site_owner      | "New Event Calendar Grid"  |
      | administrator   | "New Event Calendar Grid"  |
      | developer       | "New Event Calendar Grid"  |
      | edit_my_content | "Access Denied"            |

  @api @events @node_creation
  Scenario: An anonymous user should be able to view Event Calendar Block content.
    Given I am an anonymous user
    And I create a "events_calendar_grid" block with the label "New Event Calendar Grid"
    Then I should see "Access Denied"
