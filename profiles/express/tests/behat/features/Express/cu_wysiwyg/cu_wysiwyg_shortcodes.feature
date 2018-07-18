Feature: WYSIWYG Feature
When I login to the website
As a content editor, site owner, administrator or developer
I should be able to use the full functionality of the WYSIWYG editor

  @javascript @wysiwyg
  Scenario: A content editor should be able to add a video shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Video Shortcode"
      And I press "Video Shortcode Generator"
      And I fill in "video URL" with "https://www.youtube.com/watch?v=m-m7mBSw-5k"
      #And I press the "OK" button
      #And I press the "Save" button
    #Then I should see "My Video Shortcode"
      # @todo Figure out a step definition for elements.
      #And I should see a ".video-filter" element

  @javascript @wysiwyg
  Scenario: A content editor should be able to add a button shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Button Shortcode"
      And I press "Button Shortcode Generator"
      And I fill in "Button Text" with "New Button"
      And I fill in "URL" with "http://www.google.com"
      # @todo Figure out why OK button is weird here.
      #And I press the "OK" button
      #And I press the "Save" button
    #Then I should see "My Button Shortcode"
    #Then I should see "New Button"
      #And I click "New Button"
    #Then I should see a "#hplogo" element

  @javascript @wysiwyg @files @broken
  Scenario: A content editor should be able to add an image caption shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Image Caption Shortcode"
      And I attach the file "ralphie.jpg" to the "edit-field-photo-und-0-upload" field
      And I fill in "Alternate text" with "Ralphie"
      And I press "Upload"
    Then I should see a ".image-widget-data" element
    When I press "Insert"
      # @todo Figure out how to switch to iframe contexts http://apigen.juzna.cz/doc/Behat/Mink/source-class-Behat.Mink.Driver.DriverInterface.html#163-171
      # And I press the "Image Caption Shortcode Generator" button
      # And I fill in "Image Caption" with "Ralphie Run"
      # And I press the "OK" button
      # And I press the "Save" button
    # Then I should see "My Image Caption Shortcode"
    # Then I should see "New Button"

  @javascript @wysiwyg
  Scenario: A content editor should be able to add an icon shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Icon Shortcode"
      And I press "Icon Shortcode Generator"
      And I select "ambulance" from "Icon [View Icons]"
      And I select "fa-5x" from "Icon Size"
      And I select "Black" from "Icon Color"
      And I select "square" from "Icon Wrapper"
      #And I press the "OK" button
      #And I press the "Save" button
    #Then I should see a ".fa-ambulance.icon-color-black.icon-wrapper-square" element

  @javascript @wysiwyg
  Scenario: A content editor should be able to add a give button shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Give Button Shortcode"
      And I press "Give Button Shortcode Generator"
      And I select "light" from "give Color"
      And I fill in "Give Button Text" with "Give Now!"
      And I fill in "URL" with "http://www.google.com"
      And I select "large" from "Give Button Size"
      And I select "full" from "Give Button Style"
      # @todo Figure out why OK button is weird here.
      #And I press the "OK" button
      #And I press the "Save" button
    #Then I should see "Give Now!"
      #And I should see a ".cu-give-button.button-large.button-full.cu-give-button-light" element
    #When I click "Give Now!"
    #Then I should see a "#hplogo" element

  @javascript @wysiwyg
  Scenario: A content editor should be able to add a expand content shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Expandable Content Shortcode"
      And I press "Expand Content Shortcode Generator"
      And I fill in "Title" with "Example FAQ #1"
      And I fill in "Expand Content Text" with "Example content #1"
      # @todo Figure out why OK button is weird here.
      #And I press the "OK" button
      #And I press the "Expand Content Shortcode Generator" button
      #And I fill in "Title" with "Example FAQ #2"
      #And I fill in "Expand Content Text" with "Example content #2"
      #And I press the "OK" button
      #And I press the "Save" button
    #Then I should see "Example FAQ #1"
      #And I should see "Example FAQ #2"
    #When I click "Example FAQ #1"
    #Then I should see "Example content #1"
      #And I should not see the text "Example content #2"
    #When I click "Example FAQ #1"
      #And I click "Example FAQ #2"
    #Then I should see "Example content #2"
      #And I should not see "Example content #1"

  @javascript @wysiwyg
  Scenario: A content editor should be able to add a map embed shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Map Shortcode"
      And I press "Map Shortcode Generator"
      And I fill in "Map Embed Code" with "<iframe src=\"https://www.google.com/maps/embed?pb=!1m18!1m12!1m3!1d3055.782574009197!2d-105.25404084826329!3d40.0133039793131!2m3!1f0!2f0!3f0!3m2!1i1024!2i768!4f13.1!3m3!1m2!1s0x876bedc53ea9c665%3A0x2927ccc033644a4b!2s3100+Marine+St%2C+Boulder%2C+CO+80303!5e0!3m2!1sen!2sus!4v1445292423584\" width=\"600\" height=\"450\" frameborder=\"0\" style=\"border:0\" allowfullscreen></iframe>"
      #And I press the "OK" button
      #And I press the "Save" button
    # @todo make switch frame work or make step to grab "src" attribute and compare it to the following
    #Then the "iframe" element should contain "src=\"https://www.google.com/maps/embed?pb=!1m18!1m12!1m3!1d3055.782574009197!2d-105.25404084826329!3d40.0133039793131!2m3!1f0!2f0!3f0!3m2!1i1024!2i768!4f13.1!3m3!1m2!1s0x876bedc53ea9c665%3A0x2927ccc033644a4b!2s3100+Marine+St%2C+Boulder%2C+CO+80303!5e0!3m2!1sen!2sus!4v1445292423584&ie=UTF8&output=embed\""

  @javascript @wysiwyg
  Scenario: A content editor should be able to add a box shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "My Box Shortcode"
      And I press "Box Shortcode Generator"
      And I fill in "Title (optional)" with "Box Shortcode"
      And I fill in "Box Text" with "Box Shortcode Text"
      And I select "black" from "Box Color"
      And I select "right" from "Float"
      #And I press the "OK" button
      #And I press the "Save" button
    #Then I should see "Box Shortcode"
      #And I should see "Box Shortcode Text"
      #And I should see a ".cu-box.box-black.float-right.box-style-filled" element

  @wysiwyg @javascript
  Scenario: A content editor should be able to add a countup shortcode
    Given  I am logged in as a user with the "content_editor" role
    When I go to "node/add/page"
      And I wait for the "#cke_1_top" element to appear
      And I fill in "Title" with "Countup Shortcode"
      And I follow "Disable rich-text"
      And I fill in "Body" with "[countup]3000[/countup]"
      And I press "Save"
      And I wait 5 seconds
    Then I should see "3000"
      And I should not see "[countup]3000[/countup]"
