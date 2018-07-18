<?php

/**
 * @file
 * Defines the default query object which builds queries for the
 * Google Analytics Reports API.
 */

/**
 * Object used to create a Google Analytics Core Reporting API query.
 */
class google_analytics_reports_plugin_query_google_analytics extends views_plugin_query {

  /**
   * Constructor; Create the basic query object and fill with default values.
   */
  function init($base_table, $base_field, $options) {
    parent::init($base_table, $base_field, $options);
    $this->unpack_options($this->options, $options);
  }

  /**
   * Add a metric or dimension to the query.
   *
   * @param $table
   *   NULL in most cases, we could probably remove this altogether.
   * @param $field
   *   The name of the metric/dimension/field to add.
   * @param $alias
   *   Probably could get rid of this too.
   * @param $params
   *   Probably could get rid of this too.
   *
   * @return $name
   *   The name that this field can be referred to as.
   */
  function add_field($table, $field, $alias = '', $params = array()) {
    // We check for this specifically because it gets a special alias.
    if ($table == $this->base_table && $field == $this->base_field && empty($alias)) {
      $alias = $this->base_field;
    }

    if ($table && empty($this->table_queue[$table])) {
      $this->ensure_table($table);
    }

    if (!$alias && $table) {
      $alias = $table . '_' . $field;
    }

    // Make sure an alias is assigned.
    $alias = $alias ? $alias : $field;

    // We limit the length of the original alias up to 60 characters
    // to get a unique alias later if its have duplicates.
    $alias = substr($alias, 0, 60);

    // Create a field info array.
    $field_info = array(
      'field' => $field,
      'table' => $table,
      'alias' => $alias,
    ) + $params;

    // Test to see if the field is actually the same or not. Due to
    // differing parameters changing the aggregation function, we need
    // to do some automatic alias collision detection:
    $base = $alias;
    $counter = 0;
    while (!empty($this->fields[$alias]) && $this->fields[$alias] != $field_info) {
      $field_info['alias'] = $alias = $base . '_' . ++$counter;
    }

    if (empty($this->fields[$alias])) {
      $this->fields[$alias] = $field_info;
    }

    // Keep track of all aliases used.
    $this->field_aliases[$table][$field] = $alias;

    return $alias;
  }

  /**
   * Add a filter string to the query. 
   *
   * @param $group
   *   The filter group to add these to; groups are used to create AND/OR
   *   sections of the Google Analytics query. Groups cannot be nested. 
   *   Use 0 as the default group.  If the group does not yet exist it will 
   *   be created as an AND group.
   * @param $field
   *   The name of the metric/dimension/field to check.
   * @param $value
   *   The value to test the field against. In most cases, this is a scalar. 
   * @param $operator
   *   The comparison operator, such as =, <, or >=. 
   */
  function add_where($group, $field, $value = NULL, $operator = NULL) {
    // Ensure all variants of 0 are actually 0. Thus '', 0 and NULL are all
    // the default group.
    if (empty($group)) {
      $group = 0;
    }

    // Check for a group.
    if (!isset($this->where[$group])) {
      $this->set_where_group('AND', $group);
    }

    $this->where[$group]['conditions'][] = array(
      'field' => $field,
      'value' => $value,
      'operator' => $operator,
    );
  }

  /**
   * Add SORT attribute to the query.
   *
   * @param $table
   *   NULL, don't use this.
   * @param $field
   *   The metric/dimensions/field
   * @param $order
   *   Either '' for ascending or '-' for descending.
   * @param $alias
   *   Don't use this yet (at all?).
   * @param $params
   *   Don't use this yet (at all?).
   */
  function add_orderby($table, $field = NULL, $order = 'ASC', $alias = '', $params = array()) {
    $this->orderby[] = array(
      'field' => $field,
      'direction' => (strtoupper($order) == 'DESC') ? '-' : '',
    );
  }

  /**
   * Generate a query and a countquery from all of the information supplied
   * to the object.
   *
   * @param $get_count
   *   Provide a countquery if this is true, otherwise provide a normal query.
   */
  function query($get_count = FALSE) {
    $available_fields = google_analytics_reports_get_fields();
    $query = array();

    foreach ($this->fields as $field) {
      $field_name = google_analytics_reports_variable_to_custom_field($field['field']);

      if ($available_fields[$field_name]) {
        $type = $available_fields[$field_name]->type;
        $type = ($type == 'dimension') ? 'dimensions' : 'metrics';
        $query[$type][] = 'ga:' . $field['field'];
      }
    }

    $filters = array();

    if (isset($this->where)) {
      foreach ($this->where as $where_group => $where) {
        foreach ($where['conditions'] as $condition) {
          $field_name = google_analytics_reports_variable_to_custom_field($condition['field']);

          if ($field_name == 'start_date' || $field_name == 'end_date' || $field_name == 'profile_id') {
            $query[$field_name] = intval($condition['value']);
          }
          elseif ($available_fields[$field_name]) {
            $filters[$where_group][] = 'ga:' . $condition['field'] . $condition['operator'] . $condition['value'];
          }
        }
        if (!empty($filters[$where_group])) {
          $glue = ($where['type'] == 'AND') ? ';' : ',';
          $filters[$where_group] = implode($glue, $filters[$where_group]);
        }
      }
    }

    if (!empty($filters)) {
      $glue = ($this->group_operator == 'AND') ? ';' : ',';
      $query['filters'] = implode($glue, $filters);
    }

    if (isset($this->orderby)) {
      foreach ($this->orderby as $field) {
        $query['sort_metric'][] = $field['direction'] . 'ga:' . $field['field'];
      }
    }

    // Change reports profile.
    if (isset($this->options['reports_profile']) && (!empty($this->options['profile_id']))) {
      $query['profile_id'] = $this->options['profile_id'];
    }

    return $query;
  }

