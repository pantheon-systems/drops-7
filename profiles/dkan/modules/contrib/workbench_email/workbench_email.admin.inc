<?php
/**
 * @file
 * Administrative forms for Workbench Email Module.
 */

/**
 * Administration form to create and delete email transitions.
 *
 * @param array $form
 *   The form array
 *
 * @param array $form_state
 *   The form_state array
 *
 * @return form
 *   A ready form array
 */
function workbench_email_form($form, &$form_state) {
  // List existing states.
  $types = drupal_map_assoc(workbench_moderation_moderate_node_types());
  $transitions = workbench_moderation_transitions();
  $roles = workbench_email_determine_valid_roles();

  if (!$types) {
    drupal_set_message(t('Moderation is not enabled for any content types. Visit
                         the <a href="@url"> content type administration
                         page</a> to enable moderation for one or more types.',
      array('@url' => url('admin/structure/types'))), 'warning');
    $form['#disabled'] = TRUE;
  }
  elseif (!$roles) {
    drupal_set_message(t('Moderation is not enabled for any roles. Visit the
                         <a href="@url"> user permissions page</a> to enable moderation
                         for one or more roles.',
      array('@url' => url('admin/people/permissions', array('fragment' => 'module-workbench_moderation')))), 'warning');
    $form['#disabled'] = TRUE;
  }
  else {
    $form['transitions'] = array(
      '#tree' => TRUE,
    );
  }

  $form['#attached']['js'] = array(
    drupal_get_path('module', 'workbench_email') . '/js/workbench_email.js',
  );

  $form['config_container'] = array(
    '#type' => 'fieldset',
    '#title' => t('Configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['config_container']['queue_mail'] = array(
    '#type' => 'checkbox',
    '#title' => t('Queue Mail'),
    '#description' => t('This will send emails on cron instead of on form submission. Useful for sites with lots of users.'),
    '#options' => array(0 => t('Off'), 1 => t('On')),
    '#weight' => -100,
    '#default_value' => variable_get('workbench_email_queue_mail', FALSE),
  );

  $form['transitions_container'] = array(
    '#type' => 'fieldset',
    '#title' => t('Transitions'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  foreach ($transitions as $transition) {
    $element = array();

    $element['transition'] = array(
      '#type' => 'value',
      '#value' => $transition,
    );
    $element['from_name'] = array(
      '#markup' => check_plain(workbench_moderation_state_label($transition->from_name)),
    );
    $element['to_name'] = array(
      '#markup' => check_plain(workbench_moderation_state_label($transition->to_name)),
    );

    foreach ($roles as $rid => $role) {
      ($rid == WORKBENCH_EMAIL_AUTHOR ? $author = 1 : $author = 0);
      $workbench_email = workbench_email_get($transition, $rid, $author);
      $element[$role]['notify'] = array(
        '#type' => 'checkbox',
        '#title' => check_plain(t('Notify')),
        '#default_value' => $workbench_email ? TRUE : FALSE,
        '#prefix' => '<div class="workbench-email-notify-container">',
        '#suffix' => '</div>',
        '#attributes' => array('class' => array('workbench-email-notify-checkbox')),
      );

      $default_value = 0;
      if (isset($workbench_email[$transition->from_name . '_to_' . $transition->to_name][$rid])) {
        $default_value = $workbench_email[$transition->from_name . '_to_' . $transition->to_name][$rid]->automatic;
      }
      $display = 'display:none';
      if ($workbench_email) {
        $display = 'display:block';
      }
      $element[$role]['auto_notify'] = array(
        '#type' => 'checkbox',
        '#title' => check_plain(t('Auto Notify')),
        '#default_value' => $default_value ? TRUE : FALSE,
        '#prefix' => '<div class="workbench-email-auto-notify-container" style="' . $display . '">',
        '#suffix' => '</div>',
        '#attributes' => array('class' => array('workbench-email-auto-notify-checkbox')),
      );
    }
    $form['transitions'][] = $element;
  }

  $form['transitions_container']['help_text_original_author'] = array(
    '#markup' => t('<strong>Original Author</strong>: The original author
    is available for each state transition to provide the ability
    to notify the author of the state changes even the change was
    triggered by someone else.<br />'),
  );
  $form['transitions_container']['help_text_notify'] = array(
    '#markup' => t('<strong>Notify</strong>: When content moves from
    state X to state Y, the user performing the operation will be presented
    with a UI select list to notify user(s) of the given role.<br />'),
  );
  $form['transitions_container']['help_text_auto_notify'] = array(
    '#markup' => t('<strong>Auto Notify</strong>: When content moves from
    state X to state Y, all users within the role with be notified without
    displaying an option to the user performing the action. In the case
    of the author, the single user is notified automatically. Note that
    "Notify" must be selected as well as "Auto Notify".<br />'),
  );

  $workbench_emails = workbench_email_get();
  $style = 'display:none';
  if ($workbench_emails) {
    $style = 'display:block';
  }
  $form['emails'] = array(
    '#type' => 'fieldset',
    '#title' => t('Emails'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array('style' => $style),
    '#prefix' => '<div id="workbench-email-emails-container">',
    '#suffix' => '</div>',
  );

  if ($workbench_emails) {
    $state_labels = workbench_moderation_state_labels();
    foreach ($workbench_emails as $transition_label => $email_transition_set) {
      $state_label = $email_transition_set;
      $transition = array_shift($state_label);
      $label = ucwords($state_labels[$transition->from_name] . ' To ' . $state_labels[$transition->to_name]);
      $transition_title = t('!label', array('!label' => $label));
      $form['emails'][$transition_label] = array(
        '#type' => 'fieldset',
        '#title' => check_plain($transition_title),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      foreach ($email_transition_set as $rid => $email_transition) {
        $subject = $email_transition->subject;
        $message = $email_transition->message;
        if ($rid) {
          $role = user_role_load($rid);
        }
        else {
          $role = workbench_email_get_author_role();
        }
        $form['emails'][$transition_label][$rid] = array(
          '#type' => 'fieldset',
          '#title' => check_plain(t("Email @role_name",
            array('@role_name' => ucwords($role->name)))),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
        );

        $form['emails'][$transition_label][$rid]['subject'] = array(
          '#type' => 'textfield',
          '#default_value' => $subject,
          '#title' => t('Subject'),
          '#maxlength' => 255,
        );

        $form['emails'][$transition_label][$rid]['message'] = array(
          '#type' => 'textarea',
          '#default_value' => $message,
          '#title' => t('Message'),
          '#size' => 30,
        );
      }
    }

    if ($workbench_emails) {
      $form['#tree'] = TRUE;

      // Display list of available placeholders if token module is installed.
      if (module_exists('token')) {
        module_load_include('inc', 'token', 'token.pages');
        $form['emails']['token_set'] = array(
          '#type' => 'fieldset',
          '#title' => t('Available Tokens'),
          '#collapsible' => TRUE,
        );

        // Use the token_tree_link theme if available.
        // Introduced in Token 7.x-1.2
        // went back against this since token doesn't support
        // #token_types, so node and user variables are not
        // outputted.
        // https://drupal.org/node/1684984
        // https://drupal.org/node/1842738
        // if (function_exists('theme_token_tree_link')) {
        // $token_tree_theme = 'token_tree_link';
        // } else {
        $token_tree_theme = 'token_tree';
        // }
        $form['emails']['token_set']['tokens'] = array(
          '#theme' => $token_tree_theme,
          '#token_types' => array('node', 'user'),
        );
      }
    }
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Configuration'),
    '#ajax' => array(
      'callback' => 'workbench_email_ajax_form_submit',
      'wrapper' => 'workbench-email-emails-container',
      'effect' => 'fade',
    ),
  );

  return $form;
}

/**
 * Ajax form submission callback.
 *
 * Clears out the messages and rebuilds the form.
 *
 * @param array $form
 *   The form array
 * @param array $form_state
 *   The form state array
 *
 * @return array
 *   The form container
 */
function workbench_email_ajax_form_submit(&$form, &$form_state) {
  $form = drupal_rebuild_form('workbench_email_form', $form_state, $form);
  drupal_get_messages();
  return $form['emails'];
}

/**
 * Transforms the email transitions administration form into a table.
 *
 * @param array $variables
 *   The form array to render into a table
 *
 * @return output
 *   A rendered form in table structure
 */
function theme_workbench_email_form($variables) {
  $form = $variables['form'];

  $header = array(
    t('From'),
    '',
    t('To'),
  );

  $roles = workbench_email_determine_valid_roles();
  foreach ($roles as $rid => $role) {
    if ($rid == WORKBENCH_EMAIL_AUTHOR) {
      $role = 'Original Author';
    }
    $header[] = t("@role", array('@role' => ucwords($role)));
  }

  $rows = array();
  foreach (element_children($form['transitions']) as $key) {
    $element = & $form['transitions'][$key];
    if (!isset($element['#markup'])) {
      $row = array('data' => array());
      $row['data']['from'] = drupal_render($element['from_name']);
      $row['data'][] = '--&gt;';
      $row['data']['to'] = drupal_render($element['to_name']);
      foreach ($roles as $rid => $role) {
        $data = drupal_render($element[$role]['notify']);
        $data .= drupal_render($element[$role]['auto_notify']);
        $row['data'][$role] = $data;
      }
      $rows[] = $row;
    }
  }

  $table = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array(
        'class' => array('width-auto'),
        'id' => array('workbench-email-table'),
      ),
    )
  );

  $form['transitions_container']['#value'] = $table;
  $output = drupal_render_children($form);
  return $output;
}

/**
 * Validates the form values.
 *
 * @param array $form_state
 *   The form_state array
 */
function workbench_email_form_validate($form, &$form_state) {
  $workbench_emails = workbench_email_get();
  foreach ($workbench_emails as $transition_label => $email_transition_set) {
    foreach ($email_transition_set as $rid => $email_transition) {
      if (isset($form_state['values']['emails'][$transition_label])) {
        // Determine if subject is set with no message.
        if ($form_state['values']['emails'][$transition_label][$rid]['subject'] != NULL
          && $form_state['values']['emails'][$transition_label][$rid]['message'] == NULL
        ) {
          form_set_error("$transition_label][$rid][message",
            t('You must add an email message if a subject is provided'));
        }
        // Determine if message is set with no subject.
        if ($form_state['values']['emails'][$transition_label][$rid]['subject'] == NULL
          && $form_state['values']['emails'][$transition_label][$rid]['message'] != NULL
        ) {
          form_set_error("$transition_label][$rid][subject",
            t('You must add a subject if an email message is provided'));
        }
      }
    }
  }
}


/**
 * Form submit handler for email transitions.
 *
 * Adds or deletes email transitions depending on user input.
 *
 * @param array $form
 *   The form array
 *
 * @param array $form_state
 *   The form_state array
 */
function workbench_email_form_submit($form, &$form_state) {
  variable_set('workbench_email_queue_mail', $form_state['values']['config_container']['queue_mail']);
  $roles = workbench_email_determine_valid_roles();
  foreach ($form_state['values']['transitions'] as $transition) {
    foreach ($roles as $rid => $role) {
      if ($transition[$role]['notify']) {
        ($rid == WORKBENCH_EMAIL_AUTHOR ? $author = 1 : $author = 0);
        $automatic = $transition[$role]['auto_notify'];
        $transition_label = $transition['transition']->from_name . '_to_' . $transition['transition']->to_name;
        $subject = NULL;
        $message = NULL;
        if (isset($form_state['values']['emails'])) {
          $message = $form_state['values']['emails'][$transition_label][$rid]['message'];
          $subject = $form_state['values']['emails'][$transition_label][$rid]['subject'];
        }
        workbench_email_save($transition['transition'], $rid, $subject, $message, $author, $automatic);
      }
      else {
        workbench_email_delete($transition['transition'], $rid);
      }
    }
  }
  drupal_set_message(t('Settings have been updated'));
}
