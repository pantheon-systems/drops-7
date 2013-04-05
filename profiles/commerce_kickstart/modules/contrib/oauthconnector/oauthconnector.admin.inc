<?php

/**
 * @file
 * Administrative functions for the OAuth Connector module.
 *
 * This provides the UI to list, create, edit and delete providers.
 */

/**
 * Output a list of fields.
 */

function oauthconnector_list_fields() {
  $build = array();
  $build['add'] = drupal_get_form('oauthconnector_field_form');
  $header = array(
    array(
      'data' => t('Name'),
      'class' => 'oauthconnector-fields-name',
    ),
    array(
      'data' => t('Title'),
      'class' => 'oauthconnector-fields-title',
    ),
    array(
      'data' => t('Operations'),
      'class' => 'oauthconnector-fields-operations',
    ),
  );
  $fields = oauthconnector_fields();
  $rows = array();

  foreach ($fields as $field_name => $field) {
    $operations = array();

    if (!in_array($field_name, _oauthconnector_fieldkeys_not_in_db())) {
      $operations[] = array(
        'title' => t('Edit'),
        'href' => 'admin/structure/oauthconnector/fields/' . $field_name . '/edit',
      );
      $operations[] = array(
        'title' => t('Delete'),
        'href' => 'admin/structure/oauthconnector/fields/' . $field_name . '/delete',
      );
    }
    $rows[$field_name] = array(
      'data' => array(
        'name' => check_plain($field_name),
        'title' => check_plain($field['title']),
        'operations' => theme('links', array('links' => $operations, 'attributes' => array('class' => array('links', 'inline')))),
      ),
    );
  }
  $build['table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'oauthconnector-list-fields'))),
  );
  return $build;
}

/**
 * Output a list of providers.
 */
function oauthconnector_list_provider($js = NULL) {
  $build = array();
  $build['presets'] = drupal_get_form('oauthconnector_presets_form');
  $header = array(
    array(
      'data' => t('Title') . '/' . t('URL'),
      'class' => 'oauthconnector-provider-title',
    ),
    array(
      'data' => t('Connect URL'),
      'class' => 'oauthconnector-provider-connect-url',
    ),
    array(
      'data' => t('Callback URL'),
      'class' => 'oauthconnector-provider-callback-url',
    ),
    array(
      'data' => t('Storage'),
      'class' => 'oauthconnector-provider-storage',
    ),
    array(
      'data' => t('Operations'),
      'class' => 'oauthconnector-provider-operations',
    ),
  );

  $providers = oauthconnector_provider_load_all();
  $rows = array();

  foreach ($providers as $provider) {
    $operations = array();

    if (empty($provider->disabled)) {
      $operations[] = array(
        'title' => t('Edit'),
        'href' => 'admin/structure/oauthconnector/' . $provider->name . '/edit',
      );
      $operations[] = array(
        'title' => t('Export'),
        'href' => 'admin/structure/oauthconnector/' . $provider->name . '/export',
      );
    }

    if ($provider->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
      $operations[] = array(
        'title' => t('Revert'),
        'href' => 'admin/structure/oauthconnector/' . $provider->name . '/delete',
      );
    }
    elseif ($provider->export_type != EXPORT_IN_CODE) {
      $operations[] = array(
        'title' => t('Delete'),
        'href' => 'admin/structure/oauthconnector/' . $provider->name . '/delete',
      );
    }
    elseif (empty($provider->disabled)) {
      $operations[] = array(
        'title' => t('Disable'),
        'href' => 'admin/structure/oauthconnector/' . $provider->name . '/disable',
        'query' => drupal_get_destination(),
      );
    }
    else {
      $operations[] = array(
        'title' => t('Enable'),
        'href' => 'admin/structure/oauthconnector/' . $provider->name . '/enable',
        'query' => drupal_get_destination(),
      );
    }
    $callbackurl = empty($provider->csid) ? '' : l('oauth/authorized/' . $provider->csid, 'oauth/authorized/' . $provider->csid);
    if ($provider->consumer_advanced['oauth2']) {
      $callbackurl = empty($provider->csid) ? '' : l('oauth/authorized2/' . $provider->csid, 'oauth/authorized2/' . $provider->csid);
    }
    $connect_url = 'connect/oauthconnector_' . $provider->name;
    $connect_url = empty($provider->csid) ? '' :l($connect_url, $connect_url);
    $rows[$provider->name] = array(
      'data' => array(
        'title' => check_plain($provider->title) . '<br/>' . l($provider->url, $provider->url),
        'connect_url' => $connect_url,
        'callback_url' => $callbackurl,
        'storage' => $provider->type,
        'operations' => theme('links', array('links' => $operations, 'attributes' => array('class' => array('links', 'inline')))),
      ),
      'class' => (!empty($provider->disabled) ? array('oauthconnector-provider-disabled') : ''),
    );

    if (empty($rows[$provider->name]['data']['callback_url'])) {
      $rows[$provider->name]['data']['callback_url'] = array(
        'data' => t('Missing consumer key') . (empty($provider->disabled) ? ' (' . l(t('Add'), 'admin/structure/oauthconnector/' . $provider->name . '/edit') . ')' : ''),
        'class' => 'oauthconnector-provider-warning',
      );
    }
  }

  $build['table'] = array(
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'oauthconnector-list-provider'))),
  );

  drupal_add_css(drupal_get_path('module', 'oauthconnector') . '/oauthconnector.admin.css');

  return $build;
}

