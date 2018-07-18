<?php

/**
 * @file
 * Internationalization (i18n) hooks.
 */

/**
 * Implements hook_i18n_object_info().
 */
function metatag_panels_i18n_object_info() {
  // Compile all of the tags to add to the translation stack.
  $meta_tag_info = metatag_get_info();
  $groups = $meta_tag_info['groups'];
  $properties = array();
  foreach ($meta_tag_info['tags'] as $tag_info) {
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
    $tag_name = $tag_info['name'];
    $title = $tag_info['label'];
    if (!empty($tag_info['group'])) {
      $tag_group = $tag_info['group'];
      $group_label = !empty($groups[$tag_group]['label']) ? $groups[$tag_group]['label'] : $tag_group;
      $title = $group_label . ': ' . $title;
    }

    $properties[$tag_name] = array(
      'title' => $title,
      'field' => "conf.metatag_panels.metatags.{$tag_name}.value",
    );
  }

  $info['metatag_panels'] = array(
    'title' => t('Metatag:Panels configurations'),
    // Callback to load all config objects.
    'list callback' => 'metatag_panels_i18n_list_panels',
    // The object load callback.
    // 'load callback' => 'metatag_panels_i18n_load',
    // @todo Custom i18n object overrides.
    // 'class' => 'metatag_panels_i18n_metatag',
    // @todo Is this needed? What does it do?
    // 'translation set' => TRUE,
    // The object's "key" field, this tells i18n_string to use the $panel->name
    // attribute.
    'key' => 'name',
    // Placeholders for automatic paths. This connects the 'key' to strings in
    // the paths listed below.
    // 'placeholders' => array(
    //   '%did' => 'did',
    // ),
    // To produce edit links automatically.
    // 'edit path' => 'admin/config/search/metatags/config/%instance',
    // Auto-generate a 'translate' tab.
    // 'translate tab' => 'admin/config/search/metatags/config/%instance/translate',
    // Properties for string translation.
    'string translation' => array(
      // The textgroup, type and (below) name will be concatenated into a single
      // string as the {locales_source} context.
      'textgroup' => 'metatag',
      'type' => 'metatag_panels',
      // Table where the object is stored, to automate string lists.
      // 'table' => 'page_manager_handlers',
      // Translatable properties of these objects, this will be added later.
      'properties' => $properties,
      // The path to translate individual strings.
      // 'translate path' => 'admin/config/search/metatags/config/%instance/translate/%i18n_language',
    ),
  );

  return $info;
}

/**
 * List callback.
 */
function metatag_panels_i18n_list_panels() {
  // Load all of the CTools objects.
  ctools_include('export');
  $all_objects = array();

  // Load the handlers first.
  $handlers = ctools_export_crud_load_all('page_manager_handlers');
  if (!empty($handlers)) {
    // Unserialize the config array.
    foreach ($handlers as $name => $handler) {
      if (!empty($handler)) {
        if (is_string($handler->conf)) {
          $handler->conf = unserialize($handler->conf);
        }
        if (!empty($handler->conf['metatag_panels']['enabled']) && !empty($handler->conf['metatag_panels']['metatags'])) {
          $all_objects[$name] = $handler;
        }
      }
    }
  }

  // Load the pages next, they need extra "handling".
  $pages = ctools_export_crud_load_all('page_manager_pages');
  if (!empty($pages)) {
    // Unserialize the config array.
    foreach ($pages as $key => $page) {
      if (!empty($page) && !empty($page->default_handlers)) {
        foreach ($page->default_handlers as $name => $handler) {
          if (is_string($handler->conf)) {
            $handler->conf = unserialize($handler->conf);
          }
          if (!empty($handler->conf['metatag_panels']['enabled']) && !empty($handler->conf['metatag_panels']['metatags'])) {
            $all_objects[$name] = $handler;
          }
        }
      }
    }
  }

  return $all_objects;
}
