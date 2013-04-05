<?php
// $Id$

/**
 * @file
 * Contains all non-admin pages for the Connector module
 */

/**
 * Menu callback for the user settings page
 */
function connector_user_settings($account) {
  global $user;
  $output = array();

  $connections = _connector_get_user_connections($account);
  $primary     = _connector_get_primary_connection($account);
  if ($account->uid == $user->uid) {
    // TODO: find a way to connect in name of other users.
    $output['form'] = drupal_get_form('connector_button_form', $account, 'Add !title account');
  }
  if (!empty($connections)) {
    $output['list'] = drupal_get_form('connector_connections_list', $connections, $primary, $account);
  }
  return $output;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function connector_connections_list($form, $form_state, $connections, $primary, $account) {
  $header = array(
    'connector' => t('Connection'),
    'cid' => t('External Id'),
    'operations' => t('Operations'),
  );

  $options = array();
  foreach ($connections as $connection) {
    $connector = _connector_get_connectors($connection->connector);
    $operations = array();
    $operations[] = array(
      'title' => t('Remove'),
      'href' => 'user/' . $account->uid . '/connections/' . $connection->connector . '__' . $connection->cid . '/delete',
    );
    if (user_access('sync local profile with connections')) {
      $operations[] = array(
        'title' => t('Sync local profile with @name', array('@name' => $connector['title'])),
        'href' => 'user/' . $account->uid . '/connections/' . $connection->connector . '__' . $connection->cid . '/sync',
      );
    }
    $options[$connection->connector . '__' . $connection->cid] = array(
      'connector' => $connector['title'],
      'cid' => $connection->cid,
      'operations' => theme('links', array('links' => $operations, 'attributes' => array('class' => array('links', 'inline')))),
    );
  }

  $primary = ($primary->connector . '__' . $primary->cid);

  $radios  = array();
  $form['header'] = array();
  foreach ($header as $key => $value) {
    $form['header'][$key] = array('#markup' => $value);
  }
  foreach ($options as $key => $option) {
    $radios[$key] = '';
    $form['connector'][$key]  = array('#markup' => check_plain($option['connector']));
    $form['cid'][$key]        = array('#markup' => check_plain($option['cid']));
    $form['operations'][$key] = array('#markup' => $option['operations']);
  }
  $form['primary'] = array(
    '#type' => 'radios',
    '#options' => $radios,
    '#default_value' => $primary,
  );
  $form['#theme'] = 'connector_connections_list_tableselect';


  $form['account'] = array(
    '#type' => 'value',
    '#value' => $account,
  );

  if (!empty($connections)) {
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Set primary connection'),
    );
  }

  return $form;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function theme_connector_connections_list_tableselect($variables) {
  $form = $variables['form'];
  $header = array('');

  foreach (element_children($form['header']) as $key) {
    $header[] = drupal_render($form['header'][$key]);
  }

  $rows = array();
  $number_of_rows = 0;
  if (!empty($form['connector'])) {
    foreach (element_children($form['connector']) as $key) {
      $row = array();
      $row[] = drupal_render($form['primary'][$key]);
      $row[] = drupal_render($form['connector'][$key]);
      $row[] = drupal_render($form['cid'][$key]);
      $row[] = drupal_render($form['operations'][$key]);
      $rows[] = $row;
      $number_of_rows++;
    }

    if ($number_of_rows == 1) {
      unset($rows[0][0]);
      unset($header[0]);
    }
  }
  else {
    $rows[] = array(array(
        'data' => t('No connections available.'),
        'colspan' => '6',
      ));
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  if ($number_of_rows < 2) {
    unset($form['actions']);
  }

  $output .= drupal_render_children($form);

  return $output;
}

/**
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function connector_connections_list_submit($form, &$form_state) {
  $primary = $form_state['values']['primary'];
  $account = $form_state['values']['account'];
  _connector_set_primary_connection($account, $primary);
  //TODO: Trigger an update of the users information somehow?
  //TODO: Need to make sure that all values are rechecked - eg checking for an avatar even if the old source didn't have one
}


function connector_user_delete_form($form, &$form_state, $account, $cid) {
  $form = array();
  list($connector_name, $xuid) = explode('__', $cid);

  $connector = _connector_get_connectors($connector_name);

  $form['uid'] = array(
    '#value' => $account->uid,
    '#type' => 'value',
  );
  $form['cid'] = array(
    '#value' => $cid,
    '#type' => 'value',
  );
  $form['sure'] = array(
    '#type' => 'markup',
    '#markup' => t('Are you sure you want to remove the connection with %connector account %id?', array('%id' => $xuid, '%connector' => $connector['title']))
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Remove connection'),
  );
  return $form;
}
function connector_user_delete_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $connections = _connector_get_user_connections($values['uid']);
  $connection = NULL;
  foreach ($connections as $_connection) {
    if ($_connection->connector . '__' . $_connection->cid == $values['cid']) {
      $connection = $_connection;
      break;
    }
  }
  db_delete('authmap')
    ->condition('uid', $values['uid'])
    ->condition('authname', $values['cid'])
    ->condition('module', 'connector')
    ->execute();
  $form_state['redirect'] = array(
    'user/' . $values['uid'] . '/connections',
  );
}

function connector_user_sync_form($form, &$form_state, $account, $cid) {

  module_load_include('inc', 'user', 'user.pages');
  $form = user_profile_form($form, $form_state, $account);
  $hooks = array('form', 'form_user_profile_form');
  $form_id = 'form_user_profile_form';
  drupal_alter($hooks, $form, $form_state, $form_id);

  list($connector_name, $xuid) = explode('__', $cid);

  $connector = _connector_get_connectors($connector_name);
  drupal_set_title(t('Synchronize profile with @title', array('@title' => $connector['title'])));



  $info = array();
  if (is_callable($connector['information callback'])) {
    $info = $connector['information callback']($connector, $xuid);
  }
  // We put access on false and explictly turn access on.
  $children_keys = element_children($form);
  foreach ($children_keys as $key) {
    if ($key == 'account') {
      $account_children_keys = element_children($form['account']);
      foreach ($account_children_keys as $account_key) {
        $form['account'][$account_key]['#access'] = FALSE;
      }
    }
    else {
      $form[$key]['#access'] = FALSE;
    }
  }

  // only allow fields we can sync.
  foreach ($info as $field) {
    if (isset($field['sync']) && $field['sync']) {
      if (isset($form['account'][$field['sync']])) {
        $form['account'][$field['sync']]['#access'] = TRUE;
      }
      elseif (isset($form[$field['sync']])) {
        $form[$field['sync']]['#access'] = TRUE;
      }
    }
  }

  // For some fields we need to supply the current password.
  if (isset($form['account']['current_pass_required_values']['#value'])) {
    foreach ($form['account']['current_pass_required_values']['#value'] as $key => $value) {
      if (isset($form['account'][$key]['#access']) && $form['account'][$key]['#access']) {
        $form['account']['current_pass']['#access'] = TRUE;
      }
    }
  }

  // Prefill.
  _connector_prefill_user_form($form, $info);

  array_unshift($form['#submit'], 'connector_user_sync_form_first_submit');

  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save sync with @title', array('@title' => $connector['title'])),
  );
  return $form;
}

function connector_user_sync_form_first_submit($form, &$form_state) {

  $form_state['redirect'] = 'user';
}

/**
 * TODO finish field mapping.
 *
 * For now, this does only simple text mapping.
 * We need to find another solution for this, possibly Feeds?
 * HOOK_connector_prefill_user_form_alter enables custom modules to do better
 * :) good luck!
 */
function _connector_prefill_user_form(&$form, $info) {
  foreach ($info as $field) {
    if (isset($field['sync']) && $field['sync']) {
      // simple text type.
      if (isset ($form[ ($field['sync']) ]['#language']) && isset($form[ ($field['sync']) ][ ($form[ ($field['sync']) ]['#language']) ][0]['value'])) {
        $form[ ($field['sync']) ][ ($form[ ($field['sync']) ]['#language']) ][0]['value']['#default_value'] = $field['value'];
      }

      // simple text type in account.
      elseif (isset ($form['account'][ ($field['sync']) ]['#default_value'])) {
        $form['account'][ ($field['sync']) ]['#default_value'] = $field['value'];
      }
    }
  }
  drupal_alter('connector_prefill_user_form', $form, $info);
}
