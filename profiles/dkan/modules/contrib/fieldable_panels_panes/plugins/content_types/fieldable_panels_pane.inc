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

  $ids = db_query('SELECT fpid FROM {fieldable_panels_panes} WHERE reusable = 1')->fetchCol();
  if (!empty($ids)) {
    $entities = fieldable_panels_panes_load_multiple($ids);
    if (!empty($entities)) {
      foreach ($entities as $entity) {
        $subtype = _fieldable_panels_panes_custom_content_type($entity);
        $types[$subtype['name']] = $subtype;
      }
    }
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
 * Callback to provide the administrative title of the custom content.
 */
function fieldable_panels_panes_fieldable_panels_pane_content_type_admin_title($subtype, $conf) {
  $entity = fieldable_panels_panes_load_from_subtype($subtype);

  if (!empty($entity) && is_object($entity)) {
    $output = !empty($entity->admin_title) ? $entity->admin_title : (!empty($entity->title) ? $entity->title : t('No title'));
  }
  else {
    $output = t('Deleted/removed entity pane');
  }

  return $output;
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
  $view_mode_options = array();
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

  // Sub type is not explicitly set, define it here.
  if (module_exists('uuid') && isset($entity->uuid)) {
    $entity_id = 'uuid:' . $entity->uuid;
  }
  else {
    $entity_id = 'fpid:' . $entity->fpid;
  }

  // @todo: This won't work if $form_state does not contain 'pane' which could
  // theoretically happen in a non-Panels use case. Not that anybody uses this
  // outside of Panels.
  $form_state['pane']->subtype = $entity_id;
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
    'title' => check_plain(entity_label('fieldable_panels_pane', $entity)),
    'description' => check_plain($entity->admin_description),
    'category' => $entity->category ? check_plain($entity->category) : t('Miscellaneous'),
    'all contexts' => TRUE,
    'icon' => 'icon_block_custom.png',
  );

  if (module_exists('uuid') && isset($entity->uuid)) {
    $info['name'] = 'uuid:' . $entity->uuid;
  }
  else {
    $info['name'] = 'fpid:' . $entity->fpid;
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
