<?php

/**
 * @file
 * The backoffice of module.
 */

/**
 * Settings form callback
 */
function mailjet_admin_settings_form()
{

    if (variable_get('mailjet_active', FALSE)) {
        $form['onoff'] = array(
            '#type' => 'fieldset',
            '#title' => t('General settings')
        );

        $mail_system = variable_get('mail_system');
        if (isset($mail_system['default-system']) && $mail_system['default-system'] == 'MailjetSmtpMailSystem') {
            $mailjet_on = TRUE;
        } else {
            $mailjet_on = FALSE;
        }

        $form['onoff']['mailjet_on'] = array(
            '#type' => 'checkbox',
            '#title' => t('Send emails through Mailjet'),
            '#default_value' => $mailjet_on
        );

        if ($tracking_check = mailjet_user_trackingcheck()) {

            $form['tracking'] = array(
                '#type' => 'fieldset',
                '#title' => t('Tracking')
            );

            $check = array(
                "open" => 0,
                "click" => 0,
                "bounce" => 0,
                "spam" => 0,
                "blocked" => 0,
                "unsub" => 0,
                "typofix" => 0
            );
            $tracking_url = variable_get("mj_callback_url");
            $current_events = array();
            foreach ($tracking_check->Data as $event) {
                if (array_key_exists($event->EventType, $check)) {
                    $check[$event->EventType] = 1;
                    $tracking_url = $event->Url;
                    $current_events[$event->EventType] = $event->ID;
                }
            }
            $current_events = serialize($current_events);

            $form['tracking']['tracking_open'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Open events'),
                '#default_value' => $check['open']
            );

            $form['tracking']['tracking_click'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Click events'),
                '#default_value' => $check['click']
            );

            $form['tracking']['tracking_bounce'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Bounce events'),
                '#default_value' => $check['bounce']
            );

            $form['tracking']['tracking_spam'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Spam events'),
                '#default_value' => $check['spam']
            );

            $form['tracking']['tracking_blocked'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Blocked events'),
                '#default_value' => $check['blocked']
            );

            $form['tracking']['tracking_unsub'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Unsub events'),
                '#default_value' => $check['unsub']
            );

            $form['tracking']['tracking_typofix'] = array(
                '#type' => 'checkbox',
                '#title' => t(' Typofix events'),
                '#default_value' => $check['typofix']
            );

            $form['tracking']['tracking_url'] = array(
                '#type' => 'hidden',
                '#default_value' => $tracking_url
            );

            $form['tracking']['current_events'] = array(
                '#type' => 'hidden',
                '#default_value' => $current_events
            );

            $form['#submit'][] = 'mailjet_admin_settings_tracking';
        }

        if ($user_infos = mailjet_user_infos()) {
            //print"<pre>"; print_r($user_infos);
            $form['infos'] = array(
                '#type' => 'fieldset',
                '#title' => t('Account Information')
            );

            $form['infos']['username'] = array(
                '#type' => 'textfield',
                '#title' => t('E-mail'),
                '#default_value' => $user_infos['Email'],
                '#disabled' => TRUE
            );

            $form['infos']['firstname'] = array(
                '#type' => 'textfield',
                '#title' => t('First Name'),
                '#default_value' => $user_infos['Firstname'],
                '#required' => TRUE
            );

            $form['infos']['lastname'] = array(
                '#type' => 'textfield',
                '#title' => t('Last Name'),
                '#default_value' => $user_infos['Lastname'],
                '#required' => TRUE
            );

            $form['infos']['company_name'] = array(
                '#type' => 'textfield',
                '#title' => t('Company Name'),
                '#default_value' => $user_infos['CompanyName'],
                '#required' => TRUE
            );

            $form['infos']['address_street'] = array(
                '#type' => 'textfield',
                '#title' => t('Address'),
                '#default_value' => $user_infos['AddressStreet'],
                '#required' => TRUE
            );

            $form['infos']['address_city'] = array(
                '#type' => 'textfield',
                '#title' => t('City'),
                '#default_value' => $user_infos['AddressCity'],
                '#required' => TRUE
            );

            $form['infos']['address_postal_code'] = array(
                '#type' => 'textfield',
                '#title' => t('Post Code'),
                '#default_value' => $user_infos['AddressPostalCode'],
                '#required' => TRUE
            );

            require_once DRUPAL_ROOT . '/includes/locale.inc';
            $form['infos']['address_country'] = array(
                '#type' => 'select',
                '#title' => t('Country'),
                '#options' => country_get_list(),
                '#default_value' => $user_infos['AddressCountry'],
                '#required' => TRUE
            );

            // States only show up for US citizens
            require_once dirname(__FILE__) . '/UsStates.php';
            $form['infos']['address_state'] = array(
                '#type' => 'select',
                '#title' => t('State'),
                '#options' => UsStates::getStates(),
                '#default_value' => $user_infos['AddressState'],
                '#required' => TRUE,
                '#states' => array(
                    // Only show this field when the value of type is sell.
                    'visible' => array(
                        ':input[name="address_country"]' => array('value' => 'US'),
                    ),
                ),
            );

            $form['#submit'][] = 'mailjet_admin_settings_user_infos';
        }

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => 'Save Settings'
        );

        $form['#validate'][] = 'mailjet_admin_settings_form_validate';
        $form['#submit'][] = 'mailjet_admin_settings_form_submit';

        return $form;
    } else {
        drupal_goto('admin/config/system/mailjet/api');
    }
}

