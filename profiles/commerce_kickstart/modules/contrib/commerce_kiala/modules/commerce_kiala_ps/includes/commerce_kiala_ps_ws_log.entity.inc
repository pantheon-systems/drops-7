<?php

/**
 * @file
 * Provides a base class for Web service log entity types.
 */

/**
 * A Kiala Pack & Ship web service log entity class.
 */
class CommerceKialaPSWSLogEntity extends Entity {

  /**
   * Overridden __construct()
   *  - set fixed entity type
   *  - merge in defaults
   */
  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, 'commerce_kiala_ps_ws_log');
  }

  // -----------------------------------------------------------------------
  // Base Class overridden functions

  /**
   * Specifies the default uri, which is picked up by uri() by default.
   */
  protected function defaultURI() {
    return array('path' => $this->entityType . 's/' . $this->identifier());
  }


  // -----------------------------------------------------------------------
  // Custom functions

  /**
   * Return entity controller
   */
  public function get_controller() {
    return entity_get_controller($this->entityType);
  }

  /**
   * Access callback
   */
  public function access($op = 'view', $account = NULL) {
    return entity_access($op, $this->entityType, $this, $account);
  }

  /**
   * Invoke hook
   */
  public function invoke($hook) {
    return $this->get_controller()->invoke($hook, $this);
  }

  /**
   * Commerce Order
   */
  public function get_order() {
    if (isset($this->order_id)) {
      return commerce_order_load($this->order_id);
    }
    return NULL;
  }


  // -----------------------------------------------------------------------
  // Magic Methods

  /**
   * Get / Set
   *  - method is preferred with lazy fallback to object property
   */
  public function __get($name) {
    $method = "get_$name";
    if (method_exists($this, $method)) {
      return $this->{$method}();
    }

    // allow lazy get
    if (isset($this->{$name})) {
      return $this->{$name};
    }
    return NULL;
  }

  public function __set($name, $value) {
    $method = "set_$name";
    if (method_exists($this, $method)) {
      $this->{$method}($value);
    }
    elseif (!method_exists($this, "get_$name")) {
      // allow lazy setting for non read only properties
      $this->{$name} = $value;
    }
  }

  public function __isset($name) {
    $value = $this->__get($name);
    return isset($value);
  }

  public function __unset($name) {
    unset($this->{$name});
  }
}
