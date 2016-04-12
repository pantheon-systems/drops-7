Feature: Responsive product facet search api
  In order to have a better view of product search on mobile
  As any user
  I should see concise search facets

  @javascript @demo
  Scenario: Search facets should be presented as select lists
    When I go to "/products"
      And I click "To wear (12)"
      Then I should see "There are 12 search results"
    When I click "(-) "
      And I resize the browser to mobile
      # Facets don't react on resize
      And I reload the page
    When I select "Select a collection..." from collection dropdown
    When I select "To wear (12)" from collection dropdown
