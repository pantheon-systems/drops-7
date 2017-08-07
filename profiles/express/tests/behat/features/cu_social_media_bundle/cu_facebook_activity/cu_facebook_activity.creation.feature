Feature: Facebook Activity Block Creation
  Tests creation of Facebook Activity Blocks.

  @api @social_media
  Scenario: Create an Facebook Activity Block.
  Given  CU - I am logged in as a user with the "content_editor" role
  And I am on "block/add/facebook-activity"
  And I fill in "Label" with "Facebook Activity Label"
  And I fill in "Title" with "Facebook Activity Block"
  And I fill in "field_fb_url[und][0][url]" with "https://www.facebook.com/cuboulder"
  And I select "false" from "field_fb_like_faces[und]"
  And I press "Save"
  Then I should see "Facebook Activity Block"
  And The "iframe" element should have "//www.facebook.com/plugins/likebox.php?href=https%3A%2F%2Fwww.facebook.com%2Fcuboulder&colorscheme=light&height=560&show_faces=false&border_color&stream=true&header=false&appId=137301796349387" in the "src" attribute
