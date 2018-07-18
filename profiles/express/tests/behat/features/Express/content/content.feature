# Checking /admin/content for functionality
# Content: Four tabs: Content, Blocks and Locked documents
# Content is sortable by Title, Type, Author, and Updated Date
# NOTE: SORT BY AUTHOR IS TURNED OFF IN 2.8.5

 @extended_search @rebuild
 Feature: Content page allows viewing and sorting of content
  When I go to the Admin/Content page
  As an authenticated user
  I should be able to view, sort and add content


  Scenario Outline: Devs, Admins and SOs get four tabs and 'Add content' link
    Given  I am logged in as a user with the <role> role
    When I go to "admin/content"
    And I should see the link "Content"
    And I should see the link "Blocks"
    # @todo Move to forms bundle.
    # And I should see the link "Webforms"
    And I should see the link "Locked documents"
    And I should see the link "Add content"

    Examples:
    | role            | 
    | developer       |
    | administrator   |
    | site_owner      |


 Scenario: Content Editors get two tabs and and 'Add content' link
    Given  I am logged in as a user with the "content_editor" role
    When I go to "admin/content"
    And I should see the link "Content"
    And I should see the link "Blocks"
    # LOCKED DOCUMENT TAB IS TURNED OFF FOR NOW
   # But I should not see the link "Locked documents"
    And I should see the link "Add content"


 Scenario: Edit_My_Content editors get no tabs; no 'Add content' link
    Given  I am logged in as a user with the "edit_my_content" role
    When I go to "admin/content"
    And I should not see the link "Blocks"
    # LOCKED DOCUMENT TAB IS TURNED OFF FOR NOW
    # But I should not see the link "Locked documents"
    But I should not see the link "Add content"

    

 Scenario: An anonymous user should not be able to access the form for adding page content
    When I am on "admin/content"
    Then I should see "Access denied"


  Scenario Outline: All authenticated users should see the additional fields for finding and sorting content
    Given I am logged in as a user with the <role> role
    When I go to "admin/content"
    Then I should see "Title"
     And I should see an "#edit-title" element
      And I should see "Type"
       And I should see an "#edit-type" element
      And I should see "Published"
      And I should see an "#edit-status" element
      And I should see "Author"
       And I should see an "#edit-realname" element
      And I should see an "#edit-submit-cu-content-administration-override-view" element
      And I should see a ".views-reset-button" element
      And I should see the link "sort by Title"
      And I should see the link "sort by Type"
     # SORTING BY AUTHOR TURNED OFF FOR NOW
     # And I should see the link "sort by Author"
      And I should see the link "sort by Updated date"
      
    Examples:
    | role            | 
    | developer       |
    | administrator   |
    | site_owner      |
    | content_editor  |
    | edit_my_content |
