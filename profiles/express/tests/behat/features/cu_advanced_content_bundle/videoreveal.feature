@AdvContentBundle 
Feature: the Video Reveal block
In order to create a video block with a still cover graphic
As an authenticated user
I should be able to access and use the Video Reveal Block
  

Scenario Outline: An authenticated user should be able to access the form for adding a video reveal block
  Given  I am logged in as a user with the <role> role
  When I go to "block/add/video-reveal"
  Then I should see <message>

  Examples:
  | role            | message         |
  | edit_my_content | "Access denied" |
  | content_editor  | "Create Video Reveal block" |
  | site_owner      | "Create Video Reveal block" |
  | administrator   | "Create Video Reveal block" |
  | developer       | "Create Video Reveal block" |
  

Scenario: An anonymous user should not be able to access the form
  Given I go to "block/add/video-reveal"
  Then I should see "Access denied"
  

Scenario: A simple Video Reveal block can be created
Given I am logged in as a user with the "site_owner" role
And I go to "block/add/video-reveal"
And I fill in "edit-label" with "My Video Reveal Label"
And I fill in "edit-title" with "My Video Reveal Title"
And I fill in "edit-field-video-reveal-url-und-0-video-url" with "https://youtu.be/ihnibrcwtnQ"
And I attach the file "ralphie.jpg" to "edit-field-video-reveal-image-und-0-upload"
And I fill in "edit-field-video-reveal-text-und-0-value" with "Beautiful Boulder"
And I press "Save"
Then I should see "Video Reveal My Video Reveal Title has been created."
And I should see "My Video Reveal Title"
And I should see "Beautiful Boulder"
