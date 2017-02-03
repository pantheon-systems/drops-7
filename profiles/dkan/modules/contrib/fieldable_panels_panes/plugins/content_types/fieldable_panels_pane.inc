<?php

/**
 * @file
 *
 * CTools content type to render a fielded panel pane.
 */

/**
 * Small hook implementation of plugin.
 *
 * We have to use this because the form here can be loaded via form
 * caching and if this .inc file is loaded before the plugin is
 * requested, the $plugin = array() notation doesn't work.
 */
function fieldable_panels_panes_fieldable_panels_pane_ctools_content_types() {
  return array(
    'title' => t('Fielded custom content'),
    'no title override' => TRUE,
    'description' => t('Create custom panels pane with fields'),
    'category' => t('Fielded panes'),
    'all contexts' => TRUE,
    'defaults' => array(
      'view_mode' => 'full',
    ),

    // Callback to load the FPP.
    'content type' => 'fieldable_panels_panes_fieldable_panels_pane_content_type',

    // Functionality for editing an FPP.
    'edit text' => t('Edit'),
    // Form API callback.
    'edit form' => 'fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form',
    // Access callback.
    'check editable' => 'fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_access',
  );
}
// --------------------------------------------------------------------------
// Callbacks, many of them automatically named, for rendering content.

/**
 * Return an individual FPP.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type($subtype_id, $plugin) {
  $content_type = array();

  // If an ID was passed in, try loading a corresponding FPP.
  if (strpos($subtype_id, ':') !== FALSE) {
    // If a specific FPP was requested, load it. Use the 'force' method to
    // avoid inadvertently triggering an infinite loop.
    $entity = fieldable_panels_panes_load_from_subtype_force($subtype_id);

    // The FPP could be loaded, so generate its content type definition.
    if (!empty($entity)) {
      $content_type = _fieldable_panels_panes_custom_content_type($entity);
    }

    // This shouldn't happen, but there may be situations where the FPP has been
    // deleted but is still referenced in a Panels display.
    else {
      return $content_type;
    }
  }

  // If nothing was loaded yet,
  if (empty($content_type)) {
    $type = 'fieldable_panels_pane';
    $subtypes = ctools_content_get_subtypes($type);
    if (isset($subtypes[$subtype_id])) {
      $content_type = $subtypes[$subtype_id];
    }
    // If there's only 1 and we somehow have the wrong subtype ID, do not
    // care. Return the proper subtype anyway.
    if (empty($content_type) && !empty($plugin['single'])) {
      $content_type = current($subtypes);
    }
  }

  return $content_type;
}

/**
 * Callback to return the custom content types with the specified $subtype_name.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_content_type($subtype_name) {
  $types = _fieldable_panels_panes_default_content_type();
  if (isset($types[$subtype_name])) {
    return $types[$subtype_name];
  }
  else {
    $entity = fieldable_panels_panes_load_from_subtype($subtype_name);
    if (!empty($entity)) {
      return _fieldable_panels_panes_custom_content_type($entity);
    }
  }
}

/**
 * Callback to return all custom content types available.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_content_types() {
  $types = _fieldable_panels_panes_default_content_type();

  // The current function is called as part of the standard panel pane rendering
  // pipeline. To prevent a full list of FPP entities from being generated every
  // time that a panel is rendered, generate the list only when a request is
  // made from an administrative URL.
  // @see https://www.drupal.org/node/2374577
  if (path_is_admin(current_path()) || preg_match('/panels/i', current_path())) {
    $fpp_info = entity_get_info('fieldable_panels_pane');
    foreach ($fpp_info['bundles'] as $bundle_name => $bundle) {
      $ids = db_query('SELECT fpid FROM {fieldable_panels_panes} WHERE reusable = 1 AND bundle = :bundle', array(':bundle' => $bundle_name))->fetchCol();
      if (!empty($ids)) {
        $types = array_merge($types, fieldable_panels_panes_build_content_type_info($bundle_name, $ids));
      }
    }
  }

  return $types;
}

/**
 * Returns a list of all reusable FPP entities of a given bundle.
 *
 * @param $bundle
 *   Fieldable panel pane bundle machine name.
 * @param $ids
 *   An array of fieldable panel pane entity ids.
 *
 * @return array
 *   An array of content type information for each fpp entity.
 */
