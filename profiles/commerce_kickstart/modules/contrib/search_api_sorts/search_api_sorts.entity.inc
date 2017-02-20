<?php

/**
 * @file
 * The Search API sort entity definition.
 */

/**
 * Class representing a search sort.
 */
class SearchApiSort extends Entity {

  /* Database values that will be set when object is loaded: */

  /**
   * The Search API sort id.
   *
   * @var integer
   */
  public $id = 0;

  /**
   * The Search API sort index id.
   *
   * @var string
   */
  public $index_id = '';

  /**
   * The Search API sort field name.
   *
   * @var string
   */
  public $field = '';

  /**
   * The Search API sort name.
   *
   * @var string
   */
  public $name = '';

  /**
   * The Search API sort is enabled.
   *
   * @var integer
   */
  public $enabled = 0;

  /**
   * The Search API sort weight.
   *
   * @var integer
   */
  public $weight = 0;

  /**
   * Constructor as a helper to the parent constructor.
   */
  public function __construct(array $values = array()) {
    parent::__construct($values, 'search_api_sort');
    if (empty($this->options['field_name'])) {
      $index = search_api_index_load($this->index_id);
      if (is_object($index)) {
        $fields = $index->getFields();
        $this->options['field_name'] = isset($fields[$this->field]['name']) ? $fields[$this->field]['name'] : $this->field;
      }
      else {
        $this->options['field_name'] = $this->field;
      }
    }
  }

}
