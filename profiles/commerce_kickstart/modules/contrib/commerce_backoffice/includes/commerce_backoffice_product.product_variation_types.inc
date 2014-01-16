<?php

/**
 * @file
 * Product variation specific copy of
 * commerce_product_ui/includes/commerce_product_ui.types.inc
 * and
 * commerce_product_ui/includes/commerce_product_ui.forms.inc.
 */

/**
 * Menu callback: display an overview of available types.
 */
function commerce_backoffice_product_variation_types_overview() {
  drupal_add_css(drupal_get_path('module', 'commerce_product') . '/theme/commerce_product.admin.css');

  $header = array(
    t('Name'),
    t('Operations'),
  );

  $rows = array();

  // Loop through all defined product variation types.
  foreach (commerce_product_types() as $type => $product_type) {
    // Build the operation links for the current product variation type.
    $links = menu_contextual_links('commerce-product-type', 'admin/commerce/config/product-variation-types', array(strtr($type, array('_' => '-'))));

    $description = check_plain($product_type['name']);
    $description .= ' <small>' . t('(Machine name: @type)', array('@type' => $product_type['type'])) . '</small>';

    // Add the product variation type's row to the table's rows array.
    $rows[] = array(
      $description,
      theme('links', array('links' => $links, 'attributes' => array('class' => 'links inline operations'))),
    );
  }

  // If no product variation types are defined...
  if (empty($rows)) {
    // Add a standard empty row with a link to add a new product variation type.
    $rows[] = array(
      array(
        'data' => t('There are no product variation types yet. <a href="@link">Add product variation type</a>.', array('@link' => url('admin/commerce/config/product-variation-types/add'))),
        'colspan' => 2,
      )
    );
  }

  $build = array();
  $args = array(
    '!display_type' => l(t('product display type'), 'admin/structure/types'),
  );
  $build['help'] = array(
    '#markup' => '<p>' . t('Each product display must have one or more product variations,
                            which is why each !display_type has a matching product variation type.', $args) .
                 '</p>' .
                 '<p>' . t('Different product variation types have different fields, used for
                            storing images, attributes such as color and size, as well as any other information.') .
                 '</p>',
  );
  $build['type_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );


  return $build;
}

/**
 * Form callback wrapper: create or edit a product variation type.
 *
 * @param $type
 *   The machine-name of the product variation type being created or edited by
 *   this form or a full product variation type array.
 */
function commerce_backoffice_product_variation_type_form_wrapper($type) {
  if (!is_array($type)) {
    $type = commerce_product_type_load($type);
  }

  // Return a message if the product variation type is not governed by Product UI.
  if (!empty($type['type']) && $type['module'] != 'commerce_product_ui') {
    return t('This product variation type cannot be edited, because it is not defined by the Product UI module.');
  }

  // Include the forms file from the Product module.
  module_load_include('inc', 'commerce_product_ui', 'includes/commerce_product_ui.forms');

  return drupal_get_form('commerce_product_ui_product_type_form', $type);
}

/**
 * Implements hook_form_FORM_ID_alter().
 */
function commerce_backoffice_product_form_commerce_product_ui_product_type_form_alter(&$form, &$form_state) {
  $product_type = $form_state['product_type'];

  // Remove fields not used by the Inline Entity Form.
  $form['product_type']['description']['#access'] = FALSE;
  $form['product_type']['help']['#access'] = FALSE;

  // Tweak the descriptions to mention product variation types.
  $form['product_type']['name']['#description'] = t('The human-readable name of this product variation type. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.');

  if (empty($product_type['type'])) {
    $form['product_type']['type']['#description'] = t('The machine-readable name of this product variation type. This name must contain only lowercase letters, numbers, and underscores, it must be unique.');
  }

  $form['product_type']['revision']['#title'] = t('Default product variations of this type to be saved as new revisions when edited.');

  // Revisit this once Inline Entity Form gets entity_translation support.
  // If this variable is needed, it should probably reflect the
  // language_content_type_$type variable.
  if (module_exists('entity_translation')) {
    $form['product_type']['multilingual']['#type'] = 'value';
    $form['product_type']['multilingual']['#value'] = 0;
  }

  if (empty($product_type['type'])) {
    $form['create_product_display'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create matching product display type'),
      '#default_value' => TRUE,
    );
  }

  $submit = $form['#submit'] = array();

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save product variation type'),
    '#submit' => array_merge($submit, array('commerce_backoffice_product_variation_type_form_submit')),
  );

  if (!empty($form_state['product_type']['type'])) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete product variation type'),
      '#suffix' => l(t('Cancel'), 'admin/commerce/config/product-variation-types'),
      '#submit' => array_merge($submit, array('commerce_backoffice_product_variation_type_form_delete_submit')),
      '#weight' => 45,
    );
  }
  else {
    $form['actions']['save_continue'] = array(
      '#type' => 'submit',
      '#value' => t('Save and add fields'),
      '#suffix' => l(t('Cancel'), 'admin/commerce/config/product-variation-types'),
      '#submit' => array_merge($submit, array('commerce_backoffice_product_variation_type_form_submit')),
      '#weight' => 45,
    );
  }

  return $form;
}

