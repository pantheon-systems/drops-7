<?php

/**
 * @file
 * Provides CCK integration for fivestar module
 */

/**
 * Implementation of hook_field_info().
 */
function fivestar_field_info() {
  return array(
    'fivestar' => array(
      'label' => t('Fivestar Rating'),
      'description' => t('Store a rating for this piece of content.'),
      'default_widget' => 'exposed',
      'default_formatter' => 'fivestar_formatter_default',
      'settings' => array(
        'axis' => 'vote',
      ),
      'instance_settings' => array(
        'stars' => 5,
      ),
      'property_type' => 'fivestar',
      'property_callbacks' => array('fivestar_property_info_callback'),
      'microdata' => TRUE,
    ),
  );
}

function fivestar_form_field_ui_field_edit_form_alter(&$form, $form_state) {
  $field = $form['#field'];
  $instance = $form['#instance'];
  if ($field['type'] == 'fivestar') {
    // Multiple values is not supported with Fivestar.
    $form['field']['cardinality']['#access'] = FALSE;
    $form['field']['cardinality']['#value'] = 1;
    // Setting "default value" here is confusing and for all practical purposes
    // with existing widgets provided by fivestar (and anything else available
    // in contrib) meaningless.
    $form['instance']['default_value_widget']['#access'] = FALSE;
  }
}

/**
 * Implementation of hook_field_settings_form().
 */
function fivestar_field_settings_form($field, $instance) {
  $form['axis'] = array(
    '#type' => 'select',
    '#required' => TRUE,
    '#title' => 'Voting Tag',
    '#options' => fivestar_get_tags(),
    '#description' => t('The tag this rating will affect. Enter a property on which that this rating will affect, such as <em>quality</em>, <em>satisfaction</em>, <em>overall</em>, etc.'),
    '#default_value' => isset($field['settings']['axis']) ? $field['settings']['axis'] : '',
    '#disabled' => field_has_data($field),
  );

  return $form;
}

function fivestar_field_instance_settings_form($field, $instance) {
  $form = array();

  $widget_title = ($instance['widget']['type'] == 'select') ? t('Number of options') : t('Number of stars');
  $form['stars'] = array(
    '#type' => 'select',
    '#title' => check_plain($widget_title),
    '#options' => drupal_map_assoc(range(1, 10)),
    '#default_value' => isset($instance['settings']['stars']) ? $instance['settings']['stars'] : 5,
  );

  $form['allow_clear'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to cancel their ratings.'),
    '#default_value' => isset($instance['settings']['allow_clear']) ? $instance['settings']['allow_clear'] : FALSE,
    '#return_value' => 1,
  );

  $form['allow_revote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to re-vote on already voted content.'),
    '#default_value' => isset($instance['settings']['allow_revote']) ? $instance['settings']['allow_revote'] : FALSE,
    '#return_value' => 1,
  );

  $form['allow_ownvote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to vote on their own content.'),
    '#default_value' => isset($instance['settings']['allow_ownvote']) ? $instance['settings']['allow_ownvote'] : FALSE,
    '#return_value' => 1,
  );

  $options = fivestar_get_targets($field, $instance);
  $form['target'] = array(
    '#title' => t('Voting target'),
    '#type' => 'select',
    '#default_value' => (isset($instance['settings']['target']) && $instance['widget']['type'] != 'exposed') ? $instance['settings']['target'] : 'none',
    '#options' => $options,
    '#description' => t('The voting target will make the value of this field cast a vote on another node. Use node reference fields module to create advanced reviews. Use the Parent Node Target when using fivestar with comments. More information available on the <a href="http://drupal.org/handbook/modules/fivestar">Fivestar handbook page</a>.'),
    '#access' => (count($options) > 1 && $instance['widget']['type'] != 'exposed'),
  );

  return $form;
}

/**
 * Implementation of hook_field_insert().
 */
function fivestar_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  _fivestar_field_helper($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implementation of hook_field_update().
 */
function fivestar_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  _fivestar_field_helper($entity_type, $entity, $field, $instance, $langcode, $items);
}

/**
 * Implementation of hook_field_delete().
 */
function fivestar_field_delete($entity_type, $entity, $field, $instance, $langcode, &$items) {
  _fivestar_field_helper($entity_type, $entity, $field, $instance, $langcode, $items, 'delete');
}

