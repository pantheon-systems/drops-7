<?php

/**
 * @file
 * Defines functions for administrative interface for servers
 *
 */

function ldap_servers_edit_index() {

  return ldap_servers_servers_list(NULL, TRUE);

}
/**
 * LDAP servers list.
 *
 * @param $sid
 *   server id.  if empty list all servers
 * @param $actions
 *   True or False indicating if server edit, delete, enable, disable, etc links should show
 * @return
 *   The HTML table with the servers list.
 */
function ldap_servers_servers_list($sid = NULL, $actions = FALSE) {

  $variables =  array(
    'ldap_servers' => $ldap_server = ldap_servers_get_servers($sid, 'all'),
    'action' => TRUE,
    'type' => 'table',
    );

  $html = theme('ldap_servers_list', $variables);

  return $html;
}


/**
 * Implements the LDAP server edit page.
 *
 * @param $form_state
 *   A form state array.
 * @param $op
 *   An operatin - add or edit.
 * @param $sid
 *   A LDAP server ID.
 *
 * @return
 *   The form structure.
 */

function ldap_servers_admin_form($form, &$form_state, $op = NULL, $sid = NULL) {
  ldap_servers_module_load_include('php', 'ldap_servers', 'LdapServerAdmin.class');
  $server = new LdapServerAdmin($sid);
  $form = $server->drupalForm($op);
  return $form;
}

/**
 * Validate hook for the LDAP server form.
 */
function ldap_servers_admin_form_validate($form, &$form_state) {

  $op = drupal_strtolower($form_state['clicked_button']['#value']);
  ldap_servers_module_load_include('php', 'ldap_servers', 'LdapServerAdmin.class');
  $server = new LdapServerAdmin($form_state['values']['sid']);

  $errors = $server->drupalFormValidate($op, $form_state['values']);
  foreach ($errors as $error_name => $error_text) {
    form_set_error($error_name, t($error_text));
  }
  $warnings = $server->drupalFormWarnings($op, $form_state['values'], (boolean)(count($errors) > 0));
  foreach ($warnings as $warning_name => $warning_text) {
    drupal_set_message($warning_text, 'warning');
  }

  $form_state['ldap_warnings'] = (boolean)(count($warnings) > 0);

}

/**
 * Submit hook for the LDAP server form.
 */
function ldap_servers_admin_form_submit($form, &$form_state) {
  $button = drupal_strtolower($form_state['clicked_button']['#value']);
  $op = ($button == 'add') ? 'add' : 'edit';
  $verb = ($op == 'edit') ? 'edited' : $op . 'ed';
  ldap_servers_module_load_include('php', 'ldap_servers', 'LdapServerAdmin.class');
  $server = new LdapServerAdmin($form_state['values']['sid']);
  $server->drupalFormSubmit($op, $form_state['values']);  // add form data to object and save or create

  if ($server->hasError() == FALSE) {
    drupal_set_message(t('LDAP Server %name !verb.', array('!verb' => $verb, '%name' => $server->name)), 'status');
    ldap_servers_cache_clear();
    if ($form_state['ldap_warnings'] && $op != 'add') {
      // do nothing, but don't redirect away from form.
      // if there are warnings, want them to see form even if its been saved
    }
    else {
      drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/servers');
    }
  }
  else {
    form_set_error($server->errorName(), $server->errorMsg());
    $server->clearError();
  }
  ldap_servers_cache_clear();
}

/**
 * Implements the LDAP server delete page.
 *
 * @param $form_state
 *   A form state array.
 * @param $sid
 *   A LDAP server ID.
 *
 * @return
 *   The form structure.
 */
