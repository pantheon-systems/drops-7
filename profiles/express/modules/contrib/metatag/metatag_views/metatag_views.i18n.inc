<?php
/**
 * @file
 * Internationalization (i18n) hooks.
 */

/**
 * Implements hook_i18n_object_info().
 */
function metatag_views_i18n_object_info() {
  $info['metatag_views'] = array(
    'title' => t('Metatag:Views configurations'),
    // Callback to load all config objects.
    'list callback' => 'metatag_views_i18n_list_displays',
    // The object load callback.
    // 'load callback' => 'metatag_views_i18n_load',
    // Custom i18n object overrides. Right now this avoids problems with the
    // object ID as defined by get_string_context().
    'class' => 'metatag_views_i18n_metatag',
    // @todo Is this needed? What does it do?
    // 'translation set' => TRUE,

    // The object key field; multiple values will be concatenated with ":" as
    // the separator.
    'key' => array('vid', 'id'),
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
      'type' => 'metatag_views',
      // Table where the object is stored, to automate string lists.
      // 'table' => 'views_display',
      // Translatable properties of these objects, this will be added later.
      'properties' => array(),
      // The path to translate individual strings.
      // 'translate path' => 'admin/config/search/metatags/config/%instance/translate/%i18n_language',
    ),
  );

  // Compile all of the tags to add to the translation stack.
  $meta_tag_info = metatag_get_info();
  $groups = $meta_tag_info['groups'];
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
      $title = $group_label . ': '. $title;
    }

    $info['metatag_views']['string translation']['properties'][$tag_name] = array(
      'title' => $title,
      'field' => "display_options.metatags.und.{$tag_name}.value",
    );
  }

  return $info;
}

/**
 * List callback.
 */
function metatag_views_i18n_list_displays() {
  $displays = array();

  foreach (views_get_all_views() as $view_id => $view) {
    foreach ($view->display as $display) {
      if (!empty($display->display_options['metatags'][LANGUAGE_NONE])) {
        // Need to rig i18n_string's ability to find the record.
        $display->view_id = $view_id;
        $displays[$display->view_id . METATAG_VIEWS_CONTEXT_SEPARATOR . $display->id] = $display;
      }
    }
  }

  return $displays;
}
