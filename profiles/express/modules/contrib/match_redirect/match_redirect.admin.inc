<?php

/**
 * @file
 * Administrative callbacks for the match redirect module.
 */

/**
 * Returns the redirect listing form that allows reordering of weights.
 */
function match_redirect_list_form($form, &$form_state) {
  $destination = drupal_get_destination();
  $form['#tree'] = TRUE;
  // Get all redirects.
  $results = match_redirect_load_multiple(array());
  // Build out form structure to be themed as a table.
  foreach ($results as $row) {
    $form['redirects'][$row->rid]['pattern'] = array('#markup' => htmlentities($row->source_pattern));
    $form['redirects'][$row->rid]['target'] = array('#markup' => htmlentities($row->target));
    $form['redirects'][$row->rid]['code'] = array('#markup' => $row->status_code);
    $form['redirects'][$row->rid]['weight'] = array(
      '#type' => 'weight',
      '#title' => t('Weight'),
      '#title_display' => 'invisible',
      '#default_value' => $row->weight,
    );
    // Edit and delete operations for redirects.
    $operations = array();
    $operations['edit'] = array(
      'title' => t('Edit'),
      'href' => 'admin/config/search/match_redirect/edit/' . $row->rid,
      'query' => $destination,
    );
    $operations['delete'] = array(
      'title' => t('Delete'),
      'href' => 'admin/config/search/match_redirect/delete/' . $row->rid,
      'query' => $destination,
    );
    $form['redirects'][$row->rid]['operations'] = array(
      '#markup' => theme(
        'links',
        array(
          'links' => $operations,
          'attributes' => array(
            'class' => array('links', 'inline', 'nowrap'),
          ),
        )
      ),
    );
  }
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save changes'),
  );

  return $form;
}

/**
 * Submit callback for redirect listing form (saves weights).
 */
function match_redirect_list_form_submit($form, &$form_state) {
  if (isset($form_state['values']['redirects'])) {
    foreach ($form_state['values']['redirects'] as $rid => $weight) {
      // Save new weights.
      $redirect = new stdClass();
      $redirect->rid = $rid;
      $redirect->weight = $weight['weight'];
      match_redirect_save($redirect);
    }
  }
}

/**
 * Theme callback for redirect listing form (renders form as reorderable table).
 *
 * @param array $variables
 *   An array containing the form to be rendered.
 *
 * @return string
 *   Rendered table HTML output.
 */
function theme_match_redirect_list_form($variables) {
  $form = $variables['form'];

  $rows = array();
  if (!empty($form['redirects'])) {
    foreach (element_children($form['redirects']) as $id) {
      // Break the form into variables theme_table expects and add needed classes.
      $form['redirects'][$id]['weight']['#attributes']['class'] = array('text-format-order-weight');
      $rows[] = array(
        'data' => array(
          drupal_render($form['redirects'][$id]['pattern']),
          drupal_render($form['redirects'][$id]['target']),
          drupal_render($form['redirects'][$id]['code']),
          drupal_render($form['redirects'][$id]['weight']),
          drupal_render($form['redirects'][$id]['operations']),
        ),
        'class' => array('draggable'),
      );
    }
  }
  // Handle no redirects.
  if (empty($rows)) {
    $rows[] = array(
      'data' => array(
        array('data' => t('There are currently no redirects.'), 'colspan' => 5),
      ),
    );
  }
  // Header row.
  $header = array(
    t('Pattern'),
    t('Target'),
    t('Code'),
    t('Weight'),
    t('Operations'),
  );
  $output = theme(
    'table',
    array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('id' => 'text-format-order'),
    )
  );
  $output .= drupal_render_children($form);
  // Add re-ordering js.
  drupal_add_tabledrag('text-format-order', 'order', 'sibling', 'text-format-order-weight');

  return $output;
}

/**
 * Returns the redirect edit/add form.
 *
 * @param object $existing 
 *   Redirect object that is loaded for editing.
 */
