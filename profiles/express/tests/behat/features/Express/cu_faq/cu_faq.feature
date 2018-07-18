@faqs @core
Feature: Frequently Asked Questions Content Type
When I login to a Web Express website
As an authenticated user
I should be able to create, edit, and delete FAQ content

# 1) CHECK NODE ADD PRIVILEGES
# 2) CHECK THAT A SIMPLE NODE CAN BE CREATED AND REVISED
# 3) CHECK EDITING AND DELETING PRIVILEGES ON THE NODE JUST MADE
# 4) CHECK THAT THE DELETE BUTTON ACTUALLY WORKS
# 5) CHECK MORE COMPLEX NODE CREATION

# 1) CHECK NODE ADD PRIVILEGES
Scenario Outline: Node Access - Some roles can add FAQ content
Given I am logged in as a user with the <role> role
When I go to "node/add/faqs"
Then I should see <message>

Examples:
| role                  | message                      |
| developer             | "Create Frequently Asked Questions" |
| administrator         | "Create Frequently Asked Questions" |
| site_owner            | "Create Frequently Asked Questions" |
| content_editor        | "Create Frequently Asked Questions" |
| edit_my_content       | "Access denied"                     |
| site_editor           | "Create Frequently Asked Questions" |
| edit_only             | "Access denied"                     |

Scenario: FAQ Access -  An anonymous user cannot add FAQ content
  When I am on "node/add/faqs"
  Then I should see "Access denied"
  
# 2) CHECK THAT A SIMPLE NODE CAN BE CREATED AND REVISED
Scenario: Node Functionality - a simple FAQ node can be created
Given I am logged in as a user with the "site_owner" role
 And I am on "node/add/faqs"
 And fill in "edit-title" with "My FAQs"
 And fill in "Body" with "Lorem ipsum dolor sit amet"
 When I press "edit-submit"
Then I should be on "/my-faqs"
And I should see "My FAQs"
And I should see "Lorem ipsum dolor sit amet"
 
#  2.5 CREATE REVISIONS TO THE NEW NODE
Scenario: Node functionality - Create Revision of FAQ
Given I am logged in as a user with the "site_owner" role
And I am on "admin/content"
And I follow "My FAQs"
And I follow "Edit"
 # BROKEN AT THIS TIME And fill in "edit-name" with "osr-test-edit-own" 
  And fill in "Body" with "Find out more here"
 And I press "Save"
 Then I should see "Frequently Asked Questions My FAQs has been updated."
  And I should see the link "Revisions"

# 3) CHECK EDITING AND DELETING PRIVILEGES ON THE NODE JUST MADE

Scenario Outline: Node Access -  Some roles can edit and delete FAQ
Given I am logged in as a user with the <role> role
And I am on "admin/content"
And I follow "My FAQs"
Then I should see the link "View"
And I should see the link "Edit"
And I should see the link "Edit Layout"
And I should see the link "Revisions"
And I should see the link "Clear Page Cache"
When I follow "Edit"
Then I should see "This document is now locked against simultaneous editing."
And I should see an "#edit-delete" element
And I press "Cancel edit"

Examples: 
| role         |
| developer    |    
| administrator |   
| site_owner    | 
| content_editor |
| site_editor |

Scenario: Node Access -  EditOnly can edit and revise but not delete FAQ; can clear page cache
Given I am logged in as a user with the "edit_only" role
And I am on "admin/content"
And I follow "My FAQs"
Then I should see the link "View"
And I should see the link "Edit"
And I should not see the link "Edit Layout"
And I should see the link "Revisions"
And I should see the link "Clear Page Cache"
When I follow "Edit"
Then I should see "This document is now locked against simultaneous editing."
And I should not see an "#edit-delete" element
And I press "Cancel edit"

Scenario: Node Access -  EditMyContent can not edit FAQs
Given I am logged in as a user with the "edit_my_content" role
And I am on "admin/content"
And I follow "My FAQs"
Then I should see the link "View"
And I should not see the link "Edit"
And I should not see the link "Clear Page Cache"

# 4) CHECK THAT THE DELETE BUTTON ACTUALLY WORKS

Scenario: Verify that the Delete button actually works
Given I am logged in as a user with the "site_owner" role
And I am on "admin/content"
And I follow "My FAQs"
And I follow "Edit"
And I press "Delete"
Then I should see "Are you sure you want to delete My FAQs?"
And I press "Delete"
Then I should see "Frequently Asked Questions My FAQs has been deleted."
And I am on "/"


# 5) CHECK MORE COMPLEX NODE CREATION
 
Scenario: Node Functionality - a more complicated FAQ node can be created
Given I am logged in as a user with the "site_owner" role
    And I am on "node/add/faqs"
    And fill in "edit-title" with "My New FAQ Page"
    And fill in "edit-body-und-0-value" with "Demo FAQ explanatory text"
    And fill in "edit-field-qa-collection-und-0-field-qa-collection-title-und-0-value" with "Section One Header"
    And fill in "edit-field-qa-collection-und-0-field-qa-und-0-field-qa-question-und-0-value" with "Question One"
    And fill in "edit-field-qa-collection-und-0-field-qa-und-0-field-qa-answer-und-0-value" with "An Answer to the Question"
    And press "Save"
    Then I should see "My New FAQ Page"
    And I should see "Demo FAQ explanatory text"
    And I should see "Section One Header"
    And I should see "Question One"
    # THIS LINE FAILS But I should not see "An Answer to the Question"
   
Scenario: Node Functionality -  Pressing "Add More" adds another FAQ section
Given I am logged in as a user with the "site_owner" role
And I am on "node/add/faqs"
When I press "edit-field-qa-collection-und-0-field-qa-und-add-more"
And I wait 5 seconds
#THIS IS THE ID FOR THE TITLE OF THE NEW QUESTION
Then the response should contain "id=\"edit-field-qa-collection-und-0-field-qa-und-1-field-qa-question-und-0-value\""
# THIS DOESN'T WORK Then I should see an "edit-field-qa-collection-und-0-field-qa-und-1-field-qa-question-und-0-value" element

And I press "edit-field-qa-collection-und-add-more"
And I wait 5 seconds
#THIS IS THE ID FOR THE TITLE OF THE NEW FAQ SECTION
Then the response should contain "id=\"edit-field-qa-collection-und-1-field-qa-collection-title-und-0-value\""
# THIS DOESN'T WORK Then I should see an "edit-field-qa-collection-und-1-field-qa-collection-title-und-0-value" element
 Scenario: The provide menu link box should be checked on node creation but remain unchecked if user chooses to uncheck that box.
    Given  I am logged in as a user with the "site_owner" role
    When I go to "node/add/faqs"
    And  I fill in "edit-title" with "New FAQ"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck "edit-menu-enabled"
    And I press "Save"
    And I follow "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked
