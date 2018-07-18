Feature: WYSIWYG Access Feature
When I login to the website
As a content editor, site owner, administrator or developer
I should be able to access the functionality of the WYSIWYG editor

  @wysiwyg @javascript
  Scenario Outline: An authenticated user should have WYSIWYG selected as the the default text format
    Given  I am logged in as a user with the <role> role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
    Then I should see a ".cke_button__bold" element

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | developer      |


  @wysiwyg @javascript
  Scenario Outline: An authenticated user should have all the WYSIWYG shortcode and other buttons available
    Given I am logged in as a user with the <role> role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
    Then I should see a ".cke_button__button_sc_generator_button" element
      And I should see a ".cke_button__caption_sc_generator_button" element
      And I should see a ".cke_button__icon_sc_generator_button" element
      And I should see a ".cke_button__give_sc_generator_button" element
      And I should see a ".cke_button__expand_sc_generator_button" element
      And I should see a ".cke_button__map_sc_generator_button" element
      And I should see a ".cke_button__box_sc_generator_button" element
      And I should see a ".cke_button__video_sc_generator_button" element
      And I should see a ".cke_button__soundcloud_embed_button" element
      And I should see a ".cke_button__link_icon" element

    Examples:
      | role           |
      | content_editor |
      | site_owner     |
      | administrator  |
      | developer      |
