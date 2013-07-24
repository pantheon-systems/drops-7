<?php

/**
 * @file
 * Commerce discount editing UI.
 */

/**
 * UI controller.
 */
class CommerceDiscountUIController extends EntityDefaultUIController {

  /**
   * Overridden to customize the field location.
   */
  public function entityFormSubmitBuildEntity($form, &$form_state) {
    // We cannot use entity_form_submit_build_entity() any more.
    $entity = $form_state['commerce_discount'];

    // Extract form values.
    form_state_values_clean($form_state);
    foreach ($form_state['values'] as $key => $value) {
      if ($key != 'commerce_discount_fields') {
        $entity->$key = $value;
      }
      else {
        $discount_offer = $value['commerce_discount_offer'][LANGUAGE_NONE]['form'];
        unset($discount_offer['actions']);
        $entity->commerce_discount_offer['commerce_discount_fields'] = $discount_offer;
      }
    }
    // Invoke all specified builders for copying form values to entity
    // properties.
    // @see entity_form_submit_build_entity()
    if (isset($form['#entity_builders'])) {
      foreach ($form['#entity_builders'] as $function) {
        $function('commerce_discount', $entity, $form, $form_state);
      }
    }
    field_attach_submit('commerce_discount', $entity, $form['commerce_discount_fields'], $form_state);
    return $entity;
  }

  /**
   * Overriden EntityDefaultUIController::hook_menu().
   *
   * Call our own page callback to show the "commerce_discount_overview"
   * view, as this view has exposed filters which will not work inside
   * Entity API's default form.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path] = array(
      'title' => 'Discounts',
      'description' => 'Manage discounts.',
      'page callback' => 'commerce_discount_overview_form',
      'access callback' => 'entity_access',
      'access arguments' => array('view', $this->entityType),
      'file' => 'includes/commerce_discount.admin.inc',
      'file path' => drupal_get_path('module', 'commerce_discount'),
    );
    $items[$this->path . '/list'] = array(
      'title' => 'Discounts',
      'type' => MENU_DEFAULT_LOCAL_TASK,
    );
    $items[$this->path . '/add']['title callback'] = 'commerce_discount_get_action_title';
    $items[$this->path . '/import']['title callback'] = 'commerce_discount_get_action_title';

    $items[$this->path . '/settings'] = array(
      'title' => 'Settings',
      'description' => 'Additional settings for Commerce Discounts.',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('commerce_discount_settings'),
      'access arguments' => array('administer commerce discounts'),
      'type' => MENU_LOCAL_TASK,
      'file' => 'includes/commerce_discount.admin.inc',
      'file path' => drupal_get_path('module', 'commerce_discount'),
    );
    return $items;
  }
}

/**
 * Builds the entity overview form.
 */
function commerce_discount_overview_form() {
  $view = views_get_view('commerce_discount_overview');
  $view->override_url = $_GET['q'];
  $form['view'] = array('#markup' => $view->preview());
  $form['#attached']['css'][] = drupal_get_path('module', 'commerce_discount') . '/css/commerce_discount.css';
  // Load rtl.css file if needed.
  if ($GLOBALS['language']->direction == 1) {
    $form['#attached']['css'][] = drupal_get_path('module', 'commerce_discount') . '/css/commerce_discount-rtl.css';
  }
  return $form;
}

/**
 * Gets the page/menu title for local action operations.
 */
function commerce_discount_get_action_title($op, $entity_type) {
  switch ($op) {
    case 'add':
      return t('Add discount');
    case 'import':
      return t('Import discount');
  }
}

/**
 * Form callback: create or edit a discount.
 *
 * @param $discount
 *   The discount type array to edit or for a create form an empty discount
 *   type array with properties instantiated but not populated.
 */
function commerce_discount_form($form, &$form_state, CommerceDiscount $commerce_discount, $op = 'edit') {
  // We might have gotten the commerce discount type via ajax, so set it
  // in the commerce discount entity.
  if (!empty($form_state['values']['commerce_discount_type'])) {
    $commerce_discount->type = $form_state['values']['commerce_discount_type'];
  }

  $options = array();
  foreach (commerce_discount_types() as $type => $info) {
    $options[$type] = $info['label'];
  }

  $form['label'] = array(
    '#title' => t('Admin title'),
    '#type' => 'textfield',
    '#default_value' => $commerce_discount->label,
    '#description' => t('Shown only on managment screens, not shown to customers.'),
    '#required' => TRUE,
  );

  $form['component_title'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $commerce_discount->component_title,
    '#description' => t('Shown to customers.'),
  );

  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    // Strip the 'discount_' prefix from the beginning of the string
    '#default_value' => isset($commerce_discount->name) ? substr($commerce_discount->name, 9) : '',
    '#disabled' => $commerce_discount->hasStatus(ENTITY_IN_CODE),
    '#machine_name' => array(
      'exists' => '_commerce_discount_name_exists',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this message type. It must only contain lowercase letters, numbers, and underscores.'),
    // 32 characters minus the 'discount_' prefix.
    '#maxlength' => 23,
    // This field should stay LTR even for RTL languages.
    '#field_prefix' => '<span dir="ltr">discount_',
    '#field_suffix' => '</span>&lrm;',
  );

  $form['enabled'] = array(
    '#title' => t('Discount status'),
    '#type' => 'select',
    '#options' => array(
      TRUE => t('Enabled'),
      FALSE => t('Disabled'),
    ),
    '#required' => FALSE,
    '#default_value' => $commerce_discount->enabled,
  );

  // Show a list of commerce discounts.
  $form['commerce_discount_type'] = array(
    '#title' => t('Choose discount type'),
    '#type' => 'radios',
    '#options' => $options,
    '#required' => FALSE,
    '#default_value' => $commerce_discount->type,
    '#ajax' => array(
       'callback' => 'commerce_discount_fields_ajax_callback',
       'wrapper' => 'commerce-discount-fields-wrapper',
    ),
  );

  $form['commerce_discount_fields'] = array(
    '#prefix' => '<div id="commerce-discount-fields-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#parents' => array('commerce_discount_fields'),
  );
  field_attach_form('commerce_discount', $commerce_discount, $form['commerce_discount_fields'], $form_state);
  // Remove the title and surrounding fieldset from the offer reference field.
  $form['commerce_discount_fields']['commerce_discount_offer'][LANGUAGE_NONE]['#title'] = NULL;
  $form['commerce_discount_fields']['commerce_discount_offer'][LANGUAGE_NONE]['#type'] = 'container';

  // Add a class to custom fields for easier styling.
  // Ignore the offer reference field and any date fields. Date fields are
  // ignored because their markup makes them hard to style generically.
  foreach (element_get_visible_children($form['commerce_discount_fields']) as $field_name) {
    $field = field_info_field($field_name);
    if ($field_name == 'commerce_discount_offer' || in_array($field['type'], array('date', 'datestamp', 'datetime'))) {
      continue;
    }

    $field_form = &$form['commerce_discount_fields'][$field_name];
    if (!isset($field_form['#attributes']['class'])) {
      $field_form['#attributes']['class'] = array();
    }
    $field_form['#attributes']['class'][] = 'commerce-discount-box';
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save discount'),
    '#weight' => 40,
    '#ief_submit_all' => TRUE,
  );

  if (!$commerce_discount->hasStatus(ENTITY_IN_CODE) && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete discount'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('commerce_discount_form_submit_delete')
    );
  }

