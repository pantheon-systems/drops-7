<?php

/**
 * @file
 * Commerce discount editing UI.
 */

/**
 * Class CommerceDiscountUIController.
 */
class CommerceDiscountUIController extends EntityDefaultUIController {

  /**
   * {@inheritdoc}
   */
  public function entityFormSubmitBuildEntity($form, &$form_state) {
    // We cannot use entity_form_submit_build_entity() any more.
    $entity = $form_state['commerce_discount'];

    // Extract form values.
    form_state_values_clean($form_state);

    foreach ($form_state['values'] as $key => $value) {
      if ($key != 'commerce_discount_fields') {
        $entity->{$key} = $value;
      }
      else {
        $discount_offer = $value['commerce_discount_offer'][LANGUAGE_NONE]['form'];
        unset($discount_offer['actions']);
        $entity->commerce_discount_offer['commerce_discount_fields'] = $discount_offer;
      }
    }

    // Invoke all specified builders for copying values to entity properties.
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
      'title' => 'List',
      'type' => MENU_DEFAULT_LOCAL_TASK,
    );
    unset($items[$this->path . '/add']['title callback']);
    unset($items[$this->path . '/import']['title callback']);
    $items[$this->path . '/add']['title'] = t('Add discount');
    $items[$this->path . '/import']['title'] = t('Import discount');

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

    if (module_exists('rules_admin')) {
      $items[$this->path . '/rules'] = array(
        'title' => 'Discount rules',
        'description' => 'Manage discount rules',
        'page callback' => 'drupal_goto',
        'page arguments' => array(
          'admin/config/workflow/rules',
          array(
            'query' => array(
              'event' => 0,
              'tag' => 'Commerce Discount',
            ),
          ),
        ),
        'access arguments' => array('administer commerce discounts'),
        'type' => MENU_LOCAL_TASK,
        'weight' => 10,
      );
    }

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

  return $form;
}