function _fivestar_field_helper($entity_type, $entity, $field, $instance, $langcode, &$items, $op = '') {
  foreach ($items as $delta => $item) {
    if ((isset($entity->status) && !$entity->status) || $op == 'delete') {
      $rating = 0;
    }
    else {
      $rating = (isset($items[$delta]['rating'])) ? $items[$delta]['rating'] : 0;
    }
    $target = _fivestar_field_target($entity, $field, $instance, $item, $langcode);    
    if (!empty($target)) {
      if ($entity_type == 'comment' && $op == 'delete') {
        $target['vote_source'] = $entity->hostname;
      }
      else {
        $target['vote_source'] = NULL;
      }
      _fivestar_cast_vote($target['entity_type'], $target['entity_id'], $rating, $field['settings']['axis'], $entity->uid, TRUE, $target['vote_source']);
      votingapi_recalculate_results($target['entity_type'], $target['entity_id']);
    }
    // The original callback is only called for a single updated field, but the Field API
    // then updates all fields of the entity. For an update, the Field API first deletes
    // the equivalent row in the database and then adds a new row based on the
    // information in $items here. If there are multiple Fivestar fields on an entity, the
    // one being updated is handled ok ('rating' having already been set to the new value),
    // but other Fivestar fields are set to NULL as 'rating' isn't set - not what an update
    // would be expected to do. So set 'rating' for all of the Fivestar fields from the
    // existing user data in $items. This preserves the user vote thru Field API's
    // delete/re-insert process.
    if (!isset($items[$delta]['rating'])) {
      $items[$delta]['rating'] = $items[$delta]['user'];
    }
  }
}

/**
 * Helper function to find the id that should be rated when a field is changed.
 */
function _fivestar_field_target($entity, $field, $instance, $item, $langcode) {
  if ($instance['widget']['type'] == 'exposed') {
    return NULL;
  }
  if (isset($instance['settings']['target'])) {
    $target = fivestar_get_targets($field, $instance, $instance['settings']['target'], $entity, $langcode);
  }
  else {
    // If all else fails, default to voting on the instance the field is attached to.
    list($id, $vid, $bundle) = entity_extract_ids($instance['entity_type'], $entity);
    $target = array(
      'entity_id' => $id,
      'entity_type' => $instance['entity_type'],
    );
  }
  return $target;
}

/**
 * Helper function to store a rating into the field storage.
 */
function _fivestar_update_field_value($entity_type, $entity, $field_name, $langcode, $value) {
  $entity->{$field_name}[$langcode][0]['rating'] = $value;
  $entity->original = isset($entity->original) ? $entity->original : NULL;
  field_attach_presave($entity_type, $entity);
  field_attach_update($entity_type, $entity);
}

/**
 * Implementation of hook_field_is_empty().
 */
function fivestar_field_is_empty($item, $field) {
  return empty($item['rating']) || $item['rating'] == '-';
}

/**
 * Implementation of hook_field_widget_info().
 */
function fivestar_field_widget_info() {
  return array(
    'exposed' => array(
      'label' => t('Stars (rated while viewing)'),
      'field types' => array('fivestar'),
      'behaviors' => array('multiple values' => FIELD_BEHAVIOR_NONE),
    ),
    'stars' => array(
      'label' => t('Stars (rated while editing)'),
      'field types' => array('fivestar'),
      'behaviors' => array('multiple values' => FIELD_BEHAVIOR_NONE),
      'settings' => array(
        'widget' => array(
          'fivestar_widget' => 'default',
        )
      )
    ),
    'fivestar_select' => array(
      'label' => t('Select list (rated while editing)'),
      'field types' => array('fivestar'),
      'behaviors' => array('multiple values' => FIELD_BEHAVIOR_NONE),
    ),
  );
}

/**
 * Implementation of hook_field_widget_settings_form().
 */
