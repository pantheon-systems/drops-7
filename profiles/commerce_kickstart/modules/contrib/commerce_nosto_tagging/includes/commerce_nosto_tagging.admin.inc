<?php

/**
 * @file
 * Administration pages for the module.
 */

/**
 * Creates the form for the admin page.
 *
 * @return array
 *   The form structure.
 */
function _commerce_nosto_tagging_admin() {
  $form = array();

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_SERVER_ADDRESS;
  $form[$name] = array(
    '#type' => 'textfield',
    '#title' => t('Nosto Marketing Automation Service Host') ,
    '#default_value' => variable_get($name, COMMERCE_NOSTO_TAGGING_SERVER_ADDRESS),
    '#description' => t('Typically there is no need to change this from the default connect.nosto.com'),
    '#required' => TRUE,
  );

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_ACCOUNT_NAME;
  $form[$name] = array(
    '#type' => 'textfield',
    '#title' => t('Nosto Account Name'),
    '#default_value' => variable_get($name, ''),
    '#description' => t('Your Nosto marketing automation service account name.'),
    '#required' => TRUE,
  );

  _commerce_nosto_tagging_admin_get_product_tagging_form($form);
  _commerce_nosto_tagging_admin_get_category_tagging_form($form);

  $form['#validate'][] = '_commerce_nosto_tagging_admin_validate';

  return system_settings_form($form);
}

/**
 * Creates the product tagging settings form.
 *
 * @param array $form
 *   The form structure.
 *
 * @see _commerce_nosto_tagging_admin()
 */
function _commerce_nosto_tagging_admin_get_product_tagging_form(&$form) {
  $form['fieldset_product_tagging'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product tagging settings'),
    '#collapsible' => TRUE,
    '#description' => t('These settings define which fields are used for product information in Nosto.'),
  );

  $options = _commerce_nosto_tagging_admin_get_field_options('node', 'commerce_product_reference');
  $options[0] = '<' . t('None') . '>';
  ksort($options);

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_PRODUCT_FIELD_ID;
  $form['fieldset_product_tagging'][$name] = array(
    '#type' => 'select',
    '#title' => t('Content type used for products'),
    '#options' => $options,
    '#default_value' => variable_get($name, 0),
    '#description' => t('The module will add hidden Nosto product tagging to these content types. The hidden tagging is used for sending product information to Nosto.'),
  );

  $form['fieldset_product_tagging']['fieldset_product_tagging_images'] = array(
    '#type' => 'fieldset',
    '#title' => t('Product image settings'),
    '#collapsible' => TRUE,
    '#description' => t('This setting controls which product image is used for showing Nosto recommendations. Nosto will create thumbnails automatically using this image. Note that you should configure only one of the following.'),
  );

  $options = _commerce_nosto_tagging_admin_get_field_options('commerce_product', 'image');
  $options[0] = '<' . t('None') . '>';
  ksort($options);
  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_PRODUCT_VARIATION_IMAGE_FIELD_ID;
  $form['fieldset_product_tagging']['fieldset_product_tagging_images'][$name] = array(
    '#type' => 'select',
    '#title' => t('Product variation image field'),
    '#options' => $options,
    '#default_value' => variable_get($name, 0),
    '#description' => t('Use this setting if you have configured your product images at the variation level. Default for Commerce Kickstart installations.'),
  );

  $options = _commerce_nosto_tagging_admin_get_field_options('node', 'image');
  $options[0] = '<' . t('None') . '>';
  ksort($options);
  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_CONTENT_TYPE_IMAGE_FIELD_ID;
  $form['fieldset_product_tagging']['fieldset_product_tagging_images'][$name] = array(
    '#type' => 'select',
    '#title' => t('Product content type image field'),
    '#options' => $options,
    '#default_value' => variable_get($name, 0),
    '#description' => t('Use this setting if you have configured your product images at the display level.'),
  );

  $options = _commerce_nosto_tagging_admin_get_field_options('node', 'taxonomy_term_reference');
  $options[0] = '<' . t('None') . '>';
  ksort($options);
  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_CATEGORY_FIELD_ID;
  $form['fieldset_product_tagging'][$name] = array(
    '#type' => 'select',
    '#title' => t('Product category field'),
    '#options' => $options,
    '#default_value' => variable_get($name, 0),
    '#description' => t('Field used for product categories in Nosto. Default for Commerce Kickstart installations is field_collection.'),
  );

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_BRAND_FIELD_ID;
  $form['fieldset_product_tagging'][$name] = array(
    '#type' => 'select',
    '#title' => t('Product brand field'),
    '#options' => $options,
    '#default_value' => variable_get($name, 0),
    '#description' => t('Field used for product brand in Nosto. Default for Commerce Kickstart installations is field_brand.'),
  );
}

