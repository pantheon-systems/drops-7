Feature: Block Row Block

@api @block-row-block
Scenario Outline: An authenticated user should be able to access the form for adding a block row block
    Given  CU - I am logged in as a user with the <role> role
    When I go to "block/add/block-row"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | content_editor | "Access denied" |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

@api @block-row-block
Scenario: An anonymous user should not be able to access the form
  Given I am an anonymous user
  When I go to "block/add/block-row-block"
  Then I should see "Access denied"