function fivestar_field_widget_settings_form($field, $instance) {
  $form = array();
  if ($instance['widget']['type'] == 'stars') {
    $form['widget'] = array(
      '#tree' => TRUE,
      '#type' => 'fieldset',
      '#title' => t('Star display options'),
      '#description' => t('Choose a style for your widget.'),
      '#weight' => -2,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $widgets = module_invoke_all('fivestar_widgets');

    $form['widget']['fivestar_widget'] = array(
      '#type' => 'radios',
      '#options' => array('default' => t('Default')) + $widgets,
      '#default_value' => isset($instance['widget']['settings']['widget']['fivestar_widget']) ? $instance['widget']['settings']['widget']['fivestar_widget'] : 'default',
      '#attributes' => array('class' => array('fivestar-widgets', 'clearfix')),
      '#pre_render' => array('fivestar_previews_expand'),
      '#attached' => array('css' => array(drupal_get_path('module', 'fivestar') . '/css/fivestar-admin.css')),
    );
  }

  return $form;
}

function fivestar_previews_expand($element) {

  foreach (element_children($element) as $css) {
    $vars = array(
      'css' => $css,
      'name' => strtolower($element[$css]['#title']),
    );
    $element[$css]['#description'] = theme('fivestar_preview_widget', $vars);
  }

  return $element;
}

/**
 * Implementation of hook_field_widget_form().
 */
function fivestar_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $element['#tree'] = TRUE;
  $i18n = function_exists('i18n_field_translate_property');
  if ($instance['widget']['type'] == 'fivestar_select' || ($instance['widget']['type'] == 'stars' && isset($form['#title']) && $form['#title'] == 'Default value')) {
    $options = array(0 => t('No stars'));
    if (empty($instance['settings']['stars'])) {
      $instance['settings']['stars'] = 5;
    }
    for ($i = 1; $i <= $instance['settings']['stars']; $i++) {
      $percentage = ceil($i * 100 / $instance['settings']['stars']);
      $options[$percentage] = format_plural($i, '1 star', '@count stars');
    }
    $element['rating'] = array(
      '#type' => 'select',
      '#title' => isset($instance['label']) ? (($i18n) ? i18n_field_translate_property($instance, 'label') : t($instance['label'])) : FALSE,
      '#options' => $options,
      '#default_value' => isset($items[$delta]['rating']) ? $items[$delta]['rating'] : NULL,
      '#description' => isset($instance['description']) ? (($i18n) ? i18n_field_translate_property($instance, 'description') : t($instance['description'])) : FALSE,
      '#required' => isset($instance['required']) ? $instance['required'] : FALSE,
    );
  }

  elseif ($instance['widget']['type'] == 'stars') {
    $widgets = module_invoke_all('fivestar_widgets');
    $active = isset($instance['widget']['settings']['widget']['fivestar_widget']) ? $instance['widget']['settings']['widget']['fivestar_widget'] : 'default';
    $widget = array(
      'name' => isset($widgets[$active]) ? strtolower($widgets[$active]) : 'default',
      'css' => $active,
    );

    $values = array(
      'user' => 0,
      'average' => 0,
      'count' => 0,
    );

    $settings = array(
      'stars' => $instance['settings']['stars'],
      'allow_clear' => !empty($instance['settings']['allow_clear']) ? $instance['settings']['allow_clear'] : FALSE,
      'allow_revote' => !empty($instance['settings']['allow_revote']) ? $instance['settings']['allow_revote'] : FALSE,
      'allow_ownvote' => !empty($instance['settings']['allow_ownvote']) ? $instance['settings']['allow_ownvote'] : FALSE,
      'style' => 'user',
      'text' => 'none',
      'widget' => $widget,
    );

    $element['rating'] = array(
      '#type'=> 'fivestar',
      '#title' => isset($instance['label']) ? (($i18n) ? i18n_field_translate_property($instance, 'label') : t($instance['label'])) : FALSE,
      '#stars' => isset($instance['settings']['stars']) ? $instance['settings']['stars'] : 5,
      '#allow_clear' => isset($instance['settings']['allow_clear']) ? $instance['settings']['allow_clear'] : FALSE,
      '#allow_revote' => isset($instance['settings']['allow_revote']) ? $instance['settings']['allow_revote'] : FALSE,
      '#allow_ownvote' => isset($instance['settings']['allow_ownvote']) ? $instance['settings']['allow_ownvote'] : FALSE,
      '#default_value' => isset($items[$delta]['rating']) ? $items[$delta]['rating'] : (isset($instance['default_value'][$delta]['rating']) ? $instance['default_value'][$delta]['rating'] : 0),
      '#widget' => $widget,
      '#settings' => $settings,
      '#values' => $values,
      '#description' => isset($instance['description']) ? (($i18n) ? i18n_field_translate_property($instance, 'description') : t($instance['description'])) : FALSE,
      '#required' => isset($instance['required']) ? $instance['required'] : FALSE,
    );
  }

  return array($element);
}