  /**
   * {@inheritdoc}
   */
  function alter(&$view) {
    foreach (module_implements('views_query_alter') as $module) {
      $function = $module . '_views_query_alter';
      $function($view, $this);
    }
  }

  /**
   * Builds the necessary info to execute the query.
   */
  function build(&$view) {
    // Store the view in the object to be able to use it later.
    $this->view = $view;

    $view->init_pager();

    // Let the pager modify the query to add limits.
    $this->pager->query();

    $view->build_info['query'] = $this->query();
    $view->build_info['count_query'] = $this->query(TRUE);
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->current_page.
   */
  function execute(&$view) {
    // Initial check to see if we should attempt to run the query.
    if (!variable_get('google_analytics_reports_api_access_token', FALSE)) {
      // Optionally do not warn users on every query attempt before auth.
      if (variable_get('google_analytics_reports_warning', TRUE)) {
        drupal_set_message(t('You must <a href="@url">authorize</a> Drupal to use your Google Analytics account before you can view reports.',
          array('@url' => url('admin/config/system/google-analytics-reports-api'))), 'warning', FALSE);
      }
      return;
    }

    $query = $view->build_info['query'];
    $count_query = $view->build_info['count_query'];
    $start = microtime(TRUE);

    // Query for total number of items.
    $count_query['max_results'] = 9999;
    $count_query['start_index'] = 1;
    $count_feed = google_analytics_reports_api_report_data($count_query);

    // Process only if data is available.
    if (!empty($count_feed->results->rows)) {
      $this->pager->total_items = count($count_feed->results->rows);
      $this->pager->update_page_info();

      // Adjust based on the pager's modifications to limit and offset.
      if (!empty($this->limit) || !empty($this->offset)) {
        $query['max_results'] = intval(!empty($this->limit) ? $this->limit : 1000);
        $query['start_index'] = intval(!empty($this->offset) ? $this->offset : 0) + 1;
      }

      $feed = google_analytics_reports_api_report_data($query);

      $rows = $feed->results->rows;
      foreach ($rows as $row) {
        $views_result[] = (object) $row;
      }

      $view->result = isset($views_result) ? $views_result : array();
      $view->execute_time = microtime(TRUE) - $start;
      if ($this->pager->use_pager()) {
        $view->total_rows = $this->pager->get_total_items();
      }

      // Add to build_info['query'] to render query in Views UI query summary
      // area.
      $view->build_info['query'] = print_r($feed->results->query, TRUE);
    }
    else {
      // Set empty query instead of current query array to prevent error
      // in Views UI.
      $view->build_info['query'] = '';
      // Display the error from Google.
      $response_data = drupal_json_decode($count_feed->response->data);
      if (isset($response_data['error']['message'])) {
        drupal_set_message(check_plain($response_data['error']['message']), 'error');
      }
    }

  }

  /**
   * Definition of views options.
   */
  function option_definition() {
    $options = parent::option_definition();

    // Load profiles list.
    module_load_include('inc', 'google_analytics_reports_api', 'google_analytics_reports_api.admin');
    $profile_list = google_analytics_reports_api_profiles_list();

    if ($profile_list) {
      $options['reports_profile'] = array(
        'default' => FALSE,
        'translatable' => FALSE,
        'bool' => TRUE,
      );
      $options['profile_id'] = array(
        'default' => $profile_list['profile_id'],
      );
    }

    return $options;
  }

  /**
   * Add settings for the ui.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // Load profiles list.
    module_load_include('inc', 'google_analytics_reports_api', 'google_analytics_reports_api.admin');
    $profile_list = google_analytics_reports_api_profiles_list();

    if ($profile_list) {
      $form['reports_profile'] = array(
        '#title' => t('Use another reports profile'),
        '#description' => t('This view will use another reports profile rather than system default profile: %profile.', array(
          '%profile' => theme('google_analytics_reports_api_profile_label',
            array(
              'profile' => $profile_list['current_profile'],
              'active' => TRUE,
            )),
        )),
        '#type' => 'checkbox',
        '#default_value' => !empty($this->options['reports_profile']),
      );
      $form['profile_id'] = array(
        '#type' => 'select',
        '#title' => t('Reports profile'),
        '#options' => $profile_list['options'],
        '#description' => t('Choose your Google Analytics profile.'),
        '#default_value' => $this->options['profile_id'],
        '#dependency' => array('edit-query-options-reports-profile' => '1'),
      );
    }

  }
}