  // Add assets.
  $form['#attached']['js'][] = drupal_get_path('module', 'commerce_discount') . '/js/commerce_discount.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'commerce_discount') . '/css/commerce_discount.css';
  // Load rtl.css file if needed.
  if ($GLOBALS['language']->direction == 1) {
    $form['#attached']['css'][] = drupal_get_path('module', 'commerce_discount') . '/css/commerce_discount-rtl.css';
  }
  $form['#attributes']['class'][] = 'commerce-discount-form';
  return $form;
}

/**
 * AJAX callback to attach the commerce discount fields to the form.
 *
 * Since the controlling logic for populating the form is in the form builder
 * function, all we do here is select the element and return it to be updated.
 */
function commerce_discount_fields_ajax_callback($form, &$form_state) {
  return $form['commerce_discount_fields'];
}

/**
 * Form API validation callback for the type form.
 */
function commerce_discount_form_validate($form, &$form_state) {
  field_attach_form_validate('commerce_discount', $form_state['commerce_discount'], $form['commerce_discount_fields'], $form_state);

  if (!empty($form_state['values']['name'])) {
    form_set_value($form['name'], 'discount_' . $form_state['values']['name'], $form_state);
  }
}

/**
 * Form API submit callback for the type form.
 */
function commerce_discount_form_submit(&$form, &$form_state) {
  $commerce_discount = entity_ui_form_submit_build_entity($form, $form_state);
  $commerce_discount->save();
  $form_state['redirect'] = 'admin/commerce/store/discounts';
}

/**
 * Form API submit callback for the delete button.
 */
function commerce_discount_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/commerce/store/discounts/manage/' . $form_state['commerce_discount']->name . '/delete';
}

/**
 * Checks if a Discount machine name is taken.
 *
 * @param $value
 *   The machine name, not prefixed with 'discount_'.
 *
 * @return
 *   Whether or not the field machine name is taken.
 */
function _commerce_discount_name_exists($value) {
  // Prefix with 'discount_'.
  $name = 'discount_' . $value;

  return entity_load_single('commerce_discount', $name);
}

/**
 * Builds the discount settings form.
 */
function commerce_discount_settings() {
  $types = commerce_line_item_type_get_name();
  unset($types['commerce_discount']);
  $form['commerce_discount_line_item_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Line item types to use for discounts'),
    '#default_value' => variable_get('commerce_discount_line_item_types', array('product')),
    '#options' => $types,
    '#description' => t('Select the line item types that will be taken into account for calculating the discount amounts in percentage offers.'),
  );
  return system_settings_form($form);
}