/**
 * Form callback: create or edit a discount.
 *
 * @param array $form
 *   Standard Drupal $form.
 * @param array $form_state
 *   Standard Drupal $form_state.
 * @param CommerceDiscount $commerce_discount
 *   Comment me.
 * @param string $op
 *   Current operation.
 *
 * @return array
 *   A renderable form build array.
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
    '#description' => t('Shown only on management screens, not shown to customers.'),
    '#required' => TRUE,
  );

  $form['component_title'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => empty($commerce_discount->component_title) ? t('Discount') : $commerce_discount->component_title,
    '#description' => t('Shown to customers. Defaults to "Discount".'),
  );

  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    // Strip the 'discount_' prefix from the beginning of the string.
    '#default_value' => isset($commerce_discount->name) ? substr($commerce_discount->name, 9) : '',
    '#disabled' => $commerce_discount->hasStatus(ENTITY_IN_CODE),
    '#machine_name' => array(
      'exists' => '_commerce_discount_name_exists',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this message type. It must only contain lowercase letters, numbers, and underscores.'),
    // 64 characters minus the 9 character 'discount_' prefix.
    '#maxlength' => 64 - 9,
    // This field should stay LTR even for RTL languages.
    '#field_prefix' => '<span dir="ltr">discount_',
    '#field_suffix' => '</span>&lrm;',
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
    '#pre_render' => array('commerce_discount_form_pre_render'),
  );
  // Vertical tabs container.
  $form['commerce_discount_fields']['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
    'discount_options' => array(
      '#type' => 'fieldset',
      '#title' => t('Discount options'),
      '#collapsible' => TRUE,
      '#weight' => -10,
    ),
    'commerce_discount_compatibility' => array(
      '#type' => 'fieldset',
      '#title' => t('Discount compatibility'),
      '#collapsible' => TRUE,
    ),
  );

  // Add radios for enabling or disabling this discount.
  $form['commerce_discount_fields']['additional_settings']['discount_options']['status'] = array(
    '#title' => t('Discount status'),
    '#type' => 'radios',
    '#parents' => array('status'),
    '#options' => array(
      TRUE => t('Active'),
      FALSE => t('Disabled'),
    ),
    '#required' => FALSE,
    '#default_value' => $commerce_discount->status,
  );

  // Add the sort order select list.
  $form['commerce_discount_fields']['additional_settings']['discount_options']['sort_order'] = array(
    '#type' => 'select',
    '#parents' => array('sort_order'),
    '#title' => t('Sort order'),
    '#description' => t('Discounts will be sorted by this value to be evaluated in that order.'),
    '#options' => drupal_map_assoc(range(1, 21)),
    '#default_value' => !empty($commerce_discount->sort_order) ? $commerce_discount->sort_order : 10,
  );

  field_attach_form('commerce_discount', $commerce_discount, $form['commerce_discount_fields'], $form_state);
  // Remove the title and surrounding fieldset from the offer reference field.
  $form['commerce_discount_fields']['commerce_discount_offer'][LANGUAGE_NONE]['#title'] = NULL;
  $form['commerce_discount_fields']['commerce_discount_offer'][LANGUAGE_NONE]['#type'] = 'container';

  // Add a class to custom fields for easier styling.
  // Ignore the offer reference field and any date fields. Date fields are
  // ignored because their markup makes them hard to style generically.
  $date_field_types = array('date', 'datestamp', 'datetime');
  foreach (element_get_visible_children($form['commerce_discount_fields']) as $field_name) {
    $field = field_info_field($field_name);
    if ($field_name == 'commerce_discount_offer' || in_array($field['type'], $date_field_types)) {
      continue;
    }

    $field_form = &$form['commerce_discount_fields'][$field_name];
    if (!isset($field_form['#attributes']['class'])) {
      $field_form['#attributes']['class'] = array();
    }
    $field_form['#attributes']['class'][] = 'commerce-discount-box';
  }

  // Ensure the compatibility strategy field has a default value.
  if (empty($form['commerce_discount_fields']['commerce_compatibility_strategy'][LANGUAGE_NONE]['#default_value'])) {
    $form['commerce_discount_fields']['commerce_compatibility_strategy'][LANGUAGE_NONE]['#default_value'] = 'any';
  }

  // Add visibility states to the discount compatibility selection field.
  $form['commerce_discount_fields']['commerce_compatibility_selection']['#states'] = array(
    'visible' => array(
      ':input[name="commerce_discount_fields[commerce_compatibility_strategy][' . LANGUAGE_NONE . ']"]' => array(
        array('value' => 'except'),
        array('value' => 'only'),
      ),
    ),
  );

  // We have to do some extra adjustments to move compatibility fields into
  // the vertical tabs.
  $form['commerce_discount_fields']['additional_settings']['commerce_discount_compatibility']['commerce_compatibility_strategy'] = $form['commerce_discount_fields']['commerce_compatibility_strategy'];
  $form['commerce_discount_fields']['additional_settings']['commerce_discount_compatibility']['commerce_compatibility_strategy']['#parents'] = array(
    'commerce_discount_fields', 'commerce_compatibility_strategy',
  );
  $form['commerce_discount_fields']['additional_settings']['commerce_discount_compatibility']['commerce_compatibility_selection'] = $form['commerce_discount_fields']['commerce_compatibility_selection'];
  $form['commerce_discount_fields']['additional_settings']['commerce_discount_compatibility']['commerce_compatibility_selection']['#parents'] = array(
    'commerce_discount_fields', 'commerce_compatibility_selection',
  );
  unset($form['commerce_discount_fields']['commerce_compatibility_strategy'], $form['commerce_discount_fields']['commerce_compatibility_selection']);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save discount'),
    '#weight' => 40,
    '#ief_submit_all' => TRUE,
    '#submit' => NULL,
  );

  if (!$commerce_discount->hasStatus(ENTITY_IN_CODE) && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete discount'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('commerce_discount_form_submit_delete'),
      '#suffix' => l(t('Cancel'), 'admin/commerce/discounts'),
    );
  }
  else {
    $form['actions']['submit']['#suffix'] = l(t('Cancel'), 'admin/commerce/discounts');
  }

  $form['commerce_discount_fields']['additional_settings']['commerce_discount_usage'] = array(
    '#type' => 'fieldset',
    '#title' => t('Maximum usage'),
    '#collapsible' => TRUE,
  );
  $form['commerce_discount_fields']['additional_settings']['discount_date'] = array(
    '#type' => 'fieldset',
    '#title' => t('Discount dates'),
    '#collapsible' => TRUE,
    '#weight' => -1,
  );

  // Add assets.
  $form['#attached']['js'][] = drupal_get_path('module', 'commerce_discount') . '/js/commerce_discount.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'commerce_discount') . '/css/commerce_discount.css';
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

  // Check if this is a percentage offer.
  if (isset($form_state['values']['commerce_discount_fields']['commerce_discount_offer'][LANGUAGE_NONE]['form']['commerce_percentage'])) {
    $percentage = $form_state['values']['commerce_discount_fields']['commerce_discount_offer'][LANGUAGE_NONE]['form']['commerce_percentage'][LANGUAGE_NONE][0]['value'];
    if (!empty($percentage) && $percentage <= 0) {
      form_set_error('commerce_discount_fields][commerce_discount_offer][' . LANGUAGE_NONE . '][form][commerce_percentage', t('Percentage should be a positive, non-zero number.'));
    }
  }
}

/**
 * Form API submit callback for the type form.
 */
