# SEE ARTICLES FOR TAXONOMY PERMISSION TESTS

 @people
Feature: People Taxonomy and Filters
In order to group and filter my people
As an authenticated user
I should be able to add taxonomy and rename filters

Scenario: The People Vocabularies are added to Structure/Taxonomy when bundle is enabled
Given I am logged in as a user with the "site_owner" role
And I am on "admin/structure/taxonomy"
Then I should see "Department"
And I should see "Job Type"
And I should see "People Filter 1"
And I should see "People Filter 2"
And I should see "People Filter 3"
 
Scenario: The People Filter Labels can be renamed
Given I am logged in as a user with the "site_owner" role
And I am on "admin/settings/people/settings"
And I fill in "edit-department-label" with "Division"
And I fill in "edit-type-label" with "Appointment"
And I fill in "edit-filter-one-label" with "Research Group"
And I fill in "edit-filter-two-label" with "Area of Expertise"
And I fill in "edit-filter-three-label" with "Committees"
And I press "edit-submit"
Then I should see "Settings have been saved."

Scenario: The updated People Filter Labels appear on the Person Content Type
Given I am logged in as a user with the "site_owner" role
And I am on "node/add/person"
Then I should see "Job Type/Appointment"
And I should see "Department/Division"
And I should see "Filter 1/Research Group"
And I should see "Filter 2/Area of Expertise"
And I should see "Filter 3/Committees"