/**
 * Form submit handler: save a product variation type.
 */
function commerce_backoffice_product_variation_type_form_submit($form, &$form_state) {
  commerce_product_ui_product_type_form_submit($form, $form_state);
  if (!empty($form_state['product_type']['type'])) {
    $type_name = $form_state['product_type']['type'];
  }
  else {
    $type_name = $form_state['values']['product_type']['type'];
  }
  $type_name = trim($type_name);
  $human_name = trim($form_state['values']['product_type']['name']);
  if (!empty($form_state['values']['create_product_display']) && !node_type_load($type_name)) {
    // Create the product display type.
    $type = node_type_set_defaults();
    $type->type = $type_name;
    $type->name = $human_name;
    $type->custom = TRUE;
    $type->base = 'node_content';
    $status = node_type_save($type);
    // node_type_save doesn't invoke this.
    field_attach_create_bundle('node', $type_name);
    // Don't display author information
    variable_set('node_submitted_' . $type_name, 0);
    // Remove promoted to front page.
    variable_set('node_options_' . $type_name, array('status'));
    $entity_info = entity_get_info('node');
    // Update the weight of the "title" field.
    if (!empty($entity_info['field replacement'])) {
      if ($instance = field_info_instance('node', 'title_field', $type_name)) {
        if (isset($instance['display'])) {
          foreach ($instance['display'] as $view_mode_key => $view_mode_settings) {
            $instance['display'][$view_mode_key]['weight'] = -20;
            $instance['display'][$view_mode_key]['type'] = 'title_linked';
            $instance['display'][$view_mode_key]['label'] = 'hidden';
            $instance['display'][$view_mode_key]['module'] = 'title';
            $instance['display'][$view_mode_key]['settings']['title_link'] = 'content';
          }
        }
        field_update_instance($instance);
      }
    }

    // Create the product reference field.
    $field = field_info_field('field_product');
    if (!$field) {
      $field = array(
        'field_name' => 'field_product',
        'type' => 'commerce_product_reference',
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'entity_types' => array('node'),
        'translatable' => FALSE,
        'locked' => FALSE,
      );
      $field = field_create_field($field);
    }

    $instance = array(
      'field_name' => 'field_product',
      'entity_type' => 'node',
      'bundle' => $type_name,
      'label' => 'Product variations',
      'required' => TRUE,
      'settings' => array(
        'field_injection' => 1,
        'referenceable_types' => array(
          $type_name => $type_name,
        ),
        'user_register_form' => FALSE,
      ),
      'widget' => array(
        'active' => 1,
        'module' => 'inline_entity_form',
        'settings' => array(
          'fields' => array(),
          'type_settings' => array(
            'allow_existing' => 0,
            'autogenerate_title' => 1,
            'delete_references' => 1,
            'match_operator' => 'CONTAINS',
            'use_variation_language' => 1,
          ),
        ),
        'type' => 'inline_entity_form',
      ),
      'display' => array(
        'default' => array(
          'label' => 'above',
          'module' => 'commerce_cart',
          'settings' => array(
            'attributes_single' => 1,
            'combine' => 1,
            'default_quantity' => '1',
            'line_item_type' => 'product',
            'show_quantity' => 1,
            'show_single_product_attributes' => FALSE,
          ),
          'type' => 'commerce_cart_add_to_cart_form',
          'weight' => '3',
        ),
        'full' => array(
          'label' => 'hidden',
          'module' => 'commerce_cart',
          'settings' => array(
            'combine' => 1,
            'default_quantity' => '1',
            'line_item_type' => 'product',
            'show_quantity' => 1,
            'show_single_product_attributes' => FALSE,
          ),
          'type' => 'commerce_cart_add_to_cart_form',
          'weight' => '5',
        ),
      ),
    );
    field_create_instance($instance);
  }

  drupal_get_messages('status', TRUE);
  drupal_set_message(t('Product variation type %type saved.', array('%type' => $human_name)));

  if ($form_state['triggering_element']['#parents'][0] == 'save_continue') {
    $form_state['redirect'] = 'admin/commerce/config/product-variation-types/' . strtr($type_name, '_', '-') . '/fields';
  }
  else {
    $form_state['redirect'] = 'admin/commerce/config/product-variation-types';
  }
}

