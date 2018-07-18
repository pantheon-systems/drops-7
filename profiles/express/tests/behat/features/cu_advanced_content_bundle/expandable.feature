@AdvContentBundle @expandable
Feature: the Expandable (Quicktabs) block
In order to create the look of tabbed content
As an authenticated user
I should be able to access and use the Expandable Block
  

Scenario Outline: An authenticated user should be able to access the form for adding an expandable block
  Given  I am logged in as a user with the <role> role
  When I go to "block/add/expandable"
  Then I should see <message>

  Examples:
  | role            | message         |
  | edit_my_content | "Access denied" |
  | content_editor  | "Create Expandable block" |
  | site_owner      | "Create Expandable block" |
  | administrator   | "Create Expandable block" |
  | developer       | "Create Expandable block" |
  

Scenario: An anonymous user should not be able to access the form
  Given I go to "block/add/expandable"
  Then I should see "Access denied"
  

Scenario: An authenticated user should see a number of display options
Given I am logged in as a user with the "site_owner" role
When I go to "block/add/expandable"
Then the "edit-field-expandable-section-open-und" checkbox should be checked
And I select "accordion" from "edit-field-expandable-display-und"
And I select "tabs" from "edit-field-expandable-display-und"
And I select "tabs-vertical" from "edit-field-expandable-display-und"
And I select "select" from "edit-field-expandable-display-und"


Scenario: A simple Expandable block can be created
Given I am logged in as a user with the "site_owner" role
And I go to "block/add/expandable"
And I fill in "edit-label" with "Expandable Label"
And I fill in "edit-title" with "Expandable Title"
# FIRST CELL
And I fill in "edit-field-expandable-section-und-0-field-expandable-title-und-0-value" with "Heading One"
And fill in "edit-field-expandable-section-und-0-field-expandable-text-und-0-value" with "Cupcake ipsum dolor sit amet ice cream carrot cake"
And I press "Add another item"
# SECOND CELL
And I fill in "edit-field-expandable-section-und-1-field-expandable-title-und-0-value" with "Heading Two"
And fill in "edit-field-expandable-section-und-1-field-expandable-text-und-0-value" with "Veggie ipsum dolor sit amet cucumber broccoli carrot stringbean"
And I press "Save"
Then I should see "Expandable Expandable Title has been created."
And I should see "Expandable Title" 
And I should see "Heading One"
And I should see "Cupcake ipsum dolor sit amet ice cream carrot cake"
And I should see "Heading Two"
