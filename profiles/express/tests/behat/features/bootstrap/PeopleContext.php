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
class PeopleContext extends RawDrupalContext implements SnippetAcceptingContext {

}

