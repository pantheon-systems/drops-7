@file @core
Feature: File Content Type
When I login to a Web Express website
As an authenticated user
I should be able to create, edit, and delete File content

# 1) TEST NODE ADD PRIVILEGES
# 2) TEST THAT A SIMPLE NODE CAN BE CREATED AND REVISED
# 3) TEST EDITING AND DELETING PRIVILEGES ON THE NODE JUST MADE
# 4) TEST THAT THE DELETE BUTTON ACTUALLY WORKS
# 5) TEST MORE COMPLEX NODE CREATION

# 1) TEST NODE ADD PRIVILEGES
Scenario Outline: Node Access - Some roles can add File content
Given I am logged in as a user with the <role> role
When I go to "node/add/file"
Then I should see <message>

Examples:
 | role                  | message                      |
 | developer             | "Create File" |
| administrator         | "Create File" |
| site_owner            | "Create File" |
| content_editor        | "Create File" |
| edit_my_content       | "Access denied"    |
| site_editor           | "Create File" |
| edit_only             | "Access denied"    |

 Scenario: FAQ Access -  An anonymous user cannot add File content
  When I am on "node/add/file"
  Then I should see "Access denied"
  
# 2) TEST THAT A SIMPLE NODE CAN BE CREATED AND REVISED
 Scenario: Node Functionality - A simple File can be created; with secure https URL
 Given I am logged in as a user with the "site_owner" role
  And I am on "node/add/file"
  And fill in "edit-title" with "My File"
  And I fill in "edit-body-und-0-value" with "A photo of Ralphie and handlers"
  And I attach the file "ralphie.jpg" to "edit-field-file-attachment-und-0-upload"
  And I press "Upload"
    # And I wait for AJAX
  Then I should see "File Information"
  And I should see "Operations"
  And I press "edit-submit"
  And I should see "A user without editing permissions would have been redirected"
  And I should see "My File"
  And I should see "Access the top file listed below with the following url"
# NEXT LINE: CHECKING FOR HTTPS://
  And I should not see "http://www.colorado.edu"
 
#  2.5 CREATE REVISIONS TO THE NEW NODE
Scenario: Node functionality - Create Revision of File node
Given I am logged in as a user with the "site_owner" role
And I am on "admin/content"
And I follow "My File"
And I follow "Edit"
 And I fill in "body[und][0][value]" with "A Scenic Photo"
 And I press "Save"
 Then I should see "File My File has been updated."
  And I should see the link "Revisions"

# 3) TEST EDITING AND DELETING PRIVILEGES ON THE NODE JUST MADE

Scenario Outline: Node Access -  Some roles can edit and delete File content
Given I am logged in as a user with the <role> role
And I am on "admin/content"
And I follow "My File"
Then I should see the link "View"
And I should see the link "Edit"
And I should see the link "Revisions"
And I should see the link "Clear Page Cache"
When I follow "Edit"
Then I should see "This document is now locked against simultaneous editing."
And I should see an "#edit-delete" element
And I press "Cancel edit"

Examples: 
| role |
| developer       | 
| administrator   | 
| site_owner      | 
| content_editor  |
| site_editor |

Scenario: Node Access -  EditOnly can edit and revise but not delete File; can clear page cache
Given I am logged in as a user with the "edit_only" role
And I am on "admin/content"
And I follow "My File"
Then I should see the link "View"
And I should see the link "Edit"
And I should see the link "Revisions"
And I should see the link "Clear Page Cache"
When I follow "Edit"
Then I should see "This document is now locked against simultaneous editing."
And I should not see an "#edit-delete" element
And I press "Cancel edit"

Scenario: Node Access - EditMyContent can not edit File nodes
Given I am logged in as a user with the "edit_my_content" role
And I am on "admin/content"
And I follow "My File"
Then the url should match "sites/default/files/attached-files"

# 4) TEST THAT THE DELETE BUTTON ACTUALLY WORKS
 Scenario: Node Functionality - Verify that the Delete button actually works
 Given I am logged in as a user with the "site_owner" role
And I am on "admin/content"
And I follow "My File"
And I follow "Edit"
 And I press "Delete"
 Then I should see "Are you sure you want to delete My File?"
 And I press "Delete"
 Then I should see "File My File has been deleted"
And I am on "/"


# 5) TEST MORE COMPLEX NODE CREATION

 Scenario: Node Functionality - The File Content Type verifies that a file has been uploaded
 Given I am logged in as a user with the "site_owner" role
  When I go to "node/add/file"
   And I fill in "edit-title" with "Test Page"
    And I fill in "body[und][0][value]" with "Do not keep this page"
    And I press "Save"
   Then I should see "File Attachment field is required."
    
