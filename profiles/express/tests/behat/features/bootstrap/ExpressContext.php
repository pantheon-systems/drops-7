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
   * loading to finish. If a test failure, then take a screenshot of failed step.
   *
   * @AfterStep
   */
  public function afterStep($scope) {
    if (0 === $scope->getTestResult()->getResultCode()) {
      $driver = $this->getSession()->getDriver();
      if (!($driver instanceof Selenium2Driver)) {
        return;
      }
      $this->iWaitForAjax();
    }

    if (99 === $scope->getTestResult()->getResultCode()) {
      $driver = $this->getSession()->getDriver();
      if (!($driver instanceof Selenium2Driver)) {
        return;
      }
      file_put_contents('/tmp/test.png', $this->getSession()->getDriver()->getScreenshot());
    }
  }

  /**
   * Set timestamp for clearing data.
   *
   * @BeforeScenario
   */
  public function before($scope) {
    /*
    $this->getSession()->visit('behat/set');

    if (!$this->getSession()->getPage()->getText('Set Behat testing timestamp.')) {
      throw new \Exception(sprintf("Failed to set timestamp marker for clearing test data."));
    }
    */
  }

  /**
   * Clear testing data.
   *
   * @AfterScenario
   */
  public function after($scope) {
    /*
    $this->getSession()->visit('behat/clear');

    if (!$this->getSession()->getPage()->getText('Cleared test data created during scenario.')) {
      throw new \Exception(sprintf("Failed to clear test data."));
    }
    */
  }

  /**
   * Creates and authenticates a user with the given role(s).
   *
   * @Given CU - I am logged in as a user with the :role role(s)
   * @Given CU - I am logged in as a/an :role
   */
  public function assertAuthenticatedByRole2($role) {

    // Go to user login page.
    $this->getSession()->visit($this->locatePath('/user'));
    $element = $this->getSession()->getPage();

    // Logout if logged in.
    if ($element->getText('CU Login Name')) {
      $this->getSession()->visit($this->locatePath('/user/logout'));
      $this->getSession()->visit($this->locatePath('/user'));
      $element = $this->getSession()->getPage();
    }

    // Fill fields with login information.
    $element->fillField('CU Login Name', $role);
    $element->fillField('IdentiKey Password', $role);
    $submit = $element->findButton('Log in');

    if (empty($submit)) {
      throw new \Exception(sprintf("No submit button at %s", $this->getSession()
        ->getCurrentUrl()));
    }

    // Log in.
    $submit->click();

    // Need to figure out better way to check if logged in.
    if (!$this->getSession()->getPage()->getText('Dashboard')) {
      throw new \Exception(sprintf("Failed to log in as user '%s'", $role));
    }
  }

  /**
   * Change the size of the window on Javascript tests.
   *
   * @param string $type
   *   Predefined type to resize window.
   *
   * @throws Exception
   *
   * @Given I resize the window to a :type resolution.
   */
  function iChangeTheScreenSize($type) {
    // Only change the window size on Javascript tests.
    $driver = $this->getSession()->getDriver();
    if (!($driver instanceof Selenium2Driver)) {
      throw new \Exception('Only tests with the @javascript tag can resize the browser window.');
    }

    // Resize the window based on pre-defined types of resolutions.
    switch ($type) {
      case 'mobile':
        $this->getSession()->resizeWindow(320, 480, 'current');
        break;
      case 'desktop':
        $this->getSession()->resizeWindow(1280, 1024, 'current');
        break;
      default:
        $this->getSession()->resizeWindow(1280, 1024, 'current');
    }
  }

  /**
   * Wait for AJAX to finish.
   *
   * @Given I wait for AJAX
   */
  public function iWaitForAjax() {

    // Polling for the sake of my intern tests
    $script = '
    var interval = setInterval(function() {
    console.log("checking");
      if (document.readyState === "complete") {
        clearInterval(interval);
        done();
      }
    }, 1000);';

    //$this->getSession()->evaluateScript($script);
    $this->getSession()->wait(2000, 'typeof jQuery !== "undefined" && jQuery.active === 0 && document.readyState === "complete"');
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
   * @When /^I enable the "(?P<text>(?:[^"]|\\")*)" module$/
   */
  public function iEnableTheModule($text) {
    module_enable(array($text));
  }

  /**
   * @Then /^I select autosuggestion option "([^"]*)"$/
   *
   * @param $text Option to be selected from autosuggestion
   * @throws \InvalidArgumentException
   */
  // @todo Fix brokenness or use keystroke step on tests where this step was intended to go.
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
    $page = $this->getSession()->getPage();

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

  /**
   * @When I write :text into field :field
   */
  public function iWriteIntoField2($text, $field) {
    // This function is used to
    $this->getSession()
      ->getDriver()
      ->getWebDriverSession()
      ->element('xpath', $this->getSession()->getSelectorsHandler()->selectorToXpath('named_exact', array('field', $field)))
      ->postValue(array('value' => array($text)));
  }

  /**
   * @Given /^I manually press "([^"]*)"$/
   */
  public function iManuallyPress($key) {
    $script = "jQuery.event.trigger({ type : 'keypress', key : '" . $key . "' });";
    $this->getSession()->evaluateScript($script);
  }

  /**
   * @Given /^I switch to the iframe "([^"]*)"$/
   */
  public function iSwitchToIframe($arg1 = null) {
    $this->getSession()->switchToIFrame($arg1);
  }

  /**
   * @When I wait for the :arg1 element to appear
   */
  public function iWaitForTheElementToAppear2($arg1) {
    $this->spinner(function($context, $arg1) {

      $el = $context->getSession()->getPage()->findById($arg1);

      if ($el !== NULL && $el->isVisible()) {
        return true;
      }

      return false;
    }, $arg1);
  }

  public function spinner($lambda, $element, $wait = 60) {
    for ($i = 0; $i < $wait; $i++) {
      try {
        if ($lambda($this, $element)) {
          return true;
        }
      } catch (Exception $e) {
        // do nothing
      }
      sleep(1);
    }

    $backtrace = debug_backtrace();

    throw new Exception(
      "Timeout thrown by " . $backtrace[1]['class'] . "::" . $backtrace[1]['function'] . "()\n" .
      $backtrace[1]['file'] . ", line " . $backtrace[1]['line']
    );
  }

}
