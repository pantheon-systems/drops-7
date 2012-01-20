<?php
/**
 * @file
 * Hooks provided by the Date module.
 */

/**
 * This allows other modules to alter the $dates array created
 * by date_formatter_process().
 *
 * @param $dates - the $dates array created by the Date module.
 * @param $context - an array containing:
 *   'field' - the $field array.
 *   'instance' - the $instance array.
 *   'format' - the string $format.
 *   'entity_type' - the $entity_type.
 *   'entity' - the $entity object.
 *   'langcode' - the string $langcode.
 *   'item' - the $item array.
 *   'display' - the $display array.
 */
function hook_date_formatter_dates_alter(&$dates, $context) {

  $field = $context['field'];
  $instance = $context['instance'];
  $format = $context['format'];
  $entity_type = $context['entity_type'];
  $entity = $context['entity'];
  $date1 = $dates['value']['local']['object'];
  $date2 = $dates['value2']['local']['object'];

  $is_all_day = date_all_day_field($field, $instance, $date1, $date2);

  $all_day1 = '';
  $all_day2 = '';
  if ($format != 'format_interval' && $is_all_day) {
    $all_day1 = theme('date_all_day', array(
      'field' => $field,
      'instance' => $instance,
      'which' => 'date1',
      'date1' => $date1,
      'date2' => $date2,
      'format' => $format,
      'entity_type' => $entity_type,
      'entity' => $entity));
    $all_day2 = theme('date_all_day', array(
      'field' => $field,
      'instance' => $instance,
      'which' => 'date2',
      'date1' => $date1,
      'date2' => $date2,
      'format' => $format,
      'entity_type' => $entity_type,
      'entity' => $entity));
    $dates['value']['formatted_time'] = theme('date_all_day_label');
    $dates['value2']['formatted_time'] = theme('date_all_day_label');
    $dates['value']['formatted'] = $all_day1;
    $dates['value2']['formatted'] = $all_day2;
  }
}

/**
 * This hook lets other modules make changes to the date_combo element
 * after the Date module is finished with it.
 *
 * @param $element - the $element array.
 * @param $fom_state - the $form_state array.
 * @param $context, and array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'form' - the $form array.
 */
function hook_date_combo_process_alter(&$element, &$form_state, $context) {

  $field = $context['field'];
  $instance = $context['instance'];
  $field_name = $element['#field_name'];
  $delta = $element['#delta'];

  // Add a date repeat form element, if needed.
  // We delayed until this point so we don't bother adding it to hidden fields.
  if (date_is_repeat_field($field, $instance)) {
    
    $item = $element['#value'];
    $element['rrule'] = array(
      '#type' => 'date_repeat_rrule',
      '#theme_wrappers' => array('date_repeat_rrule'),
      '#default_value' => isset($item['rrule']) ? $item['rrule'] : '',
      '#date_timezone' => $element['#date_timezone'],
      '#date_format'      => date_limit_format(date_input_format($element, $field, $instance), $field['settings']['granularity']),
      '#date_text_parts'  => (array) $instance['widget']['settings']['text_parts'],
      '#date_increment'   => $instance['widget']['settings']['increment'],
      '#date_year_range'  => $instance['widget']['settings']['year_range'],
      '#date_label_position' => $instance['widget']['settings']['label_position'],
      '#prev_value' => isset($item['value']) ? $item['value'] : '',
      '#prev_value2' => isset($item['value2']) ? $item['value2'] : '',
      '#prev_rrule' => isset($item['rrule']) ? $item['rrule'] : '',
      '#date_repeat_widget' => str_replace('_repeat', '', $instance['widget']['type']),
      '#date_repeat_collapsed' => $instance['widget']['settings']['repeat_collapsed'],
      '#date_flexible' => 0,
      '#weight' => $instance['widget']['weight'] + .4,
    );
  }

}

/**
 * This hook lets other modules alter the element or the form_state before the rest
 * of the date_combo validation gets fired.
 *
 * @param $element - the $element array.
 * @param $fom_state - the $form_state array.
 * @param $context, and array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'item' - the $item array.
 */