/**
 * Implementation of hook_field_formatter_info().
 */
function fivestar_field_formatter_info() {
  return array(
    'fivestar_formatter_default' => array(
      'label' => t('As Stars'),
      'field types' => array('fivestar'),
      'settings' =>  array(
        // Note: Do not set to widget to 'default' by
        // default. "Stars (rated while editing)" should
        // default to whatever was selected as a widget
        // setting. Let hook_field_formatter_view() handle
        // defaults for instances that aren't set to anything.
        'widget' => array('fivestar_widget' => NULL),
        'style' => 'average',
        'text' => 'average',
        'expose' => TRUE,
      ),
    ),
    'fivestar_formatter_rating' => array(
      'label' => t('Rating (i.e. 4.2/5)'),
      'field types' => array('fivestar'),
    ),
    'fivestar_formatter_percentage' => array(
      'label' => t('Percentage (i.e. 92)'),
      'field types' => array('fivestar'),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function fivestar_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  if ($display['type'] != 'fivestar_formatter_default') {
    return;
  }

  $element['widget'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Star display options'),
    '#description' => t('Choose a style for your widget.'),
    '#weight' => -2,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $widgets = module_invoke_all('fivestar_widgets');

  $element['widget']['fivestar_widget'] = array(
    '#type' => 'radios',
    '#options' => array('default' => t('Default')) + $widgets,
    '#default_value' => isset($settings['widget']['fivestar_widget']) ? $settings['widget']['fivestar_widget'] : 'default',
    '#attributes' => array('class' => array('fivestar-widgets', 'clearfix')),
    '#pre_render' => array('fivestar_previews_expand'),
    '#attached' => array('css' => array(drupal_get_path('module', 'fivestar') . '/css/fivestar-admin.css')),
  );

  if ($instance['widget']['type'] == 'exposed') {
    $element['expose'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow voting on the entity.'),
      '#default_value' => $settings['expose'],
      '#return_value' => 1
    );
  }

  $element['style'] = array(
    '#type' => 'select',
    '#title' => t('Value to display as stars'),
    '#default_value' => $settings['style'],
    '#options' => array(
      'average' => t('Average vote'),
      'user'    => t("User's vote"),
      'smart'   => t("User's vote if available, average otherwise"),
      'dual'    => t("Both user's and average vote"),
    ),
  );
  $element['text'] = array(
    '#type' => 'select',
    '#title' => t('Text to display under the stars'),
    '#default_value' => $settings['text'],
    '#options' => array(
      'none'    => t('No text'),
      'average' => t('Average vote'),
      'user'    => t("User's vote"),
      'smart'   => t("User's vote if available, average otherwise"),
      'dual'    => t("Both user's and average vote"),
    ),
  );

  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function fivestar_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $settings = $display['settings'];

  if ($display['type'] != 'fivestar_formatter_default') {
    return;
  }

  $widgets = module_invoke_all('fivestar_widgets');

  if ($instance['widget']['type'] == 'exposed') {
    $summary = t("Style: @widget, Exposed: @expose, Stars display: @style, Text display: @text", array(
      '@widget' => isset($widgets[$settings['widget']['fivestar_widget']]) ? strtolower($widgets[$settings['widget']['fivestar_widget']]) : t('default'),
      '@expose' => ($settings['expose']) ? 'yes' : 'no',
      '@style' => strtolower($settings['style']),
      '@text' => strtolower($settings['text'])));
    return $summary;
  }

  $summary = t("Style: @widget, Stars display: @style, Text display: @text", array(
    '@widget' => isset($widgets[$settings['widget']['fivestar_widget']]) ? $widgets[$settings['widget']['fivestar_widget']] : t('default'),
    '@style' => strtolower($settings['style']),
    '@text' => strtolower($settings['text'])));

  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 *
 * This function returns a renderable array for each fivestar field
 * to be displayed when viewing a node (in any view mode).
 * The render array will be either a form array created with
 * drupal_get_form() or a custom render array, to be sent to a
 * fivestar theme function.
 *
 * @param $items
 *  Array. Generated by fivestar_field_prepare_view(). This array contains
 *  processed voting info.
 *
 * @return $element
 *  Renderable array. This array will always be $element[0], with only one
 *  top level item, because Fivestar does not offer multi-value fields.
 */
function fivestar_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];
  $widgets = module_invoke_all('fivestar_widgets');
  $widget = _fivestar_get_widget($widgets, $display, $instance);
  $values = $items[0];

  // Determine if any set of stars to be displayed need to be
  // displayed in a form. (That is, can the user click the stars
  // to cast a vote?) If yes, hand off everything we know to the
  // fivestar_custom_widget form, and let it take care of the rest.
  // Note: Stars will only be displayed in a form in the following circumstance:
  // - Fivestar widget selected is "Stars (rated while viewing)"
  // - Fivestar display setting = "exposed"
  $is_form = ($instance['widget']['type'] == 'exposed'
              && user_access('rate content')
              && $display['type'] == 'fivestar_formatter_default'
              && $display['settings']['expose']) ? TRUE : FALSE;
  if ($is_form) {
    // TODO. Get rid of voting categories setting, then change this so
    // axis = field name.
    $tag = (isset($field['settings']['axis'])) ? $field['settings']['axis'] : 'vote';
    list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);
    $settings = _fivestar_custom_widget_settings($entity_type, $instance, $display, $id, $tag, $widget);
    // Store entity and field data for later reuse.
    $settings += array(
      'entity_id' => $id,
      'entity_type' => $entity_type,
      'field_name' => $instance['field_name'],
      'langcode' => $langcode,
    );
    // If microdata module is enabled, attach the microdata attributes.
    $settings['microdata'] = module_exists('microdata') ? $entity->microdata[$field['field_name']] : array();

    $element[0] = drupal_get_form('fivestar_custom_widget', $values, $settings);
    // Our work here is done.
    return $element;
  }

  // No stars will be displayed in a form. Build a renderable array.
  $element[0] = array(
    // Add a container div around this field with the clearfix class on it.
    '#attributes' => array('class' => array('clearfix')),
    '#theme_wrappers' => array('container'),
  );

  // Determine if we are going to display stars, rating or percentage.
  $formatter = $display['type'];
  if ($formatter == 'fivestar_formatter_percentage' || $formatter == 'fivestar_formatter_rating') {
    $element[0]['user'] = array(
      '#theme' => $formatter,
      '#instance_settings' => $instance['settings'],
      '#display_settings' => $settings,
      '#item' => $values,
    );
    // No stars to display. Our work here is done.
    return $element;
  }

  // Determine which sets of stars are going to be displayed.
  // Options:
  // - Only show average of all votes.
  // - Only show the user his/her own vote.
  // - Show both the average and the user's own votes.
  $style = $display['settings']['style'];
  $show_average_stars = ($style == 'average' || $style == 'dual' || ($style == 'smart' && empty($values['user'])));
  $show_user_stars = ($style == 'user' || $style == 'dual' || ($style == 'smart' && !empty($values['user'])));
  if ($show_user_stars) {
    $element[0]['user'] = array(
      '#theme' => $display['type'],
      '#rating' => $values['user'],
      '#instance_settings' => $instance['settings'],
      '#display_settings' => $settings,
      '#widget' => $widget,
    );
    $element[0]['#attributes']['class'][] = 'fivestar-user-stars';
  }
  if ($show_average_stars) {
    $element[0]['average'] = array(
      '#theme' => $display['type'],
      '#rating' => $values['average'],
      '#instance_settings' => $instance['settings'],
      '#display_settings' => $settings,
      '#widget' => $widget,
    );
    $element[0]['#attributes']['class'][] = 'fivestar-average-stars';
  }
  if ($style === 'smart') {
    $element[0]['#attributes']['class'][] = 'fivestar-smart-stars';
  }
  elseif ($style === 'dual') {
    $element[0]['#attributes']['class'][] = 'fivestar-combo-stars';
  }

  // Determine which text is to be displayed.
  $text = $display['settings']['text'];
  $summary_options = array(
    'stars' => $instance['settings']['stars'],
    'votes' => NULL,
  );

 $summary_options['microdata'] = _fivestar_get_microdata_property_info($entity_type, $entity, $field, $instance);

  // If we're displaying both user and average ratings, add a description to
  // both the 'user' and 'average' elements.
  if ($style === 'dual') {
    $element[0]['user']['#description'] = theme('fivestar_summary', array(
      'user_rating' => $values['user'],
    ) + $summary_options);
    $element[0]['average']['#description'] = theme('fivestar_summary', array(
      'average_rating' => $values['average'],
      'votes' => $values['count'],
    ) + $summary_options);
  }
  // If we're only creating one element (either 'user' or 'average'), prepare
  // the correct description, and place it on that element.
  else {
    // Prepare the description.
    $show_average_text = ($text === 'average' || $text === 'dual' || ($text === 'smart' && empty($values['user'])));
    $show_user_text = ($text === 'user' || $text === 'dual' || ($text === 'smart' && !empty($values['user'])));
    if ($show_user_text) {
      $summary_options['user_rating'] = $values['user'];
      $element[0]['#attributes']['class'][] = 'fivestar-user-text';
    }
    if ($show_average_text) {
      $summary_options['average_rating'] = $values['average'];
      $summary_options['votes'] = $values['count'];
      $element[0]['#attributes']['class'][] = 'fivestar-average-text';
    }
    if ($text === 'smart') {
      $element[0]['#attributes']['class'][] = 'fivestar-smart-text';
    }
    elseif ($text === 'dual') {
      $element[0]['#attributes']['class'][] = 'fivestar-combo-text';
    }
    // Add the description to the set of stars. It might be named either 'user'
    // or 'average', so first figure out its name.
    $children = element_children($element[0]);
    $name = reset($children);
    $element[0][$name]['#description'] = theme('fivestar_summary', $summary_options);
  }

  return $element;
}

/**
 * Generate the $settings parameter to be passed to fivestar_custom_widget().
 *
 * @params
 *
 * @return $settings
 *  Array. @see fivestar_custom_widget().
 */
function _fivestar_custom_widget_settings($entity_type, $instance, $display, $id, $tag, $widget) {
  $settings = $display['settings'];
  $settings = array(
    'stars' => (!empty($instance['settings']['stars'])) ? $instance['settings']['stars'] : 5,
    'allow_clear' => (!empty($instance['settings']['allow_clear'])) ? $instance['settings']['allow_clear'] : 0,
    'allow_revote' => (!empty($instance['settings']['allow_revote'])) ? $instance['settings']['allow_revote'] : 0,
    'allow_ownvote' => (!empty($instance['settings']['allow_ownvote'])) ? $instance['settings']['allow_ownvote'] : 0,
    'style' => $settings['style'],
    'text' => $settings['text'],
    'content_type' => $entity_type,
    'content_id' => $id,
    'tag' => $tag,
    'autosubmit' => TRUE,
    'title' => FALSE,
    'labels_enable' => FALSE,
    'labels' => array(),
    'widget' => $widget,
    'description' => $instance['description'],
  );

  return $settings;
}

/**
 * @param $widgets
 *  Array, $widgets = module_invoke_all('fivestar_widgets');
 *  $widgets = array('path/to/css' => 'Widget Name', 'path/to/more/css' => 'Widget 2');
 *
 * @param $display
 *  Array. This is the $display parameter passed to fivestar_field_formatter_view().
 *
 * @param $instance
 *  Array. This is the $instance parameter passed to fivestar_field_formatter_view().
 *
 * @return $widget
 *  Array. $widget = array('name' => 'my widget', 'css' => 'sites/all/mymodule/mywidget.css');
 */
function _fivestar_get_widget($widgets, $display, $instance) {
  // If the type doesn't required widgets lets get out of here.
  // TODO: Implement this WAY better.
  if (in_array($display['type'], array('fivestar_formatter_rating', 'fivestar_formatter_percentage'))) {
    return FALSE;
  }

  // Stars (rated while viewing) is $type = 'exposed'.
  // Stars (rated while editing) is $type = 'stars'.
  $type = $instance['widget']['type'];

  // Determine which widget to display.
  if (!$fivestar_widget = $display['settings']['widget']['fivestar_widget']) {
    // No display has been selected and saved by the user.
    if ($type == 'exposed') {
      // Stars rated while viewing, that is, $type = 'exposed', fall backs on 'default'
      // (which is the same as nothing).
      $fivestar_widget = 'default';
    }
    elseif ($type == 'stars') {
      // Stars rated while editing, that is, $type = stars,
      // falls back on whatever the user selected to be displayed on node/add and node/%/edit
      $fivestar_widget = $instance['widget']['settings']['widget']['fivestar_widget'];
    }
  }

  $widget = array(
    'name' => isset($widgets[$fivestar_widget]) ? strtolower($widgets[$fivestar_widget]) : 'default',
    'css' => $fivestar_widget,
  );

  return $widget;
}

/**
 * Implements hook_field_prepare_view().
 */
function fivestar_field_prepare_view($entity_type, $entities, $field, $instances, $langcode, &$items) {
  // TODO: Clean this function up!
  $exposed_widgets = array();
  foreach ($entities as $id => $entity) {
    // Ensure the items aren't processed twice.
    if (!isset($items[$id][0]['count'])) {
      if ($instances[$id]['widget']['type'] == 'exposed') {
        $exposed_widgets[] = $id;
      }
      else {
        // If the widget type is not exposed, then the count is always 1 or 0.
        // The count is pointless to display.
        if (!empty($items[$id][0]['rating'])) {
          $values['count'] = 1;
          $values['user'] = $items[$id][0]['rating'];
          $values['average'] = $items[$id][0]['rating'];
        }
        else {
          $values['count'] = 0;
          $values['user'] = 0;
          $values['average'] = 0;
        }
        $items[$id] = array($values);
      }
    }
    if(!empty($exposed_widgets)) {
      $votes = fivestar_get_votes_multiple($entity_type, $exposed_widgets, $field['settings']['axis']);
      foreach($votes[$entity_type] as $id => $vote) {
        // Populating the $items[$id] array even for items with no value forces
        // the render system to output a widget.
        $values['user'] = isset($vote['user']['value']) ? $vote['user']['value'] : 0;
        $values['average'] = isset($vote['average']['value']) ? $vote['average']['value'] : 0;
        $values['count'] = isset($vote['count']['value']) ? $vote['count']['value'] : 0;
        $items[$id] = array($values);
      }
    }
  }
}

/**
 * Implements hook_microdata_value_type_alter().
 */
function fivestar_microdata_value_types_alter(&$types) {
  $types['fivestar'] = 'item_option';
}

/**
 * Callback to alter the property info of fivestar fields.
 */
function fivestar_property_info_callback(&$info, $entity_type, $field, $instance, $field_type) {
  $name = $field['field_name'];
  $property = &$info[$entity_type]['bundles'][$instance['bundle']]['properties'][$name];

  $property['property info'] = array(
    'average_rating' => array(
      'label' => t('Average Rating'),
      'type' => 'text',
      'microdata' => TRUE,
    ),
    'user_rating' => array(
      'label' => t('User\'s Rating'),
      'type' => 'text',
      'microdata' => TRUE,
    ),
    'rating_count' => array(
      'label' => t('Rating count'),
      'type' => 'text',
      'microdata' => TRUE,
    ),
  );
}

/**
 * Get microdata attributes for Fivestar field.
 */
function _fivestar_get_microdata_property_info($entity_type, $entity, $field, $instance) {
  // If microdata module is enabled, attach the microdata attributes the module
  // adds to the entity object.
  if (module_exists('microdata')) {
    $microdata = $entity->microdata[$field['field_name']];
  }
  // If microdata is not enabled, add empty arrays for each property so we
  // don't have to check later in the theme functions.
  else {
    $microdata = array(
      '#attributes' => array(),
    );
    // If Entity API is enabled, we can use that to get the properties.
    // Otherwise, replicate the Entity API logic for getting the properties.
    if (module_exists('entity')) {
      $entity_info = entity_get_all_property_info($entity_type);
    }
    else {
      $info = array();
      $info[$entity_type]['bundles'][$instance['bundle']]['properties'][$field['field_name']] = array();
      fivestar_property_info_callback($info, $entity_type, $field, $instance, 'fivestar');
      $entity_info = $info[$entity_type]['bundles'][$instance['bundle']]['properties'];
    }
    foreach ($entity_info[$field['field_name']]['property info'] as $property_name => $property) {
      $microdata[$property_name]['#attributes'] = array();
    }
  }

  return $microdata;
}
