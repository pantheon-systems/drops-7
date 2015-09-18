<?php

/**
 * @file
 * Native handler join handler.
 */

/**
 * Join handler for extra join conditions.
 */
class draggableviews_join_handler extends views_join {
  /**
   * Build the SQL for the join this object represents.
   */
  function build_join($select_query, $table, $view_query) {
    $view = $view_query->view;

    if (empty($this->definition['table formula'])) {
      $right_table = $this->table;
    }
    else {
      $right_table = $this->definition['table formula'];
    }

    if ($this->left_table) {
      $left = $view_query->get_table_info($this->left_table);
      $left_field = "$left[alias].$this->left_field";
    }
    else {
      // This can be used if left_field is a formula or something. It should be used only *very* rarely.
      $left_field = $this->left_field;
    }

    $condition = "$left_field = $table[alias].$this->field";

    // Check whether setting view is set.
    $arguments = array();
    $weight_key = _draggableviews_get_draggable_sort($view);
    if (!empty($view->sort[$weight_key]->options['draggableviews_setting_view'])) {
      $condition .= " AND $table[alias].view_name = :view_name";
      $condition .= " AND $table[alias].view_display = :view_display";

      // If it is setting view, set current view name and display name.
      if (isset($view->field['draggableviews'])) {
      // if ($view->sort[$weight_key]->options['draggableviews_setting_view'] == 'self') {
        $arguments[':view_name'] = $view->name;
        $arguments[':view_display'] = $view->current_display;
      }
      else {
        list($setting_view_name, $setting_view_display) = explode(':', $view->sort[$weight_key]->options['draggableviews_setting_view']);
        $arguments[':view_name'] = $setting_view_name;
        $arguments[':view_display'] = $setting_view_display;
      }

      // Arguments passed to view (including exposed filters).
      $view_arguments = isset($view->args) ? $view->args : array();
      if (isset($view->exposed_raw_input)) {
        $view_arguments += $view->exposed_raw_input;
        // Remove 'items_per_page' argument if added by exposed pager filter.
        unset($view_arguments['items_per_page']);
        ksort($view_arguments);
      }
      // Alter arguments according to sort criteria settings.
      if ($view->sort[$weight_key]->options['draggableviews_setting_arguments'] == 'none') {
        $view_arguments = array();
      }
      // If PHP arguments processing is set.
      if ($view->sort[$weight_key]->options['draggableviews_setting_arguments'] == 'php') {
        $clone_view = clone $view;
        $view_arguments = _draggableviews_eval_return($view->sort[$weight_key]->options['draggableviews_setting_arguments_php'], $view_arguments, $clone_view);
      }
      $condition .= " AND $table[alias].args = :view_arguments";
      $arguments[':view_arguments'] = json_encode($view_arguments);
    }

    $select_query->addJoin($this->type, $right_table, $table['alias'], $condition, $arguments);

    // Add also parent field.
    if ($order_view = _draggableviews_load_order_view($view)) {
      if (isset($order_view->field['draggableviews']->options['draggableviews']['hierarchy_handler'])
        && $order_view->field['draggableviews']->options['draggableviews']['hierarchy_handler'] == 'draggableviews_hierarchy_handler_native') {
        $select_query->addField($table['alias'], 'parent', 'draggableviews_structure_parent');
      }
    }
  }
}