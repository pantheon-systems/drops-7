Feature: Create products
  In order to sell products
  As an administrator user
  I should be able to create a product

  @api
  Scenario: Create a product
    When I am logged in as a user with the "administrator" role
      And I click "Products"
      And I click "Add product"
      And I click "Bags & Cases"
      And I fill in "Title" with "Supercool bag"
      And I fill in "Body" with "Buy this supercool bag"
      And I select "Blue" from "Color"
      And I select "One Size" from "Size"
      And I fill in "Price" with "20"
      And I press "Create variation"
      And I press "Add new variation"
      And I select "Red" from "Color"
      And I select "One Size" from "Size"
      And I fill in "Price" with "20"
      And I press "Create variation"
      And I select "To carry" from "Collection"
      And I select "Messenger Bags" from "Category"
      And I select "Unisex" from "Gender"
      And I select "iSleeve" from "Brand"
      And I press "Save"
    Then I should see "Bags & Cases Supercool bag has been created"
    When I click "Edit" in the "Tabs" region
    Then I should see "Supercool bag (Blue, One Size)"
      And I should see "Supercool bag (Red, One Size)"

  @api
  Scenario: Create a product without clicking Create Variation explicitly #2150067
    When I am logged in as a user with the "administrator" role
    And I click "Products"
    And I click "Add product"
    And I click "Bags & Cases"
    And I fill in "Title" with "Supercool bag"
    And I fill in "Body" with "Buy this supercool bag"
    And I select "Blue" from "Color"
    And I select "One Size" from "Size"
    And I fill in "Price" with "20"
    And I select "To carry" from "Collection"
    And I select "Messenger Bags" from "Category"
    And I select "Unisex" from "Gender"
    And I select "iSleeve" from "Brand"
    And I press "Save"
    Then I should see "Bags & Cases Supercool bag has been created"
    When I click "Edit" in the "Tabs" region
    Then I should see "Supercool bag (Blue, One Size)"

  @api
  Scenario: Make sure that unpublished products are not shown #2264127
    When I am logged in as a user with the "administrator" role
    And I click "Products"
    And I click "Add product"
    And I click "Bags & Cases"
    And I fill in "Title" with "Supercool unpublished bag"
    And I fill in "Body" with "Buy this supercool bag"
    And I select "Blue" from "Color"
    And I select "One Size" from "Size"
    And I fill in "Price" with "20"
    And I select "To carry" from "Collection"
    And I select "Messenger Bags" from "Category"
    And I select "Unisex" from "Gender"
    And I select "iSleeve" from "Brand"
    And I uncheck the box "Published"
    When I press "Save"
    Then I should see "Bags & Cases Supercool unpublished bag has been created"
    When I click "To carry"
    Then I should not see "Supercool unpublished bag"
    When I fill in "Search" with "Supercool unpublished bag"
    And I press "edit-submit-display-products"
    Then I should see "There are 0 search results"
    And I should see "No results found for your search."