function fieldable_panels_panes_build_content_type_info($bundle, $ids) {
  $types = array();
  $cid = "fieldable_panels_panes_{$bundle}_content_type";

  if (($cache = cache_get($cid, 'cache_panels')) && !empty($cache->data)) {
    $types = $cache->data;
  }
  else {
    $entities = fieldable_panels_panes_load_multiple($ids);
    if (!empty($entities)) {
      foreach ($entities as $entity_id => $entity) {
        $subtype = _fieldable_panels_panes_custom_content_type($entity);
        $types[$subtype['name']] = $subtype;

        // Re-key $entities such that $types and $entities are keyed
        // identically.
        $entities[$subtype['name']] = $entity;
        unset($entities[$entity_id]);
      }
    }

    // Trigger hook_fieldable_panels_panes_content_types_alter().
    drupal_alter('fieldable_panels_panes_content_types', $types, $bundle, $entities);
    cache_set($cid, $types, 'cache_panels');
  }

  return $types;
}

/**
 * Callback to render our content type.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_render($subtype, $conf, $panel_args = array(), $context = array()) {
  $entity = fieldable_panels_panes_load_from_subtype($subtype);
  if (!empty($entity) && !empty($entity->fpid) && fieldable_panels_panes_access('view', $entity)) {
    $view_mode = isset($conf['view_mode']) ? $conf['view_mode'] : 'full';
    $settings = field_bundle_settings('fieldable_panels_pane', $entity->bundle);
    // Add pane config to the entity object before hook_view().
    $entity->conf = $conf;

    $block = new stdClass();
    $block->title = '';
    $block->content = fieldable_panels_panes_view($entity, $view_mode);

    // Was there a title defined?
    if (!empty($entity->title)) {
      // Is this view mode configured?
      $is_view_mode_set = isset($settings['extra_fields']['display']['title'][$view_mode]['visible']);

      // Is the title configured for this view mode?
      $show_view_mode_title = ($is_view_mode_set && $settings['extra_fields']['display']['title'][$view_mode]['visible']);

      // Should the default view mode show the title?
      $show_default_title = !empty($settings['extra_fields']['display']['title']['default']['visible']);

      // Combine all of the above logic.
      if (empty($settings['extra_fields']['display']) || (!$is_view_mode_set && $show_default_title) || $show_view_mode_title) {
        $block->title = filter_xss_admin($entity->title);
      }
    }

    return $block;
  }
}

/**
 * Gets the admin title from the entity.
 */
function _fieldable_panels_panes_admin_title_from_entity($entity) {
  if (!empty($entity) && is_object($entity)) {
    if (!empty($entity->admin_title)) {
      $output = $entity->admin_title;
    }
    elseif (!empty($entity->title)) {
      $output = $entity->title;
    }
    else {
      $output = entity_label('fieldable_panels_pane', $entity);
    }
    // Make sure no XSS strings go through the title.
    $output = filter_xss($output);
  }
  else {
    $output = t('Deleted/removed entity pane');
  }

  // Indicate when the FPP is reusable.
  if (!empty($entity->reusable)) {
    $output .= ' (' . t('reusable') . ')';
  }

  return $output;
}

