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
class PhotoGalleryContext extends RawDrupalContext implements SnippetAcceptingContext {

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
   * Enable bundle modules and add authentication data.
   */
  public static function prepare($scope) {
    $data = array(
      'sids' => array (
        'directory' => 'directory',
        ),
      'authenticationMode' => 1,
      'loginConflictResolve' => 2,
      'acctCreation' => 4,
      'loginUIUsernameTxt' => NULL,
      'loginUIPasswordTxt' => NULL,
      'ldapUserHelpLinkUrl' => NULL,
      'ldapUserHelpLinkText' => 'Logon Help',
      'emailOption' => 3,
      'emailUpdate' => 1,
      'allowOnlyIfTextInDn' => array (),
      'excludeIfTextInDn' => array (),
      'allowTestPhp' => '',
      'excludeIfNoAuthorizations' => NULL,
      'ssoRemoteUserStripDomainName' => NULL,
      'seamlessLogin' => NULL,
      'ldapImplementation' => NULL,
      'cookieExpire' => NULL,
    );
    variable_set('ldap_authentication_conf', $data);

    //module_enable(array('cu_photo_gallery_bundle'));
    //drupal_flush_all_caches();
  }

  /**
   * @AfterSuite
   * Disable bundle modules.
   */
  public static function tearDown($scope) {
    //module_disable(array('cu_photo_gallery_bundle'));
    //drupal_uninstall_modules(array('cu_photo_gallery_bundle'));
    //drupal_flush_all_caches();
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
    $this->getSession()->wait(5000, 'typeof jQuery !== "undefined" && jQuery.active === 0');
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
   * @Then /^The "(?P<element>(?:[^"]|\\")*)" link should have "(?P<text>(?:[^"]|\\")*)" in the "(?P<attribute>(?:[^"]|\\")*)" attribute$/
   *
   */
  public function elementShouldHaveForAttribute($element, $text, $attribute) {
    $session = $this->getSession();
    $page = $session->getPage();

    $page_element = $page->findLink($element);
    if ($page_element == NULL) {
      throw new \Exception(sprintf('Couldn\'t find "%s" link', $element));
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
}

