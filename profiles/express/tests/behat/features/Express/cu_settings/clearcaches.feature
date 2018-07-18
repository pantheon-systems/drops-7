@settings
Feature: Cache Clearing
In order to get a page to show new edits
An authenticated user with the proper role
Should be able to clear the site caches

# ACCESSING THE CLEAR CACHES LANDING PAGE

Scenario Outline: Devs, Admins, SOs and ConMgrs can access the 'Clear Caches' page where they see four tabs
  Given I am logged in as a user with the <role> role
  And am on "admin/settings/cache/clear"
 Then I should see "Which Cache to Clear?"
  And I should see the link "Clear Page by Path"
  And I should see the link "Clear Page Full"
  And I should see the link "Clear Database Full"
    
Examples:
    | role            | 
    | developer       | 
    | administrator   | 
    | site_owner      |
    | configuration_manager |

Scenario Outline: Roles that can edit a page can access Clear Cache page where they see two tabs
Given I am logged in as a user with the <role> role
And am on "admin/settings/cache/clear"
Then I should see "Which Cache to Clear?"
And I should see the link "Clear Page by Path"
And I should not see "Clear Page Full"
And I should not see "Clear Database Full"

Examples:
| role |
| content_editor |
| site_editor |
| edit_only |

Scenario Outline: EMCs and AccMgrs cannot access the 'Clear Caches' landing page
Given I am logged in as a user with the <role> role
And am on "admin/settings/cache/clear"
Then I should see "Access denied"

Examples:
| role |
| access_manager |
| edit_my_content |


Scenario: An anonymous user should not be able to access the 'Clear Caches' landing page
 When I am on "admin/settings/cache/clear"
 Then I should see "Access denied"

# ACCESSING THE CLEAR-PAGE-FULL PAGE

Scenario Outline: Devs, Admins, SOs and ConMgrs can access the 'Clear Page Full' tag; CEs and EMCs cannot
  Given I am logged in as a user with the <role> role
  When I go to "admin/settings/cache/clear/varnish-full"
 Then I should see <message>
    
Examples:
    | role             | message |
    | developer        | "Repeatedly clearing caches will cause performance problems for you" |
    | administrator    | "Repeatedly clearing caches will cause performance problems for you" |
    | site_owner       | "Repeatedly clearing caches will cause performance problems for you" |
    | content_editor   | "Access denied" |
    | edit_my_content  | "Access denied" |
    | site_editor      | "Access denied" |
    | edit_only        | "Access denied" |
    | access_manager   | "Access denied" |
    | configuration_manager | "Repeatedly clearing caches will cause performance problems for you" |

# ACCESSING THE CLEAR-DATABASE-FULL PAGE
Scenario Outline: Devs, Admins, SOs and ConMgrs can access the 'Clear Database Full' tag; CEs and EMCs cannot
  Given I am logged in as a user with the <role> role
  When I go to "admin/settings/cache/clear/drupal-full"
 Then I should see <message>
    
Examples:
    | role             | message |
    | developer        | "Repeatedly clearing caches will cause performance problems for you" |
    | administrator    | "Repeatedly clearing caches will cause performance problems for you" |
    | site_owner       | "Repeatedly clearing caches will cause performance problems for you" |
    | content_editor   | "Access denied" |
    | edit_my_content  | "Access denied" |
    | site_editor      | "Access denied" |
    | edit_only        | "Access denied" |
    | access_manager   | "Access denied" |
    | configuration_manager | "Repeatedly clearing caches will cause performance problems for you" |

@broken
Scenario: The proper status message is displayed when Full Database Cache is cleared
  Given I am logged in as a user with the "site_owner" role
  When I go to "admin/settings/cache/clear/drupal-full"
  And I press "Clear Full Database Cache"
  And I wait for the ".messages.status" element to appear
  Then I should see "Full Database Cache Cleared"
  And the "#edit-clear-drupal-cache" element should have "disabled" in the "disabled" attribute
  
# NOTE: NO VARNISH ON TRAVIS; NO USE TESTING
# TESTING THE CLEAR-PAGE-BY-PATH FUNCTIONALITY
# 
#  Scenario Outline: Devs, Admins, SOs and CEs can Clear Page by Path.
#  Given I am logged in as a user with the <role> role
#  When I go to "admin/settings/cache/clear/varnish-path"
#  Then I should see "Enter the specific path or URL to clear from the Page cache."
#  And I fill in "edit-clear-varnish-path-cache-path" with <testpath>
#  And I press "edit-clear-varnish-path-cache"
#   Then the response should contain "class=\"messages status\""  
#   And I should see a "div" element with the "class" attribute set to "messages status"
   
#Examples:
#    | role           | testpath   |
#    | developer      | "node/123" |
#    | administrator  | "node/234" |
#    | site_owner     | "node/345" |
#    | content_editor | "node/456" |
 
# NOTE: NO VARNISH ON TRAVIS 
# THE PROPER STATUS MESSAGE IS DISPLAYED WHEN FULL PAGE CACHE IS CLEARED
# 
# Scenario: Clearing Full Page Cache is limited to once per hour 
#  Given I am logged in as a user with the "site_owner" role
#  When I go to "admin/settings/cache/clear/varnish-full"
#  And I press "edit-clear-varnish-cache"
#  Then I should see "The whole Page Cache was recently cleared"
#  And the "#edit-clear-varnish-cache" element should have "disabled" in the "disabled" attribute
