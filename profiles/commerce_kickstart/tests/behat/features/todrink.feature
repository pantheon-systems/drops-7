Feature: Add drinking item to cart
  In order to buy something to drink with
  As any user
  I should be able to add a coffee mug to my cart

  Scenario: View the drinking options text and links on the page
    Given I am on the homepage
    When I follow "To drink with"
    Then I should see the heading "Coffee Mugs"
    And I should see the link "Drupal Commerce to Wake You Up"

  Scenario: View coffee mug product information
    When I go to "/drinks/drupal-commerce-wake-you"
    Then I should see the following <texts>
      | texts                           |
      | Drupal Commerce to Wake You Up  |
      | Acme gear                       |
      | $8.00                           |
      | Product Description             |
      | SKU: MG1-BLU-OS                 |

  Scenario: Add coffee mug to cart
    When I go to "/drinks/drupal-commerce-wake-you"
    And I press "Add to cart"
    Then I should see "ITEM SUCCESSFULLY ADDED TO YOUR CART"

  Scenario: View coffee mug service links
    When I go to "/drinks/drupal-commerce-wake-you"
    Then I should see an "div.service-links" element
