<?php

/**
 * @file
 * Creates the form for Mailjet_list
 */

/**
 * Generates the mailjet list editing form. This form is used to edit the content of the list
 */
function mailjet_list_form($form, &$form_state, $entity, $op)
{
    if ($op == 'delete') {
        drupal_set_message(t('Confirm deleting the list'), 'status');
        $form['label'] = array(
            '#title' => t('Label'),
            '#type' => 'hidden',
            '#default_value' => isset($entity->label) ? $entity->label : '',
            '#description' => t('The human-readable name of this list.'),
            '#required' => FALSE,
            '#size' => 30,
            '#weight' => -12,
        );

        // Machine-readable type name.
        $form['name'] = array(
            '#title' => t('Name'),
            '#type' => 'hidden',
            '#default_value' => isset($entity->name) ? $entity->name : '',
            '#maxlength' => 32,
            '#description' => t('A unique machine-readable name for this list. It must only contain lowercase letters and numbers.'),
            '#required' => TRUE,
            '#size' => 30,
            '#weight' => -11,
            //'#suffix' => '@lists.mailjet.com',
        );

        $form['list_id'] = array(
            '#title' => t('List ID'),
            '#type' => 'hidden',
            '#default_value' => isset($entity->list_id) ? $entity->list_id : '',
            '#description' => t('The Mailjet List ID'),
            '#disabled' => TRUE,
            '#size' => 30,
            '#weight' => -10,
        );

        // Status property.
        $form['status'] = array(
            '#type' => 'hidden',
            '#title' => t('Status'),
            '#options' => array(0 => t('Disabled'), 1 => t('Enabled')),
            '#default_value' => isset($entity->status) ? $entity->status : 1,
            '#weight' => 35,
            '#required' => TRUE,
        );

        // Delete the list hidden var value send.
        $form['del'] = array(
            '#type' => 'hidden',
            '#default_value' => isset($entity->delete) ? $entity->delete : 'delete',
        );


        $form['actions'] = array('#type' => 'actions');
        $form['actions']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Confirm'),
            '#weight' => 40,
        );
    } else {
        $form['label'] = array(
            '#title' => t('Label'),
            '#type' => 'hidden',
            '#default_value' => isset($entity->label) ? $entity->label : '',
            '#description' => t('The human-readable name of this list. Only letters and numbers are alllowed'),
            '#required' => FALSE,
            '#size' => 30,
            '#weight' => -12,
        );

        // Machine-readable type name.
        $form['name'] = array(
            '#title' => t('Name'),
            '#type' => 'textfield',
            '#default_value' => isset($entity->name) ? $entity->name : '',
            '#maxlength' => 32,
            '#description' => t('A unique machine-readable name for this list. It must only contain lowercase letters and numbers.'),
            '#required' => TRUE,
            '#size' => 30,
            '#weight' => -11,
            #'#suffix' => '@lists.mailjet.com', //new API creates automatic address for the list
        );

        $form['list_id'] = array(
            '#title' => t('Label'),
            '#type' => 'textfield',
            '#default_value' => isset($entity->list_id) ? $entity->list_id : '',
            '#description' => t('The Mailjet List ID'),
            '#disabled' => TRUE,
            '#size' => 30,
            '#weight' => -10,
        );

        // Status property.
        $form['status'] = array(
            '#type' => 'radios',
            '#title' => t('Status'),
            '#options' => array(0 => t('Disabled'), 1 => t('Enabled')),
            '#default_value' => isset($entity->status) ? $entity->status : 1,
            '#weight' => 35,
            '#required' => TRUE,
        );

        field_attach_form('mailjet_list', $entity, $form, $form_state);

        $form['actions'] = array('#type' => 'actions');
        $form['actions']['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Save list'),
            '#weight' => 40,
        );
    }
    return $form;
}

/**
 * Validiation
 */
