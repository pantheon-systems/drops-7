Feature: Content Management
  When I install the website
  As a user
  I should be able to see certain content

  Scenario: A user should see "Welcome!" on the homepage
      Given I am on the homepage
      Then I should see the text "Welcome to your new Web Express website! This content area is used for your homepage content. You can edit this section by clicking on the Edit link above. Once in edit mode, just edit the body text area. For help with your site, you can view examples of all the Web Express Features or view online tutorials."
        And I should see the link "view examples of all the Web Express Features"
        And I should see the link "view online tutorials"

  # Test to cover the regression in FIT-902.
  @api
  Scenario: A user should not see a subnavigation menu header
      Given I am logged in as a user with the "content_editor" role
      When I visit "node/add/page"
        And for "Title" I enter "Basic Page"
        And for "Menu link title" I enter "Basic Page"
        And I select "-- Home" from "Parent item"
        And I press the "Save" button
      Then I should not see the text "Home" in the "Sidebar Second" region
        And I should see the text "Basic Page" in the "Sidebar Second" region

  # TODO write test to see no messages in the error regions
