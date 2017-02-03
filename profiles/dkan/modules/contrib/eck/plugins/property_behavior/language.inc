<?php
/**
 * @file
 * Implementation of Language property behavior, which represents the language 
 * of an entity.
 */

$plugin = array(
  'label' => "Language",
  'entity_info' => 'eck_language_property_entity_info',
  'entity_save' => 'eck_language_property_entity_save',
  'property_info' => 'eck_language_property_property_info',
  'default_widget' => 'eck_language_property_widget',
  'views_data_alter' => 'eck_language_property_views_data_alter',
  'bundle_form' => 'eck_language_property_bundle_form',
  'unique' => TRUE,
);

/**
 * Letting the system know what property is the language key of the entity.
 */
function eck_language_property_entity_info($property, $var) {
  $info = $var;
  // Nodes also put its language into entity keys, so we do the same.
  $info['entity keys']['language'] = $property;
  return $info;
}

/**
 * When saving the property, only one language can be set, and only once.
 */
function eck_language_property_entity_save($property, $vars) {
  $entity = $vars['entity'];
  if (module_exists('locale')) {
    if (isset($entity->is_new) && $entity->is_new && empty($entity->{$property})) {
      $default = language_default();
      $entity->{$property} = $default->language;
    }
  }
  else {
    $entity->{$property} = LANGUAGE_NONE;
  }
}

/**
 * Extending property's info default information.
 */
function eck_language_property_property_info($property, $vars) {
  if (isset($vars['properties'][$property])) {
    $vars['properties'][$property] += array(
      'type' => 'token',
      'options list' => 'entity_metadata_language_list',
      'description' => t('Entity language.'),
    );
  }
  return $vars;
}

/**
 * Language widget.
 * 
 * Select one of the enabled languages.
 */
function eck_language_property_widget($property, $vars) {
  if (module_exists('locale')) {
    $entity = $vars['entity'];
    $language = isset($entity->{$property}) ? $entity->{$property} : '';
    if (eck_language_bundle_multilingual($entity)) {
      return array(
        '#type' => 'select',
        '#title' => $vars['properties'][$property]['label'],
        '#default_value' => $language,
        '#options' => array(LANGUAGE_NONE => t('Language neutral')) + locale_language_list('name'),
      );
    }
    else {
      $default = language_default();
      return array(
        '#type' => 'value',
        '#value' => !empty($language) ? $language : $default->language,
      );
    }
  }
}

/**
 * What?.
 */
function eck_language_bundle_multilingual($entity) {
  if ($bundle = Bundle::loadByMachineName($entity->entityType() . '_' . $entity->bundle())) {
    return isset($bundle->config['multilingual']) && $bundle->config['multilingual'];
  }
  return FALSE;
}

/**
 * Informing views that this property contains language information.
 */
function eck_language_property_views_data_alter($property, $vars) {
  $entity_type = $vars['entity_type'];
  if (module_exists('locale')) {
    $property_info = $entity_type->properties[$property];
    $vars['data']["eck_{$entity_type->name}"][$property] = array(
      'title' => $property_info['label'],
      'help' => t('Entity language'),
      'field' => array(
        'handler' => 'views_handler_field_locale_language',
        'click sortable' => TRUE,
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_locale_language',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_locale_language',
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
    );
  }
  else {
    unset($vars['data']["eck_{$entity_type->name}"][$property]);
  }
  return $vars;
}

/**
 * Setting multilingual options in bundle form.
 * 
 * @todo Why is the behavior setting things in the bundle form?
 */
function eck_language_property_bundle_form($property, $vars) {
  if (module_exists('locale')) {
    $entity_type = $vars['form']['entity_type']['#value'];
    $bundle = $vars['form']['bundle']['#value'];
    $property_info = $entity_type->properties[$property];
    $vars['form']['config_multilingual'] = array(
      '#type' => 'radios',
      '#title' => t('Multilingual support'),
      '#default_value' => isset($bundle->config["multilingual"]) ? $bundle->config["multilingual"] : 0,
      '#options' => array(t('Disabled'), t('Enabled')),
      '#description' => t('Enable multilingual support for this bundle. If enabled, a language selection field will be added to the editing form, allowing you to select from one of the <a href="!languages">enabled languages</a>. If disabled, new posts are saved with the default language. Existing content will not be affected by changing this option.', array('!languages' => url('admin/config/regional/language'))),
    );
  }
  return $vars;
}