function date_all_day_date_combo_pre_validate_alter(&$element, &$form_state, $context) {

  if (!empty($context['item']['all_day'])) {

    $field = $context['field'];

    // If we have an all day flag on this date and the time is empty,
    // change the format to match the input value so we don't get validation errors.
    $element['#date_is_all_day'] = TRUE;
    $element['value']['#date_format'] = date_part_format('date', $element['value']['#date_format']);
    if (!empty($field['settings']['todate'])) {
      $element['value2']['#date_format'] = date_part_format('date', $element['value2']['#date_format']);
    }
  }
}

/**
 * This hook lets other modules alter the local date objects created by the date_combo validation
 * before they are converted back to the database timezone and stored.
 *
 * @param $date - the $date object.
 * @param $fom_state - the $form_state array.
 * @param $context, and array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'item' - the $item array.
 *  'element' - the $element array.
 */
function date_all_day_date_combo_validate_date_start_alter(&$date, &$form_state, $context) {

   // If this is an 'All day' value, set the time to midnight.
   if (!empty($context['element']['#date_is_all_day'])) {
     $date->setTime(0, 0, 0);
   }
}

/**
 * This hook lets other modules alter the local date objects created by the date_combo validation
 * before they are converted back to the database timezone and stored.
 *
 * @param $date - the $date object.
 * @param $fom_state - the $form_state array.
 * @param $context, and array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'item' - the $item array.
 *  'element' - the $element array.
 */
 */
function date_all_day_date_combo_validate_date_end_alter(&$date, &$form_state, $context) {

   // If this is an 'All day' value, set the time to midnight.
   if (!empty($context['element']['#date_is_all_day'])) {
     $date->setTime(0, 0, 0);
   }
}

/**
 * This hook lets other modules alter the field settings form.
 *
 * @param $form - the $form array.
 * @context - an array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'has_date' => the value of $has_data.
 */
function hook_date_field_settings_form_alter(&$form, $context) {

  $field = $context['field'];
  $instance = $context['instance'];
  $has_data = $context['has_data'];

  $form['repeat'] = array(
    '#type' => 'select',
    '#title' => t('Repeating date'),
    '#default_value' => $field['settings']['repeat'],
    '#options' => array(0 => t('No'), 1 => t('Yes')),
    '#attributes' => array('class' => array('container-inline')),
    '#description' => t("Repeating dates use an 'Unlimited' number of values. Instead of the 'Add more' button, they include a form to select when and how often the date should repeat."),
    '#disabled' => $has_data,
  );
}

/**
 * This hook lets other modules alter the field instance settings form.
 *
 * @param $form - the $form array.
 * @context - an array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 */
function hook_date_field_instance_settings_form_alter(&$form, $context) {
  $field = $context['field'];
  $instance = $context['instance'];
  $form['new_setting'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => t('My new setting'),
  );
}

/**
 * This hook lets other modules alter the field widget settings form.
 *
 * @param $form - the $form array.
 * @context - an array containing:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 */
function hook_date_field_widget_settings_form_alter(&$form, $context) {

  $field = $context['field'];
  $instance = $context['instance'];
  $form['new_setting'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#title' => t('My new setting'),
  );
}

/**
 * This hook lets other modules alter the field formatter settings form.
 *
 * @param $form - the form array.
 * @param $form_state - the form state array.
 * @param $context, an array that includes:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'view_mode' - the formatter view mode.
 */
function hook_date_field_formatter_settings_form_alter(&$form, &$form_state, $context) {

  $field = $context['field'];
  $instance = $context['instance'];
  $view_mode = $context['view_mode'];
  $display = $instance['display'][$view_mode];
  $formatter = $display['type'];
  if ($formatter == 'date_default') {
    $form['show_repeat_rule'] = array(
      '#title' => t('Repeat rule:'),
      '#type' => 'select',
      '#options' => array(
        'show' => t('Display repeat rule'),
        'hide' => t('Hide repeat rule')),
      '#default_value' => $settings['show_repeat_rule'],
      '#access' => $field['settings']['repeat'],
      '#weight' => 5,
    );
  }
}

