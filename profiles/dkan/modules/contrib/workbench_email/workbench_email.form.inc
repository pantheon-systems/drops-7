<?php
/**
 * @file
 * Code for the Workbench Email Module.
 */

/**
 * Implements hook_form_BASE_FORM_ID_alter().
 *
 * Determines the current state and next state. Depending on that criteria
 * builds a form element(s) to allow the user to select users to send
 * an email to.
 */
function workbench_email_form_node_form_alter(&$form, $form_state) {
  if (workbench_moderation_node_type_moderated($form['type']['#value'])) {
    $available = FALSE;
    $wrapper_id = 'revision_information';
    // Workbench Moderation uses "options" fieldset in favor
    // of "revision information" if "administer roles" perm is
    // given to content moderator.
    if (isset($form['revision_information']['workbench_moderation_state_new'])) {
      $wrapper_id = 'revision_information';
      $available = TRUE;
    }
    else {
      if (isset($form['options']['workbench_moderation_state_new'])) {
        $wrapper_id = 'options';
        $available = TRUE;
      }
    }

    if (!$available) {
      return;
    }

    $form[$wrapper_id]['workbench_moderation_state_new']['#ajax'] = array(
      'callback' => 'workbench_email_form_node_callback',
      'wrapper' => 'workbench-email-form-node',
      'effect' => 'fade',
      'event' => 'change',
    );

    $form[$wrapper_id]['workbench_email'] = array(
      '#prefix' => '<div id="workbench-email-form-node">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );

    // Determine current state.
    if (isset($form['#node']->workbench_moderation['current']->state)) {
      $current_from_state = $form['#node']->workbench_moderation['current']->state;
    }
    else {
      $current_from_state = variable_get('workbench_moderation_default_state_' . $form['type']['#value'], workbench_moderation_state_none());
    }
    if ($current_from_state == workbench_moderation_state_published()) {
      $current_from_state = workbench_moderation_state_none();
    }

    // Initialize to the current state.
    $form_moderation_state = $current_from_state;
    if (!empty($form_state['values']) &&
      isset($form_state['values']['workbench_moderation_state_new'])
    ) {
      $form_moderation_state = $form_state['values']['workbench_moderation_state_new'];
    }
    if (!empty($form_state['values']) &&
      isset($form_state['values'][$wrapper_id]['workbench_moderation_state_new'])
    ) {
      $form_moderation_state = $form_state['values'][$wrapper_id]['workbench_moderation_state_new'];
    }

    $workbench_emails = workbench_email_get();
    foreach ($workbench_emails as $transition => $email_roles) {
      foreach ($email_roles as $rid => $email_transition) {
        $accounts = array(); $editors = array();
        $flags = workbench_email_get_all_transition_users(array('form' => $form, 'form_state' => $form_state), 'form', $email_transition->rid, $accounts, $editors);
        $emails = workbench_email_filter_users($email_transition->rid, $accounts, $editors, $flags);
        if ($email_transition->from_name == $current_from_state &&
          $email_transition->to_name == $form_moderation_state &&
          !$email_transition->automatic && $emails) {

          $settings = array(
            'email_transition' => $email_transition,
            'emails' => $emails,
            'author' => $flags['author'],
            'editors' => $editors,
            'users' => $accounts,
          );

          workbench_email_create_form_element($form, $settings);
        }
      }
    }

    $form['#validate'][] = 'workbench_email_notification_validate';
    $form['actions']['submit']['#submit'][] = 'workbench_email_notification_submit';
  }
}

/**
 * Ajax callback function, targets the workbench_email container.
 *
 * @param array $form
 *   The form array
 *
 * @param array $form_state
 *   The form_state array
 */
function workbench_email_form_node_callback($form, $form_state) {
  if ($form['options']['#access']) {
    return $form['options']['workbench_email'];
  }
  return $form['revision_information']['workbench_email'];
}

/**
 * Validates the options chosen.
 *
 * Users can go back and forth between fieldsets
 * causing the email options of a previous Workbench
 * Access section displayed and chosen when the
 * user actually went back and changed there initial
 * decision. Instead of trying to do something fancy
 * with dom and JS, I chose to validate in PHP and throw
 * an error.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form_state array.
 */
function workbench_email_notification_validate($form, &$form_state) {
  if (module_exists('workbench_access') && isset($form_state['values']['workbench_email'])) {
      // Get selected section(s)/selected user(s) and verify that
      // its a valid selection.
      $selected_wb_email = $form_state['values']['workbench_email'];
      $rid = key($selected_wb_email);
      $valid_sections = workbench_email_get_workbench_access_sections('form', array(
        'data' => array(
          'form' => $form,
          'form_state' => $form_state,
        ),
      ));
      $editors = workbench_email_get_workbench_access_editors($rid, $valid_sections);
      if ($selected_wb_email && $editors) {
        $error_messages = array();
        foreach ($selected_wb_email as $role_id => $wb_email) {
          $mail = current($wb_email);
          // Handle the situation where "All Users" was selected or
          // no option was selected.
          if ($mail == '0') {
            break;
          }
          $not_found = TRUE;
          foreach ($editors as $uid => $possible_editor) {
            if ($mail == $possible_editor['mail']) {
              $not_found = FALSE;
            }
          }
          if ($not_found) {
            $error_messages[$mail] = t('The combination of section and email
              (@email) are not valid. Please try again or chose another
              combination of sections and users.', array('@email' => $mail));
          }
        }
        if ($error_messages) {
          $list_items = theme('item_list', array(
            'items' => $error_messages,
            'title' => 'Error with email selection',
            'type' => 'ul',
          ));
          form_set_error('workbench_email', $list_items);
        }
      }
    }
}

/**
 * Submit handler for the workbench_email form element.
 *
 * Sends emails to the specific users selected in the form element.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array
 */
function workbench_email_notification_submit($form, &$form_state) {
  if (isset($form_state['values']['workbench_email'])) {
    ($form['options']['#access'] ? $wrapper_id = 'options' : $wrapper_id = 'revision_information');
    foreach ($form_state['values']['workbench_email'] as $rid => $role_emails) {
      foreach ($role_emails as $email) {
        // If !$email means all users option selected.
        if (!$email) {
          $accounts = array(); $editors = array();
          $author = workbench_email_get_all_transition_users($form_state['node'], 'node', $rid, $accounts, $editors);
          $emails = workbench_email_filter_users($rid, $accounts, $editors, $author);
          $email_transition = $form[$wrapper_id]['workbench_email'][$rid]['#hidden'];
          foreach ($emails as $email => $name) {
            workbench_email_mail_send($email, $email_transition, $form_state['node']);
          }
        }
        else {
          $email_transition = $form[$wrapper_id]['workbench_email'][$rid]['#hidden'];
          workbench_email_mail_send($email, $email_transition, $form_state['node']);
        }
      }
    }
  }
}

/**
 * Create the email selection form element.
 *
 * Creates the appropriate multi select list for a given role and stores
 * some email transition information into the form element.
 *
 * @param array $form
 *   The form array passed by reference
 * @param array $settings
 *   The array of additional settings
 */
function workbench_email_create_form_element(&$form, $settings) {
  $email_transition = $settings['email_transition'];
  $emails = $settings['emails'];
  $author = $settings['author'];
  $editors = $settings['editors'];
  $users = $settings['users'];

  if ($author) {
    $role = workbench_email_get_author_role();
  }
  else {
    $role = user_role_load($email_transition->rid);
    if (count($emails) > 1) {
      array_unshift($emails, t('-- All Users --'));
    }
  }

  if ($email_transition->rid == WORKBENCH_EMAIL_AUTHOR) {
    $description = t('Select the author for notification');
  }
  else {
    $description = t('Select one or more users to notify');
  }

  ($form['options']['#access'] ? $wrapper_id = 'options' : $wrapper_id = 'revision_information');
  $form[$wrapper_id]['workbench_email'][$role->rid] = array(
    '#type' => 'select',
    '#title' => t("@role_name", array('@role_name' => ucwords($role->name))),
    '#options' => $emails,
    '#description' => $description,
    '#multiple' => TRUE,
    '#hidden' => $email_transition,
  );

  $user_groups = array('editors' => $editors, 'users' => $users);
  drupal_alter('workbench_email_create_form_element', $form, $email_transition, $user_groups);
}
