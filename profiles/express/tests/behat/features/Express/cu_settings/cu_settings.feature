Feature: CU Settings
  An Express user should see the following options on the Express Settings pages.

  @api @settings
  Scenario Outline: Content editors should not be able to access settings
    Given  CU - I am logged in as a user with the "content_editor" role
      And am on <path>
    Then I should see "Access Denied"

    Examples:
    | path |
    | "admin/config/search/redirect/add"                   |
    | "admin/settings/site-configuration/site-name"        |
    | "admin/settings/site-configuration/site-description" |
    | "admin/settings/site-configuration/contact"          |
    | "admin/settings/site-configuration/google-analytics" |
    | "admin/settings/bundles/list"                        |
    | "admin/settings/cache/clear/varnish-full"            |

  @api @settings
  Scenario Outline: An site owner/administrator/developer user should be able to access the settings page
    Given  CU - I am logged in as a user with the <role> role
    When I go to "admin/settings"
    Then I should not see <message>

    Examples:
      | role           | message               |
      | content_editor | "Site Configurations" |
      
  @api @settings
  Scenario Outline: An site owner/administrator/developer user should be able to access the settings page
    Given  CU - I am logged in as a user with the <role> role
    When I go to "admin/settings"
    Then I should see <message>

    Examples:
      | role           | message               |
      | site_owner     | "Site Configurations" |
      | administrator  | "Site Configurations" |
      | developer      | "Site Configurations" |

  @api @settings
  Scenario Outline: An site owner/administrator/developer should be able to access the settings contact page
    Given  CU - I am logged in as a user with the <role> role
    When I go to "admin/settings/site-configuration/contact"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings
  Scenario Outline: An site owner/administrator/developer should be able to access the settings redirects page
    Given  CU - I am logged in as a user with the <role> role
    When I go to "admin/settings/site-configuration/redirects"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings @redirects
  Scenario Outline: An site owner/administrator/developer should be able to access the settings redirects page
    Given  CU - I am logged in as a user with the <role> role
    When I go to "admin/config/search/redirect/add"
    Then I should not see <message>

    Examples:
    | role           | message         |
    | site_owner     | "Access denied" |
    | administrator  | "Access denied" |
    | developer      | "Access denied" |

  @api @settings @clean_install
  Scenario: A site_owner should see a new title on the homepage
    Given  CU - I am logged in as a user with the "site_owner" role
      And am on "admin/settings/site-configuration/site-name"
      And fill in "Site name - line 1" with "New Site Title"
      And fill in "Site name - line 2 (optional)" with "Second line"
    When I press the "Save" button
      And I go to the homepage
    Then I should see "New Site Title Second Line"

  ## Not sure where livechat tests should go
  #@api @settings @livechat
  #Scenario: A site_owner should see livechat configs
  #  Given  CU - I am logged in as a user with the "site_owner" role
  #    And am on "admin/settings"
  #  Then I should see "LiveChatINC.com license number"

  #@api @settings @livechat @clean_install
  #Scenario: Livechat settings should not accept alpha characters
  #  Given  CU - I am logged in as a user with the "site_owner" role
  #    And am on "admin/settings"
  #    And fill in "LiveChatINC.com license number" with "abcdefg"
  #  When I press the "Save" button
  #  Then I should see "The livechat license number must only contain numbers."

  @api @settings @contact @clean_install
  Scenario: A site_owner should see contact info form
    Given  CU - I am logged in as a user with the "site_owner" role
      And am on "admin/settings/site-configuration/contact"
    Then I should see "Put your contact information here"

  @api @settings @contact @clean_install
  Scenario: A site_owner should be able to update the contact info form
    Given  CU - I am logged in as a user with the "site_owner" role
      And am on "admin/settings/site-configuration/contact"
      And fill in "edit-site-info-body-value" with "Put site contact information here"
    When I press the "Save" button
      And I go to the homepage
    Then I should see "Put site contact information here"

  @api @settings @cache
  # No Varnish on Travis
  Scenario Outline: A site_owner/administrator/developer should be able to see and use cache clearing.
    Given CU - I am logged in as a user with the <role> role
      When I go to "admin/settings"
    Then I should see <message>
    When I go to "admin/settings/cache/clear/drupal-full"
      #And I press "Clear Full Database Cache"
      #And I wait 15 seconds
    #The cache clear and reload of page is too slow to test the returned warning message.
    Then I should see "Repeatedly clearing caches will cause performance problems for you and your website's users, therefore full cache clears are limited to once per hour"
    When I go to "admin/settings/cache/clear/varnish-full"
      #And I press "Clear Full Page Cache"
    Then I should see "Repeatedly clearing caches will cause performance problems for you and your website's users, therefore full cache clears are limited to once per hour"
    When I go to "admin/settings/cache/clear/varnish-path"
      And I fill in "Path To Clear" with "node/1"
      #And I press "Clear Path From Page Cache"
    Then I should see "Enter the specific path or URL to clear from the Page cache."

  Examples:
  | role           | message        |
  | site_owner     | "Clear Caches" |
  | administrator  | "Clear Caches" |
  | developer      | "Clear Caches" |
  
  @api @settings @cache
  # No Varnish on Travis.
  Scenario: As a CE, I should be able to see and use page cache clearing by path.
    Given CU - I am logged in as a user with the "content_editor" role
    When I go to "admin/settings/cache/clear"
      And I click "Clear Page by Path"
      And I fill in "Path To Clear" with "node/1"
      #And I press "Clear Path From Page Cache"
    #Then I should see "The front page of a site can only be cleared by users with permission to clear the Full Page Cache."
    Then I should see "Enter the specific path or URL to clear from the Page cache."
    #When I go to "admin/settings/cache/clear/varnish-path"
      #And I fill in "Path To Clear" with "node/2"
      #And I press "Clear Path From Page Cache"
    #Then I should see <message>
  
  @api @settings @cache
  Scenario Outline: A content_editor/edit_my_content should not be able to see and use cache clearing.
    Given CU - I am logged in as a user with the <role> role
    When I go to "admin/settings/cache/clear/varnish-full"
      Then I should see <message>
    When I go to "admin/settings/cache/clear/varnish-path"
      Then I should see <message>
    When I go to "node/1/clear-varnish"
      Then I should see <message>

  Examples:
  | role             | message         |
  | edit_my_content  | "Access Denied" |
  | "authenticated user"    | "Access Denied" |
