Feature: Frontpage
  To have people being interested in your shop
  As any user
  I should be an appealing homepage

  Background:
    Given I am on the homepage

  Scenario: I should be able to checkout
    Then I should see the link "Checkout"
    When I click "Checkout"
    Then I should get a "200" HTTP response

  Scenario: User should be able to login
    Then I should see the link "Log in"
    When I click "Log in"
    Then I should get a "200" HTTP response

  Scenario: User should be able to create account
    Then I should see the link "Create account"
    When I click "Create account"
    Then I should get a "200" HTTP response

  Scenario: User should see discount
    Then I should see "SAVE 25%"
    And I should see "Purchases made between June 5 - 12 will be discounted"
    And I should see "Offer Details"

  Scenario: User should see free shipping
    Then I should see "Free shipping"
    And I should see "on orders over"
    And I should see "$99.99"

  Scenario: User should see the top menu
    Then I should see the following <links>
      | links |
      | To carry      |
      | To drink with |
      | To geek out   |
      | To wear       |
      | All products  |
      | Blog          |
      | Contact       |
      | About         |

  Scenario: User should see the bottom menu
    Then I should see the following <texts>
      | texts              |
      | Company info       |
      | Service & support  |
      | Security & privacy |
      | Shipping & returns |
    Then I should see the following <links>
      | links               |
      | About us            |
      | Service agreements  |
      | Shipping fees       |
      | Terms of use        |
      | Our security policy |
      | Press links         |

  Scenario: User should see the social menu
    Then I should see the following <links>
      | links                     |
      | Like us on Facebook       |
      | Follow Us on Twitter      |
      | What We Like on Pinterest |

  Scenario: User should see the payment menu
    Then I should see the following <links>
      | links            |
      | MasterCard       |
      | PayPal           |
      | Visa             |
      | American Express |

  Scenario: User should be able to search
    Then I should see "Search"
    When I fill in "Search" with "Cap"
    And I press "edit-submit-display-products"
    Then I should get a "200" HTTP response