/**
 * Implements hook_admin_settings_validate().
 */
function mailjet_admin_settings_form_validate($form, &$form_state)
{
    $configs = array(
        array(
            'ssl://',
            465
        ),
        array(
            'tls://',
            587
        ),
        array(
            '',
            587
        ),
        array(
            '',
            588
        ),
        array(
            'tls://',
            25
        ),
        array(
            '',
            25
        )
    );

    $host = variable_get('mailjet_host', 'in-v3.mailjet.com');
    $connected = FALSE;

    for ($i = 0; $i < count($configs); ++$i) {
        $soc = @ fsockopen($configs[$i][0] . $host, $configs[$i][1], $errno, $errstr, 5);

        if ($soc) {
            fClose($soc);
            $connected = TRUE;
            break;
        }
    }

    if ($connected) {
        if ('ssl://' == $configs[$i][0]) {
            variable_set('mailjet_protocol', 'ssl');
        } elseif ('tls://' == $configs[$i][0]) {
            variable_set('mailjet_protocol', 'tls');
        } else {
            variable_set('mailjet_protocol', 'standard');
        }

        variable_set('mailjet_port', $configs[$i][1]);
    } else {
        form_set_error('mailjet_on', t('Please contact Mailjet support to sort this out.<br /><br />Error @errno - @errstr', array(
            '@errno' => $errno,
            '@errstr' => $errstr
        )));
    }
}

/**
 * Implements hook_admin_settings_validate().
 */
function mailjet_admin_settings_form_submit($form, &$form_state)
{
    $mail_system = variable_get('mail_system', array(
        'default-system' => 'DefaultMailSystem'
    ));
    if ($form_state['values']['mailjet_on']) {
        $mail_system['default-system'] = 'MailjetSmtpMailSystem';
        variable_set('mail_system', $mail_system);
    } else {
        $mail_system['default-system'] = 'DefaultMailSystem';
        variable_set('mail_system', $mail_system);
    }
}

/**
 * Set user tracking.
 */
