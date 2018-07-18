Feature: Facebook Like Creation Feature
  Test creation of Facebook like beans.

@social_media
Scenario: Create an Facebook Like Block.
  Given  I am logged in as a user with the "content_editor" role
    And I am on "block/add/facebook-like-button"
    And I fill in "Label" with "Facebook Activity Label"
    And I fill in "Title" with "Facebook Activity Block Title"
    And I fill in "field_fb_url[und][0][url]" with "https://www.facebook.com/cuboulder"
    And I select "true" from "field_fb_like_faces[und]"
    And I select "recommend" from "field_fb_verb[und]"
    And I press "Save"
  Then I should see "Facebook Activity Block Title"
    And The "iframe" element should have "//www.facebook.com/plugins/like.php?href=https%3A%2F%2Fwww.facebook.com%2Fcuboulder&send=false&layout=standard&show_faces=true&action=recommend&colorscheme=light&font&height=80&appId=137301796349387" in the "src" attribute
