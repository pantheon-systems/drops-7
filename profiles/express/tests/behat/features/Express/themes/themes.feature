Feature: Themes

@api @themes @clean_install
Scenario: As a site_owner I should see all available themes
  Given I am logged in as a user with the "site_owner" role
    And am on "admin/theme"
  Then I should see "Modern"
    And I should see "Ivory"
    And I should see "Minimal"

@api @themes @clean_install
Scenario: As a content_editor I should not be able to change themes
  Given I am logged in as a user with the "content_editor" role
    And am on "admin/theme"
  Then I should see "Access Denied"

@api @themes @clean_install
Scenario: As a site_owner I should not see jquery theme settings
  Given I am logged in as a user with the "site_owner" role
    And am on "admin/theme/config/cumodern"
  Then I should not see "jQuery Update"
