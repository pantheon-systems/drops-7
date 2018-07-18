#@status_checks
#Feature: Site Install Status Checks
#  When the site is installed, there should be no errors.
#
#  @run_first @javascript
#  Scenario: On install the status report page shouldn't show any errors.
#    Given I am logged in as a user with the "developer" role
#    When I go to "admin/reports/status"
#    Then the response should not contain "<tr class=\"error\">"
#      And the response should not contain "<div class=\"messages error\">"
