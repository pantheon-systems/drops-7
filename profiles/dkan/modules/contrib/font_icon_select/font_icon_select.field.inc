<?php

/**
 * @file
 * Implement an icon selection field.
 */

/**
 * Implements hook_field_info().
 */
function font_icon_select_field_info() {
  return array(
    'font_icon_select_icon' => array(
      'label'             => t('Font Icon Select'),
      'description'       => t('This field stores the html value of the selected icon.'),
      'settings'          => array(
        'max_length' => 255,
        'font' => '',
        'font_blacklist' => '',
      ),
      'default_widget'    => 'font_icon_select_icon_widget',
      'default_formatter' => 'font_icon_select_icon_default_formatter',
      // Support default token formatter for field tokens.
      'default_token_formatter' => 'font_icon_select_plain',
      'instance_settings' => array(
        // Text processing allows us to use the default text formatter
        // without it we get an undefined error.
        'text_processing' => 0,
        'blacklist_fieldset' => array(
          'suppress' => array(),
          // Blacklist/whitelist boolean.
          'blacklist' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * Implements hook_field_settings_form().
 *
 * @see _font_icon_select_get_available_fonts()
 */
function font_icon_select_field_settings_form($field, $instance, $has_data) {
  $settings = $field['settings'];

  $form = array();

  module_load_include('inc', 'font_icon_select', 'font_icon_select.admin');

  $form['font'] = array(
    '#type' => 'select',
    '#title' => t('Font'),
    '#default_value' => $settings['font'],
    '#options' => _font_icon_select_get_available_fonts(),
    '#required' => TRUE,
    '#description' => t('The font to use.'),
    '#disabled' => $has_data,
    '#ajax' => array(
      'callback' => '_font_icon_select_font_options_ajax',
      'wrapper' => 'ajax-target',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  return $form;
}

/**
 * Implements hook_instance_settings_form().
 *
 * @see _font_icon_select_get_font_icons()
 * @see _font_icon_select_field_instance_settings_form_validate()
 */
function font_icon_select_field_instance_settings_form($field, $instance) {
  $settings = $instance['settings'];
  $font_id = $field['settings']['font'];
  $form = array();

  // Need to pull the icons here...
  module_load_include('inc', 'font_icon_select', 'font_icon_select.admin');
  $available_icons = _font_icon_select_get_font_icons($font_id);

  // Grab the settings from the db.
  $entity = entity_load('font_icon_select_font_library', array($font_id));
  $entity = array_shift($entity);
  $global_settings = unserialize($entity->settings);
  $global_suppression = gettype($global_settings['suppress']) == 'array' ? $global_settings['suppress'] : array();
  // Remove the global blacklisted items from the available icons array.
  foreach ($global_suppression as $icon_key) {
    unset($available_icons[$icon_key]);
  }

  $form['flid'] = array(
    '#type' => 'value',
    '#value' => $font_id,
  );

  $form['blacklist_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Whitelist/Blacklist Toggle'),
    '#group' => 'advanced',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['blacklist_fieldset']['blacklist'] = array(
    '#type' => 'radios',
    '#title' => 'Blacklist/Whitelist Select',
    '#default_value' => $settings['blacklist_fieldset']['blacklist'],
    '#options' => array(0 => t('Whitelist'), 1 => t('Blacklist')),
    '#description' => t('Toggle the behavior of the Black/Whitelist selection below.  If you choose blacklist all icons not selected will be available to the user.  If you select Whitelist only the selected icons will be available to the user'),
  );

  $extra_class = $settings['blacklist_fieldset']['blacklist'] == 1 ? 'blacklist' : 'whitelist';

  $form['blacklist_fieldset']['suppress'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Black/Whitelist Icons for this Field Instance.'),
    '#options' => $available_icons,
    '#default_value' => $settings['blacklist_fieldset']['suppress'],
    '#attributes' => array(
      'class' => array(
        'font_icon_select_options',
        'hideLabels',
        $extra_class,
      ),
    ),
    '#theme' => 'font_icon_select_checkboxes',
    '#element_validate' => array('_font_icon_select_field_instance_settings_form_validate'),
    // Attach css to keep shift click selection from highlighting.
    '#attached' => array(
      'css' => array(
        array(
          'type' => 'inline',
          'data' => "
            .form-item-instance-settings-blacklist-fieldset-suppress div.form-checkboxes{
              -webkit-user-select: none; /* webkit (safari, chrome) browsers */
              -moz-user-select: none; /* mozilla browsers */
              -khtml-user-select: none; /* webkit (konqueror) browsers */
              -ms-user-select: none; /* IE10+ */
            }
          ",
        ),
      ),
    ),
  );

  return $form;
}

/**
 * Validation handler for the font_icon_select blacklist_fieldset instance.
 */
function _font_icon_select_field_instance_settings_form_validate(&$form, &$form_state) {
  $suppress_settings = array_values(array_unique($form_state['values']['instance']['settings']['blacklist_fieldset']['suppress']));
  $suppress_settings = array_filter($suppress_settings, function($i) {
    return $i != '0';
  });
  $form_state['values']['instance']['settings']['blacklist_fieldset']['suppress'] = $suppress_settings;
}

/**
 * Implements hook_field_prepare_view().
 */
function font_icon_select_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  foreach ($items as $id => $entity_items) {
    foreach ($entity_items as $delta => $item) {
      if (is_array($item)) {
        // Default to 'accessible'.
        $items[$id][$delta]['access'] = TRUE;
      }
    }
  }
}

/**
 * Implements hook_field_widget_info().
 */
function font_icon_select_field_widget_info() {
  return array(
    'font_icon_select_icon_widget' => array(
      'label' => t('Font Icon Select check boxes/radio buttons'),
      'field types' => array('font_icon_select_icon'),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 *
 * @see font_icon_select_field_widget_validate()
 */
function font_icon_select_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  switch ($instance['widget']['type']) {

    case 'font_icon_select_icon_widget':
      $module_path = drupal_get_path('module', 'font_icon_select');

      $defaults = array();
      foreach ($items as $key => $value) {
        $defaults[$key] = $value['value'];
      }

      // Pull the font entity info here.
      $entity = entity_load('font_icon_select_font_library', array($field['settings']['font']));
      $entity = array_shift($entity);
      // Get the font file info here.
      $entity_fids = array(
        $entity->eot_fid,
        $entity->svg_fid,
        $entity->ttf_fid,
        $entity->woff_fid,
      );
      $files = file_load_multiple($entity_fids);

      $module_path = drupal_get_path('module', 'font_icon_select');

      // There has to be a better way to do this...
      $options = (array) module_invoke($field['module'], 'options_list', $field, $instance, $element['#entity_type'], $element['#entity']);
      $field_cardinality = $field['cardinality'] == FIELD_CARDINALITY_UNLIMITED ? 0 : $field['cardinality'];
      $field_id = isset($instance['field_id']) ? $instance['field_id'] : 0;
      $element += array(
        '#attributes' => array(
          'class' => array(
            'font_icon_select_options',
            'hideLabels',
            ($instance['settings']['blacklist_fieldset']['blacklist'] == 0 ? 'whitelist' : 'blacklist'),
            'font_icon_select_instance_options',
          ),
        ),
        '#attached' => array(
          'js' => array(
            $module_path . '/font_icon_select.js',
            array(
              'data' => array(
                'font_icon_select' => array(
                  $field['field_name'] => array(
                    'cardinality' => $field_cardinality,
                  ),
                ),
              ),
              'type' => 'setting',
            ),
          ),
          'css' => array(
            $module_path . '/font_icon_select.css',
            array(
              'type' => 'inline',
              'data' => "
                @font-face {
                  font-family: 'font_icon_select_current_font';
                  src: url('" . file_create_url($files[$entity->eot_fid]->uri) . "');
                  src: url('" . file_create_url($files[$entity->eot_fid]->uri) . "?#iefix') format('embedded-opentype'), url('" . file_create_url($files[$entity->woff_fid]->uri) . "') format('woff'), url('" . file_create_url($files[$entity->ttf_fid]->uri) . "') format('truetype'), url('" . file_create_url($files[$entity->svg_fid]->uri) . "') format('svg');
                  font-weight: normal;
                  font-style: normal;
                }
              ",
            ),
          ),
        ),
        '#type' => 'checkboxes',
        '#value_key' => 'value',
        '#default_value' => $defaults,
        '#options' => $options,
        // @TODO - add element validation
        '#element_validate' => array('font_icon_select_field_widget_validate'),
        '#properties' => array(
          'filter_xss' => 1,
          'empty_option' => 'option_none',
          'strip_tags' => '',
          'optgroups' => '',
        ),
        '#theme' => 'font_icon_select_checkboxes',
        '#suffix' => '<div class="fontIconLabelVisibilityToggleWrapper"><input type="checkbox" id="fontIconLabelToggleVisibility' . $field_id . '" name="fontIconLabelToggleVisibility' . $field_id . '" value="1"><label for="fontIconLabelToggleVisibility' . $field_id . '">Show Icon Labels</label></div>',
      );

      if ($element['#entity'] == NULL || (is_string($element['#entity']) && trim($element['#entity']) == '')) {
        // Instance settings specific js to deal with the extra defaults field.
        $element['#attached']['js'][] = $module_path . '/font_icon_select_instance_settings.js';
      }

      break;
  }

  return $element;
}

/**
 * Form element validation handler for icon select element.
 *
 * Based on options element validation.
 */
function font_icon_select_field_widget_validate($element, &$form_state) {
  if ($element['#required'] && $element['#value'] == '_none') {
    form_error($element, t('@name field is required.', array('@name' => $element['#title'])));
  }
  // Transpose selections from field => delta to delta => field, turning
  // multiple selected options into multiple parent elements.
  $items = _options_form_to_storage($element);
  form_set_value($element, $items, $form_state);
}

/**
 * Implements hook_field_formatter_info().
 */
function font_icon_select_field_formatter_info() {
  $ret = array(
    'font_icon_select_icon_default_formatter' => array(
      'label' => t('default'),
      'description' => t('Output unencoded icon.'),
      'field types' => array('font_icon_select_icon'),
    ),
  );
  return $ret;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function font_icon_select_field_formatter_settings_summary($field, $instance, $view_mode) {
  // Returning an empty value suppresses the formatter settings form.
  return '';
}

/**
 * Implements hook_field_formatter_view().
 */
function font_icon_select_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $result = array();

  switch ($display['type']) {
    case 'font_icon_select_icon_default_formatter':
      foreach ($items as $delta => $item) {
        if ($item['access']) {
          $result[$delta] = array(
            '#prefix' => '<span class="font-icon-select-' . $field['settings']['font'] . ' font-icon-select-' . $field['settings']['font'] . '-' . drupal_substr($item['value'], 1) . '">',
            '#suffix' => '</span>',
          );
        }
      }
      break;
  }

  return $result;
}

/**
 * Themes a set of icon select checkboxes.
 *
 * @param array $vars
 *   Contains element to be themed.
 */
function theme_font_icon_select_checkboxes(array $vars) {
  $element = $vars['element'];

  $element['#default_value'] = is_array($element['#default_value']) ? $element['#default_value'] : array($element['#default_value']);

  $output = array();

  foreach ($element['#options'] as $key => $value) {
    $input_options = array(
      '#type' => 'checkbox',
      '#name' => $element['#name'] . '[]',
      '#attributes' => array(
        'value' => $key,
      ),
      '#id' => $element['#id'] . '-' . $key,
    );
    if (in_array($key, $element['#default_value'])) {
      $input_options['#attributes']['checked'] = 'checked';
    }

    $current_element = array(
      'key' => $key,
      'value' => $value,
      'field' => drupal_render($input_options),
      'checked' => in_array($key, $element['#default_value']),
      'element_id' => $element['#id'],
    );
    $output[] = theme(array('font_icon_select_checkbox'), array('element' => $current_element));
  }
  return implode("\n", $output);
}
