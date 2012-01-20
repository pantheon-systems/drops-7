<?php

/**
 * @file
 */

class views_boxes_handler_filter_sort extends views_handler_filter_in_operator {
  function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Sort Plugins');
      $this->view->display_handler->get_handlers('sort');
      $options = array();
      foreach ($this->view->display_handler->handlers['sort'] as $key => $handler) {
        // Only show items that have been exposed
        // using the exposed filters means that the exposed form gets involved
        // and we do not like the way it does it.
        $name = $handler->options['ui_name'] ? $handler->options['ui_name'] : $key;
        $options[$key] = $name;
      }
      $this->value_options = $options;
    }
  }


  function query() {
    // This should not be necessary but for some reason when we use
    // $view->set_exposed_input() it is not making it to value
    if (isset($this->view->exposed_input[$this->options['id']])) {
      $this->value = $this->view->exposed_input[$this->options['id']];
    }

    if (!is_array($this->value)) {
      $this->value = array($this->value => $this->value);
    }

    // Run though each value and grab the sort handler that we already have
    // then reset the sort array on the view
    // note this method lets us also change the order of the sort if 
    // it is set correctly on the way in.
    if (!empty($this->value)) {
      $pre_sort = array();
      $sort_canidates = array();
      $hit_sort = FALSE;
      foreach ($this->view->sort as $key => $handle) {
        if (in_array($key, $this->options['value'])) {
          $sort_canidates[$key] = $handle;
          $hit_sort = TRUE;
          unset($this->view->sort[$key]);
        }
        elseif (!$hit_sort) {
          $pre_sort[$key] = $handle;
          unset($this->view->sort[$key]);
        }
      }
      $sort_ordered = array();
      foreach ($this->value as $key) {
        $sort_ordered[$key] = $sort_canidates[$key];
      }
      $this->view->sort = $pre_sort + $sort_ordered + $this->view->sort;
    }
  }

}
