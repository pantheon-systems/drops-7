<?php

/**
 * @file
 * First Data Global Gateway e4 API Web Services Feature.
 */


/**
 * @file
 * Abstract and interface plugin implementation for
 * First Data Global Gateway e4 components
 */

/**
 * Provide a separate Exception so it can be caught separately.
 */
class CommerceFirstDataGGE4ComponentException extends Exception { }

/**
 * Ensures basic required behavior for a license.
 *
 * EntityBundlePluginProvideFieldsInterface also mandates a fields() method.
 */
interface CommerceFirstDataGGE4ComponentInterface {
  /**
   * Returns TRUE if the plugin is valid based on the current settings
   */
  public function isValid();

  /**
   * Default settings
   */
  public function defaultSettings();
  
  /**
   * Settings form
   */
  public function settingsForm();
}

/**
 * Base component class
 */
abstract class CommerceFirstDataGGE4ComponentBase implements CommerceFirstDataGGE4ComponentInterface {
  /**
   * The plugin definition.
   */
  protected $plugin;

  /**
   * The First Data GGE4 Controller
   */
  protected $controller;

  /**
   * Constructor.
   *
   * @param $controller
   *   The controller class
   */
  public function __construct($plugin, CommerceFirstDataGGE4Controller $controller) {
    $this->plugin = $plugin;
    $this->controller = $controller;
  }

  /**
   * Default settings
   */
  public function defaultSettings() {
    return array('enable' => FALSE);
  }

  /**
   * Returns TRUE if the plugin is enabled
   */
  public function isValid() {
    $settings = $this->getSettings();
    $defaults = $this->defaultSettings();
    return count(array_filter($settings)) >= count($defaults);
  }

  /**
   * Returns the controller settings
   *
   * @param $name
   *   Optional. A property name
   */
  public function getSettings($name = NULL) {
    $settings = $this->controller->getSettings($this->plugin['name']);
    if (!empty($settings)) {
      $settings += $this->defaultSettings();
    }
    else {
      $settings = $this->defaultSettings();
    }

    if (isset($name)) {
      return isset($settings[$name]) ? $settings[$name] : NULL;
    }

    return $settings;
  }

  /**
   * Settings form
   */
  public function settingsForm() {
    $form = array();

    $form['enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable @title', array('@title' => $this->plugin['title'])),
      '#description' => t('@description', array('@description' => !empty($this->plugin['description']) ? $this->plugin['description'] : '')),
      '#default_value' => $this->getSettings('enable'),
    );

    return $form;
  }
}
