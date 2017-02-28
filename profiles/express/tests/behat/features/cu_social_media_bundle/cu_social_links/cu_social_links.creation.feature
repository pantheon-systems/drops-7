Feature: Social Links Creation Feature
  Test creation of social link beans.

@api @social_media
Scenario: Create a Social Links Block.
Given  CU - I am logged in as a user with the "content_editor" role
And I am on "block/add/social-links"
And I fill in "Label" with "New Social Links Block"
And I fill in "Title" with "Social Links Block Title"
And I select "facebook" from "field_social_links_collection[und][0][field_social_link_type][und]"
And I fill in "field_social_links_collection[und][0][field_social_link_url][und][0][url]" with "https://www.facebook.com/cuboulder"
And I press the "field_social_links_collection_add_more" button
And I select "twitter" from "field_social_links_collection[und][1][field_social_link_type][und]"
And I fill in "field_social_links_collection[und][1][field_social_link_url][und][0][url]" with "https://twitter.com/cuboulder"
And I press "Save"
Then I should see "Social Links Block Title"
And I should see the link "Facebook"
And I should see the link "Twitter"
