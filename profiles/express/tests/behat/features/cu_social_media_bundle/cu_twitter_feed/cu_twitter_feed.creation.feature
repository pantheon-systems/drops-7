Feature: Facebook Like Creation Feature
  Test creation of Facebook like beans.

  @api @social_media @javascript
  Scenario: Create a Twitter Feed Block.
  Given CU - I am logged in as a user with the "content_editor" role
    And I am on "block/add/twitter-block"
    And I fill in "Label" with "Twitter Block Label"
    And I fill in "Title" with "Twitter Block Title"
    And I fill in "Twitter User Name" with "cuboulder"
    And I fill in "Number of Tweets" with "7"
    And I select "dark" from "field_twitter_style[und]"
    And I press "Save"
  Then I should see "Twitter Block Title"
    And The "iframe" element should have "twitter-timeline twitter-timeline-rendered" in the "class" attribute