/**
 * Callback to provide the administrative title of the custom content.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_admin_title($subtype, $conf) {
  $entity = fieldable_panels_panes_load_from_subtype($subtype);
  return _fieldable_panels_panes_admin_title_from_entity($entity);
}

/**
 * Callback to provide administrative information for a fieldable panels pane.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_admin_info($subtype, $conf) {
  return fieldable_panels_panes_fieldable_panels_pane_content_type_render($subtype, $conf);
}

/**
 * Form used to edit our content type.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form($form, &$form_state) {
  $conf = &$form_state['conf'];

  if (!isset($form_state['entity'])) {
    $form_state['entity'] = fieldable_panels_panes_load_from_subtype($form_state['subtype_name']);
  }
  $entity = $form_state['entity'];

  // It's possible that we have a reference to an entity that is no longer
  // valid. If so, bail, because otherwise field API will whitescreen.
  if (empty($entity)) {
    $form['error'] = array(
      '#markup' => t('The pane entity referenced does not appear to be valid. It was probably deleted and you should remove this pane.'),
    );
    return $form;
  }

  ctools_form_include_file($form_state, $form_state['plugin']['path'] . '/' . $form_state['plugin']['file']);

  $entity_info = entity_get_info('fieldable_panels_pane');

  // Show all of the available view modes.
  foreach ($entity_info['view modes'] as $mode => $option) {
    $view_mode_options[$mode] = $option['label'];
  }
  $form['view_mode'] = array(
    '#title' => t('View mode'),
    '#type' => 'select',
    '#description' => t('Select a view mode for this pane.'),
    '#options' => $view_mode_options,
    '#default_value' => $conf['view_mode'],
  );

  // If we're adding a reusable type, the only thing we want on the form is
  // the view mode, so skip the rest.
  if ($form_state['op'] == 'add' && !empty($form_state['subtype']['entity_id'])) {
    $form_state['no update entity'] = TRUE;
    return $form;
  }

  $form = fieldable_panels_panes_entity_edit_form($form, $form_state);

  $form['reusable']['warning'] = array(
    '#markup' => '<div class="description">' . t('Note: Editing any value on a reusable pane will change the value everywhere this pane is used.') . '</div>',
  );

  return $form;
}

/**
 * Validate submission of our content type edit form.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_validate($form, &$form_state) {
  if (!empty($form_state['no update entity'])) {
    return;
  }

  if ($form_state['entity']) {
    fieldable_panels_panes_entity_edit_form_validate($form, $form_state);
  }
}

/**
 * Submit our content type edit form.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['view_mode'] = $form_state['values']['view_mode'];

  if (!empty($form_state['no update entity'])) {
    return;
  }

  $entity = $form_state['entity'];
  if (!$entity) {
    return;
  }

  fieldable_panels_panes_entity_edit_form_submit($form, $form_state);

  // Determine how to handle revision locking.
  $revision_context_aware = fieldable_panels_panes_revision_is_lockable($entity);

  // If this is a new entity entity, or revision locking is enabled, look for a
  // specific ID to use.
  if (!empty($entity->is_new) || $revision_context_aware) {
    // If UUID is available, use it.
    if (module_exists('uuid') && isset($entity->uuid)) {
      if ($revision_context_aware) {
        $subtype = 'vuuid:' . $entity->vuuid;
      }
      else {
        $subtype = 'uuid:' . $entity->uuid;
      }
    }

    // Otherwise use the native ID values.
    else {
      if ($revision_context_aware) {
        $subtype = 'vid:' . $entity->vid;
      }
      else {
        $subtype = 'fpid:' . $entity->fpid;
      }
    }
  }

  // The 'current' key will tell FPP to load up the current revision. This
  // bypasses the entity cache on the FPP entity class and loads the revision
  // that was just saved instead of the previous revision. This is only really
  // relevant in Panels IPE administration or similar circumstances.
  else {
    $subtype = 'current:' . $entity->fpid;
  }

  // @todo: This won't work if $form_state does not contain 'pane' which could
  // theoretically happen in a non-Panels use case. Not that anybody uses this
  // outside of Panels.
  $form_state['pane']->subtype = $subtype;
}

/**
 * Callback for the 'edit' permission.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_edit_form_access($content_type, $subtype, $view_mode = 'full') {
  return fieldable_panels_panes_check_access_update($subtype);
}

// --------------------------------------------------------------------------
// Internal methods used by the above callbacks.

/**
 * Provide the default content types.
 *
 * These are all visible in the UI as the content types that allow a user
 * to add new panel pane entities that will then be stored in the database.
 */
function _fieldable_panels_panes_default_content_type() {
  $types = array();

  $entity_info = entity_get_info('fieldable_panels_pane');

  foreach ($entity_info['bundles'] as $bundle => $info) {
    $types[$bundle] = array(
      'name' => $bundle,
      'title' => $info['label'],
      'category' => !empty($info['pane category']) ? $info['pane category'] : t('Fielded panes'),
      'top level' => !empty($info['pane top level']) ? $info['pane top level'] : FALSE,
      'icon' => !empty($info['pane icon']) ? $info['pane icon'] : NULL,
      'description' => !empty($info['description']) ? t($info['description']) : t('Create a new custom entity.'),
      'all contexts' => TRUE,
      'bundle' => $bundle,
      'create content access' => 'fieldable_panels_panes_content_type_create_access',
    );
  }

  return $types;
}

/**
 * Return an info array for a specific custom content type.
 */
function _fieldable_panels_panes_custom_content_type($entity) {
  $info = array(
    'title' => _fieldable_panels_panes_admin_title_from_entity($entity),
    'description' => check_plain($entity->admin_description),
    'category' => $entity->category ? check_plain($entity->category) : t('Miscellaneous'),
    'all contexts' => TRUE,
    'icon' => 'icon_block_custom.png',
  );

  // Determine how to handle revision locking.
  $revision_context_aware = fieldable_panels_panes_revision_is_lockable($entity);

  // If UUID is available, use it.
  if (module_exists('uuid') && isset($entity->uuid)) {
    if ($revision_context_aware) {
      $info['name'] = 'vuuid:' . $entity->vuuid;
    }
    else {
      $info['name'] = 'uuid:' . $entity->uuid;
    }
  }

  // Otherwise use the native ID values.
  else {
    if ($revision_context_aware) {
      $info['name'] = 'vid:' . $entity->vid;
    }
    else {
      $info['name'] = 'fpid:' . $entity->fpid;
    }
  }

  $info['entity_id'] = $info['name'];
  return $info;
}

/**
 * Access callback for creating a new content type.
 */
function fieldable_panels_panes_content_type_create_access($type, $subtype) {
  return fieldable_panels_panes_access('create', $subtype['name']);
}