/**
 * Creates the category tagging settings form.
 *
 * @param array $form
 *   The form structure.
 *
 * @see _commerce_nosto_tagging_admin()
 */
function _commerce_nosto_tagging_admin_get_category_tagging_form(&$form) {
  $form['fieldset_category_tagging'] = array(
    '#type' => 'fieldset',
    '#title' => t('Category tagging settings'),
    '#collapsible' => TRUE,
    '#description' => t('Settings to choose which vocabulary to use for product categories in Nosto'),
  );

  $options = _commerce_nosto_tagging_admin_get_vocabulary_options();
  $options[0] = '<' . t('None') . '>';
  ksort($options);
  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_CATEGORY_VOCABULARY_ID;
  $form['fieldset_category_tagging'][$name] = array(
    '#type' => 'select',
    '#title' => t('Category vocabulary'),
    '#options' => $options,
    '#default_value' => variable_get($name, 0),
    '#description' => t('The value should typically be the vocabulary that was chosen above as Product category field. Default for Commerce Kickstart installations is collection.'),
  );
}

/**
 * Validates form data from admin page.
 *
 * @param array $form
 *   Form structure.
 *
 * @param array $form_state
 *   The current form state data.
 */
function _commerce_nosto_tagging_admin_validate($form, $form_state) {
  $values = isset($form_state['values']) ? $form_state['values'] : array();

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_SERVER_ADDRESS;
  $server_address = isset($values[$name]) ? $values[$name] : 0;
  if (empty($server_address)) {
    form_set_error($name, t('Server address is required.'));
  }
  elseif (!valid_url($server_address)) {
    form_set_error($name, t('Server address is not a valid url.'));
  }
  elseif (preg_match('@^https?://@i', $server_address)) {
    form_set_error($name, t('Server address cannot include the protocol (http:// or https://).'));
  }

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_ACCOUNT_NAME;
  $account_name = isset($values[$name]) ? $values[$name] : 0;
  if (empty($account_name)) {
    form_set_error($name, t('Account name is required.'));
  }

  _commerce_nosto_tagging_admin_validate_product_tagging_settings($form, $form_state);
  _commerce_nosto_tagging_admin_validate_category_tagging_settings($form, $form_state);
}

/**
 * Validates form data for the product tagging settings.
 *
 * @param array $form
 *   Form structure.
 *
 * @param array $form_state
 *   The current form state data.
 *
 * @see _commerce_nosto_tagging_admin_validate()
 */
function _commerce_nosto_tagging_admin_validate_product_tagging_settings($form, $form_state) {
  $values = isset($form_state['values']) ? $form_state['values'] : array();

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_PRODUCT_FIELD_ID;
  $field_id = isset($values[$name]) ? $values[$name] : 0;
  if (!_commerce_nosto_tagging_admin_validate_field_id($field_id, 'commerce_product_reference')) {
    form_set_error($name, t('Invalid product field selection.'));
  }
  elseif ((int) $field_id === 0) {
    drupal_set_message(t('Products will not be tagged. It is highly discouraged to leave the product field blank.'), 'warning');
  }

  $variation_field_name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_PRODUCT_VARIATION_IMAGE_FIELD_ID;
  $variation_field_id = isset($values[$variation_field_name]) ? $values[$variation_field_name] : 0;
  $content_type_field_name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_CONTENT_TYPE_IMAGE_FIELD_ID;
  $content_type_field_id = isset($values[$content_type_field_name]) ? $values[$content_type_field_name] : 0;
  if ((int) $variation_field_id > 0 && (int) $content_type_field_id > 0) {
    form_set_error($variation_field_name, t('You cannot configure the product image field on the variation level while also having configured it on the display level. You should configure either or, depending on your setup.'));
    form_set_error($content_type_field_name, t('You cannot configure the product image field on the display level while also having configured it on the variation level. You should configure either or, depending on your setup.'));
  }
  elseif ((int) $variation_field_id === 0 && (int) $content_type_field_id === 0) {
    drupal_set_message(t('Product image url will not be included in the product tagging.'), 'warning');
  }
  else {
    if (!_commerce_nosto_tagging_admin_validate_field_id($variation_field_id, 'image')) {
      form_set_error($variation_field_name, t('Invalid product variation image field selection.'));
    }
    if (!_commerce_nosto_tagging_admin_validate_field_id($content_type_field_id, 'image')) {
      form_set_error($content_type_field_name, t('Invalid product content type image field selection.'));
    }
  }

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_CATEGORY_FIELD_ID;
  $field_id = isset($values[$name]) ? $values[$name] : 0;
  if (!_commerce_nosto_tagging_admin_validate_field_id($field_id, 'taxonomy_term_reference')) {
    form_set_error($name, t('Invalid product category field selection.'));
  }
  elseif ((int) $field_id === 0) {
    drupal_set_message(t('Product categories will not be included in the product tagging.'), 'warning');
  }

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_BRAND_FIELD_ID;
  $field_id = isset($values[$name]) ? $values[$name] : 0;
  if (!_commerce_nosto_tagging_admin_validate_field_id($field_id, 'taxonomy_term_reference')) {
    form_set_error($name, t('Invalid product brand field selection.'));
  }
  elseif ((int) $field_id === 0) {
    drupal_set_message(t('Product brand name will not be included in the product tagging.'), 'warning');
  }
}

