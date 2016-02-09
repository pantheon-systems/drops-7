<?php
/**
 * @file
 * Internationalization (i18n) hooks.
 */

/**
 * Implements hook_i18n_string_info().
 */
function metatag_i18n_string_info() {
  // Text groups.
  $groups['metatag'] = array(
    'title' => t('Metatag'),
    'description' => t('Configurable meta tags.'),

    // This group does not have strings with text formats.
    'format' => FALSE,

    // This group can list all strings.
    // @todo What does this actually do?
    'list' => TRUE,
  );
  return $groups;
}

/**
 * Implements hook_i18n_object_info().
 */
function metatag_i18n_object_info() {
  // Compile all of the tags to add to the translation stack.
  $meta_tag_info = metatag_get_info();
  $groups = $meta_tag_info['groups'];
  $config_properties = $output_properties = array();
  foreach ($meta_tag_info['tags'] as $tag_name => $tag_info) {
    // Ignore certain field types that aren't translatable, mostly fields that
    // list predetermined options in various forms.
    if (!empty($tag_info['class']) && $tag_info['class'] == 'DrupalListMetaTag') {
      continue;
    }
    elseif (!empty($tag_info['form']['#type']) && $tag_info['form']['#type'] == 'select') {
      continue;
    }
    elseif (!empty($tag_info['form']['#options'])) {
      continue;
    }

    // Build a suitable structure for this meta tag.
    if (!empty($tag_info['group']) && !empty($groups[$tag_info['group']]['label'])) {
      $group_label = $groups[$tag_info['group']]['label'] . ': ';
    }
    elseif (!empty($tag_info['group'])) {
      $group_label = $tag_info['group'] . ': ';
    }
    else {
      $group_label = '';
    }

    // Configuration items.
    $config_properties[$tag_name] = array(
      'title' => $group_label . $tag_info['label'],
      'field' => "config.{$tag_name}.value",
    );

    // Output items.
    $output_properties[$tag_name] = array(
      'title' => $group_label . ': ' . $tag_info['label'],
      'field' => "data.{$tag_name}.value",
    );
  }

  // The main Metatag configuration items.
  $info['metatag_config'] = array(
    'title' => t('Metatag configuration'),
    // Callback to load all config objects.
    'list callback' => 'metatag_i18n_list_metatag_config',
    // The object load callback.
    'load callback' => 'metatag_config_load',
    // @todo Custom i18n object overrides.
    // 'class' => 'metatag_i18n_metatag',
    // @todo Is this needed? What does it do?
    // 'translation set' => TRUE,

    // The object key field.
    'key' => 'instance',
    // Placeholders for automatic paths. This connects the 'key' to strings in
    // the paths listed below.
    'placeholders' => array(
      '%instance' => 'instance',
    ),
    // To produce edit links automatically.
    'edit path' => 'admin/config/search/metatags/config/%instance',
    // Auto-generate a 'translate' tab.
    'translate tab' => 'admin/config/search/metatags/config/%instance/translate',

    // Properties for string translation.
    'string translation' => array(
      // The textgroup, type and (below) name will be concatenated into a single
      // string as the {locales_source} context.
      'textgroup' => 'metatag',
      'type' => 'metatag_config',
      // Table where the object is stored, to automate string lists.
      'table' => 'metatag_config',
      // Translatable properties of these objects.
      'properties' => $config_properties,
      // The path to translate individual strings.
      'translate path' => 'admin/config/search/metatags/config/%instance/translate/%i18n_language',
    ),
  );

  // The final meta tags being output on the page.
  if (variable_get('metatag_i18n_translate_output', FALSE)) {
    $info['metatag_output'] = array(
      'title' => t('Metatag final tag output'),

      // Properties for string translation.
      'string translation' => array(
        // The textgroup, type and (below) name will be concatenated into a
        // single string as the {locales_source} context.
        'textgroup' => 'metatag',
        'type' => 'metatag_output',
        // Translatable properties of these objects.
        'properties' => $output_properties,
      ),
    );
  }

  return $info;
}

/**
 * List callback for {metatag_config} strings.
 */
function metatag_i18n_list_metatag_config() {
  ctools_include('export');
  $configs = ctools_export_crud_load_all('metatag_config');

  if (!empty($configs)) {
    // Unserialize the config array.
    foreach ($configs as &$config) {
      if (is_string($config->config)) {
        $config->config = unserialize($config->config);
      }
    }
  }

  return $configs;
}
