Feature: Event Blocks Creation
  Create different configurations of Event Calendar Grids and Blocks.

  @api @events
  Scenario: Create an Events Calendar Block.
    Given I am logged in as a user with the "content_editor" role
      And I am on "block/add/cu-events-calendar-block"
      And I fill in "Label" with "New Events Calendar Block"
      And I fill in "Title" with "Events Block Title"
      And I fill in "field_event_date_range[und][0][value][date]" with "02/01/2016"
      And I fill in "field_event_date_range[und][0][value2][date]" with "02/01/2020"
      And I check the box "Academic Advising"
      And I fill in "field_event_link[und][0][title]" with "Link Title"
      And I fill in "field_event_link[und][0][url]" with "www.google.com"
      And I press "Save"
    Then I should see "Events Block Title"
      And I should see "Link Title"

  @api @events @javascript
  Scenario: Create an Events Calendar Block.
    Given I am logged in as a user with the "content_editor" role
      And I am on "block/add/events-calendar-grid"
      And I fill in "Label" with "Grid Label"
      And I fill in "Title" with "Events Calendar Grid"
      And I check the box "Academic Advising"
      And I select "2" from "field_event_months[und]"
      And I select the radio button "Yes" with the id "edit-field-event-show-all-grids-und-1"
      And I press "Save"
    Then I should see "View Entire Month"