/**
 * Handle the add provider page.
 */
function oauthconnector_add_provider() {

  $provider = oauthconnector_provider_new();
  if (arg(4)) {
    $presets = oauthconnector_presets();
    if (isset($presets[arg(4)]['data'])) {
      $provider = $presets[arg(4)]['data'];
    }
  }
  drupal_set_title(t('Add provider'));
  return oauthconnector_edit_provider($provider);
}

/**
 * Edit a provider.
 *
 * Called from both the add and edit points to provide for common flow.
 */
function oauthconnector_edit_provider($provider) {
  if (!is_object($provider)) {
    $provider = oauthconnector_provider_load($provider);
  }
  if (!empty($provider->csid)) {
    $provider->consumer = DrupalOAuthConsumer::loadById($provider->csid, FALSE);
  }
  if ($provider && !empty($provider->title)) {
    drupal_set_title($provider->title);
  }

  return drupal_get_form('oauthconnector_edit_form_provider', $provider);
}

/**
 * Form to edit the settings of a provider.
 */
function oauthconnector_edit_form_provider($form, &$form_state, $provider) {
  $form = array();

  $form['pid'] = array(
    '#type' => 'value',
    '#value' => isset($provider->pid) ? $provider->pid : '',
  );
  $form['provider'] = array(
    '#type' => 'value',
    '#value' => $provider,
  );

  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('A human-readable title for the provider.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $provider->title,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 24,
    '#default_value' => $provider->name,
    '#title' => t('Name'),
    '#description' => t('A unique machine-readable name used to identify this provider internally. It may only contain lowercase alphanumeric characters and underscores. No spaces or uppercase characters.'),
    '#required' => TRUE,
    '#machine_name' => array(
      'exists' => 'oauthconnector_edit_form_provider_validate_name_exist',
      'source' => array('title'),
    )
  );
  $form['url'] = array(
    '#type' => 'textfield',
    '#title' => t('Base URL'),
    '#description' => t('A URL to the OAuth provider.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $provider->url,
  );

  $form['consumer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth Consumer Key'),
    '#description' => t('Your consumer key provided by the OAuth provider.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#default_value' => (isset($provider->consumer->key)?$provider->consumer->key:NULL),
  );

  $form['consumer_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth Consumer Secret'),
    '#description' => t('Your consumer secret provided by the OAuth provider.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#default_value' => (isset($provider->consumer->secret)?$provider->consumer->secret:NULL),
  );

  $form['consumer_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('OAuth Consumer Advanced Settings'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE, //TODO: Change if a value is non-default
  );

  $form['consumer_advanced']['oauth2'] = array(
    '#type' => 'checkbox',
    '#title' => t('Oauth v2'),
    '#default_value' => $provider->consumer_advanced['oauth2'],
  );

  $sign_methods = array(
    'HMAC-SHA1' => 'HMAC-SHA1',
  );
  foreach (hash_algos() as $algo) {
    $sign_methods['HMAC-' . strtoupper($algo)] = 'HMAC-' . strtoupper($algo);
  }
  $sign_methods['PLAINTEXT'] = 'PLAINTEXT';
  $form['consumer_advanced']['signature method'] = array(
    '#type' => 'select',
    '#title' => t('Signature method'),
    '#options' => $sign_methods,
    '#required' => TRUE,
    '#default_value' => $provider->consumer_advanced['signature method'],
  );



  $form['consumer_advanced']['authentication realm'] = array(
    '#type' => 'textfield',
    '#title' => t('Authentication realm'),
    '#size' => 32,
    '#maxlength' => 255,
    '#default_value' => $provider->consumer_advanced['authentication realm'],
    '#states' => array(
      'invisible' => array(
        ':input[name="consumer_advanced\[oauth2\]"]' => array('checked' => TRUE),
      ),
    ),
  );

  $form['consumer_advanced']['request token endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Request token endpoint'),
    '#description' => t('Absolute path or path relative to base URL.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#default_value' => $provider->consumer_advanced['request token endpoint'],
    '#states' => array(
      'invisible' => array(
        ':input[name="consumer_advanced\[oauth2\]"]' => array('checked' => TRUE),
      ),
      'required' => array(
        ':input[name="consumer_advanced\[oauth2\]"]' => array('checked' => FALSE),
      ),
    ),
  );
  $form['consumer_advanced']['authorization scope'] = array(
    '#type' => 'textarea',
    '#title' => t('Scope'),
    '#description' => t('Scope for the authorization endpoint.'),
    '#default_value' => $provider->consumer_advanced['authorization scope'],
    '#states' => array(
      'invisible' => array(
        ':input[name="consumer_advanced\[oauth2\]"]' => array('checked' => FALSE),
      ),
      'required' => array(
        ':input[name="consumer_advanced\[oauth2\]"]' => array('checked' => TRUE),
      ),
    ),
  );
  $form['consumer_advanced']['authorization endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Authorization endpoint'),
    '#description' => t('Absolute path or path relative to base URL.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $provider->consumer_advanced['authorization endpoint'],
  );

  $form['consumer_advanced']['access token endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Access token endpoint'),
    '#description' => t('Absolute path or path relative to base URL.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => $provider->consumer_advanced['access token endpoint'],
  );
  $mapping_description = t('Map the attributes from the API response to the attributes useable by OAuth Connector.');
  if (!module_exists('querypath')) {
    $mapping_description .= '<br/>';
    $mapping_description .= t('If you have the !QueryPath module installed the field selection can be made by a CSS selector.', array(
      '!QueryPath' => l('QueryPath', 'http://drupal.org/project/querypath')
    ));
  }
  $form['mapping'] = array(
    '#type' => 'fieldset',
    '#title' => t('Mapping'),
    '#description' => $mapping_description,
    '#tree' => TRUE,
    'fields' => array(),
  );

  $form['mapping']['format'] = array(
    '#type' => 'radios',
    '#title' => t('Format'),
    '#options' => array(
      'json' => 'JSON',
      'php' => 'PHP',
      'xml' => 'XML',
    ),
    '#default_value' => empty($provider->mapping['format']) ? 'json' : $provider->mapping['format'],
    '#weight' => -50,
  );

  $mappings = oauthconnector_fields();

  $profile_fields = array(
    'name' => t('Username'),
    'mail' => t('E-mail address'),
  );
  if (variable_get('user_pictures', 0)) {
    $profile_fields['picture'] = t('Picture');
  }
  if (variable_get('configurable_timezones', 0)) {
    $profile_fields['timezone'] = t('Time zone');
  }
  if (variable_get('user_signatures', 0)) {
    $profile_fields['signature'] = t('Signature');
  }
  if (variable_get('user_signatures', 0)) {
    $profile_fields['language'] = t('Language');
  }
  if (module_exists('field')) {
    $fields = field_info_instances('user', 'user');
    foreach ($fields as $key => $field) {
      $profile_fields[$key] = t($field['label']);
    }
  }

  foreach ($mappings as $key => $mapping) {
    $form['mapping']['fields'][$key] = array(
      '#type' => 'fieldset',
      '#title' => $mapping['title'],
      '#description' => $mapping['description'],
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    if ($key == 'uid') {
      $form['mapping']['fields'][$key]['#collapsible'] = FALSE;
      $form['mapping']['fields'][$key]['#weight'] = -49;
    }

    $form['mapping']['fields'][$key]['resource'] = array(
      '#type' => 'textfield',
      '#title' => t('Resource'),
      '#description' => t('The URL of the API resource representing the authorized user.'),
      '#size' => 32,
      '#maxlength' => 255,
      '#required' => !empty($mapping['required']),
      '#default_value' => empty($provider->mapping['fields'][$key]['resource']) ? '' : $provider->mapping['fields'][$key]['resource'],
    );

    $form['mapping']['fields'][$key]['method post'] = array(
      '#type' => 'checkbox',
      '#title' => t('POST request'),
      '#default_value' => !empty($provider->mapping['fields'][$key]['method post']),
    );

    $form['mapping']['fields'][$key]['field'] = array(
      '#type' => 'textfield',
      '#title' => t('Field'),
      '#size' => 32,
      '#maxlength' => 32,
      '#required' => !empty($mapping['required']),
      '#default_value' => empty($provider->mapping['fields'][$key]['field']) ? '' : $provider->mapping['fields'][$key]['field'],
    );

    $form['mapping']['fields'][$key]['querypath'] = array(
      '#type' => 'checkbox',
      '#title' => t('Field is a CSS selector'),
      '#default_value' => !empty($provider->mapping['fields'][$key]['querypath']),
      '#access' => module_exists('querypath'),
    );

    $form['mapping']['fields'][$key]['sync_with_field'] = array(
      '#type' => 'select',
      '#options' => array('' => t('- Does not match -')) + $profile_fields,
      '#title' => t('Field to match on user profile'),
      '#default_value' => isset($provider->mapping['fields'][$key]['sync_with_field'])?$provider->mapping['fields'][$key]['sync_with_field']:'',
    );
  }

  $label = empty($provider->pid) ? t('Save and proceed') : t('Save');
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => $label,
  );

  return $form;
}



/**
 * Validate submission of the provider edit form.
 */
function oauthconnector_edit_form_provider_validate($form, &$form_state) {
  $values = $form_state['values'];

  if (!valid_url($values['url'])) {
    form_error($form['url'], t('The url is not valid.'));
  }
  foreach ($values['mapping']['fields'] as $key => $mapping) {
    if (!empty($mapping['resource']) && !valid_url($mapping['resource'], TRUE)) {
      form_error($form['mapping']['fields'][$key]['resource'], t('The resource is not a valid url.'));
    }
  }
  //TODO: Validate that all resources are either completely filled out or completely empty
  //TODO: Maybe add some more validation? Eg. check for whitespace in empty mappings?
  //TODO: Add triming of eg valid_url() values?
}

// Name  exist for machine name
function oauthconnector_edit_form_provider_validate_name_exist($name) {
  $query = db_select('oauthconnector_provider')
    ->fields('oauthconnector_provider', array('pid'))
    ->condition('name', $name);
  return $query->execute()->fetchField();
}
/**
 * Process submission of the provider edit form.
 */
function oauthconnector_edit_form_provider_submit($form, &$form_state) {
  $values = $form_state['values'];

  $provider = $values['provider'];

  $provider->name              = $values['name'];
  $provider->title             = $values['title'];
  $provider->url               = $values['url'];

  //TODO: Need to save differently?
  $provider->consumer_key      = $values['consumer_key'];
  $provider->consumer_secret   = $values['consumer_secret'];
  $provider->consumer_advanced = $values['consumer_advanced'];

  $provider->mapping           = array_filter($values['mapping']);

  if (empty($provider->pid)) {
    drupal_set_message(t('Your new provider %title has been saved.', array('%title' => $provider->title)));
    oauthconnector_provider_save($provider);
  }
  else {
    drupal_set_message(t('Your changes have been saved.'));
    oauthconnector_provider_save($provider);
  }

  $form_state['redirect'] = 'admin/structure/oauthconnector';
}

/**
 * Page callback to export a provider to PHP code.
 */
function oauthconnector_export_provider($form, &$form_state, $provider) {
  if (!is_object($provider)) {
    $provider = oauthconnector_provider_load($provider);
  }
  drupal_set_title($provider->title);

  $code = oauthconnector_provider_export($provider);
  $lines = substr_count($code, "\n");
  $form['code'] = array(
    '#type' => 'textarea',
    '#title' => $provider->name,
    '#default_value' => $code,
    '#rows' => $lines,
  );

  return $form;
}

/**
 * Provide a form to confirm deletion of a provider.
 */
function oauthconnector_delete_confirm_provider($form, &$form_state, $provider) {
  if (!is_object($provider)) {
    $provider = oauthconnector_provider_load($provider);
  }
  if ($provider->export_type == (EXPORT_IN_CODE | EXPORT_IN_DATABASE)) {
    $title  = t('Are you sure you want to revert the provider "@name"?', array('@name' => $provider->title));
    $submit = t('Revert');
  }
  elseif ($provider->export_type != EXPORT_IN_CODE) {
    $title  = t('Are you sure you want to delete the provider "@name"?', array('@name' => $provider->title));
    $submit = t('Delete');
  }
  else {
    drupal_not_found();
    die; // legitimate
  }
  $form['provider'] = array(
    '#type' => 'value',
    '#value' => $provider,
  );
  return confirm_form($form,
    $title,
    !empty($_GET['destination']) ? $_GET['destination'] : 'admin/structure/oauthconnector',
    t('This action cannot be undone.'),
    $submit, t('Cancel')
  );
}

/**
 * Handle the submit button to delete a provider.
 */
function oauthconnector_delete_confirm_provider_submit($form, &$form_state) {
  $preset = $form_state['values']['provider'];
  oauthconnector_provider_delete($preset);
  $form_state['redirect'] = 'admin/structure/oauthconnector';
}

/**
 * Enable a default provider.
 */
function oauthconnector_enable_provider($provider) {
  if (!is_object($provider)) {
    $provider = oauthconnector_provider_load($provider);
  }
  ctools_include('export');
  ctools_export_set_status('oauthconnector_provider', $provider->name, FALSE);
  drupal_goto();
}

/**
 * Disable a default provider.
 */
function oauthconnector_disable_provider($provider) {
  if (!is_object($provider)) {
    $provider = oauthconnector_provider_load($provider);
  }
  ctools_include('export');
  ctools_export_set_status('oauthconnector_provider', $provider->name, TRUE);
  drupal_goto();
}

// the presets, with an alter to add extra presets.
function oauthconnector_presets() {

  $twitter = new stdClass;
  $twitter->name = '';
  $twitter->title = 'Twitter';
  $twitter->url = 'https://api.twitter.com';
  $twitter->consumer_advanced = array(
    'oauth2' => 0,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => '',
    'request token endpoint' => '/oauth/request_token',
    'authorization scope' => '',
    'authorization endpoint' => '/oauth/authorize',
    'access token endpoint' => '/oauth/access_token',
  );
  $twitter->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://api.twitter.com/1.1/account/verify_credentials.json',
        'method post' => 0,
        'field' => 'id',
        'querypath' => 0,
      ),
      'name' => array(
        'resource' => 'https://api.twitter.com/1.1/account/verify_credentials.json',
        'method post' => 0,
        'field' => 'name',
        'querypath' => 0,
      ),
      'avatar' => array(
        'resource' => 'https://api.twitter.com/1.1/account/verify_credentials.json',
        'method post' => 0,
        'field' => 'profile_image_url',
        'querypath' => 0,
      ),
    ),
    'format' => 'json',
  );

  $linkedin = new stdClass;
  $linkedin->name = '';
  $linkedin->title = 'LinkedIn';
  $linkedin->url = 'https://api.linkedin.com';
  $linkedin->consumer_advanced = array(
    'oauth2' => 0,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => '',
    'request token endpoint' => '/uas/oauth/requestToken',
    'authorization scope' => '',
    'authorization endpoint' => '/uas/oauth/authenticate',
    'access token endpoint' => '/uas/oauth/accessToken',
  );
  $linkedin->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://api.linkedin.com/v1/people/~:(id,first-name,picture-url)',
        'method post' => 0,
        'field' => 'id',
        'querypath' => 0,
      ),
      'name' => array(
        'resource' => 'https://api.linkedin.com/v1/people/~:(id,first-name,picture-url)',
        'method post' => 0,
        'field' => 'first-name',
        'querypath' => 0,
      ),
      'avatar' => array(
        'resource' => 'https://api.linkedin.com/v1/people/~:(id,first-name,picture-url)',
        'method post' => 0,
        'field' => 'picture-url',
        'querypath' => 0,
      ),
    ),
    'format' => 'xml',
  );

  $facebook = new stdClass;
  $facebook->name = '';
  $facebook->title = 'Facebook';
  $facebook->url = 'https://graph.facebook.com';
  $facebook->consumer_advanced = array(
    'oauth2' => 1,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => '',
    'request token endpoint' => '/oauth/request_token',
    'authorization scope' => 'email,read_stream',
    'authorization endpoint' => 'https://www.facebook.com/dialog/oauth',
    'access token endpoint' => '/oauth/access_token',
  );
  $facebook->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://graph.facebook.com/me',
        'method post' => 0,
        'field' => 'id',
        'querypath' => 0,
      ),
      'name' => array(
        'resource' => 'https://graph.facebook.com/me',
        'method post' => 0,
        'field' => 'name',
        'querypath' => 0,
      ),
      'avatar' => array(
        'resource' => '',
        'method post' => 0,
        'field' => '',
        'querypath' => 0,
      ),
    ),
    'format' => 'json',
  );


  $google = new stdClass;
  $google->name = '';
  $google->title = 'Google';
  $google->url = 'https://accounts.google.com';
  $google->consumer_advanced = array(
    'oauth2' => 1,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => 'authorization_code',
    'request token endpoint' => 'none',
    'authorization scope' => 'https://www.googleapis.com/auth/userinfo.profile https://www.googleapis.com/auth/userinfo.email',
    'authorization endpoint' => '/o/oauth2/auth',
    'access token endpoint' => '/o/oauth2/token',
  );
  $google->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'https://www.googleapis.com/oauth2/v1/userinfo',
        'method post' => 0,
        'field' => 'id',
        'querypath' => 0,
      ),
      'name' => array(
        'resource' => 'https://www.googleapis.com/oauth2/v1/userinfo',
        'method post' => 0,
        'field' => 'name',
        'querypath' => 0,
      ),
      'avatar' => array(
        'resource' => '',
        'method post' => 0,
        'field' => 'picture',
        'querypath' => 0,
      ),
    ),
    'format' => 'json',
  );

  $flickr = new stdClass();
  $flickr->name = '';
  $flickr->title = 'Flickr';
  $flickr->url = 'http://www.flickr.com';
  $flickr->consumer_advanced = array(
    'oauth2' => 0,
    'signature method' => 'HMAC-SHA1',
    'authentication realm' => '',
    'request token endpoint' => '/services/oauth/request_token',
    'authorization scope' => '',
    'authorization endpoint' => '/services/oauth/authorize',
    'access token endpoint' => '/services/oauth/access_token',
  );
  $flickr->mapping = array(
    'fields' => array(
      'uid' => array(
        'resource' => 'http://api.flickr.com/services/rest/?method=flickr.test.login&format=json&nojsoncallback=1',
        'method post' => 0,
        'field' => 'user.id',
        'querypath' => FALSE,
      ),
      'name' => array(
        'resource' => 'http://api.flickr.com/services/rest/?method=flickr.test.login&format=json&nojsoncallback=1',
        'method post' => 0,
        'field' => 'user.username._content',
        'querypath' => FALSE,
      ),
      'avatar' => array(
        'resource' => '',
        'method post' => 0,
        'field' => '',
        'querypath' => FALSE,
      ),
    ),
    'format' => 'json',
  );

  $presets = array(
    'twitter' => array (
      'data' => $twitter,
      'name' => 'Twitter',
    ),
    'linkedin' => array (
      'data' => $linkedin,
      'name' => 'LinkedIn',
    ),
    'facebook' => array (
      'data' => $facebook,
      'name' => 'Facebook',
    ),
    'google' => array (
      'data' => $google,
      'name' => 'Google',
    ),
    'flickr' => array (
      'data' => $flickr,
      'name' => 'Flickr',
    ),
  );
  drupal_alter('oauthconnector_presets', $presets);
  return $presets;
}
// add provider from preset form
function oauthconnector_presets_form($form, &$form_state) {
  $presets = oauthconnector_presets();
  $options = array();
  foreach ($presets as $key => $preset) {
    $options[$key] = $preset['name'];
  }

  $form['wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add provider from preset'),
    '#attributes' => array('class' => array('container-inline')),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['wrapper']['preset'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#title' => t('Preset'),
  );
  $form['wrapper']['actions'] = array(
    '#type' => 'actions',
  );
  $form['wrapper']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add provider'),
  );
  return $form;
}
// redirect to add form with preset
function oauthconnector_presets_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  drupal_goto('admin/structure/oauthconnector/add/' . $values['preset']);
}

