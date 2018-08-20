<?php

use Behat\Behat\Context\Context;
use Behat\Behat\Context\SnippetAcceptingContext;
use Behat\Behat\Hook\Scope\BeforeScenarioScope;
use Drupal\DrupalExtension\Context\MinkContext;
use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;

use Drupal\DrupalExtension\Context\RawDrupalContext;

/**
 * Define application features from the specific context.
 */
class FeatureContext extends RawDrupalContext implements Context, SnippetAcceptingContext {
  /**
   * Initializes context.
   * Every scenario gets its own context object.
   *
   * @param array $parameters
   *   Context parameters (set them in behat.yml)
   */
  public function __construct(array $parameters = []) {
    // Initialize your context here
  }

  /** @var \Drupal\DrupalExtension\Context\MinkContext */
  private $minkContext;
  /** @BeforeScenario */
  public function gatherContexts(BeforeScenarioScope $scope)
  {
      $environment = $scope->getEnvironment();
      $this->minkContext = $environment->getContext('Drupal\DrupalExtension\Context\MinkContext');
  }

//
// Place your definition and hook methods here:
//
//  /**
//   * @Given I have done something with :stuff
//   */
//  public function iHaveDoneSomethingWith($stuff) {
//    doSomethingWith($stuff);
//  }
//

    /**
     * Fills in form field with specified id|name|label|value
     * Example: And I enter the value of the env var "TEST_PASSWORD" for "edit-account-pass-pass1"
     *
     * @Given I enter the value of the env var :arg1 for :arg2
     */
    public function fillFieldWithEnv($value, $field)
    {
        $this->minkContext->fillField($field, getenv($value));
    }

    /**
     * Checks, that option from select with specified id|name|label|value is selected.
     *
     * @Then the :arg1 option from :arg2 should be selected
     */
    public function theOptionFromShouldBeSelected($option, $select)
    {
        $selectField = $this->getSession()->getPage()->findField($select);
        if (null === $selectField) {
            throw new ElementNotFoundException($this->getSession(), 'select field', 'id|name|label|value', $select);
        }

        $optionField = $selectField->find('named', array(
            'option',
            $option,
        ));

        if (null === $optionField) {
            throw new ElementNotFoundException($this->getSession(), 'select option field', 'id|name|label|value', $option);
        }

        if (!$optionField->isSelected()) {
            throw new ExpectationException('Select option field with value|text "'.$option.'" is not selected in the select "'.$select.'"', $this->getSession());
        }
    }

    /**
     * @Given I have wiped the site
     */
    public function iHaveWipedTheSite()
    {
        $site = getenv('TERMINUS_SITE');
        $env = getenv('TERMINUS_ENV');

        passthru("terminus --yes env:wipe {$site}.{$env}");
    }

    /**
     * @Given I have reinstalled :arg1
     */
    public function iHaveReinstalled($arg1)
    {
        $site = getenv('TERMINUS_SITE');
        $env = getenv('TERMINUS_ENV');
        $password = getenv('ADMIN_PASSWORD');

        $replacements = [
          '{site-name}' => $site,
          '{env}' => $env,
        ];

        $arg1 = str_replace(array_keys($replacements), array_values($replacements), $arg1);

        $cmd = "terminus --yes drush {$site}.{$env} -- site-install pantheon --yes --site-name=\"$arg1\" --account-name=admin";
        if (!empty($password)) {
          $cmd .= " --account-pass='$password'";
        }

        passthru($cmd);
    }

    /**
     * @Given I have run the drush command :arg1
     */
    public function iHaveRunTheDrushCommand($arg1)
    {
        $site = getenv('TERMINUS_SITE');
        $env = getenv('TERMINUS_ENV');

        $return = '';
        $output = array();
        exec("terminus  --yes drush {$site}.{$env} -- --yes $arg1", $output, $return);
        $output = implode("\n", $output);

        if ($return) {
          throw new Exception("Error running Drush command:\n$output");
        }

        print "$output";
    }

    /**
     * @Given I have committed my changes with comment :arg1
     */
    public function iHaveCommittedMyChangesWithComment($arg1)
    {
        $site = getenv('TERMINUS_SITE');
        $env = getenv('TERMINUS_ENV');

        passthru("terminus --yes env:commit {$site}.{$env} --message='$arg1'");
    }

    /**
     * @Given I wait :seconds seconds
     */
    public function iWaitSeconds($seconds)
    {
        sleep($seconds);
    }

    /**
     * @When I print the page contents
     */
    public function iPrintThePageContents()
    {
        $content = $this->getSession()->getPage()->getContent();
        print $content;
    }

    /**
     * @Given I wait :seconds seconds or until I see :text
     */
    public function iWaitSecondsOrUntilISee($seconds, $text)
    {
        $errorNode = $this->spin( function($context) use($text) {
            $node = $context->getSession()->getPage()->find('named', array('content', $text));
            if (!$node) {
              return false;
            }
            return $node->isVisible();
        }, $seconds);

        // Throw to signal a problem if we were passed back an error message.
        if (is_object($errorNode)) {
          throw new Exception("Error detected when waiting for '$text': " . $errorNode->getText());
        }
    }

    // http://docs.behat.org/en/v2.5/cookbook/using_spin_functions.html
    // http://mink.behat.org/en/latest/guides/traversing-pages.html#selectors
    public function spin ($lambda, $wait = 60)
    {
        for ($i = 0; $i <= $wait; $i++)
        {
            if ($i > 0) {
              sleep(1);
            }

            $debugContent = $this->getSession()->getPage()->getContent();
            file_put_contents("/tmp/mink/debug-" . $i, "\n\n\n=================================\n$debugContent\n=================================\n\n\n");

            try {
                if ($lambda($this)) {
                    return true;
                }
            } catch (Exception $e) {
                // do nothing
            }

            // If we do not see the text we are waiting for, fail fast if
            // we see a Drupal 8 error message pane on the page.
            $node = $this->getSession()->getPage()->find('named', array('content', 'Error'));
            if ($node) {
              $errorNode = $this->getSession()->getPage()->find('css', '.messages--error');
              if ($errorNode) {
                return $errorNode;
              }
              $errorNode = $this->getSession()->getPage()->find('css', 'main');
              if ($errorNode) {
                return $errorNode;
              }
              return $node;
            }
        }

        $backtrace = debug_backtrace();

        throw new Exception(
            "Timeout thrown by " . $backtrace[1]['class'] . "::" . $backtrace[1]['function'] . "()\n" .
            $backtrace[1]['file'] . ", line " . $backtrace[1]['line']
        );

        return false;
    }
}
