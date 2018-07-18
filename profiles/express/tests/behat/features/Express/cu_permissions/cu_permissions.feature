Feature: CU Permissions

  @cu_permissions
  Scenario Outline: An site owner/administrator/content editor user should not be able to access certain admin settings
    Given  I am logged in as a user with the <role> role
    When I go to "admin/index"
    Then I should not see "<message>"
    And I should not see "<message1>"

    Examples:
      | role           |   message     | message1                |
      | site_owner     | jQuery Update | Express Layout Settings |
      | administrator  | jQuery Update | Express Layout Settings |
      | content_editor | jQuery Update | Express Layout Settings |


  @cu_permissions
  Scenario: A developer should be able to access certain admin settings
    Given  I am logged in as a user with the "developer" role
    When I go to "admin/index"
    Then I should see "jQuery Update"
    Then I should see "Express Layout Settings"
