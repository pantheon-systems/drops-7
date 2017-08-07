<?php

use Drupal\DrupalExtension\Context\RawDrupalContext;
use Behat\Behat\Context\SnippetAcceptingContext;
use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;
use Behat\Mink\Driver\Selenium2Driver;
use Behat\MinkExtension\Context\MinkContext;
use Behat\Mink\Session;
use Behat\Mink\Driver\DriverInterface;
use Behat\Behat\Context\Step\Given;

/**
 * Defines application features from the specific context.
 */
class ExpressContext extends RawDrupalContext implements SnippetAcceptingContext {

  /**
   * Initializes context.
   *
   * Every scenario gets its own context instance.
   * You can also pass arbitrary arguments to the
   * context constructor through behat.yml.
   */
  public function __construct() {

  }


  /**
   * @BeforeSuite
   * Enable bundle and add authentication data.
   */
  public static function prepare($scope) {

    // List needed users.
    $users = array('developer', 'administrator', 'content_editor', 'site_owner');

    // Create users.
    foreach ($users as $user_name) {

      // For some reason, I ran into the issue where the same user was created multiple times.
      // If user exists, skip creation.
      if (user_load_by_name($user_name)) {
        continue;
      }

      // Get role ID.
      $role = user_role_load_by_name($user_name);

      $new_user = array(
        'name' => $user_name,
        'pass' => $user_name, // note: do not md5 the password
        'mail' => 'noreply@nowhere.com',
        'status' => 1,
        'init' => 'noreply@nowhere.com',
        'roles' => array(
          DRUPAL_AUTHENTICATED_RID => 'authenticated user',
          $role->rid => $user_name,
        ),
      );

      // The first parameter is sent blank so a new user is created.
      user_save('', $new_user);
    }

    // Set LDAP variable to mixed mode.
    $ldap_conf = variable_get('ldap_authentication_conf');
    // 1 is mixed mode, 2 is LDAP only.
    $ldap_conf['authenticationMode'] = 1;
    variable_set('ldap_authentication_conf', $ldap_conf);

  }

  /**
   * @AfterSuite
   */
  public static function tearDown($scope) {
    // Delete created users.
    // Since they all have the same email, we can load them by that parameter.
    $uids = db_query("SELECT uid FROM {users} WHERE mail = 'noreply@nowhere.com'")->fetchCol();
    user_delete_multiple($uids);
  }


  /**
   * @BeforeScenario
   */
  public function before($event) {
    //set_time_limit(60);
  }


  /**
   * Creates and authenticates a user with the given role(s).
   *
   * @Given CU - I am logged in as a user with the :role role(s)
   * @Given CU - I am logged in as a/an :role
   */
  public function assertAuthenticatedByRole($role) {
    // Load custom created user.
    // User has the same name as the role.
    $user = user_load_by_name($role);

    // Translate to what is expected in $this->user.
    $this->user = (object) array(
      'name' => $user->name,
      'pass' => $role,
      'role' => $role,
      'mail' => $user->mail,
      'status' => $user->status,
      'uid' => $user->uid,
    );

    // Check if logged in.
    if ($this->loggedIn()) {
      $this->logout();
    }

    if (!$this->user) {
      throw new \Exception('Tried to login without a user.');
    }

    $this->getSession()->visit($this->locatePath('/user'));
    $element = $this->getSession()->getPage();
    $element->fillField($this->getDrupalText('username_field'), $this->user->name);
    $element->fillField($this->getDrupalText('password_field'), $this->user->pass);
    $submit = $element->findButton($this->getDrupalText('log_in'));
    if (empty($submit)) {
      throw new \Exception(sprintf("No submit button at %s", $this->getSession()
        ->getCurrentUrl()));
    }

    // Log in.
    $submit->click();

    // Need to figure out better way to check if logged in.
    /*
    if (!$this->loggedIn()) {
      throw new \Exception(sprintf("Failed to log in as user '%s' with role '%s'", $this->user->name, $this->user->role));
    }
    */
  }

  /**
   * Get a region by name.
   *
   * @param string $region
   *   The name of the region from the behat.yml file.
   *
   * @return Behat\Mink\Element\Element
   *   An element representing the region.
   */
  public function getRegion($region) {
    $session = $this->getSession();
    $regionObj = $session->getPage()->find('region', $region);
    if (!$regionObj) {
      throw new \Exception(sprintf('No region "%s" found on the page %s.', $region, $session->getCurrentUrl()));
    }
    return $regionObj;
  }

  /**
   * After every step in a @javascript scenario, we want to wait for AJAX
   * loading to finish.
   *
   * @AfterStep
   */
  public function afterStep($event) {
    if (isset($this->javascript) && $this->javascript && empty($this->iframe)) {
      $text = $event->getStep()->getText();
      if (preg_match('/(follow|press|click|submit|viewing|visit|reload|attach)/i', $text)) {
        $this->iWaitForAjax();
      }
    }
  }

