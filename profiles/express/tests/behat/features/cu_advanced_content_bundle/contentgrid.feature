@AdvContentBundle 
Feature: the Content Grid block
In order to create interesting layouts of graphics and text
As an authenticated user
I should be able to access and use the Content Grid Block
  

Scenario Outline: An authenticated user can access the form for adding a content list block
  Given I am logged in as a user with the <role> role
  When I go to "block/add/feature-callout"
  Then I should see <message>

  Examples:
  | role            | message         |
  | edit_my_content | "Access denied" |
  | content_editor  | "Create Content Grid block" |
  | site_owner      | "Create Content Grid block" |
  | administrator   | "Create Content Grid block" |
  | developer       | "Create Content Grid block" |
  

Scenario: An anonymous user can not access the form for adding a content list block
  Given I go to "block/add/feature-callout"
  Then I should see "Access denied"
  

Scenario: An authenticated user should see a number of Grid Style options
Given I am logged in as a user with the "site_owner" role
And am on "block/add/feature-callout"
Then I should see an "#edit-field-callout-style-und-columns" element
And I should see an "#edit-field-callout-style-und-overlay" element
And I should see an "#edit-field-callout-style-und-large-teaser" element
And I should see an "#edit-field-callout-style-und-large-teaser-alt" element
And I should see an "#edit-field-callout-style-und-feature" element
And I should see an "#edit-field-callout-style-und-offset" element
And I should see an "#edit-field-callout-style-und-cards" element
And I should see an "#edit-field-callout-style-und-teaser" element
And I should see an "#edit-field-callout-style-und-tiles-alt" element


Scenario: An authenticated user should see a number of Column options
Given I am logged in as a user with the "site_owner" role
And am on "block/add/feature-callout"
When I select "2" from "edit-field-callout-columns-und"
And I select "3" from "edit-field-callout-columns-und"
And I select "4" from "edit-field-callout-columns-und"
And I select "5" from "edit-field-callout-columns-und"
And I select "6" from "edit-field-callout-columns-und"


Scenario: An authenticated user should see a number of Image Size options
Given I am logged in as a user with the "site_owner" role
And am on "block/add/feature-callout"
When I select "Wide" from "edit-field-callout-image-size-und"
When I select "Square" from "edit-field-callout-image-size-und"


Scenario: A simple Content Grid can be created
Given I am logged in as a user with the "site_owner" role
And I go to "block/add/feature-callout"
And I fill in "edit-label" with "My Content Grid Label"
And I fill in "edit-title" with "My Content Grid Title"
# FIRST CELL
And I fill in "edit-field-callouts-und-0-field-callout-title-und-0-title" with "Heading One"
# And I follow "Disable rich-text"
And fill in "edit-field-callouts-und-0-field-callout-text-und-0-value" with "Cupcake ipsum dolor sit amet ice cream carrot cake"
And I press "Add another item"
# SECOND CELL
And I fill in "edit-field-callouts-und-1-field-callout-title-und-0-title" with "Heading Two"
# And I follow "Disable rich-text"
And fill in "edit-field-callouts-und-1-field-callout-text-und-0-value" with "Veggie ipsum dolor sit amet cucumber broccoli carrot stringbean"
And I press "Save"
Then I should see "Content Grid My Content Grid Title has been created."
And I should see "Cupcake ipsum dolor sit amet ice cream carrot cake" 
And I should see "Veggie ipsum dolor sit amet cucumber broccoli carrot stringbean"

