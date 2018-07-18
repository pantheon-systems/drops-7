<?php

use Behat\Mink\Driver\Selenium2Driver;
use Behat\Mink\Exception\UnsupportedDriverActionException;
use Behat\MinkExtension\Context\MinkContext;
use Behat\Behat\Hook\Scope\BeforeScenarioScope;
use Behat\Behat\Hook\Scope\AfterScenarioScope;
use Behat\Behat\Hook\Scope\BeforeStepScope;
use Behat\Behat\Hook\Scope\AfterStepScope;


/**
 * @file
 * The main Behat context.
 */

class FeatureContext extends MinkContext
{

  /*
   * @AfterScenario
   * @param \Behat\Behat\Hook\Scope\AfterScenarioScope $scope

  public function after(AfterScenarioScope $scope)
  {
    $this->getSession()->visit($this->locatePath('/user/logout'));
  } */

  /*
   * After every step in a @javascript scenario, we want to wait for AJAX
   * loading to finish. If a test failure, then take a screenshot of failed step.
   *
   * @AfterStep
   *
   * @param \Behat\Behat\Hook\Scope\AfterStepScope $scope

  /*
  public function afterStep(AfterStepScope $scope)
  {
    if (0 === $scope->getTestResult()->getResultCode()) {
      $driver = $this->getSession()->getDriver();
      if (!($driver instanceof Selenium2Driver)) {
        return;
      }
      $this->iWaitForAjax();
    }
  } */

  /**
   * Wait for AJAX to finish.
   *
   * @Given I wait for AJAX
   */
  public function iWaitForAjax() {
    $this->getSession()->wait(2000, 'typeof jQuery !== "undefined" && jQuery.active === 0 && document.readyState === "complete"');
  }

  /**
   * Creates and authenticates a user with the given role(s).
   *
   * @Given I am logged in as a user with the :role role(s)
   * @Given I am logged in as a/an :role
   *
   * @param $role
   *
   * @throws \Behat\Mink\Exception\ElementNotFoundException
   * @throws \Exception
   */
  public function assertAuthenticatedByRole($role)
  {
    // Go to user login page.
    $this->getSession()->visit($this->locatePath('/user'));
    $element = $this->getSession()->getPage();

    // Logout if logged in.
    if ($element->hasContent('Who\'s online')) {
      $this->getSession()->visit($this->locatePath('/user/logout'));
      $this->getSession()->visit($this->locatePath('/user'));
      $element = $this->getSession()->getPage();
    }

    // Fill fields with login information.
    $element->fillField('CU Login Name', $role);
    $element->fillField('IdentiKey Password', $role);
    $submit = $element->findButton('Log in');

    if (empty($submit)) {
      throw new Exception(sprintf("No submit button at %s", $this->getSession()
        ->getCurrentUrl()));
    }

    // Log in.
    $submit->click();

    // Need to figure out better way to check if logged in.
    if (!$this->getSession()->getPage()->hasContent('Dashboard')) {
      throw new Exception(sprintf("Failed to log in as user '%s'", $role));
    }
  }

  /**
   * @When /^I click the "(?P<element>(?:[^"]|\\")*)" element$/
   *
   * @param $element
   */
  public function iClickTheElement($element) {
    $page_element = $this->getSession()
      ->getPage()
      ->find("css", $element)
      ->click();
  }

  /**
   * @When I click the :element element with :value for :attribute
   *
   * @param $element
   * @param $value
   * @param $attribute
   *
   * @throws \Exception
   */
  public function iClickTheElementWithFor($element, $value, $attribute) {
    $page_elements = $this->getSession()
      ->getPage()
      ->findAll("css", $element);
    if ($page_elements == NULL) {
      throw new Exception(sprintf('Couldn\'t find "%s" elements', $element));
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
      throw new Exception(sprintf('Couldn\'t find "%s" attribute', $attribute));
    }
  }

  /**
   * @Given /^I wait (\d+) seconds$/
   *
   * @param $seconds
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
      throw new Exception('Only tests with the @javascript tag can resize the browser window.');
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
   * @When I wait for the :arg1 element to appear
   *
   * Wait for an element to appear before continuing a test.
   *
   * @param string $arg1
   *   The CSS selector you are waiting to appear.
   */
  public function iWaitForTheElementToAppear($arg1) {
    $this->spinner(function($context, $arg1) {

      $el = $context->getSession()->getPage()->find("css", $arg1);

      if ($el !== NULL && $el->isVisible()) {
        return true;
      }

      return false;
    }, $arg1);
  }

  /**
   * Accepts and element and "spins" until the element appears.
   *
   * @param string $lambda
   *   The function to test for truthiness.
   * @param string $element
   *   The CSS selector used for truthiness.
   * @param int $wait
   *   The timeout to wait for before the step fails.
   *
   * @return bool
   * @throws \Exception
   */
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

