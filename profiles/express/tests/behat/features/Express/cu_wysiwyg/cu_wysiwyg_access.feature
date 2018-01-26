Feature: WYSIWYG Access Feature
When I login to the website
As a content editor, site owner, administrator or developer
I should be able to access the functionality of the WYSIWYG editor

  @api @wysiwyg
  Scenario Outline: An authenticated user should have WYSIWYG selected as the the default text format
    Given  CU - I am logged in as a user with the <role> role
    When I am at "node/add/page"
      And I wait for the "cke_1_top" element to appear
    Then I should see the button "Bold"

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | developer      |


  @api @wysiwyg
  Scenario Outline: An authenticated user should have all the WYSIWYG shortcode and other buttons available
    Given CU - I am logged in as a user with the <role> role
    When I am at "node/add/page"
      And I wait for the "cke_1_top" element to appear
    Then I should see the button "Button Shortcode Generator"
      And I should see the button "Image Caption Shortcode Generator"
      And I should see the button "Image Caption Shortcode Generator"
      And I should see the button "Icon Shortcode Generator"
      And I should see the button "Give Button Shortcode Generator"
      And I should see the button "Expand Content Shortcode Generator"
      And I should see the button "Map Shortcode Generator"
      And I should see the button "Box Shortcode Generator"
      And I should see the button "Video Shortcode Generator"
      And I should see the button "Link"

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | developer      |
