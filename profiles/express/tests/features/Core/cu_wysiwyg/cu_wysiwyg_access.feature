Feature: WYSIWYG Access Feature
When I login to the website
As a content editor, site owner, administrator or developer
I should be able to access the functionality of the WYSIWYG editor

  @api @wysiwyg
  Scenario Outline: An authenticated user should have WYSIWYG selected as the the default text format
    Given I am logged in as a user with the <role> role
    When I go to "node/add/page"
    Then I should see "WYSIWYG" in the "#edit-body-und-0-format--2" element

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | developer      |

  @api @javascript @wysiwyg
  Scenario Outline: An authenticated user should have all the WYSIWYG shortcode and other buttons available
    Given I am logged in as a user with the <role> role
    When I go to "node/add/page"
    Then I should see the "Button Shortcode Generator" button
      And I should see the "Image Caption Shortcode Generator" button
      And I should see the "Image Caption Shortcode Generator" button
      And I should see the "Icon Shortcode Generator" button
      And I should see the "Give Button Shortcode Generator" button
      And I should see the "Expand Content Shortcode Generator" button
      And I should see the "Map Shortcode Generator" button
      And I should see the "Box Shortcode Generator" button
      And I should see the "Video Shortcode Generator" button
      And I should see the "Link" button

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | developer      |
