Feature: Checkout and pay
  In order to buy a product and pay for it
  As any user
  I should be able to checkout my cart and pay online

  Background:
    When I go to "/drinks/drupal-commerce-wake-you"
      And I press "Add to cart"
    Then I should see "ITEM SUCCESSFULLY ADDED TO YOUR CART"
    When I click "Go to checkout"

  @javascript
  Scenario: Add coffee mug to cart and update quantity
    Then I should see "Shopping cart"
    Then I should see the following <texts>
      | texts                           |
      | $8.00                           |
      | SKU: MG1-BLU-OS                 |
      And the "edit_quantity[0]" field should contain "1"
    When I fill in "2" for "edit_quantity[0]"
      And I press "Update cart"
    Then I should see "Your shopping cart has been updated."
      And I should see "$16.00"
    When I press "Checkout"
    Then I should see "I don't have an account"
    When I fill in "admin" for "Username"
      And I fill in "admin" for "Password"
      And I press "Log in"
    Then I should see "Checkout"
    Then I should see the following <texts>
      | texts                           |
      | $8.00                           |
      | $16.00                          |
      | Coffee Mug 1                    |
      | Billing information             |
    When I select "United States" from "Country"
    Given I wait for AJAX loading to finish
    When I fill in the following:
      | Full name | My full name |
      | Address 1 | My address   |
      | City      | My city      |
      | ZIP Code  | 90120        |
      And I select "California" from "State"
      And I press "Continue to next step"
      # Just choose the default shipping method
      And I press "Continue to next step"
    Then I should see "Review order"
      # The default shipping method
      And I should see "Express shipping: 1 business day"
      # Order total
      And I should see "$31.00"
    When I fill in "4111111111111111" for "Card number"
      And I select "03" from "commerce_payment[payment_details][credit_card][exp_month]"
      And I select "24" from "commerce_payment[payment_details][credit_card][exp_year]"
      And I press "Continue to next step"
    Then I should see "Checkout complete"
      And I should see "Your order number is"
      And I should see "You can view your order on your account page when logged in"
      And I should see "Return to the front page"