/**
 * This hook lets other modules alter the field formatter settings summary.
 *
 * @param $summary - an array of text strings that will be concatonated into a summary description.
 * @param $context, an array that includes:
 *  'field' - the $field array.
 *  'instance' - the $instance array.
 *  'view_mode' - the formatter view mode.
 */
function hook_date_field_formatter_settings_summary_alter(&$summary, $context) {

  $field = $context['field'];
  $instance = $context['instance'];
  $view_mode = $context['view_mode'];
  $display = $instance['display'][$view_mode];
  $formatter = $display['type'];
  $settings = $display['settings'];
  if (array_key_exists('show_repeat_rule', $settings) && !empty($field['settings']['repeat'])) {
    if (!empty($settings['show_repeat_rule'])) {
      $summary[] = t('Show repeat rule');
    }
    else {
      $summary[] = t('Do not show repeat rule');
    }
  }
}

/**
 * This hook lets other modules make changes to the date_text element.
 *
 * @param $element - the $element array created by the date_text element.
 */
function hook_date_text_process_alter(&$element) {
  $all_day_id = !empty($element['#date_all_day_id']) ? $element['#date_all_day_id'] : '';
  if ($all_day_id != '') {
    // All Day handling on text dates works only if the user leaves the time out of the input value.
    // There is no element to hide or show.
  }
}

/**
 * This hook lets other modules make changes to the date_select element.
 *
 * @param $element - the $element array created by the date_select element.
 */
function hook_date_select_process_alter(&$element) {

  // Hide or show this element in reaction to the all_day status for this element.
  $all_day_id = !empty($element['#date_all_day_id']) ? $element['#date_all_day_id'] : '';
  if ($all_day_id != '') {
    foreach(array('hour', 'minute', 'second', 'ampm') as $field) {
      if (array_key_exists($field, $element)) {
        $element[$field]['#states'] = array(
          'visible' => array(
            'input[name="' . $all_day_id . '"]' => array('checked' => FALSE),
          ));
      }
    }
  }
}

/**
 * This hook lets other modules make changes to the date_popup element.
 *
 * @param $element - the $element array created by the date_popup element.
 */
function hook_date_popup_process_alter(&$element) {

  // Hide or show this element in reaction to the all_day status for this element.
  $all_day_id = !empty($element['#date_all_day_id']) ? $element['#date_all_day_id'] : '';
  if ($all_day_id != '' && array_key_exists('time', $element)) {
    $element['time']['#states'] = array(
      'visible' => array(
        'input[name="' . $all_day_id . '"]' => array('checked' => FALSE),
      ));
  }
}

/**
 * This hook lets other modules alter the element or the form_state before the rest
 * of the date_select validation gets fired.
 *
 * @param $element - the $element passed into the validation.
 * @param $form_state - the $form_state passed into the validation, as altered by previous processing.
 * @param $input - the $input array passed into the validation.
 */
function hook_date_text_pre_validate_alter(&$element, &$form_state, &$input) {
  // Let Date module massage the format for all day values so they will pass validation.
  // The All day flag, if used, actually exists on the parent element.
  date_all_day_value($element, $form_state);
}

/**
 * This hook lets other modules alter the element or the form_state before the rest
 * of the date_select validation gets fired.
 *
 * @param $element - the $element passed into the validation.
 * @param $form_state - the $form_state passed into the validation, as altered by previous processing.
 * @param $input - the $input array passed into the validation.
 */
function hook_date_select_pre_validate_alter(&$element, &$form_state, &$input) {
  // Let Date module massage the format for all day values so they will pass validation.
  // The All day flag, if used, actually exists on the parent element.
  date_all_day_value($element, $form_state);
}

/**
 * This hook lets other modules alter the element or the form_state before the rest
 * of the date_popup validation gets fired.
 *
 * @param $element - the $element passed into the validation.
 * @param $form_state - the $form_state passed into the validation, as altered by previous processing.
 * @param $input - the $input array passed into the validation.
 */
function hook_date_popup_pre_validate_alter(&$element, &$form_state, &$input) {
  // Let Date module massage the format for all day values so they will pass validation.
  // The All day flag, if used, actually exists on the parent element.
  date_all_day_value($element, $form_state);
}

