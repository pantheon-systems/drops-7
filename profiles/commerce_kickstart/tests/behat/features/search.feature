Feature: Search the site
  In order to find products on the site
  As any user
  I should be able to search for products

  Scenario: Use the global search field
    Given I am on the homepage
    When I fill in "Search" with "Cap"
      And I press "edit-submit-display-products"
    Then I should see "There are 2 search results"
      And I should see the link "Commerce Guys Baseball Cap"
      And I should see the link "Drupal Commerce Ski Cap"

  Scenario: Facet search on all products
    When I go to "/products"
    Then I should see the following <texts>
      | texts      |
      | Price      |
      | Brand      |
      | Category   |
      | Collection |
      | Gender     |
    When I click "To wear (12)"
    Then I should see "There are 12 search results"
      And I should see "To wear"
      And I should see the following <texts>
      | texts      |
      | Price      |
      | Color      |
      | Size       |
      | Size       |
      | Size       |
      | Brand      |
      | Category   |
      | Gender     |
    When I click "Blue (3)"
    Then I should see "There are 3 search results"
      And I should see "To wear"
      And I should see "Blue"
    # We are removing "To wear"
    When I click "(-) "
      Then I should see "There are 6 search results"

