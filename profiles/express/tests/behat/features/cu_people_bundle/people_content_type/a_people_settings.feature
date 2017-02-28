Feature: People Settings

  @api @settings
  Scenario Outline: An site owner/administrator/developer should be able to access the settings people page
  Given  CU - I am logged in as a user with the <role> role
  When I go to "admin/settings/people/settings"
  Then I should not see <message>

  Examples:
  | role           | message         |
  | site_owner     | "Access denied" |
  | administrator  | "Access denied" |
  | developer      | "Access denied" |

  @api @settings @people_settings @clean_install
  Scenario: A site owner should see default settings for people labels
    Given  CU - I am logged in as a user with the "site_owner" role
    And am on "admin/settings/people/settings"
    Then the "edit-type-label" field should contain "type"

  @api @settings @people_settings @clean_install
  Scenario: A site owner should be able to change people labels
    Given  CU - I am logged in as a user with the "site_owner" role
    And am on "admin/settings/people/settings"
    And fill in "edit-type-label" with "Affiliation"
    And fill in "Filter One Label" with "Label One Test"
    And fill in "Filter Two Label" with "Label Two Test"
    And fill in "Filter Three Label" with "Label Three Test"
    When I press the "Save" button
    Then the "edit-type-label" field should contain "Affiliation"
    And the "edit-filter-one-label" field should contain "Label One Test"
    And the "edit-filter-two-label" field should contain "Label Two Test"
    And the "edit-filter-three-label" field should contain "Label Three Test"
