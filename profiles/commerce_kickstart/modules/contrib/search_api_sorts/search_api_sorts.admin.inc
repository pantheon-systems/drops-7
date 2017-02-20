<?php

/**
 * @file
 * Administration page callbacks for the Search API sorts module.
 */

/**
 * Displays a form for selecting the indexed fields for which to create sorts.
 *
 * @param SearchApiIndex $index
 *   The index for which to display the settings.
 */
function search_api_sorts_index_select(array $form, array &$form_state, SearchApiIndex $index) {
  module_load_include('admin.inc', 'search_api');
  drupal_set_title(search_api_admin_item_title($index));
  $form_state['index'] = $index;

  if (!$index->server() || !$index->server()->supportsFeature('search_api_facets')) {
    // We can't offer any sorts, but maybe the user can delete old sort data.
    drupal_set_message(t("The server this index currently lies on doesn't support sorts. To use sorts, you will have to move this index to a server supporting this feature."), 'error');
    return $form;
  }

  // Get the sorts ordered by weight using EntityFieldQuery.
  $query = new EntityFieldQuery();
  $results = $query
    ->entityCondition('entity_type', 'search_api_sort')
    ->propertyCondition('index_id', $index->machine_name)
    ->propertyOrderBy('weight', 'ASC')
    ->execute();

  $sort_ids = array();
  if (isset($results['search_api_sort'])) {
    foreach ($results['search_api_sort'] as $result) {
      $sort_ids[] = $result->id;
    }
  }
  $sorts = search_api_sorts_load_multiple($sort_ids);

  $form['description'] = array(
    '#type' => 'item',
    '#title' => t('Select the available sorts'),
    '#description' => t('<p>Only index single-value strings or numbers can be used as sorts. See the Fields tab to change indexes (tick the "Indexed" checkboxes).</p>'),
  );
  $form['sorts'] = array(
    '#tree' => TRUE,
    '#theme' => 'search_api_sorts_form_table',
    '#table_header' => array(
      t('Enabled'),
      t('Default'),
      t('Default (no terms)'),
      t('Sort'),
      t('Field'),
      t('Type'),
      t('Name'),
      t('Weight'),
    ),
    '#table_empty' => t('There are currently no fields for which sorts can be displayed.'),
  );

  // Add our dummy relevance field.
  $fields = array(
    'search_api_relevance' => array(
      'name' => 'Relevance',
      'type' => 'decimal',
      'indexed' => TRUE,
    ),
  ) + $index->getFields();

  if (!empty($fields)) {
    if ($disabled = empty($index->enabled)) {
      drupal_set_message(t('Since this index is at the moment disabled, no sorts can be activated.'), 'warning');
    }
    $show_status = FALSE;
    foreach ($sorts as $sort) {
      $sorts[$sort->field][] = $sort;
      if (isset($sort->status) & ENTITY_IN_CODE) {
        $show_status = TRUE;
      }
    }
    if ($show_status) {
      $form['sorts']['#table_header'] = array(
        t('Enabled'),
        t('Default'),
        t('Default (no terms)'),
        t('Sort'),
        t('Status'),
        t('Field'),
        t('Type'),
        t('Name'),
        t('Weight'),
      );
      $empty_status = ' ';
    }

    $types = search_api_field_types();

    // Need to get the index fields ordered by sort's weight.
    foreach ($sorts as $key => $sort) {
      if (isset($fields[$key])) {
        $fields[$key]['weight'] = isset($sorts[$key][0]->weight) ? $sorts[$key][0]->weight : 0;
      }
    }

    // Order fields according to sorts weight.
    $new_fields = array();
    foreach ($sorts as $key => $sort) {
      if (!empty($fields[$key])) {
        $new_fields[$key] = $fields[$key];
        unset($fields[$key]);
      }
    }
    $fields = $new_fields + $fields;

    foreach ($fields as $key => $field) {
      if (!$field['indexed']) {
        continue;
      }
      // Skip fulltext or multi-value, you cannot sort them.
      if ($field['type'] == 'text' || strpos($field['type'], 'list<') !== FALSE) {
        continue;
      }
      $type = search_api_extract_inner_type($field['type']);
      $type = isset($types[$type]) ? $types[$type] : $type;
      if (empty($sorts[$key])) {
        $weight = 0;
        $sorts[$key][] = new SearchApiSort(array(
          'index_id' => $index->machine_name,
          'field' => $key,
          'identifier' => $index->machine_name . '__' . $key,
          'default_sort' => 0,
          'default_sort_no_terms' => 0,
          'default_order' => 'desc',
          'name' => t('@field', array('@field' => $field['name'])),
          'enabled' => 0,
          'options' => array(),
          'status' => 0,
          'weight' => $weight,
        ));
        ++$weight;
      }

      foreach ($sorts[$key] as $i => $sort) {
        $k = $i ? "$key-$i" : $key;
        $form['sorts'][$k]['sort'] = array(
          '#type' => 'value',
          '#value' => $sort,
        );
        $form['sorts'][$k]['enabled'] = array(
          '#type' => 'checkbox',
          '#default_value' => $sort->enabled,
          '#disabled' => $disabled,
        );
        $form['sorts'][$k]['default_sort'] = array(
          '#type' => 'radio',
          '#return_value' => $k,
          '#tree' => FALSE,
          '#default_value' => $sort->default_sort ? $k : NULL,
          '#states' => array(
            'enabled' => array(
              ':input[name="sorts[' . $k . '][enabled]"]' => array('checked' => TRUE),
            ),
          ),
        );
        $form['sorts'][$k]['default_sort_no_terms'] = array(
          '#type' => 'radio',
          '#return_value' => $k,
          '#tree' => FALSE,
          '#default_value' => $sort->default_sort_no_terms ? $k : NULL,
          '#states' => array(
            'enabled' => array(
              ':input[name="sorts[' . $k . '][enabled]"]' => array('checked' => TRUE),
            ),
          ),
        );
        $form['sorts'][$k]['default_order'] = array(
          '#type' => 'select',
          '#default_value' => $sort->default_order,
          '#options' => array(
            'asc' => t('Ascending'),
            'desc' => t('Descending'),
          ),
          '#states' => array(
            'visible' => array(
              ':input[name="sorts[' . $k . '][enabled]"]' => array('checked' => TRUE),
            ),
          ),
        );
        if ($show_status) {
          $form['sorts'][$k]['status']['#markup'] = $sort->status ? theme('entity_status', array('status' => $sort->status)) : $empty_status;
        }
        $form['sorts'][$k]['field'] = array(
          '#markup' => check_plain($field['name']),
        );
        $form['sorts'][$k]['type'] = array(
          '#markup' => $type,
        );
        $form['sorts'][$k]['name'] = array(
          '#type' => 'textfield',
          '#maxlength' => max(strlen($sort->name), 80),
          '#default_value' => $sort->name,
        );
        $form['sorts'][$k]['weight'] = array(
          '#type' => 'weight',
          '#default_value' => isset($sort->weight) ? (int) $sort->weight : 0,
          '#delta' => 100,
          '#attributes' => array(
            'class' => array('search-api-sorts-weight'),
          ),
        );
      }
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );
  return $form;
}

/**
 * Validation callback for search_api_sorts_index_select().
 */
function search_api_sorts_index_select_validate(array $form, array &$form_state) {
  $warn = FALSE;
  foreach ($form_state['values']['sorts'] as $key => $v) {
    if ($v['enabled']) {
      // -> This setting will be stored.
      if (!$v['name']) {
        form_set_error("sorts][$key][name", t("You can't set an empty name."));
      }
      elseif (strlen($v['name']) > 80) {
        form_set_error("sorts][$key][name", t('Names cannot be longer than 80 characters, but "@name" is @count characters long.',
            array('@name' => $v['name'], '@count' => strlen($v['name']))));
      }
    }
    elseif ($v['sort']->name != $v['name']) {
      $warn = TRUE;
    }
  }
  if ($warn) {
    drupal_set_message(t('Note that changed names of sorts that are disabled and have no settings yet, are not saved.'), 'warning');
  }
}

/**
 * Submit callback for search_api_sorts_index_select().
 */
function search_api_sorts_index_select_submit(array $form, array &$form_state) {
  $count = 0;
  $delete_count = 0;
  $reset_count = 0;

  foreach ($form_state['values']['sorts'] as $key => $v) {
    if (empty($v['enabled']) || (isset($v['sort']->status) && $v['sort']->status == ENTITY_OVERRIDDEN)) {
      // This is the only case in which a "delete" makes sense.
      if (!empty($v['delete'])) {
        $v['sort']->delete();
        $v['sort']->status == ENTITY_OVERRIDDEN ? ++$reset_count : ++$delete_count;
        continue;
      }
    }
    $f = $v['sort'];
    $default_sort = $form_state['values']['default_sort'] == $key;
    $default_sort_no_terms = $form_state['values']['default_sort_no_terms'] == $key;
    $update = $f->enabled != $v['enabled'] || $f->name != $v['name'] || $f->weight != $v['weight'] || $f->default_sort != $default_sort || $f->default_sort_no_terms != $default_sort_no_terms || $f->default_order != $v['default_order'];
    if ($update) {
      $f = clone $f;
      $f->enabled = $v['enabled'];
      $f->name = $v['name'];
      $f->default_sort = $default_sort;
      $f->default_sort_no_terms = $default_sort_no_terms;
      $f->default_order = $v['default_order'];
      $f->weight = $v['weight'];
      $f->save();
      ++$count;
    }
  }

  if ($delete_count) {
    drupal_set_message(format_plural($delete_count, 'The settings of 1 sort were successfully deleted.', 'The settings of @count sorts were successfully deleted.'));
  }
  if ($count) {
    drupal_set_message(format_plural($count, '1 sort was successfully updated.', '@count sorts were successfully updated.'));
  }
  if ($reset_count) {
    drupal_set_message(format_plural($reset_count, '1 sort configuration was reset.', '@count sort configurations were reset.'));
  }
  if (!($count + $delete_count + $reset_count)) {
    drupal_set_message(t('No values were changed.'));
  }
}

/**
 * Theming function for rendering a form as a table.
 *
 * @param array $variables
 *   An array of variables to use, containing only one entry:
 *   - element: The sub-form to render as a table.
 *
 * @return string
 *   HTML displaying the specified sub-form as a table.
 */
function theme_search_api_sorts_form_table(array $variables) {
  $form = $variables['element'];

  $rows = array();
  foreach (element_children($form) as $id) {
    $row = array();
    foreach (element_children($form[$id]) as $field) {
      if ($cell = render($form[$id][$field])) {
        $row[] = $cell;
      }
    }
    $rows[] = array(
      'data' => $row,
      'class' => array('draggable'),
    );
  }

  $vars['rows'] = $rows;
  if (isset($form['#table_header'])) {
    $vars['header'] = $form['#table_header'];
  }
  if (isset($form['#table_empty'])) {
    $vars['empty'] = $form['#table_empty'];
  }
  $vars['attributes']['id'] = 'search-api-sorts-table';

  drupal_add_tabledrag('search-api-sorts-table', 'order', 'sibling', 'search-api-sorts-weight');
  return theme('table', $vars);
}
