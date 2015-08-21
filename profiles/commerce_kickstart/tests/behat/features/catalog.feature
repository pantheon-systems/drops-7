Feature: Catalog
  As a site visitor
  I can browse the catalog
  To purchase products

  Scenario: Catalog taxonomy vocabulary doesn't have duplicate titles (#2118059)
    When I am an anonymous user
      And I am on "/collection/geek-out"
    Then the ".taxonomy-title" element should contain "To Geek Out"
      And I should not see an "#page-title" element