  /**
   * Wait for AJAX to finish.
   *
   * @Given I wait for AJAX
   */
  public function iWaitForAjax() {
    $this->getSession()->wait(5000, 'typeof jQuery !== "undefined" && jQuery.active === 0 && document.readyState === "complete"');
  }

  /**
   * Asserts that an image is present and not broken.
   *
   * @Then I should see an image in the :region region
   */
  public function assertValidImageRegion($region) {
    $regionObj = $this->getRegion($region);
    $elements = $regionObj->findAll('css', 'img');
    if (empty($elements)) {
      throw new \Exception(sprintf('No image was not found in the "%s" region on the page %s', $region, $this->getSession()->getCurrentUrl()));
    }

    if ($src = $elements[0]->getAttribute('src')) {
      $params = array('http' => array('method' => 'HEAD'));
      $context = stream_context_create($params);
      $fp = @fopen($src, 'rb', FALSE, $context);
      if (!$fp) {
        throw new \Exception(sprintf('Unable to download <img src="%s"> in the "%s" region on the page %s', $src, $region, $this->getSession()->getCurrentUrl()));
      }

      $meta = stream_get_meta_data($fp);
      fclose($fp);
      if ($meta === FALSE) {
        throw new \Exception(sprintf('Error reading from <img src="%s"> in the "%s" region on the page %s', $src, $region, $this->getSession()->getCurrentUrl()));
      }

      $wrapper_data = $meta['wrapper_data'];
      $found = FALSE;
      if (is_array($wrapper_data)) {
        foreach ($wrapper_data as $header) {
          if (substr(strtolower($header), 0, 19) == 'content-type: image') {
            $found = TRUE;
          }
        }
      }

      if (!$found) {
        throw new \Exception(sprintf('Not a valid image <img src="%s"> in the "%s" region on the page %s', $src, $region, $this->getSession()->getCurrentUrl()));
      }
    }
    else {
      throw new \Exception(sprintf('No image had no src="..." attribute in the "%s" region on the page %s', $region, $this->getSession()->getCurrentUrl()));
    }
  }


  /**
   * @Then /^I should see the image alt "(?P<text>(?:[^"]|\\")*)" in the "(?P<region>[^"]*)" region$/
   *
   * NOTE: We specify a regex to allow escaped quotes in the alt text.
   */
  public function assertAltRegion($text, $region) {
    $regionObj = $this->getRegion($region);
    $element = $regionObj->find('css', 'img');
    $tmp = $element->getAttribute('alt');
    if ($text == $tmp) {
      $result = $text;
    }
    if (empty($result)) {
      throw new \Exception(sprintf('No alt text matching "%s" in the "%s" region on the page %s', $text, $region, $this->getSession()->getCurrentUrl()));
    }
  }

  /**
   * @When /^I click the "(?P<element>(?:[^"]|\\")*)" element$/
   */
  public function iClickTheElement($element) {
    $page_element = $this->getSession()
      ->getPage()
      ->find("css", $element)
      ->click();
  }

  /**
   * @When /^I disable the "(?P<text>(?:[^"]|\\")*)" module$/
   */
  public function iDisableTheModule($text) {
    module_disable(array($text));
  }

  /**
   * @Then /^I select autosuggestion option "([^"]*)"$/
   *
   * @param $text Option to be selected from autosuggestion
   * @throws \InvalidArgumentException
   */
  public function selectAutosuggestionOption($text) {
    $session = $this->getSession();
    $element = $session->getPage()->find(
      'xpath',
      $session->getSelectorsHandler()->selectorToXpath('xpath', '*//*[text()="'. $text .'"]')
    );

    if (null === $element) {
      throw new \InvalidArgumentException(sprintf('Cannot find text: "%s"', $text));
    }
    $element->click();
  }

  /**
   * @Given /^I wait (\d+) seconds$/
   */
  public function iWaitSeconds($seconds) {
    sleep($seconds);
  }

  /**
   * @Given I setup Pathologic local paths
   *
   * Save Pathologic settings for testing.
   */
  public function pathologic_save() {

    $cu_path = 'testing';
    $cu_sid = 'p1eb825ce549';

    $pathologic_string = "/$cu_sid\r\n" .
      "/$cu_path\r\n" .
      "http://www.colorado.edu/$cu_sid\r\n" .
      "http://www.colorado.edu/$cu_path\r\n" .
      "https://www.colorado.edu/$cu_sid\r\n" .
      "https://www.colorado.edu/$cu_path";

    $format = filter_format_load("wysiwyg");

    if (empty($format->filters)) {
      // Get the filters used by this format.
      $filters = filter_list_format($format->format);
      // Build the $format->filters array...
      $format->filters = array();
      foreach($filters as $name => $filter) {
        foreach($filter as $k => $v) {
          $format->filters[$name][$k] = $v;
        }
      }
    }

    $format->filters["pathologic"]["settings"]["local_paths"] = $pathologic_string;

    filter_format_save($format);
  }