function match_redirect_add_form($form, &$form_state, $existing = NULL) {
  // No existing redirect provided create empty class.
  if (!isset($existing)) {
    $existing = new stdClass();
  }

  $form['rid'] = array(
    '#type' => 'value',
    '#value' => isset($existing->rid) ? $existing->rid : NULL,
  );
  $form['source_pattern'] = array(
    '#type' => 'textarea',
    '#title' => t('Pattern'),
    '#default_value' => isset($existing->source_pattern) ? $existing->source_pattern : NULL,
    '#required' => TRUE,
    '#description' => t("Specify redirect sources by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are blog for the blog page and blog/* for every personal blog. <front> is the front page."),
  );
  $form['target'] = array(
    '#type' => 'textfield',
    '#title' => t('Target'),
    '#default_value' => isset($existing->target) ? $existing->target : NULL,
    '#required' => TRUE,
    '#description' => t('The path for redirect target. This can be an internal Drupal path or alias such as node/add or an external URL such as http://drupal.org. Enter <front> to link to the front page.'),
  );
  $options = array(
    300 => t('300 (Multiple Choices)'),
    301 => t('301 (Moved Permanently)'),
    302 => t('302 (Found)'),
    303 => t('303 (See Other)'),
    304 => t('304 (Not Modified)'),
    305 => t('305 (Use Proxy)'),
    307 => t('307 (Temporary Redirect)'),
  );
  $form['status_code'] = array(
    '#type' => 'select',
    '#title' => t('Status Code'),
    '#options' => $options,
    '#default_value' => isset($existing->status_code) ? $existing->status_code : NULL,
    '#required' => TRUE,
  );
  $form['override'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow content to be redirected?'),
    '#default_value' => isset($existing->override) ? $existing->override : NULL,
    '#description' => t('By default if a pattern matches and there is content for that target url then it will not be redirected.'),
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => isset($existing->weight) ? $existing->weight : 0,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validate callback for redirect add/edit form.
 */
function match_redirect_add_form_validate($form, &$form_state) {
  // Don't allow target to be within it's own pattern.
  if (drupal_match_path($form_state['values']['target'], $form_state['values']['source_pattern'])) {
    form_set_error('target', t('Target falls within the specified pattern.'));
  }
  // Make sure the pattern does not interfere with the config
  // functions of this module.
  $module_paths = array(
    'admin/config/search/match_redirect',
    'admin/config/search/match_redirect/add',
    'admin/config/search/match_redirect/edit/1',
    'admin/config/search/match_redirect/delete/1',
  );
  $found = FALSE;
  foreach ($module_paths as $path) {
    if (drupal_match_path($path, $form_state['values']['source_pattern'])) {
      $found = TRUE;
    }
  }
  if ($found && $form_state['values']['override'] == 1) {
    form_set_error('source_pattern', t('Pattern will interfere with the operation of this module.'));
  }
  $redirects = match_redirect_load_multiple(array());
  foreach ($redirects as $redirect) {
    // Do not allow patterns to trigger existing targets.
    if (drupal_match_path($redirect->target, $form_state['values']['source_pattern'])) {
      form_set_error('source_pattern', t('Pattern will trigger an aready created target.'));
    }
    // Do not allow targets to fall within existing patterns.
    if (drupal_match_path($form_state['values']['target'], $redirect->source_pattern)) {
      form_set_error('target', t('Target falls within redirect patterns already created.'));
    }
  }
}

/**
 * Submit callback for redirect add/edit form.
 */
function match_redirect_add_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  $values = (object) $form_state['values'];
  match_redirect_save($values);
  drupal_set_message(t('Redirect saved.'));
  $form_state['redirect'] = 'admin/config/search/match_redirect';
}

/**
 * Returns the redirect delete confirm form.
 *
 * @param object $redirect
 *   Redirect object that is loaded for deleting.
 */
function match_redirect_delete_form($form, &$form_state, $redirect) {
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $redirect->rid,
  );
  return confirm_form(
    $form,
    t('Are you sure you want to delete the redirect to %target?',
    array('%target' => $redirect->target)),
    'admin/config/search/match_redirect'
  );
}

/**
 * Submit callback for redirect delete confirm form.
 */
function match_redirect_delete_form_submit($form, &$form_state) {
  match_redirect_delete($form_state['values']['rid']);
  drupal_set_message(t('Redirect has been deleted.'));
  $form_state['redirect'] = 'admin/config/search/match_redirect';
}
