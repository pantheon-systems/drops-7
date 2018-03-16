<?php

/**
 * @file
 * Base plugin implementation.
 */

/**
 * Parent class for all hierarchy handlers.
 */
class draggableviews_hierarchy_handler {

  /**
   * Get the parent value.
   *
   * @param object $field
   *   Draggableviews field handler. View is $field->view,
   *   to get a row $field->view->result[$index].
   * @param int $index
   *   Index of the row.
   *
   * @return int
   *   Weight value.
   */
  public function get($field, $index) {}

  /**
   * Save parent value.
   *
   * @param $form_state
   *   Array of form state of the form.
   *   View object $form_state['values']['view'].
   */
  public function set($form_state) {}

  /**
   * Form with settings of the handler.
   *
   * @param object $field
   *  Draggableviews field handler.
   *
   * @return array
   *  Form array.
   */
  public function options_form($field) {}

  /**
   * Settings form default values.
   *
   * @return array
   *   Array with default values.
   */
  public function option_definition() {}

  /**
   * Get "results" array index of and item with specific base field id.
   *
   * @param object $view
   *   Views object
   * @param type $id
   *   Base field id.
   *
   * @return int
   */
  public function get_index($view, $id) {
    foreach ($view->result as $key => $item) {
      if ($item->{$view->base_field} == $id) {
        return $key;
      }
    }
  }
}
