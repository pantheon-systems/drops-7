Feature: Administrators can access management areas
  As a store manager
  I can access the administrative toolbar
  So that I can manage the store

  @api @javascript
  Scenario: I can access the administrative toolbar
    Given I am logged in as a user with the "administrator" role
    When I am on the homepage
    Then I should see the link "Products" in the "Navbar" region
    Then I should see the link "Orders" in the "Navbar" region
    Then I should see the link "Content" in the "Navbar" region
    Then I should see the link "Store settings" in the "Navbar" region
    Then I should see the link "Site settings" in the "Navbar" region
    Then I should see the link "Help" in the "Navbar" region
    Then I should see the link "Log out" in the "Navbar" region

    Then I should see the link "Manage orders" in the "Navbar" region
    Then I should see the link "Manage products" in the "Navbar" region

    Then I should see the link "Categories" in the "Navbar" region
    Then I should see the link "Shipping" in the "Navbar" region
    Then I should see the link "Taxes" in the "Navbar" region
    Then I should see the link "Discounts" in the "Navbar" region
    Then I should see the link "Payment methods" in the "Navbar" region
    Then I should see the link "Currency settings" in the "Navbar" region
