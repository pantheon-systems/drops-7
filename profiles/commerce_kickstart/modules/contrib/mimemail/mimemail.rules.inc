<?php

/**
 * @file
 * Rules actions for sending Mime-encoded emails.
 *
 * @addtogroup rules
 * @{
 */

/**
 * Action Implementation: Send HTML mail.
 */
function rules_action_mimemail($to, $cc = NULL, $bcc = NULL, $from_name = NULL, $from_mail = NULL, $subject, $body, $plaintext = NULL, $attachments = array(), $settings, RulesState $state, RulesPlugin $element) {
  // Set the sender name and from address.
  if (empty($from_mail)) {
    $from = NULL;
  }
  else {
    $from = array(
      'name' => $from_name,
      'mail' => $from_mail,
    );
  }

  // If the recipient is a registered user with a language preference, use
  // the recipient's preferred language. Otherwise, use the system default
  // language.
  $account = user_load_by_mail($to);
  if ($account) {
    $language = user_preferred_language($account);
  }
  else {
    $language = language_default();
  }

  $params = array(
    'context' => array(
      'subject' => $subject,
      'body' => $body,
      'action' => $element,
      'state' => $state,
    ),
    'cc' => $cc,
    'bcc' => $bcc,
    'plaintext' => $plaintext,
    'attachments' => $attachments,
  );

  // Set a unique key for this mail.
  $name = isset($element->root()->name) ? $element->root()->name : 'unnamed';
  $key = 'rules_action_mimemail_' . $name . '_' . $element->elementId();

  drupal_mail('mimemail', $key, $to, $language, $params, $from);
}

/**
 * Action: Send HTML mail to all users of a specific role group(s).
 */
function rules_action_mimemail_to_users_of_role($roles, $from_name = NULL, $from_mail = NULL, $subject, $body, $plaintext = NULL, $attachments = array(), $settings, RulesState $state, RulesPlugin $element) {
  // Set the sender name and from address.
  if (empty($from_mail)) {
    $from = NULL;
  }
  else {
    $from = array(
      'name' => $from_name,
      'mail' => $from_mail,
    );
  }

  // All authenticated users, which is everybody.
  if (in_array(DRUPAL_AUTHENTICATED_RID, $roles)) {
    $result = db_query('SELECT mail FROM {users} WHERE uid > 0');
  }
  else {
    $rids = implode(',', $roles);
    // Avoid sending emails to members of two or more target role groups.
    $result = db_query('SELECT DISTINCT u.mail FROM {users} u INNER JOIN {users_roles} r ON u.uid = r.uid WHERE r.rid IN (' . $rids . ')');
  }

  $params = array(
    'context' => array(
      'subject' => $subject,
      'body' => $body,
      'action' => $element,
      'state' => $state,
    ),
    'plaintext' => $plaintext,
    'attachments' => $attachments,
  );

  // Set a unique key for this mail.
  $name = isset($element->root()->name) ? $element->root()->name : 'unnamed';
  $key = 'rules_action_mimemail_to_users_of_role_' . $name . '_' . $element->elementId();

  $message = array('result' => TRUE);
  foreach ($result as $row) {
    $message = drupal_mail('mimemail', $key, $row->mail, language_default(), $params, $from);
    if (!$message['result']) {
      break;
    }
  }
  if ($message['result']) {
    $role_names = array_intersect_key(user_roles(TRUE), array_flip($roles));
    watchdog('rules', 'Successfully sent HTML email to the role(s) %roles.', array('%roles' => implode(', ', $role_names)));
  }
}

/**
 * @}
 */

