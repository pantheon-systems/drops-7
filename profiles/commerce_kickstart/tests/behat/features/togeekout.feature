Feature: Add geek out item to cart
  In order to buy something to geek out with
  As any user
  I should be able to add a 32GB usb key to my cart

  Scenario: View the geek out options text and links on the page
    Given I am on the homepage
    When I follow "To geek out"
    Then I should see the heading "USB Keys"
    And I should see the link "Commerce Guys USB Key"

  Scenario: View usb key product information
    When I go to "/storage-devices/commerce-guys-usb-key"
    Then I should see the following <texts>
      | texts                           |
      | Commerce Guys USB Key           |
      | Bits & Bots                     |
      | $11.99                          |
      | Product Description             |
      | SKU: USB-BLU-08                 |

  @javascript
  Scenario: Add a 32gb usb key to cart
    When I go to "/storage-devices/commerce-guys-usb-key"
    When I select "16GB" from "Capacity"
      And I wait for AJAX to finish
      Then I should see "$17.99"
    When I select "32GB" from "Capacity"
      And I wait for AJAX to finish
      Then I should see "$29.99"
    And I press "Add to cart"
      Then I should see "ITEM SUCCESSFULLY ADDED TO YOUR CART"
    When I am on "/cart"
    Then I should see the following <texts>
      | texts                         |
      | SKU: USB-BLU-32               |
      | Capacity:                     |
      | 32GB                          |
      | $29.99                        |
