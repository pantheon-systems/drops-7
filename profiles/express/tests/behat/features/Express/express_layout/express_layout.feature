Feature: CU Layout Creation

  Background:
    Given  CU - I am logged in as a user with the "developer" role
    When I disable the "overlay" module
      And I go to "admin/config/content/express-layout"
      And I check the box "page[field_header]"
      And I uncheck the box "page[field_intro]"
      And I press the "Save Layout Settings" button
    When I go to "node/add/page"
      And I fill in "Title" with "Layout Page"
      And I press the "Save" button

  @api @layout @javascript
  Scenario: Adding or removing regions on settings form should be reflected on node layout forms
    When I click "Edit Layout"
      And I should not see an "#edit-field-intro" element
    Then I should see an "#edit-field-header" element


  @api @layout @javascript
  Scenario: Adding a block in the content region should appear in the region and deleting it should delete it from region
    When I click "Edit Layout"
      And I select "block" from "field_header[und][actions][bundle]"
      And I wait for AJAX
      And I fill in "Text Block Label" with "above content block"
      And I click "Disable rich-text"
      And I fill in "Body" with "above content block"
      And I press "Create block"
      And I wait for AJAX
      And I press "Update layout"
    Then I should see "above content block" in the "Content" region
    When I click "Edit Layout"
      And I click the "#edit-field-header-und-entities-0-actions-ief-entity-remove" element
      And I wait for AJAX
      And I click the "#edit-field-header-und-entities-0-form-actions-ief-remove-confirm" element
      And I wait for AJAX
      And I press "Update layout"
    Then I should not see "above content block" in the "Content" region
    ## @TODO Get autocomplete suggestion to work
    #When I click "Edit Layout"
      #And I click the "#edit-field-sidebar-first-und-actions-ief-add-existing" element
      #And I wait for AJAX
      #And I fill in "field_sidebar_first[und][form][entity_id]" with "ab"
      #And I wait 5 seconds
      #And I select autosuggestion option "above content block (8)"

  @api @layout @javascript
  Scenario: Adding a block in the left sidebar region should appear in the region
    When I click "Edit Layout"
      And I select "block" from "field_sidebar_first[und][actions][bundle]"
      And I wait for AJAX
      And I fill in "Text Block Label" with "left sidebar block"
      And I click "Disable rich-text"
      And I fill in "Body" with "left sidebar block"
      And I press "Create block"
      And I wait for AJAX
      And I press "Update layout"
    Then I should see "left sidebar block" in the "Sidebar First" region

  @api @layout @javascript
  ## @TODO Get autocomplete suggestion to work
  #Scenario: You should be able to add an existing block to the page layout
    #When I click "Edit Layout"
      #And I click the "#edit-field-sidebar-first-und-actions-ief-add-existing" element
      #And I wait for AJAX
      #And I fill in "field_sidebar_first[und][form][entity_id]" with "abo"
      #And I wait 5 seconds
      #And I select autosuggestion option "above content block"

