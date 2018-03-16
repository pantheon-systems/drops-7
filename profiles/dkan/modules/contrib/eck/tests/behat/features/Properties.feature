@api @crud
Feature: CRUD
  As a content architect
  I want to be able to forge my entities with custom attributes (properties)
  so my content will do exactly what it needs to do

  Background:
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities" permissions

  @setup
  Scenario: Setting up for the tests
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "edit-entity-type-label" with "Vehicle"
    And I fill in "edit-entity-type-name" with "vehicle"
    And I fill in "edit-bundle-label" with "Car"
    And I fill in "edit-bundle-name" with "car"
    And I press the "Save" button

  Scenario Outline: I should be able to create a property set a value and then delete it
    Given I visit "/admin/structure/entity-type/vehicle/properties"
    And I fill in "edit-property-type" with <type>
    And I fill in "edit-property-label" with <label>
    And I fill in "edit-property-name" with <name>
    And I fill in "edit-property-behavior" with "title"
    And I press the "edit-property-add" button
    And I check the box <checkbox>
    And I press the "Save" button

    Given I visit "admin/structure/entity-type/vehicle/car/add"
    And I fill in <id> with <value>
    And I press the "Save" button
    Then I should see the text <value>

    Given I visit "/admin/structure/entity-type/vehicle/properties"
    And I uncheck the box <checkbox>
    And I press the "Save" button

    Examples:
      | type               | label | name | id        | checkbox                   | value          |
      | "text"             | "T"   | "t"  | "edit-t"  | "new_properties_table[t]"  | "Toyota Prius" |
      | "integer"          | "I"   | "i"  | "edit-i"  | "new_properties_table[i]"  | "-123456"      |
      | "decimal"          | "D"   | "d"  | "edit-d"  | "new_properties_table[d]"  | "45.98"        |
      | "positive_integer" | "PI"  | "pi" | "edit-pi" | "new_properties_table[pi]" | "987"          |
      | "language"         | "L"   | "l"  | "edit-l"  | "new_properties_table[l]"  | "en"           |

  @cleanup
  Scenario: This is a clean up step
    Given I visit "/admin/structure/entity-type"
    And I click "Vehicle"
    And I click "Delete"
    And I press the "Delete" button