function ldap_servers_admin_delete($form, &$form_state, $op = NULL, $sid = NULL) {

  if ($sid && ($ldap_server = ldap_servers_get_servers($sid, 'all', TRUE))) {
    // array()

    $variables = array(
      'ldap_server' => $ldap_server, 'actions' => FALSE, 'type' => 'detail'
    );
    $form['#prefix'] = '<div>' . theme('ldap_servers_server', $variables) . '</div>';
    $form['sid'] = array(
      '#type' => 'hidden',
      '#value' => $sid,
    );
    $form['name'] = array(
      '#type' => 'hidden',
      '#value' => $ldap_server->name,
    );

    $warnings = module_invoke_all('ldap_server_in_use', $sid, $ldap_server->name);
    if (count($warnings)) {
      drupal_set_message(join("<br/>", array_values($warnings)), 'error');
      drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/servers/list');
    }
    else {
      return confirm_form(
        $form,
        'Delete Confirmation Form',
        LDAP_SERVERS_MENU_BASE_PATH . '/servers/list',
          '<p>' . t('Are you sure you want to delete the LDAP server named <em><strong>%name</strong></em> ?', array('%name' => $ldap_server->name)) . '</p><p>' .
          t('This action cannot be undone.') . '</p>',
        t('Delete'),
        t('Cancel')
      );
    }
  }
  drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/servers/list');
}

/**
 * Submit handler for the LDAP server delete page.
 */
function ldap_servers_admin_delete_submit($form, &$form_state) {
  $values = $form_state['values'];
  $sid = $values['sid'];
  ldap_servers_module_load_include('php', 'ldap_servers', 'LdapServerAdmin.class');
  $ldap_server = new LdapServerAdmin($sid);
  if ($values['confirm'] && $sid) {
    if ($result = $ldap_server->delete($sid)) {
      $tokens = array('%name' => $ldap_server->name, '!sid' => $sid);
      drupal_set_message(t('LDAP Server %name (server id = !sid) has been deleted.', $tokens), 'status');
      watchdog('ldap', 'LDAP Server deleted: %name (sid = !sid) ', $tokens);
    }
    else {
      drupal_set_message(t('LDAP Server delete failed.'), 'warning');
    }
  }
  else {
    drupal_set_message(t('LDAP Server delete cancelled.'), 'status');
  }
  ldap_servers_cache_clear();
  drupal_goto(LDAP_SERVERS_MENU_BASE_PATH . '/servers/list');
}


/**
 * Implements the LDAP server delete page.
 *
 * @param $form_state
 *   A form state array.
 * @param $sid
 *   A LDAP server ID.
 *
 * @return
 *   The form structure.
 */


function ldap_servers_admin_enable_disable($form, &$form_state, $action = NULL, $sid = NULL) {

  if ($ldap_server = ldap_servers_get_servers($sid, 'all', TRUE)) {
    $variables = array(
      'ldap_server' => $ldap_server, 'actions' => FALSE, 'type' => 'detail'
    );
    $form['#prefix'] = "<div>" . theme('ldap_servers_server', $variables) . "</div>";

    $form['sid'] = array(
      '#type' => 'hidden',
      '#value' => $sid,
    );
    $form['name'] = array(
      '#type' => 'hidden',
      '#value' => $ldap_server->name,
    );
    $form['action'] = array(
      '#type' => 'hidden',
      '#value' => $action,
    );
    return confirm_form(
      $form,
      t('Are you sure you want to') . t($action) . ' ' . t('the LDAP server named <em><strong>%name</strong></em>?', array('%name' =>  $ldap_server->name)),
      LDAP_SERVERS_MENU_BASE_PATH . '/servers/list',
      t('<p></p>'),
      t($action),
      t('Cancel')
    );
  }

}

/**
 * Submit hook for the LDAP server delete page.
 */
function ldap_servers_admin_enable_disable_submit($form, &$form_state) {
  $values = $form_state['values'];
  $sid = $values['sid'];
  $status = ($values['action'] == 'enable') ? 1 : 0;
  if ($values['confirm'] && $sid) {

    $form_state['redirect'] = LDAP_SERVERS_MENU_BASE_PATH . '/servers';
    $ldap_server = new LdapServerAdmin($sid);

    $ldap_server->status = $status;
    $ldap_server->save('edit');
    $tokens = array('%name' => $values['name'], '!sid' => $sid, '!action' => t($values['action']) . 'd');
    drupal_set_message(t('LDAP Server Configuration %name (server id = !sid) has been !action.', $tokens));
    $message = t('LDAP Server !action: %name (sid = !sid) ', $tokens);
    watchdog('ldap', $message);

  }

}
