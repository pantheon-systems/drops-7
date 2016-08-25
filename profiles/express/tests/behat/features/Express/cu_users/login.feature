Feature: Authentication tasks
  When I install the website
  As a user
  I should not be able to login without valid credentials

  @login
  Scenario: Login Error messages - Empty form
    Given I am on "/user"
    When I press "Log in"
    Then I should see the following error messages:
    | error messages                        |
    | CU Login Name field is required.      |
    | IdentiKey Password field is required. |
    And I should not see the following error messages:
    | error messages                                                                |
    | Sorry, unrecognized username or password                                      |
    | Unable to send e-mail. Contact the site administrator if the problem persists |

  @login
  Scenario: Login Error messages - Partial form Username
    Given I am on "/user"
    Given I fill in "username" for "CU Login Name"
    When I press "Log in"
    Then I should see the following error messages:
    | error messages |
    | IdentiKey Password field is required. |
    | Sorry, unrecognized username or password |
    And I should not see the following error messages:
    | error messages |
    | CU Login Name field is required. |
    | Unable to send e-mail. Contact the site administrator if the problem persists |

  @login
  Scenario: Login Error messages - Partial form Password
    Given I am on "/user"
    Given I fill in "password" for "IdentiKey Password"
    When I press "Log in"
    Then I should see the following error messages:
    | error messages |
    | CU Login Name field is required. |
    And I should not see the following error messages:
    | error messages |
    | IdentiKey Password field is required. |
    | Unable to send e-mail. Contact the site administrator if the problem persists |

  @login
  Scenario: Login Error messages - Complete form
    Given I am on "/user"
    Given I fill in "password" for "IdentiKey Password"
    Given I fill in "username" for "CU Login Name"
    When I press "Log in"
    Then I should see the following error messages:
    | error messages                            |
    | Sorry, unrecognized username or password. |
    And I should not see the following error messages:
    | error messages                        |
    | IdentiKey Password field is required. |
    | CU Login Name field is required.      |
    | Unable to send e-mail. Contact the site administrator if the problem persists |