function mailjet_list_form_validate(&$form, &$form_state)
{
    if (!variable_get('mailjet_username') && !variable_get('mailjet_password')) {
        drupal_set_message(t('You need to add your Mailjet API details before you can continue'), 'warning');
        form_set_error('');
        return FALSE;
    }

    // Don't allow chars that are not cm.
    $pattern_name = '/^[A-Za-z0-9 ]+$/';

    if (preg_match($pattern_name, $form['name']['#value']) != 1) {
        form_set_error('name', t('Only letters and numbers are allowed'));
        return FALSE;
    }

    return TRUE;
}

/**
 * Form API submit callback for the recurring form. the form that was edited will be submit
 */
function mailjet_list_form_submit(&$form, &$form_state)
{
    $form_state['values']['label'] = $form_state['values']['name'];
    $entity = entity_ui_form_submit_build_entity($form, $form_state);

    if (isset($form['del']['#value']) && isset($form['del']['#value']) == 'delete') {
        // Delete and go back.
        $entity->delete();
        $form_state['redirect'] = 'admin/mailjet/list';
    } else {
        // Save and go back.
        $entity->save();
        $form_state['redirect'] = 'admin/mailjet/list';
    }
}

/**
 * Form for syncing lists
 */

/**
 * Sync all fields
 */
function mailjet_list_sync_form($form, &$form_state, $mailjet_list)
{
    $form['mailjet_list'] = array(
        '#type' => 'hidden',
        '#mailjet' => $mailjet_list,
    );
    $view_name = $mailjet_list->mailjet_list_view[LANGUAGE_NONE][0]['vname'];
    $view_name = explode('|', $view_name);
    $view_name = $view_name[0];

    $view = views_get_view($view_name);
    $field = $view->display['default']->display_options['style_options']['email'];
    $view->execute();
    $total_rows = $view->total_rows;

    $x = 0;

    $mailjet_users = array();
    $drupal_users = array();

    while ($x != $total_rows) {
        $user_info = (array)user_load_by_mail($view->render_field($field, $x));
        if (!empty($user_info['uid'])) {
            $drupal_users[$view->render_field($field, $x)] = array(
                'uid' => $user_info['uid'],
                'name' => $user_info['name'],
                'mail' => $user_info['mail'],
                'created' => $user_info['created'],
            );

            foreach ($user_info as $key => $value) {
                // ADD ALL FIELDS
                if (strpos($key, "field_") !== false) {
                    $drupal_users[$view->render_field($field, $x)]['properties'][str_replace('field_', '', $key)] = (!empty($value)) ? $value[LANGUAGE_NONE][0]['value'] : '';
                }
            }
        }

        $x++;
    }

    // Gets list of contacts.
    $result = mailjet_list_contacts($mailjet_list);

    if ($result) {
        foreach ($result->Data as $mailjet_user) {
            $mailjet_users[] = $mailjet_user->Contact->Email->Email;
        }
    }

    $form['contacts'] = array(
        '#type' => 'fieldset',
        '#title' => t('Your  Drupal Contacts'),
    );
    $i = 0;

    foreach ($drupal_users as $key => $existing_user) {
        if ($i < 100) {
            $form['contacts'][$key] = array(
                '#type' => 'markup',
                '#markup' => ($i == 0) ? ($key) : (', ' . $key),
            );
        } else {
            $form['contacts'][$key] = array(
                '#type' => 'markup',
                '#markup' => t(" + @count Other Current Contacts", array('@count' => count($drupal_users) - 100)),
            );
            break;
        }
        $i++;
    }

    // Gets the the different users set up in $drupal_users, that are not presented in $mail_jet users.
    $form['#mailjet']['new_users'] = array();
    $form['#mailjet']['new_users'] = mailjet_compare_lists("all", $drupal_users, $mailjet_users);

    $form['expired'] = array(
        '#type' => 'fieldset',
        '#title' => t('Expired Contacts'),
    );

    $form['#mailjet']['expired_users'] = array();
    $x = 0;
    foreach ($mailjet_users as $key => $existing_user) {

        if (!array_key_exists($existing_user, $drupal_users)) {
            if ($x < 101) {
                $form['#mailjet']['expired_users'][] = $mailjet_users[$key];
                $form['expired'][$key] = array(
                    '#type' => 'markup',
                    '#markup' => ($x == 0) ? ($mailjet_users[$key]) : (', ' . $mailjet_users[$key]),
                );
            } else {
                $form['expired'][$key] = array(
                    '#type' => 'markup',
                    '#markup' => t("+ @count Other Expired Contacts", array('@count' => count($form['#mailjet']['expired_users']) - 100)),
                );
                break;
            }
            $x++;
        }
    }

    $form['existing'] = array(
        '#type' => 'fieldset',
        '#title' => t('Your Existing Mailjet Contacts'),
    );
    $x = 0;
    foreach ($mailjet_users as $key => $existing_user) {
        if ($x < 101) {
            $form['existing'][$key] = array(
                '#type' => 'markup',
                '#markup' => ($key == 0) ? ($mailjet_users[$key]) : (', ' . $mailjet_users[$key]),
            );
        } else {
            $form['existing'][$key] = array(
                '#type' => 'markup',
                '#markup' => t("+ @count Other Existing Contacts", array('@count' => count($form['#mailjet']['existing']) - 100)),
            );
            break;
        }

        $x++;
    }

    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Syncronise with Mailjet'),
        '#weight' => 40,
    );

    return $form;
}


