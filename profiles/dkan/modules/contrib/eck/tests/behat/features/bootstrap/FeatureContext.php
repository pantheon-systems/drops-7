<?php
/**
 * @file
 * Custom behat step definitions.
 */

use Behat\Behat\Context\ClosuredContextInterface,
    Behat\Behat\Context\TranslatedContextInterface,
    Behat\Behat\Context\BehatContext,
    Behat\Behat\Exception\PendingException;
use Behat\Gherkin\Node\PyStringNode,
    Behat\Gherkin\Node\TableNode;

/**
 * Features context.
 */
class FeatureContext extends Drupal\DrupalExtension\Context\DrupalContext {
  /**
   * Initializes context.
   *
   * Every scenario gets its own context object.
   *
   * @param array $parameters
   *   Context parameters (set them up through behat.yml)
   */
  public function __construct(array $parameters) {}
}
