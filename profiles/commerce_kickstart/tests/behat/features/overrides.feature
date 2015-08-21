@api @overrides
Feature: Customization overrides
  In order to customize the distribution
  As a site builder
  I need my changes to remain when using Features Override

  Background:
    Given I am logged in as a user with the "administrator" role

  Scenario: The product list view mode is disabled for Blog Post
    When I am on "/admin/structure/types/manage/blog-post/display"
    Then I should not see the link "Product list"

  Scenario: Category and tags should be hidden from the default view
    When I am on "/admin/structure/types/manage/blog-post/display"
    Then the "edit-fields-field-blog-category-type" field should contain "hidden"
    Then the "edit-fields-field-tags-type" field should contain "hidden"

  Scenario: Category and tags should be hidden from the teaser view
    When I am on "/admin/structure/types/manage/blog-post/display/teaser"
    Then the "edit-fields-field-blog-category-type" field should contain "hidden"
    Then the "edit-fields-field-tags-type" field should contain "hidden"

  Scenario: My field label changes to blog posts should remain
    When I am on "/admin/structure/types/manage/blog-post/fields"
    Then I should see "Article body"
    And I should see "Featured Image"