/**
 * Validated the form for syncing lists
 */
function mailjet_list_sync_form_validate($form, &$form_state)
{
    $mailjet_list = $form['mailjet_list']['#mailjet'];
    if (!isset($mailjet_list->list_id)) {
        form_set_error('', t('Could not find mailjet list'));
        return;
    }
    mailjet_properties_sync();
}

/**
 * Submit the form for syncing lists
 */
function mailjet_list_sync_form_submit($form, &$form_state)
{
    if ((isset($form['#mailjet']['new_users']) && empty($form['#mailjet']['new_users'])) && (isset($form['#mailjet']['expired_users']) && empty($form['#mailjet']['expired_users']))) {
        drupal_set_message(t('The lists are up to date.'), 'status');
        return;
    }
    // Start by creating a new mailjet object if we cant do this then there is no point continuing.
    $mailjet = mailjet_new();
    if (!$mailjet){
        return FALSE;
    }

    // Gets the mailjet list entity from the form.
    $mailjet_list = $form['mailjet_list']['#mailjet'];
    if (isset($form['#mailjet']['new_users']) && !empty($form['#mailjet']['new_users'])) {
        $newusers_list = $form['#mailjet']['new_users'];

        // Create the params to post to mailjet including the comma seperated new users list.

        $csv_str = "email,";
        $cnt = 0;
        foreach ($newusers_list as $key => $value) {
            if ($cnt < 1) {
                if (array_key_exists('properties', $value)) {
                    foreach ($value['properties'] as $pkey => $pvalue) {
                        $csv_str .= "{$pkey},";
                    }
                }
                $csv_str = substr($csv_str, 0, -1);
                $csv_str .= "\n";
                $cnt++;
            }
        }

        foreach ($newusers_list as $key => $value) {
            $csv_str .= "{$value['mail']},";
            if (array_key_exists('properties', $value)) {
                foreach ($value['properties'] as $pkey => $pvalue) {
                    $csv_str .= "{$pvalue},";
                }
            }
            $csv_str = substr($csv_str, 0, -1);
            $csv_str .= "\n";
        }


        $mailjet->resetRequest();
        $responseCSVData = $mailjet->data('contactslist', $mailjet_list->list_id, 'CSVData', 'text/plain', $csv_str, 'POST', null)->getResponse();

        if ($responseCSVData) {
            $clid = json_decode($responseCSVData);

            $mailjet->resetRequest();
            $paramsBatchJob = array(
                'method' => 'JSON',
                'JobType' => 'Contact list import csv',
                'DataID' => $clid->ID,
                'Status' => 'Upload',
                'RefId' => $mailjet_list->list_id,
                'Method' => 'addnoforce',
                'APIKeyALT' => variable_get('mailjet_username')
            );

            $responseBatchJob = $mailjet->batchjob($paramsBatchJob)->getResponse();

            if ($responseBatchJob && $responseBatchJob->Count > 0) {
                $bjID = $responseBatchJob->Data[0]->ID;
                $paramsBatchJobCheck = array(
                    'method' => 'GET',
                    'ID' => $bjID,
                    'APIKeyALT' => variable_get('mailjet_username')
                );
                $mailjet->resetRequest();
                $responseBatchJobCheck = $mailjet->batchjob($paramsBatchJobCheck)->getResponse();

                if ($responseBatchJobCheck && $responseBatchJobCheck->Data[0]->Status == "Completed") {
                    drupal_set_message(t('Sync is complete.'), 'status');
                } else {
                    $bjIDs = variable_get('runningBtachJobs', array());
                    $bjIDs[] = $bjID;
                    variable_set('runningBtachJobs', $bjIDs);
                    drupal_set_message(t('Sync in proggress.'), 'status');
                }
            } else {
                form_set_error('', t('Could not create a BatchJob for this import'));
            }
        } else {
            form_set_error('', t('Could not create the contact list'));
        }

    }

    if (isset($form['#mailjet']['expired_users']) && !empty($form['#mailjet']['expired_users'])) {
        $expiredusers_list = $form['#mailjet']['expired_users'];
        $csv_str = "email" . "\n";

        foreach ($expiredusers_list as $key => $value) {
            $csv_str .= "{$value}" . "\n";
        }

        $mailjet->resetRequest();
        $responseCSVData = $mailjet->data('contactslist', $mailjet_list->list_id, 'CSVData', 'text/plain', $csv_str, 'POST', null)->getResponse();

        if ($responseCSVData) {
            $clid = json_decode($responseCSVData);

            $mailjet->resetRequest();
            $paramsBatchJob = array(
                'method' => 'JSON',
                'JobType' => 'Contact list import csv',
                'DataID' => $clid->ID,
                'Status' => 'Upload',
                'RefId' => $mailjet_list->list_id,
                'Method' => 'remove',
                'APIKeyALT' => variable_get('mailjet_username')
            );

            $responseBatchJob = $mailjet->batchjob($paramsBatchJob)->getResponse();

            if ($responseBatchJob && $responseBatchJob->Count > 0) {
                $paramsBatchJobCheck = array(
                    'method' => 'GET',
                    'ID' => $responseBatchJob->Data[0]->ID,
                    'APIKeyALT' => variable_get('mailjet_username')
                );
                $mailjet->resetRequest();
                $responseBatchJobCheck = $mailjet->batchjob($paramsBatchJobCheck)->getResponse();

                if ($responseBatchJobCheck && $responseBatchJobCheck->Data[0]->Status == "Completed") {
                    drupal_set_message(t('Sync for expired contants is complete.'), 'status');
                } else {
                    drupal_set_message(t('Sync for expired contants in proggress.'), 'status');
                }
            } else {
                form_set_error('', t('Could not create a BatchJob for expired contants'));
            }
        } else {
            form_set_error('', t('Could not create the contact list for expired contants'));
        }
    }

    return;
}

/**
 * Compares mailjet lists.
 */
function mailjet_compare_lists($type, $drupal_users, $mailjet_users)
{
    $res_users = array();
    switch ($type) {
        case 'all':
            foreach ($drupal_users as $key => $value) {
                $res_users[$key] = $drupal_users[$key];
            }
            break;
        case 'new':
            foreach ($drupal_users as $key => $value) {
                if (!in_array($key, $mailjet_users)) {
                    $res_users[$key] = $drupal_users[$key];
                }
            }
            break;

        default:
            # code...
            break;
    }
    return $res_users;
}