/**
 * Submit callback for delete button on commerce_product_ui_product_type_form().
 *
 * @see commerce_product_ui_product_type_form()
 */
function commerce_backoffice_product_variation_type_form_delete_submit($form, &$form_state) {
  $form_state['redirect'] = 'admin/commerce/config/product-variation-types/' . strtr($form_state['product_type']['type'], '_', '-') . '/delete';
}

/**
 * Form callback wrapper: confirmation form for deleting a product variation type.
 *
 * @param $type
 *   The machine-name of the product variation type being created or edited by
 *   this form or a full product variation type array.
 *
 * @see commerce_product_product_type_delete_form()
 */
function commerce_backoffice_product_variation_type_delete_form_wrapper($type) {
  if (is_array($type)) {
    $product_type = $type;
  }
  else {
    $product_type = commerce_product_type_load($type);
  }

  // Return a message if the product variation type is not governed by Product UI.
  if ($product_type['module'] != 'commerce_product_ui') {
    return t('This product variation type cannot be deleted, because it is not defined by the Product UI module.');
  }

  // Don't allow deletion of product variation types that have products already.
  $query = new EntityFieldQuery();

  $query->entityCondition('entity_type', 'commerce_product', '=')
    ->entityCondition('bundle', $product_type['type'], '=')
    ->count();

  $count = $query->execute();

  if ($count > 0) {
    drupal_set_title(t('Cannot delete the %name product variation type', array('%name' => $product_type['name'])), PASS_THROUGH);

    return format_plural($count,
      'There is 1 product variation of this type. It cannot be deleted.',
      'There are @count product variations of this type. It cannot be deleted.'
    );
  }

  return drupal_get_form('commerce_backoffice_product_variation_type_delete_form', $product_type);
}

/**
 * Form callback: confirmation form for deleting a product variation type.
 *
 * @param $product_type
 *   The product variation type array to be deleted.
 *
 * @see confirm_form()
 */
function commerce_backoffice_product_variation_type_delete_form($form, &$form_state, $product_type) {
  $form_state['product_type'] = $product_type;

  $form['#submit'][] = 'commerce_backoffice_product_variation_type_delete_form_submit';

  $form = confirm_form($form,
    t('Are you sure you want to delete the %name product variation type?', array('%name' => $product_type['name'])),
    'admin/commerce/config/product-variation-types',
    '<p>' . t('This action cannot be undone.') . '</p>',
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for commerce_product_product_type_delete_form().
 */
function commerce_backoffice_product_variation_type_delete_form_submit($form, &$form_state) {
  $product_type = $form_state['product_type'];

  commerce_product_ui_product_type_delete($product_type['type']);

  drupal_set_message(t('The product variation type %name has been deleted.', array('%name' => $product_type['name'])));
  watchdog('commerce_product', 'Deleted product variation type %name.', array('%name' => $product_type['name']), WATCHDOG_NOTICE);

  $form_state['redirect'] = 'admin/commerce/config/product-variation-types';
}

