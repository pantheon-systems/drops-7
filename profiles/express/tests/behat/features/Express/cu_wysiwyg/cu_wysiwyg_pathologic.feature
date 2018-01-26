Feature: WYSIWYG Pathologic tests

  @api @wysiwyg @javascript
  Scenario: Pathologic should change URLs
    Given  CU - I am logged in as a user with the "content_editor" role
    When I setup Pathologic local paths
      And I am at "node/add/page"
      And I click "Disable rich-text"
      And I fill in "Body" with "<a href="http://www.colorado.edu/p1eb825ce549/test\">pathologic</a>"
      And I fill in "Title" with "Pathologic Test"
      And I press the "Save" button
    Then The "pathologic" link should have "//127.0.0.1:8888/test" in the "href" attribute

