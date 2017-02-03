<?php

/**
 * @file
 * Draggableviews views native handler sort.
 */

/**
 * Sort handler for ordering by weight.
 */
class draggableviews_handler_sort extends views_handler_sort {
  function query() {
    $this->ensure_my_table();
    // If new items should be placed in the bottom.
    if ($this->options['draggableviews_setting_new_items_bottom_list']) {
      // New items will get the biggest integer possible instead of NULL.
      $field = "COALESCE($this->table_alias.$this->field, 2147483647)";
      $alias = $this->table_alias . '_' . $this->field . '_coalesce';

      // No need to call $this->query->add_field(), add_orderby() adds the field.
      $this->query->add_orderby(NULL, $field, $this->options['order'], $alias);
    }
    else {
      // New items will be placed at the top as have NULL value.
      $this->query->add_orderby($this->table_alias, $this->real_field, $this->options['order']);
    }
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['draggableviews_setting_view'] = array('default' => '');
    $options['draggableviews_setting_arguments'] = array('default' => 'all');
    $options['draggableviews_setting_arguments_php'] = array('default' => '');
    $options['draggableviews_setting_new_items_bottom_list'] = array('default' => TRUE);

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['expose_button']['#access'] = FALSE;
    $form['order']['#description'] = t('Please remember to override settings of the sort criterion if you have display that sets weights and you choose descending order.');

    // Grab all of the views and their display that contain a draggableviews
    // field. If this display has one then 'view->name:view->current_display'
    // is returned.
    $options = _draggableviews_get_views_options($this->view);

    // If it is setting view.
    if (!is_array($options)) {
      $form['order']['#access'] = FALSE;
      $form['draggableviews_setting_view'] = array(
        '#type' => 'value',
        '#value' => $options,
      );
    }
    else {
      $form['draggableviews_setting_view'] = array(
        '#type' => 'select',
        '#title' => t('Display sort as'),
        '#default_value' => $this->options['draggableviews_setting_view'],
        '#options' => $options,
        '#description' => t('Please choose the view and display that sets the order.')
      );
      // If there is no setting views available, show error message.
      if (empty($options)) {
        drupal_set_message(t('First you should create a view that sets sorting order.'), 'error');
      }
    }

    $form['draggableviews_setting_arguments'] = array(
      '#title' => t('Arguments handling'),
      '#type' => 'radios',
      '#options' => array(
        'all' => t('Use all arguments'),
        'none' => t('Do not use any arguments (use empty arguments)'),
        'php' => t('Prepare arguments with PHP code'),
      ),
      '#default_value' => $this->options['draggableviews_setting_arguments'],
      '#description' => t('When sorting order is saved all arguments passed are saved with order.
          In display view we can choose how to use these arguments.')
    );
    $form['draggableviews_setting_arguments_php'] = array(
      '#title' => t('PHP code to prepare arguments'),
      '#type' => 'textarea',
      '#default_value' => $this->options['draggableviews_setting_arguments_php'],
      '#description' => t('Enter the php code to prepare the arguments. Do not enter &lt;?php ?&gt; tags.
          The following variables are available - $view (the view), $arguments (existing arguments -
          manipulate these to alter the arguments used to sort). See README.txt for more details.'),
      '#states' => array(
        'visible' => array(
          'input[name="options[draggableviews_setting_arguments]"]' => array('value' => 'php'),
        ),
      ),
    );
    $form['draggableviews_setting_new_items_bottom_list'] = array(
      '#type' => 'checkbox',
      '#title' => t('New items appear bottom of the list'),
      '#description' => t('New items means elements (for example nodes) that do not have saved weight (newly created).'),
      '#default_value' => $this->options['draggableviews_setting_new_items_bottom_list'],
    );
  }
}
