<?php

/**
 * @file
 */

// $Id: views_handler_argument_count.inc,v 1.1 2008/09/03 19:21:28 merlinofchaos Exp $
/**
 * Argument handler that ignores the argument.
 */
class views_boxes_handler_argument_limit extends views_handler_argument {

  /**
   * Override options_form()
   * 
   * We are just adding markup to warn that a pager acts after we do.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['must_not_be'] = array(
      '#markup' => t('This argument will have no effect if a pager is in use.'),
    );
  }

  /**
   * Override default_actions() to remove actions that don't
   * make sense for a count argument.
   * @todo need to see if this is necessary
   */
  function default_actions($which = NULL) {
    if ($which) {
      if (in_array($which, array('ignore', 'not found', 'empty', 'default'))) {
        return parent::default_actions($which);
      }
      return;
    }
    $actions = parent::default_actions();
    unset($actions['summary asc']);
    unset($actions['summary desc']);
    return $actions;
  }

  /**
   * Override the behavior of query() to prevent the query
   * from being changed in any way.
   */
  function query( $group_by = FALSE ) {
  }
  
  /**
   * Override pre_query()
   *
   * We set the limit on the queue if we have a non wildcard value
   */
  function pre_query() {
    if (isset($this->view->args[$this->position])) {
      $limit = $this->view->args[$this->position];
      if ($limit != $this->options['exception']['value']) {
        $this->view->query->set_limit($limit);
      }
    }
  }
}
