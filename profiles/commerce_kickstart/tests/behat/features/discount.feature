Feature: Handle discounts
  In order to use discounts in the shop
  As an administrator user
  I should be able to handle discounts

  @api @javascript
  Scenario: Administrator is able to create a discount
    When I am logged in as a user with the "administrator" role
      And I go to "/admin/commerce/store/discounts"
    When I click "Add discount"
    Then I should see "Add order discount commerce discount"
    When I fill in the following:
      | Admin title | Test create discount |
      | Name        | Test create discount |
    When I select the radio button "Order discount" with the id "edit-commerce-discount-type-order-discount"
      And I select "- All -" from "Apply to"
      And I select the radio button "$ off" with the id "edit-commerce-discount-fields-commerce-discount-offer-und-form-type-fixed-amount"
      And I fill in "5" for "Fixed amount"
      And I press "Save discount"
    Then I should see "Discounts"
      And I should see "Test create discount"
    When I click "open"
      And I wait for AJAX to finish
    When I click "delete"
    When I press "Confirm"
    Then I should see "Deleted Commerce Discount Test create discount."

  @api @javascript
  Scenario: Discounts should be added on checkout
    When I am logged in as a user with the "administrator" role
      And I go to "/admin/commerce/store/discounts"
    When I click "Add discount"
    Then I should see "Add order discount commerce discount"
    When I fill in the following:
      | Admin title | Test discount |
      | Name        | Test discount |
    When I select the radio button "Order discount" with the id "edit-commerce-discount-type-order-discount"
      And I select "- All -" from "Apply to"
      And I select the radio button "$ off" with the id "edit-commerce-discount-fields-commerce-discount-offer-und-form-type-fixed-amount"
      And I fill in "5" for "Fixed amount"
      And I press "Save discount"
    Then I should see "Discounts"
      And I should see "Test discount"
    When I click "Log out"
    When I go to "/bags-cases/commerce-guys-laptop-bag"
      And I press "Add to cart"
    Then I should see "ITEM SUCCESSFULLY ADDED TO YOUR CART"
    When I click "Go to checkout"
    Then I should see "Shopping cart"
    Then I should see "Test discount"
    Then I should see "-$5.00"
    Then I should see "$37.00"
    When I am logged in as a user with the "administrator" role
      And I go to "/admin/commerce/store/discounts"
    When I click "open"
      And I wait for AJAX to finish
    When I click "delete"
    When I press "Confirm"
    Then I should see "Deleted Commerce Discount Test discount."