// Add or edit a field.
function oauthconnector_field_form($form, &$form_state, $field_name = '') {

  $field = oauthconnector_fields($field_name);
  $form['old_name'] = array(
    '#type' => 'value',
    '#value' => $field_name,
  );
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#description' => t('A human-readable title for the field.'),
    '#size' => 32,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => isset($field['title'])?$field['title']:'',
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 24,
    '#default_value' => $field_name,
    '#title' => t('Name'),
    '#description' => t('A unique machine-readable name used to identify this provider internally. It may only contain lowercase alphanumeric characters and underscores. No spaces or uppercase characters.'),
    '#required' => TRUE,
    '#machine_name' => array(
      'exists' => 'oauthconnector_fields',
      'source' => array('title'),
    )
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('A description for this field.'),
    '#maxlength' => 255,
    '#rows' => 2,
    '#required' => FALSE,
    '#default_value' => isset($field['description'])?$field['description']:'',
  );
  $form['required'] = array(
    '#type' => 'checkbox',
    '#title' => t('Required'),
    '#required' => FALSE,
    '#default_value' => isset($field['required'])&&$field['required']?1:0,
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => (empty($field_name)?t('Add field'):t('Edit field')),
  );
  return $form;
}
// save the field.
function oauthconnector_field_form_submit($form, &$form_state) {
  drupal_set_message(t('Field saved.'));
  $values = $form_state['values'];
  oauthconnector_fields_save($values['name'], $values, $values['old_name']);
  drupal_goto('admin/structure/oauthconnector/fields');
}
// ask if we realy want to delete a field.
function oauthconnector_field_delete_form($form, &$form_state, $field_name = '') {

  $field = oauthconnector_fields($field_name);
  $form['name'] = array(
    '#type' => 'value',
    '#value' => $field_name,
  );
  $form['message'] = array(
    '#type' => 'markup',
    '#markup' => t('Are you sure you want to delete %name?', array('%name' => $field['title'])),
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete field'),
  );
  return $form;
}

