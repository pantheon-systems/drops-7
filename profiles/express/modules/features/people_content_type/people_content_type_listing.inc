<?php

/**
 * @file
 * csvImporter.module
 * Let's import a file.  Subsequent uploads will overwrite said file.
 */

/**
 * TODO: Write function doco.
 */
function _people_content_type_listing($view_mode, $type, $department, $filter_1, $filter_2, $filter_3, $order) {

  $output = array();
  // Get filter form.
  //$output['filter'] = drupal_get_form('people_content_type_form');
  // Start query - look to see if type is set, otherwise get people with type
  // set and merge with people who don't have it set.


  // Select people who do not have a job type,
  // order by last name.
  $query = db_select('node', 'n');
  $query->condition('n.type', 'person', '=');
  $query->condition('n.status', 1, '=');
  $query->fields('n', array('nid', 'title'));

  if ($order == 'type') {
    // order by type weight
    $query->join('field_data_field_person_job_type', 'job', 'n.nid = job.entity_id');
    $query->fields('job', array('field_person_job_type_tid'));
    $query->join('taxonomy_term_data', 'td', 'job.field_person_job_type_tid = td.tid');
    $query->orderBy('td.weight', 'ASC');
  }
  // then order by last name, first name
  $query->join('field_data_field_person_last_name', 'last', 'n.nid = last.entity_id');
  $query->join('field_data_field_person_first_name', 'first', 'n.nid = first.entity_id');
  $query->fields('first', array('field_person_first_name_value'));
  $query->fields('last', array('field_person_last_name_value'));
  $query->orderBy('last.field_person_last_name_value', 'ASC');
  $query->orderBy('first.field_person_first_name_value', 'ASC');


  // Filter by department
  if ($department) {
    $query->join('field_data_field_person_department', 'dept', 'n.nid = dept.entity_id');
    $query->fields('dept', array('field_person_department_tid'));
    $query->condition('dept.field_person_department_tid', $department, 'in');
  }

  // Filter by type
  if ($type) {
    $query->join('field_data_field_person_job_type', 'type', 'n.nid = type.entity_id');
    $query->fields('type', array('field_person_job_type_tid'));
    $query->condition('type.field_person_job_type_tid', $type, 'in');
  }

  // Filter by type
  if ($filter_1) {
    $query->join('field_data_field_person_filter_1', 'filter1', 'n.nid = filter1.entity_id');
    $query->fields('filter1', array('field_person_filter_1_tid'));
    $query->condition('filter1.field_person_filter_1_tid', $filter_1, 'in');
  }

  // Filter by type
  if ($filter_2) {
    $query->join('field_data_field_person_filter_2', 'filter2', 'n.nid = filter2.entity_id');
    $query->fields('filter2', array('field_person_filter_2_tid'));
    $query->condition('filter2.field_person_filter_2_tid', $filter_2, 'in');
  }

  // Filter by type
  if ($filter_3) {
    $query->join('field_data_field_person_filter_3', 'filter3', 'n.nid = filter3.entity_id');
    $query->fields('filter3', array('field_person_filter_3_tid'));
    $query->condition('filter3.field_person_filter_3_tid', $filter_3, 'in');
  }
  // execute query
  $nids = $query->execute()->fetchCol();

  if ($order == 'type') {
    // get persons who don't have a type set
    $null_query = db_select('node', 'n');
    $null_query->condition('n.type', 'person', '=');
    $null_query->condition('n.status', 1, '=');
    $null_query->fields('n', array('nid', 'title'));
    $null_query->join('field_data_field_person_last_name', 'last', 'n.nid = last.entity_id');
    $null_query->fields('last', array('field_person_last_name_value'))
      ->orderBy('last.field_person_last_name_value', 'ASC');
    $null_query->leftjoin('field_data_field_person_job_type', 'job', 'n.nid = job.entity_id');
    $null_query->fields('job', array('field_person_job_type_tid'));
    $null_query->isNull('job.field_person_job_type_tid');

    // Filter by department
    if ($department) {
      $null_query->join('field_data_field_person_department', 'dept', 'n.nid = dept.entity_id');
      $null_query->fields('dept', array('field_person_department_tid'));
      $null_query->condition('dept.field_person_department_tid', $department, 'in');
    }

    // Filter by type
    if ($type) {
      $null_query->join('field_data_field_person_job_type', 'type', 'n.nid = type.entity_id');
      $null_query->fields('type', array('field_person_job_type_tid'));
      $null_query->condition('type.field_person_job_type_tid', $type, 'in');
    }

    // Filter by type
    if ($filter_1) {
      $null_query->join('field_data_field_person_filter_1', 'filter1', 'n.nid = filter1.entity_id');
      $null_query->fields('filter1', array('field_person_filter_1_tid'));
      $null_query->condition('filter1.field_person_filter_1_tid', $filter_1, 'in');
    }

    // Filter by type
    if ($filter_2) {
      $null_query->join('field_data_field_person_filter_2', 'filter2', 'n.nid = filter2.entity_id');
      $null_query->fields('filter2', array('field_person_filter_2_tid'));
      $null_query->condition('filter2.field_person_filter_2_tid', $filter_2, 'in');
    }

    // Filter by type
    if ($filter_3) {
      $null_query->join('field_data_field_person_filter_3', 'filter3', 'n.nid = filter3.entity_id');
      $null_query->fields('filter3', array('field_person_filter_3_tid'));
      $null_query->condition('filter3.field_person_filter_3_tid', $filter_3, 'in');
    }

    // execute query
    $null_nids = $null_query->execute()->fetchCol();

    // Combine the results in to a single array.
    $all_nids = array_merge($nids, $null_nids);
    $nodes = node_load_multiple($all_nids);
  } else {
    $nodes = node_load_multiple($nids);
  }
  // build display if there are results, otherwise display no results message
  if ($nodes) {
    $output = node_view_multiple($nodes, $view_mode);
    // Add fisrt and last classes for grid mode
    if ($view_mode =='grid') {
      // Get nodes only, not other #arrays
      $nids = element_children($output['nodes']);
      $count = 0;
      foreach ($nids as $key => $nid) {
        if ($count == 0) {
          $class = 'people-grid-first';
        } elseif ($count == 2) {
          $class = 'people-grid-last';
        } else {
          $class = '';
        }
        $output['nodes'][$nid]['attributes']['classes'][] = $class;
        $count = $count + 1;
        // Reset counter
        $count = ($count > 2) ? 0 : $count;

      }
    }
    if ($view_mode == 'table') {
      // Build an array which can be themed as a table
      $nids = element_children($output['nodes']);
      $rows = array();
      // Set a variable to see if we need to display the photo column
      $photos = FALSE;
      foreach ($nids as $key => $nid) {

        // prepare contact section
        $contact = '';
        if (!empty($output['nodes'][$nid]['field_person_email'])) {
          $contact .= '<div class="table-person-email"><i class="fa fa-envelope"></i> <a href="mailto:' . render($output['nodes'][$nid]['field_person_email']) . '">Email</a></div>';
        }
        if (!empty($output['nodes'][$nid]['field_person_phone'])) {
          $contact .= '<div class="table-person-phone"><i class="fa fa-phone"></i> ' . render($output['nodes'][$nid]['field_person_phone']) . '</div>';
        }

        // If there is a photo, we must print photo column
        if (!empty($output['nodes'][$nid]['field_person_photo'])) {
          $photos = TRUE;
        }

        // Build row/cells
        $rows[$nid] = array(
          'photo' => array(
            'data' => render($output['nodes'][$nid]['field_person_photo']),
            'class' => array('person-table-photo'),
          ),
          'name' => array(
            'data' => render($output['nodes'][$nid]),
            'class' => array('person-table-name'),
          ),
          'contact' => array(
            'data' => $contact,
            'class' => array('person-table-contact'),
          ),
        );
      }

      // Table headers
      $headers = array(
        'photo' => array('data' => '<span class="element-invisible">Photo</span>'),
        'name' => array('data' => t('Name')),
        'contact' => array('data' => t('Contact Information'))
      );

      // If there were no photos, unset the column, remove header
      if ($photos == FALSE) {
        foreach ($rows as $key => $row) {
          unset($rows[$key]['photo']);
        }
        unset($headers['photo']);
      }

      $output = array(
        '#theme' => 'table',
        '#rows' => $rows,
        '#header' => $headers,
        '#attributes' => array(
          'class' => array(
            'people-list-table',),
        ),
      );
    }

    return $output;
  } else {
    return NULL;
  }
}

function _people_content_type_grouping($group) {

}