  /**
   * @When /^I click the "(?P<element>(?:[^"]|\\")*)" element with "(?P<value>(?:[^"]|\\")*)" for "(?P<attribute>(?:[^"]|\\")*)"$/
   */
  public function iClickTheElementWithFor($element, $value, $attribute) {
    $page_elements = $this->getSession()
      ->getPage()
      ->findAll("css", $element);
    if ($page_elements == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" elements', $element));
    }
    foreach ($page_elements as $element) {
      if ($page_attribute = $element->getAttribute($attribute)) {
        if ($page_attribute == $value) {
          $element->click();
          return;
        }
      }
    }
    if ($page_attribute == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" attribute', $attribute));
    }
  }

  /**
   * @Then /^The "(?P<element>(?:[^"]|\\")*)" element should have "(?P<text>(?:[^"]|\\")*)" in the "(?P<attribute>(?:[^"]|\\")*)" attribute$/
   *
   */
  public function elementShouldHaveForAttribute($element, $text, $attribute) {
    $session = $this->getSession();
    $page = $session->getPage();

    $page_element = $page->find('css', $element);
    if ($page_element == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" element', $element));
    }

    $page_attribute = $page_element->getAttribute($attribute);
    if ($page_attribute == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" attribute', $attribute));
    }

    if ($page_attribute == $text) {
      $result = $text;
    }

    if (empty($result)) {
      throw new \Exception(sprintf('The "%s" attribute did not contain "%s"', $page_attribute, $text));
    }
  }

  /**
   * @Then /^The "(?P<element>(?:[^"]|\\")*)" link should have "(?P<text>(?:[^"]|\\")*)" in the "(?P<attribute>(?:[^"]|\\")*)" attribute$/
   *
   */
  public function linkShouldHaveForAttribute($element, $text, $attribute) {
    $session = $this->getSession();
    $page = $session->getPage();

    $page_element = $page->findLink($element);
    if ($page_element == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" element', $element));
    }

    $page_attribute = $page_element->getAttribute($attribute);
    if ($page_attribute == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" attribute', $attribute));
    }

    if ($page_attribute == $text) {
      $result = $text;
    }

    if (empty($result)) {
      throw new \Exception(sprintf('The "%s" attribute did not contain "%s"', $page_attribute, $text));
    }
  }

  /**
   * @AfterStep
   */
  public function takeScreenShotAfterFailedStep($scope) {
    if (99 === $scope->getTestResult()->getResultCode()) {
      $driver = $this->getSession()->getDriver();
      if (!($driver instanceof Selenium2Driver)) {
        return;
      }
      file_put_contents('/tmp/test.png', $this->getSession()->getDriver()->getScreenshot());
    }
  }



  /**
   * @When /^I create a "(?P<content_type>(?:[^"]|\\")*)" node with the title "(?P<title>(?:[^"]|\\")*)"$/
   */
  public function imAtAWithTheTitle($content_type, $title) {
    // Create Node.
    $node = new stdClass();
    $node->title = $title;
    $node->type = $content_type;
    node_object_prepare($node);
    node_save($node);

    // Go to node page.
    // Using vistPath() instead of visit() method since it adds base URL to relative path.
    $this->visitPath('node/' . $node->nid);
  }

  /**
   * @When /^I create a "(?P<bean_type>(?:[^"]|\\")*)" block with the label "(?P<label>(?:[^"]|\\")*)"$/
   */
  public function imAtAWithTheLabel($bean_type, $label) {
    // Create Block.
    $values = array(
      'label' => $label,
      'type'  => $bean_type,
    );
    $entity = entity_create('bean', $values);
    $saved_entity = entity_save('bean', $entity);

    // Go to bean page.
    // Using vistPath() instead of visit() method since it adds base URL to relative path.
    $this->visitPath('block/' . $entity->delta);
  }

  /*
  /**
   * @AfterScenario
   *
   * @todo Get this working to cleanup node creation
   */
  /*
  public function afterNodeCreation($event) {
    $steps = $event->getScenario()->getSteps();
    $tags = $event->getScenario()->getTags();

    if (in_array('node_creation', $tags)) {
      foreach ($steps as $step) {
        $step = (array) $step;
        //print_r($step);
        if (strpos($step[Behat\Gherkin\Node\StepNodetext], 'I create a' && strpos($step[Behat\Gherkin\Node\StepNodetext], 'node'))) {
          $step_pieces = explode('"', $step[Behat\Gherkin\Node\StepNodetext]);
          print_r($step_pieces);
        }
      }
    }
  }
  */
}