/**
 * Validates form data for the category tagging settings.
 *
 * @param array $form
 *   Form structure.
 *
 * @param array $form_state
 *   The current form state data.
 *
 * @see _commerce_nosto_tagging_admin_validate()
 */
function _commerce_nosto_tagging_admin_validate_category_tagging_settings($form, $form_state) {
  $values = isset($form_state['values']) ? $form_state['values'] : array();

  $name = COMMERCE_NOSTO_TAGGING_CONFIG_KEY_CATEGORY_VOCABULARY_ID;
  $vid = isset($values[$name]) ? $values[$name] : 0;
  if (!ctype_digit((string) $vid)) {
    form_set_error($name, t('Invalid category vocabulary.'));
  }
  elseif ($vid > 0) {
    $vocabulary = taxonomy_vocabulary_load($vid);
    if (!$vocabulary) {
      form_set_error($name, t('Invalid category vocabulary.'));
    }
  }
  else {
    drupal_set_message(t('Categories will not be tagged. It is highly discouraged to leave the category vocabulary blank.'), 'warning');
  }
}

/**
 * Validation for field ids.
 *
 * Checks that the id is number and that the field info
 * for the id can be loaded. If a type is provided, then
 * the loaded field info must be of that type.
 *
 * @param int $field_id
 *   The field id to validate.
 *
 * @param null|string $field_type
 *   The type of the field the id should reference.
 *
 * @return bool
 *   If validation passed or not.
 *
 * @see _commerce_nosto_tagging_admin_validate()
 */
function _commerce_nosto_tagging_admin_validate_field_id($field_id, $field_type = NULL) {
  if (!ctype_digit((string) $field_id)) {
    return FALSE;
  }
  elseif ($field_id > 0) {
    $field_info = field_info_field_by_id($field_id);
    if ($field_type !== NULL && (!isset($field_info['type']) || $field_info['type'] !== $field_type)) {
      return FALSE;
    }
    elseif (!isset($field_info['id'])) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
 * Returns list of field options for entity.
 *
 * @param string $entity_type
 *   The entity type to fetch the fields for.
 *
 * @param string $field_type
 *   The type of fields to include, defaults to all.
 *
 * @return array
 *   Associative array of field id to name.
 *
 * @see _commerce_nosto_tagging_admin()
 */
function _commerce_nosto_tagging_admin_get_field_options($entity_type, $field_type = NULL) {
  $options = array();

  switch ($entity_type) {
    case 'node':
      $node_types = node_type_get_types();
      $types = array_keys($node_types);
      break;

    case 'commerce_product':
      $product_types = commerce_product_types();
      $types = array_keys($product_types);
      break;

    default:
      $types = array();
      break;
  }

  if (!empty($types)) {
    foreach ($types as $type) {
      foreach (field_info_instances($entity_type, $type) as $field_name => $value) {
        $field_info = field_info_field($field_name);
        if ($field_type === NULL || $field_info['type'] === $field_type) {
          if (!isset($options[$field_info['id']])) {
            $options[$field_info['id']] = array(
              'name' => $field_name,
              'labels' => array(),
            );
          }
          $options[$field_info['id']]['labels'][] = $value['label'];
        }
      }
    }
  }

  foreach ($options as $id => $data) {
    $options[(int) $id] = $data['name'] . ' (' . implode(', ', array_unique($data['labels'])) . ')';
  }

  return $options;
}

/**
 * Returns a list of vocabulary options.
 *
 * @return array
 *   Associative array of vocabulary vid to name.
 *
 * @see _commerce_nosto_tagging_admin()
 */
function _commerce_nosto_tagging_admin_get_vocabulary_options() {
  $options = array();

  $vocabularies = taxonomy_get_vocabularies();
  foreach ($vocabularies as $vocabulary) {
    $options[(int) $vocabulary->vid] = $vocabulary->machine_name . ' (' . $vocabulary->name . ')';
  }

  return $options;
}