function mailjet_admin_settings_tracking($form, &$form_state)
{
    $tracking = array(
        "url" => $form_state['values']['tracking_url'],
        "open" => $form_state['values']['tracking_open'],
        "click" => $form_state['values']['tracking_click'],
        "bounce" => $form_state['values']['tracking_bounce'],
        "spam" => $form_state['values']['tracking_spam'],
        "blocked" => $form_state['values']['tracking_blocked'],
        "unsub" => $form_state['values']['tracking_unsub'],
        "typofix" => $form_state['values']['tracking_typofix']
    );
    $current_events = unserialize($form_state['values']['current_events']);
    if (mailjet_user_trackingupdate($tracking, $current_events)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
/**
 * Checks user info.
 */
function mailjet_admin_settings_user_infos($form, &$form_state)
{
    $infos = array(
        'Firstname' => $form_state['values']['firstname'],
        'Lastname' => $form_state['values']['lastname'],
        'CompanyName' => $form_state['values']['company_name'],
        'AddressStreet' => $form_state['values']['address_street'],
        'AddressCity' => $form_state['values']['address_city'],
        'AddressPostalCode' => $form_state['values']['address_postal_code'],
        'AddressCountry' => $form_state['values']['address_country'],
        'AddressState' => empty($form_state['values']['address_state']) || $form_state['values']['address_country'] !== 'US' ?
            '' : $form_state['values']['address_state']
    );
    if (mailjet_mjuser_update($infos)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
/**
 * Test email form.
 */
function mailjet_admin_test_message_form($form_state)
{
    $form = array();

    $form['mailjet_test_address'] = array(
        '#type' => 'textfield',
        '#title' => t('Recipient of test mail'),
        '#default_value' => variable_get('mailjet_test_address', '')
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Send Test Message'
    );

    $form['#validate'][] = 'mailjet_admin_test_message_form_validate';
    $form['#submit'][] = 'mailjet_admin_test_message_form_submit';

    return $form;
}

/**
 * Validation for the test email form.
 */
function mailjet_admin_test_message_form_validate($form, &$form_state)
{
    if (!valid_email_address($form_state['values']['mailjet_test_address'])) {
        form_set_error('mailjet_test_address', t('The provided test e-mail address is not valid.'));
    }
}

/**
 * Submit form for the test email.
 */
function mailjet_admin_test_message_form_submit($form, &$form_state)
{
    $test_address = $form_state['values']['mailjet_test_address'];

    global $language;

    $from = mailjet_get_a_trusted_email();
    $params['subject'] = t('Your test mail from Mailjet');
    $params['body'] = array(
        t('Your Mailjet configuration is ok!')
    );

    $message = drupal_mail('mailjet', 'mailjet-test', $test_address, $language, $params, $from, FALSE);

    if (empty($message['from'])) {
        $message['from'] = variable_get('site_mail', '');
    }
    if (empty($message['subject'])) {
        $message['subject'] = $params['subject'];
    }
    if (empty($message['body'])) {
        $message['body'] = $params['body'];
    }

    $mail_system = new MailjetSmtpMailSystem();

    if ($mail_system->mail($message)) {
        drupal_set_message(t('A test e-mail has been sent to @email.', array(
            '@email' => check_plain($test_address)
        )));
        drupal_goto('admin/config/system/mailjet');
    } else {
        drupal_set_message(t('Unable to send e-mail.'), 'error');
    }
}

/**
 * API Form callback
 */
function mailjet_api_form($form_state)
{
    $form = array();

    $form['api'] = array(
        '#type' => 'fieldset',
    );
    $form['api']['welcome'] = array(
        '#markup' => t('Welcome to the Mailjet Configuration page.</br>
      If you are new to Mailjet, please register by clicking on the button above.</br>
      Should you already have a pre-existing Mailjet account, please copy and paste your Mailjet API Key and Secret Key which can be found ' . l('here', IFRAME_URL . 'account/api_keys', array('attributes' => array('target' => '_blank')))));
    $form['api']['mailjet_username'] = array(
        '#type' => 'textfield',
        '#title' => t('API Key'),
        '#default_value' => variable_get('mailjet_username', ''),
        '#required' => TRUE
    );
    $form['api']['mailjet_password'] = array(
        '#type' => 'textfield',
        '#title' => t('Secret Key'),
        '#default_value' => variable_get('mailjet_password', ''),
        '#required' => TRUE
    );

    return system_settings_form($form);
}

/**
 * API form validate
 */
function mailjet_api_form_validate($form, $form_state)
{
    module_load_include('api.inc', 'mailjet');
    $mailjet = new Mailjet($form_state['values']['mailjet_username'], $form_state['values']['mailjet_password']);

    $paramsProfile = array(
        'method' => 'GET',
    );

    $response = $mailjet->myprofile($paramsProfile)->getResponse();

    if ($response) {
        variable_set('mailjet_active', TRUE);

        $params = array(
            'AllowedAccess' => 'campaigns,contacts,stats,pricing,account,reports',
            'method' => 'JSON',
            'APIKeyALT' => $mailjet->getAPIKey(),
            'TokenType' => 'iframe',
            'IsActive' => true
        );
        $mailjet->resetRequest();
        $response2 = $mailjet->apitoken($params)->getResponse();
        if ($response2->Count > 0) {
            variable_set('APItoken', $response2->Data[0]->Token);
            variable_set('mailjet_username', $form_state['values']['mailjet_username']);
            variable_set('mailjet_password', $form_state['values']['mailjet_password']);
            mailjet_first_sync(mailjet_get_default_list_id(mailjet_new()));
        } else {
            drupal_set_message(t('Token was not generated! Please try again.'), 'error');
        }

    } else {
        form_set_error('mailjet_username', t('Please verify that you have entered your API and secret key correctly. Please note this plug-in is compatible for Mailjet v3 accounts only. Click <a href=" https://app.mailjet.com/support/why-do-i-get-an-api-error-when-trying-to-activate-a-mailjet-plug-in,497.htm"> here</a> for more information'));
        form_set_error('mailjet_password', '');
    }
}

/**
 * Displays the trusted domain table.
 */
function mailjet_domain_form($form_state)
{
    $form = array();
    $header = array(
        'domain' => array(
            'data' => t('Domain')
        ),
        'enabled' => array(
            'data' => t('Enabled')
        ),
        'file_name' => array(
            'data' => t('File Name')
        )
    );

    $options = array();
    if ($domains = mailjet_user_domain_list()) {
        foreach ($domains as $domain) {
            if (is_object($domain->Email)) {
                $email = $domain->Email->Email;
            } else {
                $email = $domain->Email;
            }
            $options[$email] = array(
                'domain' => str_replace('*@', '', $email),
                'enabled' => $domain->Status,
                'file_name' => $domain->Filename
            );
        }

        $form['domains'] = array(
            '#type' => 'tableselect',
            '#header' => $header,
            '#options' => $options,
            '#empty' => t('There are no authorized domains.')
        );

        $form['submit'] = array(
            '#type' => 'submit',
            '#value' => t('Check Status')
        );

        $form['#submit'][] = 'mailjet_domain_form_submit';

        return $form;
    }
}

    /**
     * Domains form submit
     */
    function mailjet_domain_form_submit($form, &$form_state)
    {
        foreach ($form_state['values']['domains'] as $domain) {
            if ($domain) {
                mailjet_user_domain_status($domain);
            }
        }
    }

