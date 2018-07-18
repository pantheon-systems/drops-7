@page @core
Feature: Basic Page Content Type
When I login to a Web Express website
As an authenticated user
I should be able to create, edit, and delete Basic Pages

  # NOTE: THERE IS CURRENTLY NO KNOWN WAY TO PRESS THE INSERT BUTTON; GRAPHICS CAN BE UPLOADED BUT NOT INSERTED
  # CONTINUE TESTING THIS PROBLEM WITH PHOTOINSERT.FEATURE

  # 1) CHECK NODE ADD PRIVILEGES
  # 2) CHECK THAT SIMPLE NODE CAN BE CREATED AND REVISED
  # 3) CHECK EDITING AND DELETING PRIVILEGES ON THE CONTENT JUST MADE
  # 4) CHECK THAT DELETE BUTTON ACTUALLY WORKS
  # 5) CHECK MORE COMPLEX NODE CREATION

  Scenario: A site_owner can assign an author on node/%/edit
    Given I am logged in as a user with the "site_owner" role
    And I am on "node/add/page"
    And I fill in "Title" with "edit_my_content Page"
    And I fill in "Body" with "edit_my_content page content"
    And I fill in "Authored by" with "edit_my_content"
    And I uncheck "Generate automatic URL alias"
    And I fill in "URL alias" with "edit-my-content-page"
    When I press "Save"
    Then I should see "edit_my_content Page"


  # 1) CHECK NODE ADD PRIVILEGES
  Scenario Outline: Node Access - Some roles can add Basic Page content
    Given I am logged in as a user with the <role> role
    When I go to "node/add/page"
    Then I should see <message>

    Examples:
      | role            | message             |
      | developer       | "Create Basic page" |
      | administrator   | "Create Basic page" |
      | site_owner      | "Create Basic page" |
      | content_editor  | "Create Basic page" |
      | edit_my_content | "Access denied"     |
      | site_editor     | "Create Basic page" |
      | edit_only       | "Access denied"     |

  Scenario: Node Access -  An anonymous user cannot add Basic Page content
    When I am on "node/add/page"
    Then I should see "Access denied"
  
  #  2) CHECK THAT SIMPLE NODE CAN BE CREATED
  Scenario: Node Functionality - A very basic Basic Page node can be created
    Given I am logged in as a user with the "site_owner" role
    And I am on "node/add/page"
    And fill in "edit-title" with "My Page"
    And fill in "Body" with "Lorem ipsum dolor sit amet"
    When I press "edit-submit"
    Then I should be on "/my-page"
    And I should see "My Page"
    And I should see "Lorem ipsum dolor sit amet"
 
  # 2.1) CHANGE AUTHOR OF THE PAGE NODE
  @javascript
  Scenario: Node functionality - Change Authorship of node on admin/content
    Given I am logged in as a user with the "developer" role
    And I am on "admin/content"
    And I check "edit-views-bulk-operations-0"
    And I select "Change the author of content" from "edit-operation"
    And I press "Execute"
    Then I should see "The username of the user to which you would like to assign ownership."
    And I select "edit_my_content" from "edit-owner-name"
    And I press "Next"
    Then I should see "Are you sure you want to perform Change the author of content on the selected items"
    And I press "Confirm"
    Then I should see "Performed Change the author of content on 1 item"

  # @todo write test for this.
  # Scenario: Node functionality - Change Authorship of node on on node/%/edit

  # 2.2) CREATE REVISION OF NODE
  Scenario: Node functionality - Create Revision of node
    Given I am logged in as a user with the "developer" role
    And I am on "admin/content"
    And I follow "My Page"
    And I follow "Edit"
    And fill in "Body" with "A world class university"
    When I press "edit-submit"
    Then I should see "Basic page My Page has been updated"
    And I should see the link "Revisions"

  # 3) CHECK EDITING AND DELETING PRIVILEGES ON THE CONTENT JUST MADE
  Scenario Outline: Node Access -  Some roles can edit and delete Basic Page content
    Given I am logged in as a user with the <role> role
    And I am on "admin/content"
    And I follow "My Page"
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
      | role           |
      | developer      |
      | administrator  |
      | site_owner     |
      | content_editor |
      | site_editor    |

  Scenario: Node Access -  EditOnly can edit and revise but not delete node; can clear page cache
    Given I am logged in as a user with the "edit_only" role
    And I am on "admin/content"
    And I follow "My Page"
    Then I should see the link "View"
    And I should see the link "Edit"
    And I should not see the link "Edit Layout"
    And I should see the link "Revisions"
    And I should see the link "Clear Page Cache"
    When I follow "Edit"
    Then I should see "This document is now locked against simultaneous editing."
    And I should not see an "#edit-delete" element
    And I press "Cancel edit"

  # Page assigned to edit_my_content role is assigned on cu_page.feature:17
  Scenario: Node Access -  EditMyContent can edit Basic Pages and Persons if owner; cannot delete; can clear page cache
    Given I am logged in as a user with the "edit_my_content" role
    And I am on "edit-my-content-page"
    Then I should see the link "View"
    And I should see the link "Edit"
    And I should not see the link "Edit Layout"
    And I should not see the link "Clear Page Cache"
    When I follow "Edit"
    # @todo move this to a locked document test or remove as a duplicate.
    # Then I should see "This document is now locked against simultaneous editing."
    Then I should not see an "#edit-delete" element
    When I fill in "Body" with "changing content..."
    And I press "Save"
    Then I should see "changing content..."
    When I follow "Edit"
    Then I should see the link "Revisions"

  # 4) CHECK THAT DELETE BUTTON ACTUALLY WORKS
  Scenario: Verify that the Delete button actually works
    Given I am logged in as a user with the "site_owner" role
    And I am on "admin/content"
    And I follow "My Page"
    And I follow "Edit"
    And I press "Delete"
    Then I should see "Are you sure you want to delete My Page?"
    And I press "Delete"
    Then I should see "Basic page My Page has been deleted."
    And I am on "/"

  # 5) CHECK MORE COMPLEX NODE CREATION
  Scenario: A graphic can be uploaded to a Basic Page node
    Given I am logged in as a user with the "site_owner" role
    And I am on "node/add/page"
    And fill in "edit-title" with "About Ralphie"
    And fill in "Body" with "Ralphie the Buffalo is the name of the live mascot of the University of Colorado Buffaloes."
    And I fill in "edit-field-photo-und-0-alt" with "Ralphie Buffalo with handlers"
    And I attach the file "ralphie.jpg" to "edit-field-photo-und-0-upload"
    And I press "edit-field-photo-und-0-upload-button"
    Then I should see "File information"
    And I should see "ralphie.jpg"
    And I should see "Insert"
    And I press "edit-submit"
    Then I should be on "/about-ralphie"
    And I should see "About Ralphie"
    And I should see "Ralphie the Buffalo is the name of the live mascot of the University of Colorado Buffaloes."

  Scenario: The provide menu link box should be checked on node creation but remain unchecked if user chooses to uncheck that box.
    Given I am logged in as a user with the "site_owner" role
    When I go to "node/add/page"
    And I fill in "edit-title" with "Not In Menu"
    Then the "edit-menu-enabled" checkbox should be checked
    When I uncheck "edit-menu-enabled"
    And I press "Save"
    # ADD CHECK FOR PAGE TITLE
    Then I should see "Not In Menu"
    And I follow "Edit"
    Then the checkbox "edit-menu-enabled" should be unchecked
