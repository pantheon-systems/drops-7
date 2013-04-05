<?php

/**
 * Class representing a search sort.
 */
class SearchApiSort extends Entity {

  /* Database values that will be set when object is loaded: */

  /**
   * @var integer
   */
  public $id = 0;

  /**
   * @var string
   */
  public $index_id = '';

  /**
   * @var string
   */
  public $field = '';

  /**
   * @var string
   */
  public $name = '';

  /**
   * @var integer
   */
  public $enabled = 0;

  /**
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