function commerce_discount_form_submit(&$form, &$form_state) {
  $commerce_discount = entity_ui_form_submit_build_entity($form, $form_state);
  $commerce_discount->save();
  $form_state['redirect'] = 'admin/commerce/discounts';
}

/**
 * Form API submit callback for the delete button.
 */
function commerce_discount_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/commerce/discounts/manage/' . $form_state['commerce_discount']->name . '/delete';
}

/**
 * Checks if a Discount machine name is taken.
 *
 * @param string $value
 *   The machine name, not prefixed with 'discount_'.
 *
 * @return entity|bool
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
    '#default_value' => variable_get('commerce_discount_line_item_types', array_diff(commerce_product_line_item_types(), array('product_discount'))),
    '#options' => $types,
    '#description' => t('Select the line item types that will be taken into account for calculating the discount amounts in percentage offers.'),
  );
  return system_settings_form($form);
}

/**
 * Add fields to the additional_settings vertical tabs container.
 */
function commerce_discount_form_pre_render($fields) {
  $fields['additional_settings']['commerce_discount_usage']['discount_usage_per_person'] = $fields['discount_usage_per_person'];
  $fields['additional_settings']['commerce_discount_usage']['discount_usage_limit'] = $fields['discount_usage_limit'];
  $fields['additional_settings']['discount_date']['commerce_discount_date'] = $fields['commerce_discount_date'];

  unset($fields['discount_usage_per_person']);
  unset($fields['discount_usage_limit']);
  unset($fields['commerce_discount_date']);

  return $fields;
}

/**
 * Implements function hook_date_combo_process_alter().
 *
 * @param $element
 *   The date field.
 * @param $form_state
 *   A keyed array containing the current state of the form. In this
 *   context, it is used to accumulate information about which button
 *   was clicked when the form was submitted, as well as the sanitized
 *   $_POST data.
 *
 * @return
 *   The altered date field.
 */
function commerce_discount_date_combo_process_alter(&$element, &$form_state, $context) {
  if ($element['#field_name'] == 'commerce_discount_date') {
    $element['#attributes'] = 'container-inline';

    // Hide the checkbox for showing the end date.
    $element['show_todate']['#type'] = 'value';
    $element['show_todate']['#default_value'] = TRUE;
    unset($element['show_todate']['#prefix']);
    unset($element['show_todate']['#suffix']);

    // Ignore the "Show end date" checkbox.
    unset($element['value2']['#states']);

    // Remove fieldset description.
    $element['#fieldset_description'] = '';
  }
}

/**
 * Implements hook_date_popup_process_alter().
 */
function commerce_discount_date_popup_process_alter(&$element, &$form_state, $context) {
  if ($element['#field']['field_name'] == 'commerce_discount_date') {
    unset($element['#attributes']['class']); // date-clear.
    array_shift($element['#wrapper_attributes']['class']);
    // Reset the titles and descriptions of the start and end date fields.
    $field_name = end($element['#parents']);
    if ($field_name == 'value') {
      $element['date']['#title'] = t('Start date');
      $element['date']['#description'] = t('Starts at 12:00:00 AM');
    }
    else {
      $element['date']['#title'] = t('End date');
      $element['date']['#description'] = t('Ends after 11:59:59 PM');
    }
    $element['#title'] = '';
  }
}
