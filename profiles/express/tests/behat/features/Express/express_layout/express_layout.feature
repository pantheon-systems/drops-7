Feature: CU Layout Creation
  Users should be able to create and edit layouts. Developers should be able to modify layout settings.

  @layout @javascript
  Scenario: Adding or removing regions on settings form should be reflected on node layout forms
    Given  I am logged in as a user with the "developer" role
      And I go to "admin/config/content/express-layout"
      And I check "edit-page-field-header"
      And I click the "#edit-page-field-intro" element
      And I press "Save Layout Settings"
    When I go to "node/1"
      And I follow "Edit Layout"
    Then I should not see an "#edit-field-intro" element
      And I should see an "#edit-field-header" element

  @layout @javascript
  Scenario: Adding a block in the content region should appear in the region and deleting it should delete it from region
    Given  I am logged in as a user with the "developer" role
    When I go to "node/1"
      And I follow "Edit Layout"
      And I select "block" from "field_header[und][actions][bundle]"
      And I wait for the ".ief-form" element to appear
      And I fill in "Text Block Label" with "above content block"
      And I follow "Disable rich-text"
      And I fill in "Body" with "above content block"
      And I press "Create block"
      And I press "Update layout"
    Then I should see "above content block"
    When I follow "Edit Layout"
      And I click the "#edit-field-header-und-entities-0-actions-ief-entity-remove" element
      And I wait for the "#edit-field-header-und-entities-0-form-actions-ief-remove-confirm" element to appear
      And I click the "#edit-field-header-und-entities-0-form-actions-ief-remove-confirm" element
      And I press "Update layout"
    Then I should not see "above content block"
    ## @TODO Get autocomplete suggestion to work
    #When I click "Edit Layout"
      #And I click the "#edit-field-sidebar-first-und-actions-ief-add-existing" element
      #And I wait for AJAX
      #And I fill in "field_sidebar_first[und][form][entity_id]" with "ab"
      #And I wait 5 seconds
      #And I select autosuggestion option "above content block (8)"

  @layout @javascript
  Scenario: Adding a block in the left sidebar region should appear in the region
    Given  I am logged in as a user with the "developer" role
    When I go to "node/1"
      And I follow "Edit Layout"
      And I select "block" from "field_sidebar_first[und][actions][bundle]"
      And I wait for the ".ief-form" element to appear
      And I fill in "Text Block Label" with "left sidebar block"
      And I follow "Disable rich-text"
      And I fill in "Body" with "left sidebar block"
      And I press "Create block"
      And I press "Update layout"
    Then I should see "left sidebar block"

  @layout @javascript @broken
  ## @TODO Get autocomplete suggestion to work
  #Scenario: You should be able to add an existing block to the page layout
    #When I click "Edit Layout"
      #And I click the "#edit-field-sidebar-first-und-actions-ief-add-existing" element
      #And I wait for AJAX
      #And I fill in "field_sidebar_first[und][form][entity_id]" with "abo"
      #And I wait 5 seconds
      #And I select autosuggestion option "above content block"