    throw new Exception("Timeout thrown by ". $backtrace[1]['class']. "::". $backtrace[1]['function']. "()\n". $backtrace[1]['file']. ", line ". $backtrace[1]['line']);
  }

  /**
   * @Then I should see the link :link
   *
   * @param $link
   *
   * @throws \Exception
   */
  public function assertLinkVisible($link)
  {
    $element = $this->getSession()->getPage();
    $result = $element->findLink($link);
    try {
      if ($result && !$result->isVisible()) {
        throw new Exception(sprintf("No link to '%s' on the page %s", $link, $this->getSession()->getCurrentUrl()));
      }
    } catch (UnsupportedDriverActionException $e) {
      // We catch the UnsupportedDriverActionException exception in case
      // this step is not being performed by a driver that supports javascript.
      // All other exceptions are valid.
    }
    if (empty($result)) {
      throw new Exception(sprintf("No link to '%s' on the page %s", $link, $this->getSession()->getCurrentUrl()));
    }
  }

  /**
   * @Then I should not see the link :link
   *
   * @param $link
   *
   * @throws \Exception
   */
  public function assertLinkNotVisible($link)
  {
    $element = $this->getSession()->getPage();
    $result = $element->findLink($link);
    try {
      if ($result && $result->isVisible()) {
        throw new Exception(sprintf("Link to '%s' found on the page %s", $link, $this->getSession()->getCurrentUrl()));
      }
    } catch (UnsupportedDriverActionException $e) {
      // We catch the UnsupportedDriverActionException exception in case
      // this step is not being performed by a driver that supports javascript.
      // All other exceptions are valid.
    }
  }

  /**
   * @Then The :element element should have :text in the :attribute attribute
   *
   * @param $element
   * @param $text
   * @param $attribute
   *
   * @throws \Exception
   */
  public function theElementShouldHaveInTheAttribute($element, $text, $attribute)
  {
    $session = $this->getSession();
    $page = $session->getPage();

    $page_element = $page->find('css', $element);
    if ($page_element == NULL) {
      throw new Exception(sprintf('Couldn\'t find "%s" element', $element));
    }

    $page_attribute = $page_element->getAttribute($attribute);
    if ($page_attribute == NULL) {
      throw new Exception(sprintf('Couldn\'t find "%s" attribute', $attribute));
    }

    if ($page_attribute == $text) {
      $result = $text;
    }

    if (empty($result)) {
      throw new Exception(sprintf('The "%s" attribute did not contain "%s"', $page_attribute, $text));
    }
  }

  /**
   * @When I attach the file :path to the :field field
   */
  public function iAttachTheFileToTheField($path, $field)
  {
    $field = $this->fixStepArgument($field);

    if ($this->getMinkParameter('files_path')) {

      // We can't use realpath() since the tests might be run on a different server.
      // $fullPath = rtrim(realpath($this->getMinkParameter('files_path')), DIRECTORY_SEPARATOR).DIRECTORY_SEPARATOR.$path;

      $fullPath = rtrim($this->getMinkParameter('files_path')).DIRECTORY_SEPARATOR.$path;
      echo $fullPath;

      if (is_file($fullPath)) {
        $path = $fullPath;
      }
    }

    try {
      $this->getSession()->getPage()->attachFileToField($field, $path);
    } catch (\Behat\Mink\Exception\ElementNotFoundException $e) {
      throw new Exception(sprintf('The "%s" path could not be located."', $path));
    }
  }

  /**
   * Pauses the scenario until the user presses a key. Useful when debugging a scenario.
   *
   * @Then (I )break
   */
  public function iPutABreakpoint()
  {
    fwrite(STDOUT, "\033[s \033[93m[Breakpoint] Press \033[1;93m[RETURN]\033[0;93m to continue, or 'q' to quit...\033[0m");
    do {
      $line = trim(fgets(STDIN, 1024));
      //Note: this assumes ASCII encoding.  Should probably be revamped to
      //handle other character sets.
      $charCode = ord($line);
      switch ($charCode) {
        case 0: //CR
        case 121: //y
        case 89: //Y
          break 2;
        // case 78: //N
        // case 110: //n
        case 113: //q
        case 81: //Q
          throw new \Exception("Exiting test intentionally.");
        default:
          fwrite(STDOUT, sprintf("\nInvalid entry '%s'.  Please enter 'y', 'q', or the enter key.\n", $line));
          break;
      }
    } while (true);
    fwrite(STDOUT, "\033[u");
  }

  /**
   * @Given /^I switch to the iframe "([^"]*)"$/
   */
  public function iSwitchToIframe($arg1 = null) {
      $this->getSession()->switchToIFrame($arg1);
  }

  /**
   * @When /^I check the "([^"]*)" radio button$/
   */
  public function iCheckTheRadioButton($labelText)
  {
    $page = $this->getSession()->getPage();
    $radioButton = $page->find('named', ['radio', $labelText]);
    if ($radioButton) {
      $select = $radioButton->getAttribute('name');
      $option = $radioButton->getAttribute('value');
      $page->selectFieldOption($select, $option);
      return;
    }

    throw new \Exception("Radio button with label {$labelText} not found");
  }
}
