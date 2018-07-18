@blocks @slider @core
Feature: Slider Block
When I login to a Web Express website
As an authenticated user
I should be able to create, edit, and delete a slider block

# 1) TEST BLOCK ADD PRIVILEGES
# 2) TEST THAT A SIMPLE BLOCK CAN BE CREATED AND REVISED
# 3) TEST EDITING AND DELETING PRIVILEGES ON THE BLOCK JUST MADE
# 4) TEST THAT THE DELETE BUTTON ACTUALLY WORKS
# 5) TEST MORE COMPLEX BLOCK CREATION

# 1) TEST BLOCK ADD PRIVILEGES
Scenario Outline: Block Access: Some roles can add a Slider block
Given I am logged in as a user with the <role> role
When I go to "block/add/slider"
Then I should see <message>

Examples:
 | role                  | message      |
 | developer             | "Create Slider block" |
| administrator         | "Create Slider block" |
| site_owner            | "Create Slider block" |
| content_editor        | "Create Slider block" |
| edit_my_content       | "Access denied"       |
| site_editor           | "Create Slider block" |
| edit_only             | "Access denied"       |

 Scenario: Block Access: An anonymous user cannot add a Slider block
  When I am on "block/add/slider"
  Then I should see "Access denied"
  
# 2) TEST THAT A SIMPLE BLOCK CAN BE CREATED AND REVISED
Scenario: Block Functionality - A very simple Slider can be created 
 Given I am logged in as a user with the "site_owner" role
  And I am on "block/add/slider"
  And fill in "edit-label" with "Slider Label"
  And fill in "edit-title" with "My Slider Title"
  And I fill in "edit-field-slider-slide-und-0-field-slider-image-und-0-alt" with "Mountain Fantasy"
And I attach the file "behatBanner1.jpg" to "edit-field-slider-slide-und-0-field-slider-image-und-0-upload"
 When I press "edit-submit"
  Then I should be on "block/slider-label/view"
 And I should see "My Slider Title"
 
#  2.5 CREATE REVISIONS TO THE BLOCK ABOVE
#Scenario: Block Functionality - Create Revision of block
#Given I am logged in as a user with the "site_owner" role
#And I am on "admin/content/blocks"
#And I follow "Slider Label"
#And I follow "Edit Block"
# - And I do stuff to Block content - 
# And I press "Save"
 #Then I should see "Slider My Slider Title has been updated."

# 3) TEST EDITING AND DELETING PRIVILEGES ON THE BLOCK JUST MADE

Scenario Outline: Block Access - SE, SO and above roles can edit, revise, theme and delete Slider 
Given I am logged in as a user with the <role> role
And I am on "admin/content/blocks"
And I follow "Slider Label"
Then I should see the link "View"
And I should see the link "Edit Block"
And I should see the link "Revisions" 
And I should see the link "Block Designer"
And I should see the link "Delete Block"
When I follow "Edit Block"
Then I should see "Edit Slider: Slider Label"
And I should see an "#edit-delete" element
And I follow "View"

Examples: 
| role |
| developer       | 
| administrator   | 
| site_owner      | 
| content_editor  |
| site_editor |


Scenario: Block Access - The EditOnly role can edit, revise, theme but not delete Slider content
Given I am logged in as a user with the "edit_only" role
And I am on "admin/content/blocks"
And I follow "Slider Label"
Then I should see the link "View"
And I should see the link "Edit Block"
And I should see the link "Revisions"
And I should see the link "Block Designer"
And I should not see the link "Delete Block"
When I follow "Edit Block"
Then I should see "Edit Slider: Slider Label"
And I should not see an "#edit-delete" element
And I follow "View"

Scenario: Block Access - EditMyContent cannot access or edit block content
Given I am logged in as a user with the "edit_my_content" role
And I am on "admin/content/blocks"
Then I should see "Access denied"
And I go to "block/slider-label/edit"
Then I should see "Access denied"

# 4) TEST THAT THE DELETE BUTTON ACTUALLY WORKS
Scenario: Verify that the Delete button actually works
 Given I am logged in as a user with the "site_owner" role
And I go to "block/slider-label/edit"
 And I press "Delete"
 Then I should see "Are you sure you want to delete My Slider Title?"
  And I press "Delete"
 Then I should see "Slider My Slider Title has been deleted"
And I am on "/"

# 5) TEST MORE COMPLEX BLOCK CREATION