// delete the field.
function oauthconnector_field_delete_form_submit($form, &$form_state) {
  drupal_set_message(t('Field deleted.'));
  $values = $form_state['values'];
  oauthconnector_fields_delete($values['name']);
  drupal_goto('admin/structure/oauthconnector/fields');
}

// devel page to test connections and endpoints.
function oauthconnector_devel($form, &$form_state) {

  // devel page.
  $values = array();
  if (isset($form_state['values'])) {
    $values = $form_state['values'];
  }

  if (empty($values['account'])) {
    global $user;
    $values['account'] = $user->name;
  }

  $form['endpoint'] = array(
    '#type' => 'textfield',
    '#title' => t('Endpoint'),
    '#size' => 64,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#default_value' => isset($values['endpoint'])?$values['endpoint']:'',
  );

  $form['account'] = array(
    '#type' => 'textfield',
    '#title' => t('User'),
    '#size' => 32,
    '#maxlength' => 60,
    '#autocomplete_path' => 'user/autocomplete',
    '#required' => TRUE,
    '#default_value' => $values['account'],
  );
  $providers = oauthconnector_provider_load_all();
  $options = array();
  foreach ($providers as $provider) {
    $options[$provider->name] = $provider->title;
  }
  $form['provider'] = array(
    '#type' => 'radios',
    '#title' => 'Provider',
    '#options' => $options,
    '#default_value' => isset($values['provider'])?$values['provider']:'',
  );

  $options = array(
    'json' => 'JSON',
    'xml' => 'XML',
    'raw' => 'RAW',
  );

  $form['format'] = array(
    '#type' => 'select',
    '#title' => 'Format',
    '#options' => $options,
    '#default_value' => isset($values['format'])?$values['format']:'',
  );
  $options = array(
    'get' => 'GET',
    'post' => 'POST',
  );

  $form['method'] = array(
    '#type' => 'select',
    '#title' => 'Method',
    '#options' => $options,
    '#default_value' => isset($values['method'])?$values['method']:'',
  );

  $form['actions'] = array(
    '#type' => 'actions',
  );


  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Run call'),
  );


  if (!empty($values['endpoint'])) {
    // get results
    $results = oauthconnector_endpoint_call_for_user($values['endpoint'], array(), $values['provider'], $values['account'], $values['method'], $values['format']);


    $form['results'] = array(
      '#type' => 'markup',
      '#markup' => dvr($results, TRUE),
      '#weight' => 255,
    );
  }

  return $form;
}
// we want to rebuild the form.
function oauthconnector_devel_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}
