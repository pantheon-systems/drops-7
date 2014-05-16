Feature: Handle orders
  In order to handle the bought products
  As an administrator user
  I should be able to handle the orders

  @api @javascript
  Scenario:
    When I am logged in as a user with the "administrator" role
      And I go to "/admin/commerce/orders"
    When I click on Quick Edit link
    Then I should see the following <texts>
      | texts           |
      | Title           |
      | Order status    |
      | Unit price      |
      | Quantity        |
      | Order total     |
      | Order History   |
      | Add new comment |
