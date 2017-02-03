Feature: Cu Settings

  @api @settings
  Scenario Outline: Content editors should not be able to access settings
    Given I am logged in as a user with the "content_editor" role
      And am on <path>
    Then I should see "Access Denied"

    Examples:
    | path |
    | "admin/settings" |
    | "admin/settings/contact" |
    | "admin/settings/feedback" |
    | "admin/settings/people" |
    | "admin/settings/redirects" |
    | "admin/config/search/redirect/add" |

  @api @settings
  Scenario Outline: An site owner/administrator/developer user should be able to access the settings page
    Given I am logged in as a user with the <role> role
    When I go to "admin/settings"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings
  Scenario Outline: An site owner/administrator/developer should be able to access the settings contact page
    Given I am logged in as a user with the <role> role
    When I go to "admin/settings/site-configuration/contact"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings
  Scenario Outline: An site owner/administrator/developer should be able to access the settings redirects page
    Given I am logged in as a user with the <role> role
    When I go to "admin/settings/site-configuration/redirects"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings @redirects
  Scenario Outline: An site owner/administrator/developer should be able to access the settings redirects page
    Given I am logged in as a user with the <role> role
    When I go to "admin/config/search/redirect/add"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings @clean_install
  Scenario: A site_owner should see a new title on the homepage
    Given I am logged in as a user with the "site_owner" role
      And am on "admin/settings/site-configuration/site-name"
      And fill in "Site name - line 1" with "New Site Title"
      And fill in "Site name - line 2 (optional)" with "Second line"
    When I press the "Save" button
      And I go to the homepage
    Then I should see "New Site Title Second Line"

  ## Not sure where livechat tests should go
  #@api @settings @livechat
  #Scenario: A site_owner should see livechat configs
  #  Given I am logged in as a user with the "site_owner" role
  #    And am on "admin/settings"
  #  Then I should see "LiveChatINC.com license number"

  #@api @settings @livechat @clean_install
  #Scenario: Livechat settings should not accept alpha characters
  #  Given I am logged in as a user with the "site_owner" role
  #    And am on "admin/settings"
  #    And fill in "LiveChatINC.com license number" with "abcdefg"
  #  When I press the "Save" button
  #  Then I should see "The livechat license number must only contain numbers."

  @api @settings @contact @clean_install
  Scenario: A site_owner should see contact info form
    Given I am logged in as a user with the "site_owner" role
      And am on "admin/settings/site-configuration/contact"
    Then I should see "Put your contact information here"

  @api @settings @contact @clean_install
  Scenario: A site_owner should be able to update the contact info form
    Given I am logged in as a user with the "site_owner" role
      And am on "admin/settings/site-configuration/contact"
      And fill in "edit-site-info-body-value" with "Put site contact information here"
    When I press the "Save" button
      And I go to the homepage
    Then I should see "Put site contact information here"

