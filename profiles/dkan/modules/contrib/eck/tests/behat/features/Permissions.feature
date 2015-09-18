@api @permissions
Feature: Permissions
  As a site administrator
  I want to control who can do what with entity types, bundles and entities
  so users don't get themselves in trouble.

  @setup
  Scenario Outline: This is a set up step
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities,Administer permissions" permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "edit-entity-type-label" with <type_label>
    And I fill in "edit-entity-type-name" with <type>
    And I fill in "edit-bundle-label" with <bundle_label>
    And I fill in "edit-bundle-name" with <bundle>
    And I check "Title"
    And I press the "Save" button
    And I visit "/admin/structure/entity-type"
    And I click <type_label>
    And I click <bundle_label>
    And I click <add_link>
    And I fill in "Title" with <entity_title>
    And I press the "Save" button
    And I visit "admin/people/permissions"

    Examples:
      | type_label | type      | bundle_label | bundle | entity_title   | link                                       | add_link  |
      | "Vehicle"  | "vehicle" | "Car"        | "car"  | "Toyota Prius" | "/admin/structure/entity-type/vehicle/car" | "Add Car" |
      | "Animal"   | "animal"  | "Dog"        | "dog"  | "Snoopy"       | "/admin/structure/entity-type/animal/dog"  | "Add Dog" |

  @entity-type
  Scenario Outline: Only allowed users can access the entity type's overview page
    Given the cache has been cleared
    Given I am logged in as a user with the "Use the administration pages and help" permission
    And I visit "/admin/structure"
    Then I should not see the text "Entity types"
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure"
    And I click "Entity types"
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                     |
      | "Use the administration pages and help,View Entity Type List"   |
      | "Use the administration pages and help,Administer Entity Types" |

  @entity-type
  Scenario Outline: Only allowed users can add entity types from the overview page
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "Add entity type"
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List,Add Entity Types" permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                                           |
      | "Use the administration pages and help,View Entity Type List,Add Entity Types"        |
      | "Use the administration pages and help,View Entity Type List,Administer Entity Types" |

  @entity-type
  Scenario Outline: Only allowed users can delete entity types
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "delete"
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type"
    And I click "delete" in the "Vehicle" row
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                                           |
      | "Use the administration pages and help,View Entity Type List,Delete Entity Types"     |
      | "Use the administration pages and help,View Entity Type List,Administer Entity Types" |

  @bundle
  Scenario: Users without the right permission can not access the bundle's overview page
    Given the cache has been cleared
    Given I am logged in as a user with the "Use the administration pages and help,View Entity Type List" permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the text "Vehicle"
    And I should not see the link "Vehicle"

  @bundle
  Scenario Outline: Users with the right permission can access the bundle's overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the link <type_label>
    When I click <type_label>
    Then I should get a "200" HTTP response

    Examples:
      | type_label | permissions                                                                      |
      | "Vehicle"  | "Use the administration pages and help,View Entity Type List,View Bundle Lists"  |
      | "Animal"   | "Use the administration pages and help,View Entity Type List,View Bundle Lists"  |
      | "Vehicle"  | "Use the administration pages and help,View Entity Type List,Administer Bundles" |
      | "Animal"   | "Use the administration pages and help,View Entity Type List,Administer Bundles" |

  @bundle
  Scenario Outline: Users with the right permission can access the bundle's overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type"
    Then I should see the link "Vehicle"
    And I should not see the link "Animal"
    And I should see the text "Animal"
    When I click "Vehicle"
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                                                |
      | "Use the administration pages and help,View Entity Type List,View List of Vehicle Bundles" |
      | "Use the administration pages and help,View Entity Type List,Administer Vehicle Bundles"   |

  @bundle
  Scenario: Users without the right permission can not add bundles from the overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should not see the link "Add bundle"

  @bundle
  Scenario Outline: Users with the right permission can add bundles from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <link>
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response

    Examples:
      | link                                   | permissions                            |
      | "/admin/structure/entity-type/vehicle" | "View Bundle Lists,Add Bundles"        |
      | "/admin/structure/entity-type/animal"  | "View Bundle Lists,Add Bundles"        |
      | "/admin/structure/entity-type/vehicle" | "View Bundle Lists,Administer Bundles" |
      | "/admin/structure/entity-type/animal"  | "View Bundle Lists,Administer Bundles" |

  @bundle
  Scenario Outline: Users with the right permission can add bundles from the overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "Add bundle"
    When I click "Add bundle"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "Add bundle"

    Examples:
      | permissions                                    |
      | "View Bundle Lists,Add Vehicle Bundles"        |
      | "View Bundle Lists,Administer Vehicle Bundles" |

  @bundle
  Scenario: Users without the right permission can not delete bundles from the overview page
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should not see the link "delete"

  @bundle
  Scenario Outline: Users with the right permission can delete bundles from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <link>
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples:
      | link                                   | permissions                            |
      | "/admin/structure/entity-type/vehicle" | "View Bundle Lists,Delete Bundles"     |
      | "/admin/structure/entity-type/animal"  | "View Bundle Lists,Delete Bundles"     |
      | "/admin/structure/entity-type/vehicle" | "View Bundle Lists,Administer Bundles" |
      | "/admin/structure/entity-type/animal"  | "View Bundle Lists,Administer Bundles" |

  @bundle
  Scenario Outline: Users with the right permission can delete bundles from the overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "delete"

    Examples:
      | permissions                                    |
      | "View Bundle Lists,Delete Vehicle Bundles"     |
      | "View Bundle Lists,Administer Vehicle Bundles" |

  @entity
  Scenario: Users without the right permission can not access the entity's overview page
    Given the cache has been cleared
    Given I am logged in as a user with the "View Bundle Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the text "Car"
    And I should not see the link "Car"

  @entity
  Scenario Outline: Users with the right permission can access the entity's overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples:
      | path                                   | link  | permissions                             |
      | "/admin/structure/entity-type/vehicle" | "Car" | "View Bundle Lists,View Entity Lists"   |
      | "/admin/structure/entity-type/animal"  | "Dog" | "View Bundle Lists,View Entity Lists"   |
      | "/admin/structure/entity-type/vehicle" | "Car" | "View Bundle Lists,Administer Entities" |
      | "/admin/structure/entity-type/animal"  | "Dog" | "View Bundle Lists,Administer Entities" |

  @entity
  Scenario Outline: Users with the right permission can access the entity's overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "Car"
    When I click "Car"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal"
    Then I should not see the link "Dog"
    But I should see the text "Dog"

    Examples:
      | permissions                                           |
      | "View Bundle Lists,View List of Vehicle Car Entities" |
      | "View Bundle Lists,Administer Vehicle Car Entities"   |

  @entity
  Scenario: Users without the right permission can not add entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "Add Car"

  @entity
  Scenario Outline: Users with the right permission can add entities from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | link      | permissions                             |
      | "/admin/structure/entity-type/vehicle/car" | "Add Car" | "View Entity Lists,Add Entities"        |
      | "/admin/structure/entity-type/animal/dog"  | "Add Dog" | "View Entity Lists,Add Entities"        |
      | "/admin/structure/entity-type/vehicle/car" | "Add Car" | "View Entity Lists,Administer Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "Add Dog" | "View Entity Lists,Administer Entities" |

  @entity
  Scenario Outline: Users with the right permission can add entities from the overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "Add Car"
    When I click "Add Car"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal/dog"
    Then I should not see the link "Add Dog"

    Examples:
      | permissions                                         |
      | "View Entity Lists,Add Vehicle Car Entities"        |
      | "View Entity Lists,Administer Vehicle Car Entities" |

  @entity
  Scenario: Users without the right permission can not view entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "Toyota Prius"

  @entity
  Scenario Outline: Users with the right permission can view entities from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | link           | permissions                             |
      | "/admin/structure/entity-type/vehicle/car" | "Toyota Prius" | "View Entity Lists,View Any Entity"     |
      | "/admin/structure/entity-type/animal/dog"  | "Snoopy"       | "View Entity Lists,View Any Entity"     |
      | "/admin/structure/entity-type/vehicle/car" | "Toyota Prius" | "View Entity Lists,Administer Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "Snoopy"       | "View Entity Lists,Administer Entities" |

  @entity
  Scenario Outline: Users with the right permission can view entities from the overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "Toyota Prius"
    When I click "Toyota Prius"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal/dog"
    Then I should not see the link "Snoopy"

    Examples:
      | permissions                                         |
      | "View Entity Lists,View Vehicle Car Entities"       |
      | "View Entity Lists,Administer Vehicle Car Entities" |

  @entity
  Scenario: Users without the right permission can not edit entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "edit"

  @entity
  Scenario Outline: Users with the right permission can edit entities from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link "edit"
    When I click "edit"
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | permissions                             |
      | "/admin/structure/entity-type/vehicle/car" | "View Entity Lists,Edit Any Entity"     |
      | "/admin/structure/entity-type/animal/dog"  | "View Entity Lists,Edit Any Entity"     |
      | "/admin/structure/entity-type/vehicle/car" | "View Entity Lists,Administer Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "View Entity Lists,Administer Entities" |

  @entity
  Scenario Outline: Users with the right permission can delete entities from the overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "edit"
    When I click "edit"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal/dog"
    Then I should not see the link "edit"

    Examples:
      | permissions                                         |
      | "View Entity Lists,Edit Vehicle Car Entities"       |
      | "View Entity Lists,Administer Vehicle Car Entities" |

  @entity
  Scenario: Users without the right permission can not delete entities from the overview page
    Given I am logged in as a user with the "View Entity Lists" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "delete"

  @entity
  Scenario Outline: Users with the right permission can delete entities from the overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | permissions                             |
      | "/admin/structure/entity-type/vehicle/car" | "View Entity Lists,Delete Any Entity"   |
      | "/admin/structure/entity-type/animal/dog"  | "View Entity Lists,Delete Any Entity"   |
      | "/admin/structure/entity-type/vehicle/car" | "View Entity Lists,Administer Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "View Entity Lists,Administer Entities" |

  @entity
  Scenario Outline: Users with the right permission can delete entities from the overview page (specific)
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response
    And I visit "/admin/structure/entity-type/animal/dog"
    Then I should not see the link "delete"

    Examples:
      | permissions                                         |
      | "View Entity Lists,Delete Vehicle Car Entities"     |
      | "View Entity Lists,Administer Vehicle Car Entities" |

  @cleanup
  Scenario Outline: This is a clean up step
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities" permissions
    Given I visit "/admin/structure/entity-type"
    And I click <type_label>
    And I click "Delete"
    And I press the "Delete" button

    Examples:
      | type_label |
      | "Vehicle"  |
      | "Animal"   |
