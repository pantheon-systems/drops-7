<?php
/**
 * @file
 * Base class for the Panelizer Entity plugin.
 */

/**
 * Interface to describe how PanelizerEntity plugin objects are implemented.
 */
interface PanelizerEntityInterface {
  /**
   * Initialize the plugin object.
   */
  public function init($plugin);

  // Public Drupal hooks
  public function hook_menu(&$items);
  public function hook_menu_alter(&$items);
  public function hook_form_alter(&$form, &$form_state, $form_id);
  public function hook_permission(&$items);
  public function hook_admin_paths(&$items);
  public function hook_views_data_alter(&$data);

  // Entity specific Drupal hooks
  public function hook_entity_load(&$entities);
  public function hook_entity_insert($entity);
  public function hook_entity_update($entity);
  public function hook_entity_delete($entity);
  public function hook_field_attach_delete_revision($entity);

  /**
   * Add entity specific form to the Panelizer settings form.
   *
   * This is primarily to allow bundle selection per entity type.
   */
  public function settings_form(&$form, &$form_state);

  /**
   * Validate entity specific settings on the Panelizer settings form.
   */
  public function settings_form_validate(&$form, &$form_state);

  /**
   * Submit entity specific settings on the Panelizer settings form.
   */
  public function settings_form_submit(&$form, &$form_state);

  /**
   * Load the named default panel for the bundle.
   */
  public function get_default_panelizer_object($bundle, $name);

  /**
   * Determine if the current user has access to the $panelizer.
   */
  public function access_default_panelizer_object($panelizer);

  /**
   * Determine if a bundle is panelized
   */
  public function is_panelized($bundle);

  /**
   * Determine if a bundle has a defalt panel
   */
  public function has_default_panel($bundle);

  /**
   * Determine if a bundle is allowed choices.
   */
  public function has_panel_choice($bundle);

  /**
   * Get a default display for a newly panelized entity.
   *
   * This is meant to give administrators a starting point when panelizing
   * new entities.
   */
  function get_default_display($bundle, $view_mode);

  /**
   * Get a panelizer object for the key.
   *
   * This must be implemented for each entity type, as the default object
   * implements a special case for handling panelizer defaults.
   */
   // @todo this seems to be unused now.
//  function get_panelizer_object($key);

  /**
   * Render a panelized entity.
   */
  function render_entity($entity, $view_mode, $langcode = NULL, $args = array(), $address = NULL);

  /**
   * Fetch an object array of CTools contexts from panelizer information.
   */
  public function get_contexts($panelizer, $entity = NULL);

  /**
   * Callback to get the base context for a panelized entity
   */
  public function get_base_contexts($entity = NULL);

  /**
   * Determine if the current user has $op access on the $entity.
   */
  public function entity_access($op, $entity);

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity);

  /**
   * Determine if an entity allows revisions and whether or not the current
   * user has access to control that.
   *
   * @param $entity
   *   The entity in question.
   * @return
   *   An array. The first parameter is a boolean as to whether or not the
   *   entity supports revisions and the second parameter is whether or not
   *   the user can control whether or not a revision is created.
   */
  public function entity_allows_revisions($entity);

  /**
   * Get the visible identifier of the identity.
   *
   * This is overridable because it can be a bit awkward using the
   * default label.
   *
   * @return
   *   A translated, safe string.
   */
  public function entity_identifier($entity);

  /**
   * Get the name of bundles on the entity.
   *
   * Entity API doesn't give us a way to determine this, so the class must
   * do this.
   *
   * @return
   *   A translated, safe string.
   */
  public function entity_bundle_label();

  /**
   * Fetch the entity out of a build for hook_entity_view.
   *
   * @param $build
   *   The render array that contains the entity.
   */
  public function get_entity_view_entity($build);
}

/**
 * Base class for the Panelizer Entity plugin.
 */
abstract class PanelizerEntityDefault implements PanelizerEntityInterface {
  /**
   * Where in the entity admin UI we should add Panelizer tabs with bundles.
   */
  public $entity_admin_root = NULL;

  /**
   * True if the entity supports revisions.
   */
  public $supports_revisions = FALSE;

  /**
   * The base table in SQL the entity uses, for views support.
   */
  public $views_table = '';

  /**
   * The plugin metadata.
   */
  public $plugin = NULL;

  /**
   * The entity type the plugin is for. This is from the $plugin array.
   */
  public $entity_type = '';

  /**
   * Initialize the plugin and store the plugin info.
   */
  function init($plugin) {
    $this->plugin = $plugin;
    $this->entity_type = $plugin['name'];
  }

  /**
   * Implements a delegated hook_permission.
   */
  public function hook_permission(&$items) {
    $entity_info = entity_get_info($this->entity_type);
    // Make a permission for each bundle we control.
    foreach ($this->plugin['bundles'] as $bundle => $settings) {
      // This is before the if because it shows up regardless of whether
      // or not a type is panelized.
      $items["administer panelizer $this->entity_type $bundle defaults"] = array(
        'title' => t('%entity_name %bundle_name: Administer Panelizer default panels, allowed content and settings.', array(
          '%entity_name' => $entity_info['label'],
          '%bundle_name' => $entity_info['bundles'][$bundle]['label'],
        )),
        'description' => t('Users with this permission can fully administer panelizer for this entity bundle.'),
      );

      if (empty($settings['status'])) {
        continue;
      }

      $items["administer panelizer $this->entity_type $bundle overview"] = array(
        'title' => t('%entity_name %bundle_name: Administer Panelizer overview', array(
          '%entity_name' => $entity_info['label'],
          '%bundle_name' => $entity_info['bundles'][$bundle]['label'],
        )),
        'description' => t('Allow access to the panelizer overview page for the entity type/bundle. Note: This permission will be required for panelizer tabs to appear on an entity.'),
      );
      foreach (panelizer_operations() as $path => $operation) {
        $items["administer panelizer $this->entity_type $bundle $path"] = array(
          'title' => t('%entity_name %bundle_name: Administer Panelizer @operation', array(
            '%entity_name' => $entity_info['label'],
            '%bundle_name' => $entity_info['bundles'][$bundle]['label'],
            '@operation' => $operation['link title'],
          )),
        );
      }

      if (!empty($settings['choice'])) {
        $items["administer panelizer $this->entity_type $bundle choice"] = array(
          'title' => t('%entity_name %bundle_name: Choose panels', array(
            '%entity_name' => $entity_info['label'],
            '%bundle_name' => $entity_info['bundles'][$bundle]['label'],
          )),
          'description' => t('Allows the user to choose which default panel the entity uses.'),
        );
      }
    }
  }

  /**
   * Implements a delegated hook_menu.
   */
  public function hook_menu(&$items) {
    if (!empty($this->plugin['entity path'])) {
      // Figure out where in the path the entity will be.
      $bits = explode('/', $this->plugin['entity path']);
      foreach ($bits as $count => $bit) {
        if (strpos($bit, '%') === 0) {
          $position = $count;
          break;
        }
      }

      if (!isset($position)) {
        return;
      }

      $total = count($bits);

      // Configure entity editing pages
      $base = array(
        'access callback' => 'panelizer_entity_plugin_callback_switcher',
        'access arguments' => array($this->entity_type, 'access', 'admin', $position, 'overview'),
        'type' => MENU_LOCAL_TASK,
      );

      $items[$this->plugin['entity path'] . '/panelizer'] = array(
        'title' => 'Panelizer',
        // make sure this is accessible to panelize entities with no defaults.
        'page callback' => 'panelizer_entity_plugin_switcher_page',
        'page arguments' => array($this->entity_type, 'overview', $position),
        'weight' => 11,
        'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      ) + $base;

      $items[$this->plugin['entity path'] . '/panelizer/overview'] = array(
        'title' => 'Overview',
        'page callback' => 'panelizer_entity_plugin_switcher_page',
        'page arguments' => array($this->entity_type, 'overview', $position),
        'type' => MENU_DEFAULT_LOCAL_TASK,
        'weight' => 11,
      ) + $base;

      // Put in all of our view mode based paths.
      $weight = 0;
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        $items[$this->plugin['entity path'] . "/panelizer/$view_mode"] = array(
          'title' => $view_mode_info['label'],
          'page callback' => 'panelizer_entity_plugin_switcher_page',
          'page arguments' => array($this->entity_type, 'settings', $position, $view_mode),
          'access arguments' => array($this->entity_type, 'access', 'admin', $position, 'settings', $view_mode),
          'weight' => $weight++,
        ) + $base;

        foreach (panelizer_operations() as $path => $operation) {
          $items[$this->plugin['entity path'] . '/panelizer/' . $view_mode . '/' . $path] = array(
            'title' => $operation['menu title'],
            'page callback' => 'panelizer_entity_plugin_switcher_page',
            'page arguments' => array($this->entity_type, $path, $position, $view_mode),
            'access arguments' => array($this->entity_type, 'access', 'admin', $position, $path, $view_mode),
            'weight' => $weight++,
          ) + $base;
          if (isset($operation['file'])) {
            $items[$this->plugin['entity path'] . '/panelizer/' . $view_mode . '/' . $path]['file'] = $operation['file'];
          }
          if (isset($operation['file path'])) {
            $items[$this->plugin['entity path'] . '/panelizer/' . $view_mode . '/' . $path]['file path'] = $operation['file path'];
          }
        }
        // Add our special reset item:
        $items[$this->plugin['entity path'] . '/panelizer/' . $view_mode . '/reset'] = array(
          'title' => t('Reset to Defaults'),
          'page callback' => 'panelizer_entity_plugin_switcher_page',
          'page arguments' => array($this->entity_type, 'reset', $position, $view_mode),
          'type' => MENU_CALLBACK,
        ) + $base;
      }
    }

    // Also add administrative links to the bundle.
    if (!empty($this->entity_admin_root)) {
      $this->add_admin_links($this->entity_admin_root, $this->entity_admin_bundle, $items);
    }
  }

  /**
   * Helper function to add administrative menu items into an entity's already existing structure.
   *
   * While this very closely follows the administrative items placed into the
   * menu in admin.inc, it is a little bit different because of how bundles
   * are placed into the URL. So the code is close but not QUITE reusable
   * without going through some hoops.
   *
   * @param $root
   *   The root path. This will be something like 'admin/structure/types/manage/%'.
   *   Everything will be placed at $root/panelizer/*.
   * @param $bundle
   *   This is either the numeric position of the bundle or, for entity types
   *   that do not support bundles, a hard coded bundle string.
   * @param &$items
   *   The array of menu items this is being added to.
   */
  public function add_admin_links($root, $bundle, &$items) {
    // Node $root = 'admin/structure/types/manage/%
    // Taxonomy $root = 'admin/structure/taxonomy/%'
    // User $root = 'admin/config/people/accounts'
    $parts = explode('/', $root);
    $base_count = count($parts);

    // Configure settings pages.
    $settings_base = array(
      'access callback' => 'panelizer_is_panelized',
      'access arguments' => array($this->entity_type, $bundle),
      'file' => 'includes/admin.inc',
    );

    // This is the base tab that will be added. The weight is set
    // to try and make sure it stays to the right of manage fields
    // and manage display.
    $items[$root . '/panelizer'] = array(
      'title' => 'Panelizer',
      'page callback' => 'panelizer_allowed_content_page',
      'page arguments' => array($this->entity_type, $bundle),
      'type' => MENU_LOCAL_TASK,
      'weight' => 5,
    ) + $settings_base;

    $items[$root . '/panelizer/allowed'] = array(
      'title' => 'Allowed content',
      'page callback' => 'panelizer_allowed_content_page',
      'page arguments' => array($this->entity_type, $bundle),
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
    ) + $settings_base;

    $weight = 1;
    foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
      $tabs_base = array(
        'access callback' => 'panelizer_has_no_choice_callback',
        'access arguments' => array($this->entity_type, $bundle, $view_mode),
        'page arguments' => array($this->entity_type, $bundle, 'default', $view_mode),
        'type' => MENU_LOCAL_TASK,
        'file' => 'includes/admin.inc',
        'weight' => $weight++,
      );

      $items[$root . '/panelizer/' . $view_mode] = array(
        'access callback' => 'panelizer_is_panelized',
        'title' => $view_mode_info['label'],
        'page callback' => 'panelizer_default_list_or_settings_page',
      ) + $tabs_base;

      $index = 0;
      foreach (panelizer_operations() as $path => $operation) {
        $items[$root . '/panelizer/' . $view_mode . '/' . $path] = array(
          'title' => $operation['menu title'],
          'page callback' => $operation['admin callback'],
          // Use the index to keep them in the proper order.
          'weight' => $index - 4,
          'type' => ($index === 0) ? MENU_DEFAULT_LOCAL_TASK : MENU_LOCAL_TASK,
        ) + $tabs_base;
        if (isset($operation['file'])) {
          $items[$root . '/panelizer/' . $view_mode . '/' . $path]['file'] = $operation['file'];
        }
        if (isset($operation['file path'])) {
          $items[$root . '/panelizer/' . $view_mode . '/' . $path]['file path'] = $operation['file path'];
        }
        $index++;
      }

      $subtabs_base = array(
        'access callback' => 'panelizer_administer_panelizer_default',
        'access arguments' => array($this->entity_type, $bundle, $base_count + 2, $base_count + 1),
        'page arguments' => array($this->entity_type, $bundle, $base_count + 2, $base_count + 1),
        'type' => MENU_LOCAL_TASK,
        'file' => 'includes/admin.inc',
      );

      $items[$root . '/panelizer/' . $view_mode . '/%'] = array(
        'title' => 'Settings',
        'page callback' => 'panelizer_default_settings_page',
        'title callback' => 'panelizer_default_name_title_callback',
        'type' => MENU_CALLBACK,
      ) + $subtabs_base;

      $index = 0;
      foreach (panelizer_operations() as $path => $operation) {
        $items[$root . '/panelizer/' . $view_mode . '/%/' . $path] = array(
          'title' => $operation['menu title'],
          'page callback' => $operation['admin callback'],
          // Use the index to keep them in the proper order.
          'weight' => $index - 4,
        ) + $subtabs_base;
        if (isset($operation['file'])) {
          $items[$root . '/panelizer/' . $view_mode . '/%/' . $path]['file'] = $operation['file'];
        }
        if (isset($operation['file path'])) {
          $items[$root . '/panelizer/' . $view_mode . '/%/' . $path]['file path'] = $operation['file path'];
        }
        $index++;
      }

      // This special tab isn't a normal operation because appears only
      // in the admin menu.
      $items[$root . '/panelizer/' . $view_mode . '/%/access'] = array(
        'title' => 'Access',
        'page callback' => 'panelizer_default_access_page',
        'weight' => -2,
      ) + $subtabs_base;

      // Also make clones of all the export UI menu items. Again there is some
      // duplicated code here because of subtle differences.
      // Load the $plugin information
      $plugin = ctools_get_export_ui('panelizer_defaults');

      $ui_items = $plugin['menu']['items'];

      // Change the item to a tab.
      $ui_items['list']['type'] = MENU_LOCAL_TASK;
      $ui_items['list']['weight'] = -6;
      $ui_items['list']['title'] = 'List';

      // menu local actions are weird.
      $ui_items['add']['path'] = 'list/add';
      $ui_items['import']['path'] = 'list/import';

      // Edit is being handled elsewhere:
      unset($ui_items['edit callback']);
      unset($ui_items['access']);
      unset($ui_items['list callback']);
      // Edit is being handled elsewhere:
      foreach (panelizer_operations() as $path => $operation) {
        $location = isset($operation['ui path']) ? $operation['ui path'] : $path;
        if (isset($ui_items[$location])) {
          unset($ui_items[$location]);
        }
      }

      // Change the callbacks for everything:
      foreach ($ui_items as $key => $item) {
        // originally admin/config/content/panelizer/%panelizer_handler
        $ui_items[$key]['access callback'] = 'panelizer_has_choice_callback_view_mode';
        $ui_items[$key]['access arguments'] = array($this->entity_type, $bundle, $view_mode);
        $ui_items[$key]['page callback'] = 'panelizer_default_list_or_settings_page';
        $ui_items[$key]['page arguments'][0] = $view_mode;
        array_unshift($ui_items[$key]['page arguments'], '');
        array_unshift($ui_items[$key]['page arguments'], $bundle);
        array_unshift($ui_items[$key]['page arguments'], $this->entity_type);
        $ui_items[$key]['path'] = str_replace('list/', '', $ui_items[$key]['path']);
      }

      foreach ($ui_items as $item) {
        // Add menu item defaults.
        $item += array(
          'file' => 'export-ui.inc',
          'file path' => drupal_get_path('module', 'ctools') . '/includes',
        );

        $path = !empty($item['path']) ? $root . '/panelizer/' . $view_mode . '/' . $item['path'] : $root . '/panelizer/' . $view_mode;
        unset($item['path']);
        $items[$path] = $item;
      }
    }
  }

  /**
   * Add the panelizer settings form to a single entity bundle config form.
   *
   * @param &$form
   *   The form array.
   * @param &$form_state
   *   The form state array.
   * @param $bundle
   *   The machine name of the bundle this form is for.
   * @param $type_location
   *   The location in the form state values that the bundle name will be;
   *   this is used so that if a machine name of a bundle is changed, Panelizer
   *   can update as much as possible.
   */
  public function add_bundle_setting_form(&$form, &$form_state, $bundle, $type_location) {
    $settings = !empty($this->plugin['bundles'][$bundle]) ? $this->plugin['bundles'][$bundle] : array('status' => FALSE, 'default' => FALSE, 'choice' => FALSE);

    $form['panelizer'] = array(
      '#type' => 'fieldset',
      '#title' => t('Panelizer'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('panelizer-node-type-settings-form'),
      ),
      '#bundle' => $bundle,
      '#location' => $type_location,
      '#tree' => TRUE,
      '#access' => panelizer_administer_entity_bundle($this, $bundle),
//      '#attached' => array(
//        'js' => array(drupal_get_path('module', 'comment') . '/panelizer-entity-form.js'),
//      ),
    );

    $form['panelizer']['status'] = array(
      '#title' => t('Panelize'),
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['status']),
      '#id' => 'panelizer-status',
    );

    foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
      if (isset($this->plugin['view mode status'][$bundle][$view_mode]) && empty($this->plugin['view mode status'][$bundle][$view_mode])) {
        continue;
      }
      $form['panelizer']['view modes'][$view_mode] = array(
        '#type' => 'item',
        '#title' => $view_mode_info['label'],
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
          ),
        ),
      );

      $form['panelizer']['view modes'][$view_mode]['status'] = array(
        '#title' => t('Panelize'),
        '#type' => 'checkbox',
        '#default_value' => !empty($settings['view modes'][$view_mode]['status']),
        '#id' => 'panelizer-' . $view_mode . '-status',
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
          ),
        ),
      );
      $form['panelizer']['view modes'][$view_mode]['default'] = array(
        '#title' => t('Provide default panel'),
        '#type' => 'checkbox',
        '#default_value' => !empty($settings['view modes'][$view_mode]['default']),
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
          ),
        ),
        '#description' => t('If checked, a default panel will be utilized for all existing and new entities.'),
      );

      $form['panelizer']['view modes'][$view_mode]['choice'] = array(
        '#title' => t('Allow panel choice'),
        '#type' => 'checkbox',
        '#default_value' => !empty($settings['view modes'][$view_mode]['choice']),
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
          ),
        ),
        '#description' => t('If checked multiple panels can be created and each entity will get a selector to choose which panel to use.'),
      );
    }

    array_unshift($form['#submit'], 'panelizer_entity_default_bundle_form_submit');

    $form_state['panelizer_entity_handler'] = $this;
  }

  /**
   * Submit callback for the bundle edit form.
   */
  public function add_bundle_setting_form_submit($form, &$form_state, $bundle, $type_location) {
    // Some types do not support changing bundles, so we don't check if it's
    // not possible to change.
    if ($type_location) {
      $new_bundle = drupal_array_get_nested_value($form_state['values'], $type_location);
    }
    else {
      $new_bundle = $bundle;
    }

    // Check to see if the bundle has changed. If so we need to move stuff around.
    if ($bundle && $new_bundle != $bundle) {
      // Remove old settings.
      variable_del('panelizer_defaults_' . $this->entity_type . '_' . $bundle);
      $allowed_layouts = variable_get('panelizer_' . $this->entity_type . ':' . $bundle . '_allowed_layouts', NULL);
      if ($allowed_layouts) {
        variable_del('panelizer_' . $this->entity_type . ':' . $bundle . '_allowed_layouts');
        variable_set('panelizer_' . $this->entity_type . ':' . $new_bundle . '_allowed_layouts', $allowed_layouts);
      }
      $default = variable_get('panelizer_' . $this->entity_type . ':' . $bundle . '_default', NULL);
      if ($default) {
        variable_del('panelizer_' . $this->entity_type . ':' . $bundle . '_default');
        variable_set('panelizer_' . $this->entity_type . ':' . $new_bundle . '_default', $default);
      }

      // Load up all panelizer defaults for the old bundle and resave them
      // for the new bundle.
      $panelizer_defaults = $this->get_default_panelizer_objects($bundle);
      foreach ($panelizer_defaults as $panelizer) {
        list($entity_type, $old_bundle, $name) = explode(':', $panelizer->name);
        $panelizer->name = implode(':', array($entity_type, $new_bundle, $name));
        if ($panelizer->view_mode != 'page_manager') {
          $panelizer->name .= ':' . $panelizer->view_mode;
        }

        $panelizer->panelizer_key = $new_bundle;
        // If there's a pnid this should change the name and retain the pnid.
        // If there is no pnid this will create a new one in the database
        // because exported panelizer defaults attached to a bundle will have
        // to be moved to the database in order to follow along and
        // then be re-exported.
        // @todo -- should we warn the user about this?
        ctools_export_crud_save('panelizer_defaults', $panelizer);
      }
    }

    variable_set('panelizer_defaults_' . $this->entity_type . '_' . $new_bundle, $form_state['values']['panelizer']);

    // Unset this so that the type save forms don't try to save it to variables.
    unset($form_state['values']['panelizer']);
  }

  /**
   * Implements a delegated hook_menu.
   */
  public function hook_admin_paths(&$items) {
    if (!empty($this->plugin['entity path'])) {
      $bits = explode('/', $this->plugin['entity path']);
      foreach ($bits as $count => $bit) {
        if (strpos($bit, '%') === 0) {
          $bits[$count] = '*';
        }
      }

      $path = implode('/', $bits);
      $items[$path . '/panelizer*'] = TRUE;
    }
  }

  public function hook_menu_alter(&$items) {

  }

  public function hook_form_alter(&$form, &$form_state, $form_id) {

  }

  // Entity specific Drupal hooks
  public function hook_entity_load(&$entities) {
    ctools_include('export');
    $ids = array();
    $vids = array();
    $bundles = array();

    foreach ($entities as $entity) {
      // Don't bother if somehow we've already loaded and are asked to
      // load again.
      if (!empty($entity->panelizer)) {
        continue;
      }

      list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
      if ($this->is_panelized($bundle)) {
        $ids[] = $entity_id;
        if ($this->supports_revisions) {
          $vids[] = $revision_id;
        }
        $bundles[$entity_id] = $bundle;
      }
    }

    if (!$ids) {
      return;
    }

    // Load all the panelizers associated with the list of entities.
    if ($this->supports_revisions) {
      $result = db_query("SELECT * FROM {panelizer_entity} WHERE entity_type = '$this->entity_type' AND entity_id IN (:ids) AND revision_id IN (:vids)", array(':ids' => $ids, ':vids' => $vids));
    }
    else {
      $result = db_query("SELECT * FROM {panelizer_entity} WHERE entity_type = '$this->entity_type' AND entity_id IN (:ids)", array(':ids' => $ids));
    }

    $panelizers = array();
    while ($panelizer = $result->fetchObject()) {
      $panelizers[$panelizer->entity_id][$panelizer->view_mode] = $panelizer;
    }

    $defaults = array();
    $dids = array();
    // Go through our entity list and generate a list of defaults and displays
    foreach ($entities as $entity_id => $entity) {
      // Don't bother if somehow we've already loaded and are asked to
      // load again.
      if (!empty($entity->panelizer)) {
        continue;
      }

      // Skip not panelized bundles.
      if (empty($bundles[$entity_id])) {
        continue;
      }

      // Check for each view mode.
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        $name = implode(':', array($this->entity_type, $bundles[$entity_id], 'default'));
        if ($view_mode != 'page_manager') {
          $name .= ':' . $view_mode;
        }

        // If no panelizer was loaded for the view mode, queue up defaults.
        if (empty($panelizers[$entity_id][$view_mode]) && $this->has_default_panel($bundles[$entity_id] . '.' . $view_mode)) {
          $defaults[$name] = $name;
        }
        // Otherwise unpack the loaded panelizer.
        else if (!empty($panelizers[$entity_id][$view_mode])) {
          $entity->panelizer[$view_mode] = ctools_export_unpack_object('panelizer_entity', $panelizers[$entity_id][$view_mode]);
          // If somehow we have no name AND no did, fill in the default.
          // This can happen if use of defaults has switched around maybe?
          if (empty($entity->panelizer[$view_mode]->did) &&
            empty($entity->panelizer[$view_mode]->name)) {
            if ($this->has_default_panel($bundles[$entity_id] . '.' . $view_mode)) {
              $entity->panelizer[$view_mode]->name = $name;
            }
            else {
              // With no default, did or name, this doesn't actually exist.
              unset($entity->panelizer[$view_mode]);
              list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

              db_delete('panelizer_entity')
                ->condition('entity_type', $this->entity_type)
                ->condition('entity_id', $entity_id)
                ->condition('revision_id', $revision_id)
                ->condition('view_mode', $view_mode)
                ->execute();
              continue;
            }
          }

          // Panelizers that do not have dids are just a selection of defaults
          // that has never actually been modified.
          if (empty($entity->panelizer[$view_mode]->did) && !empty($entity->panelizer[$view_mode]->name)) {
            $defaults[$entity->panelizer[$view_mode]->name] = $entity->panelizer[$view_mode]->name;
          }
          else {
            $dids[$entity->panelizer[$view_mode]->did] = $entity->panelizer[$view_mode]->did;
          }
        }
      }
    }

    // Load any defaults we collected.
    if ($defaults) {
      $panelizer_defaults = $this->load_default_panelizer_objects($defaults);
    }

    // if any panelizers were loaded, get their attached displays.
    if ($dids) {
      $displays = panels_load_displays($dids);
    }

    // Now, go back through our entities and assign dids and defaults
    // accordingly.
    foreach ($entities as $entity_id => $entity) {
      // Skip not panelized bundles.
      if (empty($bundles[$entity_id])) {
        continue;
      }
      // Check for each view mode.
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        if (empty($entity->panelizer[$view_mode])) {
          // @todo there should be a convenience function for this.
          $default_key = implode(':', array($this->entity_type, $bundles[$entity_id], 'default'));
          if ($view_mode != 'page_manager') {
            $default_key .= ':' . $view_mode;
          }

          if (!empty($panelizer_defaults[$default_key])) {
            $entity->panelizer[$view_mode] = clone $panelizer_defaults[$default_key];
            // make sure this entity can't write to the default display.
            $entity->panelizer[$view_mode]->did = NULL;
          }
        }
        else if (empty($entity->panelizer[$view_mode]->display)) {
          if (!empty($entity->panelizer[$view_mode]->did)) {
            if (empty($displays[$entity->panelizer[$view_mode]->did])) {
              // Somehow the display for this entity has gotten lost?
              $entity->panelizer[$view_mode]->did = NULL;
              $entity->panelizer[$view_mode]->display = $this->get_default_display($bundles[$entity_id], $view_mode);
            }
            else {
              $entity->panelizer[$view_mode]->display = $displays[$entity->panelizer[$view_mode]->did];
            }
          }
          else {
            if (!empty($panelizer_defaults[$entity->panelizer[$view_mode]->name])) {
              $entity->panelizer[$view_mode]->display = $panelizer_defaults[$entity->panelizer[$view_mode]->name]->display;
            }
          }
        }
      }
    }
  }

  public function hook_entity_insert($entity) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    if (!$this->is_panelized($bundle)) {
      return;
    }

    // If there's no panelizer information on the entity then there is nothing to do.
    if (empty($entity->panelizer)) {
      return;
    }

    // Allow exports or older data to be deployed successfully.
    if (is_object($entity->panelizer)) {
      $entity->panelizer = array('page_manager' => $entity->panelizer);
    }

    foreach ($entity->panelizer as $view_mode => $panelizer) {
      // Just a safety check to make sure we can't have a missing view mode.
      if (empty($view_mode)) {
        $view_mode = 'page_manager';
      }

      // On entity insert, we only write the display if it is not a default.
      // That probably means it came from an export or deploy or something
      // along those lines.
      if (empty($panelizer->name) && !empty($panelizer->display)) {
        // Ensure we don't accidentally overwrite existing display
        // data or anything silly like that.
        $panelizer = $this->clone_panelizer($panelizer, $entity);
        // First write the display
        panels_save_display($panelizer->display);

        // Make sure we have the new did.
        $panelizer->did = $panelizer->display->did;

        // Make sure there is a view mode.
        if (empty($panelizer->view_mode)) {
          $panelizer->view_mode = $view_mode;
        }

        // And write the new record.
        drupal_write_record('panelizer_entity', $panelizer);
      }
      else {
        // We write the panelizer record to record which name is being used.
        // And ensure the did is NULL:
        $panelizer->did = NULL;
        $panelizer->entity_type = $this->entity_type;
        $panelizer->entity_id = $entity_id;
        // The (int) ensures that entities that do not support revisions work
        // since the revision_id cannot be NULL.
        $panelizer->revision_id = (int) $revision_id;

        // Make sure there is a view mode.
        if (empty($panelizer->view_mode)) {
          $panelizer->view_mode = $view_mode;
        }

        drupal_write_record('panelizer_entity', $panelizer);
      }
    }
  }

  public function hook_entity_update($entity) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    if (!$this->is_panelized($bundle)) {
      return;
    }

    // If there's no panelizer information on the entity then there is nothing to do.
    if (empty($entity->panelizer)) {
      return;
    }

    // Allow exports or older data to be deployed successfully.
    if (is_object($entity->panelizer)) {
      $entity->panelizer = array('page_manager' => $entity->panelizer);
    }

    foreach ($entity->panelizer as $view_mode => $panelizer) {
      // Just a safety check to make sure we can't have a missing view mode.
      if (empty($view_mode)) {
        $view_mode = 'page_manager';
      }

      if ($this->supports_revisions) {
        if (empty($panelizer->revision_id) || $panelizer->revision_id != $revision_id) {
          $panelizer->revision_id = $revision_id;
          $update = array();
        }
        else {
          $update = array('entity_type', 'revision_id', 'view_mode');
        }
      }
      else {
        if (empty($panelizer->entity_id)) {
          $update = array();
        }
        else {
          $update = array('entity_type', 'entity_id', 'view_mode');
        }
      }

      // The editor will set this flag if the display is modified. This lets
      // us know if we need to clone a new display or not.
      // NOTE: This means that when exporting or deploying, we need to be sure
      // to set the display_is_modified flag to ensure this gets written.
      if (!empty($panelizer->display_is_modified)) {
        // If this is a new entry or the entry is using a display from a default,
        // clone the display.
        if (!$update || empty($panelizer->did)) {
          $entity->panelizer[$view_mode] = $panelizer = $this->clone_panelizer($panelizer, $entity);

          // Update the cache key since we are adding a new display
          $panelizer->display->cache_key = implode(':', array('panelizer', $panelizer->entity_type, $panelizer->entity_id, $view_mode));
        }

        // First write the display
        panels_save_display($panelizer->display);

        // Make sure we have the did.
        $panelizer->did = $panelizer->display->did;

        // Ensure that we always write this as NULL when we have our own panel:
        $panelizer->name = NULL;

        // Make sure there is a view mode.
        if (empty($panelizer->view_mode)) {
          $panelizer->view_mode = $view_mode;
        }

        // And write the new record.
        return drupal_write_record('panelizer_entity', $panelizer, $update);
      }
      else {
        $panelizer->entity_type = $this->entity_type;
        $panelizer->entity_id = $entity_id;
        // The (int) ensures that entities that do not support revisions work
        // since the revision_id cannot be NULL.
        $panelizer->revision_id = (int) $revision_id;

        // Make sure there is a view mode.
        if (empty($panelizer->view_mode)) {
          $panelizer->view_mode = $view_mode;
        }

        drupal_write_record('panelizer_entity', $panelizer, $update);
      }
    }
  }

  public function hook_entity_delete($entity) {
    $this->delete_entity_panelizer($entity);
  }

  public function hook_field_attach_delete_revision($entity) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    // Locate and delete all displays associated with the entity.
    $revisions = db_query("SELECT revision_id, did FROM {panelizer_entity} WHERE entity_type = '$this->entity_type' AND entity_id = :id", array(':id' => $entity_id))->fetchAllAssoc('revision_id');

    // It is possible to have the same did on multiple revisions, if none of
    // those revisions modified the display. Be careful NOT to delete a display
    // that might be in use by another revision.
    $seen = array();
    foreach ($revisions as $info) {
      if ($info->revision_id != $revision_id) {
        $seen[$info->did] = TRUE;
      }
    }

    if (!empty($revisions[$revision_id]->did) && empty($seen[$revisions[$revision_id]->did])) {
      panels_delete_display($revisions[$revision_id]->did);
    }

    db_delete('panelizer_entity')
      ->condition('entity_type', $this->entity_type)
      ->condition('entity_id', $entity_id)
      ->condition('revision_id', $revision_id)
      ->execute();
  }

  public function hook_field_attach_form($entity, &$form, &$form_state, $langcode) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    // We'll store the form array here so that we can tell at the end if we
    // have any and need to add our fieldset.
    $widgets = array();

    foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
      $view_bundle = $bundle . '.' . $view_mode;

      // Ignore view modes that don't have a choice or already have their
      // own custom panel set up.
      if (!$this->has_panel_choice($view_bundle) || !empty($entity->panelizer[$view_mode]->did)) {
        continue;
      }

      $panelizers = $this->get_default_panelizer_objects($view_bundle);

      $options = array();
      foreach ($panelizers as $name => $panelizer) {
        if (empty($panelizer->disabled)) {
          $options[$name] = $panelizer->title ? $panelizer->title : t('Default');
        }
      }

      if (!empty($entity->panelizer[$view_mode]->name)) {
        $name = $entity->panelizer[$view_mode]->name;
      }
      else {
        if ($this->has_default_panel($view_bundle)) {
          $name = implode(':', array($this->entity_type, $bundle, 'default'));
          if ($view_mode != 'page_manager') {
            $name .= ':' . $view_mode;
          }
        }
        else {
          $name = '';
        }
      }

      if (!$this->has_default_panel($view_bundle)) {
        $options = array('' => t('-- No panel --')) + $options;
      }

      $widgets[$view_mode]['name'] = array(
        '#title' => $view_mode_info['label'],
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $name,
        // Put these here because submit does not get a real entity with
        // the actual *(&)ing panelizer.
        '#revision_id' => isset($entity->panelizer[$view_mode]->revision_id) ? $entity->panelizer[$view_mode]->revision_id : NULL,
        '#entity_id' => isset($entity->panelizer[$view_mode]->entity_id) ? $entity->panelizer[$view_mode]->entity_id : NULL,
      );
    }

    if ($widgets) {
      $form_state['panelizer has choice'] = TRUE;
      $form['panelizer'] = array(
        '#type' => 'fieldset',
        '#access' => $this->panelizer_access('choice', $entity, $view_mode),
        '#title' => t('Panelizer'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#group' => 'additional_settings',
        '#attributes' => array(
          'class' => array('panelizer-entity-options'),
        ),
        '#attached' => array(
          'js' => array(ctools_attach_js('panelizer-vertical-tabs', 'panelizer')),
        ),
        '#weight' => -10,
        '#tree' => TRUE,
      ) + $widgets;
    }
  }

  public function hook_field_attach_submit($entity, &$form, &$form_state) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    if (!empty($form_state['panelizer has choice'])) {
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        if (isset($form_state['values']['panelizer'][$view_mode]['name'])) {
          $entity->panelizer[$view_mode] = $this->get_default_panelizer_object($bundle . '.' . $view_mode, $form_state['values']['panelizer'][$view_mode]['name']);
          $entity->panelizer[$view_mode]->did = NULL;

          // Ensure original values are maintained:
          $entity->panelizer[$view_mode]->entity_id = $form['panelizer'][$view_mode]['name']['#entity_id'];
          $entity->panelizer[$view_mode]->revision_id = $form['panelizer'][$view_mode]['name']['#revision_id'];
        }
      }
    }
  }

  /**
   * Determine if the entity allows revisions.
   *
   * @param $entity
   *   The entity to test.
   *
   * @return array
   *   An array containing two boolean values. The first one lets the system
   *   know whether or not the entity currently allows revisions. The second
   *   one lets us know if the user has access to control whether or not a
   *   new revision is created.
   */
  public function entity_allows_revisions($entity) {
    return array(FALSE, FALSE);
  }

  /**
   * Create a new, scrubbed version of a panelizer object.
   */
  public function clone_panelizer($panelizer, $entity) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    $panelizer_clone = clone $panelizer;

    // In order to ensure we don't actually use and modify the default display,
    // we export and re-import it.
    $code = panels_export_display($panelizer->display);
    ob_start();
    eval($code);
    ob_end_clean();

    $panelizer_clone->display = $display;
    $panelizer_clone->did = NULL;
    $panelizer_clone->name = NULL;
    $panelizer_clone->entity_type = $this->entity_type;
    $panelizer_clone->entity_id = $entity_id;
    // The (int) ensures that entities that do not support revisions work
    // since the revision_id cannot be NULL.
    $panelizer_clone->revision_id = (int) $revision_id;

    return $panelizer_clone;
  }

  function access_admin($entity, $op = NULL, $view_mode = NULL) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    if ($view_mode) {
      $bundle .= '.' . $view_mode;
    }
    else {
      $view_mode = 'page_manager';
    }

    if (!$this->is_panelized($bundle)) {
      return FALSE;
    }

    return $this->panelizer_access($op, $entity, $view_mode) && $this->entity_access('update', $entity);
  }

  /**
   * Determine if the user has access to the panelizer operation for this type.
   */
  function panelizer_access($op, $bundle, $view_mode) {
    if (is_object($bundle)) {
      $entity = $bundle;
      list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

      // If there is an $op, this must actually be panelized in order to pass.
      // If there is no op, then the settings page can provide us a "panelize it!"
      // page even if there is no panel.
      if ($op && $op != 'overview' && $op != 'settings' && $op != 'choice' && empty($entity->panelizer[$view_mode])) {
        return FALSE;
      }
    }

    return user_access('administer panelizer') || user_access("administer panelizer $this->entity_type $bundle $op");
  }

  /**
   * Switched page callback to give the overview page
   */
  function page_overview($js, $input, $entity) {
    $header = array(
      t('View mode'),
      t('Status'),
      t('Operations'),
    );

    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $rows = array();

    $base_url = $this->entity_base_url($entity);

    foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
      if (!$this->is_panelized($bundle . '.' . $view_mode)) {
        continue;
      }

      $row = array();
      $row[] = $view_mode_info['label'];
      $panelized = TRUE;

      if (!empty($entity->panelizer[$view_mode]->name)) {
        ctools_include('export');
        $panelizer = ctools_export_crud_load('panelizer_defaults', $entity->panelizer[$view_mode]->name);
        $status = !empty($panelizer->title) ? check_plain($panelizer->title) : t('Default');
      }
      else if (!empty($entity->panelizer[$view_mode]->did)) {
        $status = t('Custom');
      }
      else {
        $status = t('Not panelized');
        $panelized = FALSE;
      }
      $row[] = $status;

      if ($panelized) {
        $links_array = array();
        foreach (panelizer_operations() as $path => $operation) {
          if ($this->panelizer_access($path, $entity, $view_mode)) {
            $links_array[$path] = array(
              'title' => $operation['link title'],
              'href' => $base_url . '/' . $view_mode . '/' . $path,
            );
          }
        }
        if ($status == t('Custom')) {
          $links_array['reset'] = array(
            'title' => t('reset'),
            'href' => $base_url . '/' . $view_mode . '/reset',
          );
        }
      }
      else {
        $links_array = array(
          'panelize' => array(
            'title' => t('panelize'),
            'href' => $base_url . '/' . $view_mode,
          ),
        );
      }

      // Allow applications to add additional panelizer tabs.
      drupal_alter('panelizer_overview_links', $links_array, $this->entity_type, $entity, $view_mode, $status, $panelized);

      $links = theme('links', array(
        'links' => $links_array,
        'attributes' => array('class' => array('links', 'inline')),
      ));

      $row[] = $links;
      $rows[] = $row;
    }

    $output = theme('table', array('header' => $header, 'rows' => $rows));
    return $output;
  }

  /**
   * Provides the base panelizer URL for an entity.
   */
  function entity_base_url($entity, $view_mode = NULL) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $bits = explode('/', $this->plugin['entity path']);
    foreach ($bits as $count => $bit) {
      if (strpos($bit, '%') === 0) {
        $bits[$count] = $entity_id;
      }
    }

    $bits[] = 'panelizer';
    if ($view_mode) {
      $bits[] = $view_mode;
    }
    $base_url = implode('/', $bits);

    return $base_url;
  }

  /**
   * Provides a wrapper for the panelizer page output.
   *
   * Drupal only supports 2 levels of tabs, but we need a 3rd
   * level. We will fake it.
   */
  function wrap_entity_panelizer_pages($entity, $view_mode, $output) {
    $base_url = $this->entity_base_url($entity, $view_mode);
    return $this->make_fake_tabs($base_url, $entity, $view_mode, $output);
  }

  /**
   * Provides a wrapper for the panelizer page output.
   *
   * Drupal only supports 2 levels of tabs, but we need a 3rd
   * level. We will fake it.
   */
  function wrap_default_panelizer_pages($bundle, $output) {
    list($bundle, $view_mode) = explode('.', $bundle);
    $base_url = $this->entity_admin_root . '/panelizer/' . $view_mode;
    // We have to sub in the bundle if this is set.
    if (is_numeric($this->entity_admin_bundle)) {
      $bits = explode('/', $base_url);
      $bits[$this->entity_admin_bundle] = $bundle;
      $base_url = implode('/', $bits);
    }

    return $this->make_fake_tabs($base_url, $bundle, $view_mode, $output);
  }

  /**
   * Create some fake tabs that are attached to a page output.
   */
  function make_fake_tabs($base_url, $bundle, $view_mode, $output) {
    $links_array = array();
    foreach (panelizer_operations() as $path => $operation) {
      if ($this->panelizer_access($path, $bundle, $view_mode)) {
        $links_array[$path] = array(
          'title' => t($operation['menu title']),
          'href' => $base_url . '/' . $path,
        );
      }
    }

    // Allow applications to add additional panelizer tabs.
    drupal_alter('panelizer_tab_links', $links_array, $this->entity_type, $bundle, $view_mode);

    // Only render if > 1 link, just like core.
    if (count($links_array) <= 1) {
      return $output;
    }

    // These fake tabs are pretty despicable, but they'll do.
    $links = '<div class="tabs clearfix">' . theme('links', array(
      'links' => $links_array,
      'attributes' => array('class' => array('tabs', 'secondary')),
    )) . '</div>';

    if (is_array($output)) {
      // Use array addition because forms will already be sorted so
      // #weight may not be effective.
      $output = array(
        'panelizer_links' => array(
          '#markup' => $links,
          '#weight' => -10000,
        ),
      ) + $output;
    }
    else {
      $output = $links . $output;
    }

    return $output;
  }

  /**
   * Switched page callback to give the settings form.
   */
  function page_reset($js, $input, $entity, $view_mode) {
    $panelizer = $entity->panelizer[$view_mode];

    $form_state = array(
      'entity' => $entity,
      'panelizer' => $panelizer,
      'view_mode' => $view_mode,
      'no_redirect' => TRUE,
    );

    ctools_include('common', 'panelizer');
    $output = drupal_build_form('panelizer_reset_entity_form', $form_state);
    if (!empty($form_state['executed'])) {
      drupal_set_message(t('Panelizer information has been reset.'));
      $this->delete_entity_panelizer($entity, $view_mode);
      drupal_goto(dirname(dirname($_GET['q'])));
    }

    return $output;
  }

  /**
   * Switched page callback to give the settings form.
   */
  function page_settings($js, $input, $entity, $view_mode) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    if (empty($entity->panelizer[$view_mode])) {
      // If this entity is not yet panelized, and there is no default panel
      // to do configuration, give them the option of panelizing it.
      if ($this->has_default_panel($bundle . '.' . $view_mode)) {
        return MENU_NOT_FOUND;
      }

      // Set the form to the Panelize It! form.
      $form_id = 'panelizer_panelize_entity_form';

      // Fetch a special default panelizer that is only accessible with the
      // default_anyway flag.
      $panelizer = $this->get_internal_default_panelizer($bundle, $view_mode);
      $panelizer->name = NULL;
    }
    else {
      $form_id = 'panelizer_settings_form';
      $panelizer = $entity->panelizer[$view_mode];
    }

    $form_state = array(
      'entity' => $entity,
      'revision info' => $this->entity_allows_revisions($entity),
      'panelizer' => $panelizer,
      'view_mode' => $view_mode,
      'no_redirect' => TRUE,
    );

    ctools_include('common', 'panelizer');
    $output = drupal_build_form($form_id, $form_state);
    if (!empty($form_state['executed'])) {
      drupal_set_message(t('The settings have been updated.'));
      $entity->panelizer[$view_mode] = $form_state['panelizer'];
      // Make sure that entity_save knows that the panelizer settings
      // are modified and must be made local to the entity.
      if (empty($panelizer->did) || !empty($panelizer->name)) {
        $panelizer->display_is_modified = TRUE;
      }
      $this->entity_save($entity);

      drupal_goto($_GET['q']);
    }

    return $this->wrap_entity_panelizer_pages($entity, $view_mode, $output);
  }

  function page_context($js, $input, $entity, $view_mode) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $cache_key = $entity_id . '.' . $view_mode;
    $panelizer = panelizer_context_cache_get($this->entity_type, $cache_key);

    if (empty($panelizer)) {
      return MENU_NOT_FOUND;
    }

    $form_state = array(
      'panelizer' => &$panelizer,
      'entity' => $entity,
      'revision info' => $this->entity_allows_revisions($entity),
      'panelizer type' => $this->entity_type,
      'cache key' => $cache_key,
      'no_redirect' => TRUE,
    );

    ctools_include('common', 'panelizer');
    $output = drupal_build_form('panelizer_default_context_form', $form_state);
    if (!empty($form_state['executed'])) {
      if (!empty($form_state['clicked_button']['#write'])) {
        drupal_set_message(t('The settings have been updated.'));
        $entity->panelizer[$view_mode] = $form_state['panelizer'];
        $this->entity_save($entity);
      }
      else {
        drupal_set_message(t('Changes have been discarded.'));
      }

      panelizer_context_cache_clear($this->entity_type, $cache_key);
      drupal_goto($_GET['q']);
    }

    return $this->wrap_entity_panelizer_pages($entity, $view_mode, $output);
  }

  function page_layout($js, $input, $entity, $view_mode, $step = NULL, $layout = NULL) {
    $panelizer = $entity->panelizer[$view_mode];
    if (empty($panelizer)) {
      return MENU_NOT_FOUND;
    }

    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $display = $panelizer->display;
    $display->context = $this->get_contexts($panelizer, $entity);

    $path = $this->entity_base_url($entity, $view_mode);

    $form_state = array(
      'entity' => $entity,
      'revision info' => $this->entity_allows_revisions($entity),
      'display' => $display,
      'wizard path' => $path . '/layout/%step',
      'allowed_layouts' => 'panelizer_' . $this->entity_type . ':' . $bundle,
    );

    ctools_include('common', 'panelizer');
    $output = panelizer_change_layout_wizard($form_state, $step, $layout);
    if (!empty($form_state['complete'])) {
      $entity->panelizer[$view_mode]->display = $form_state['display'];
      $entity->panelizer[$view_mode]->display_is_modified = TRUE;
      $this->entity_save($entity);
      drupal_set_message(t('The layout has been changed.'));
      drupal_goto($path . '/content');
    }

    return $this->wrap_entity_panelizer_pages($entity, $view_mode, $output);
  }

  function page_content($js, $input, $entity, $view_mode) {
    $panelizer = $entity->panelizer[$view_mode];
    if (empty($panelizer)) {
      return MENU_NOT_FOUND;
    }

    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $form_state = array(
      'entity' => $entity,
      'revision info' => $this->entity_allows_revisions($entity),
      'display cache' => panels_edit_cache_get(implode(':', array('panelizer', $this->entity_type, $entity_id, $view_mode))),
      'no_redirect' => TRUE,
    );

    ctools_include('common', 'panelizer');
    $output = drupal_build_form('panelizer_edit_content_form', $form_state);
    if (!empty($form_state['executed'])) {
      if (!empty($form_state['clicked_button']['#save-display'])) {
        drupal_set_message(t('The settings have been updated.'));
        $entity->panelizer[$view_mode]->display = $form_state['display'];
        $entity->panelizer[$view_mode]->display_is_modified = TRUE;
        $this->entity_save($entity);
      }
      else {
        drupal_set_message(t('Changes have been discarded.'));
      }

      panels_edit_cache_clear($form_state['display cache']);
      drupal_goto($_GET['q']);
    }

    $output = $this->wrap_entity_panelizer_pages($entity, $view_mode, $output);

    ctools_set_no_blocks(FALSE);
    drupal_set_page_content($output);
    $page = element_info('page');
    return $page;
  }

  /**
   * Delete panelizers associated with the entity.
   *
   * @param object $entity
   *   The entity.
   * @param $view_mode
   *   The view mode to delete. If not specified, all view modes will be
   *   deleted.
   */
  function delete_entity_panelizer($entity, $view_mode = NULL) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    if (empty($view_mode)) {
      // Locate and delete all displays associated with the entity.
      $dids = db_query("SELECT did FROM {panelizer_entity} WHERE entity_type = '$this->entity_type' AND entity_id = :id", array(':id' => $entity_id))->fetchCol();
    }
    else {
      $dids = db_query("SELECT did FROM {panelizer_entity} WHERE entity_type = '$this->entity_type' AND entity_id = :id AND view_mode = :view_mode", array(':id' => $entity_id, ':view_mode' => $view_mode))->fetchCol();
    }

    foreach (array_unique($dids) as $did) {
      panels_delete_display($did);
    }

    $delete = db_delete('panelizer_entity')
      ->condition('entity_type', $this->entity_type)
      ->condition('entity_id', $entity_id);

    if ($view_mode) {
      $delete->condition('view_mode', $view_mode);
    }

    $delete->execute();
  }

  /**
   * Determine if a bundle is panelized.
   */
  public function is_panelized($bundle) {
    if (strpos($bundle, '.') === FALSE) {
      return !empty($this->plugin['bundles'][$bundle]) && !empty($this->plugin['bundles'][$bundle]['status']);
    }
    else {
      list($bundle, $view_mode) = explode('.', $bundle);
      return !empty($this->plugin['bundles'][$bundle]) && !empty($this->plugin['bundles'][$bundle]['status']) && !empty($this->plugin['bundles'][$bundle]['view modes'][$view_mode]['status']);
    }
  }

  /**
   * Determine if a bundle has a default panel.
   *
   * @param $bundle
   *   A $bundle.$view_mode combo string. If no view mode is specified
   *   then the 'page_manager' view mode will be assumed.
   */
  public function has_default_panel($bundle) {
    if (strpos($bundle, '.') === FALSE) {
      $bundle .= '.page_manager';
    }
    list($bundle, $view_mode) = explode('.', $bundle);

    return $this->is_panelized($bundle) && !empty($this->plugin['bundles'][$bundle]['view modes'][$view_mode]['default']);
  }

  /**
   * Determine if a bundle is allowed choices.
   */
  public function has_panel_choice($bundle) {
    if (strpos($bundle, '.') === FALSE) {
      $bundle .= '.page_manager';
    }
      list($bundle, $view_mode) = explode('.', $bundle);

    return $this->is_panelized($bundle) && !empty($this->plugin['bundles'][$bundle]['view modes'][$view_mode]['choice']);
  }

  /**
   * Get the default panels, keyed by names.
   */
  public function load_default_panelizer_objects($names) {
    ctools_include('export');
    $panelizers = ctools_export_load_object('panelizer_defaults', 'names', $names);
    return $panelizers;
  }

  /**
   * Get the default panelizers for the given bundle.
   */
  public function get_default_panelizer_objects($bundle) {
    if (strpos($bundle, '.') !== FALSE) {
      list($bundle, $view_mode) = explode('.', $bundle);
    }
    $conditions = array(
      'panelizer_type' => $this->entity_type,
      'panelizer_key' => $bundle,
    );

    if (!empty($view_mode)) {
      $conditions['view_mode'] = $view_mode;
    }

    ctools_include('export');
    return ctools_export_load_object('panelizer_defaults', 'conditions', $conditions);
  }

  /**
   * Determine if the current user has access to the $panelizer.
   */
  public function access_default_panelizer_object($panelizer) {
    // Automatically true for this, regardless of anything else.
    if (user_access('administer panelizer')) {
      return TRUE;
    }

    ctools_include('context');
    return user_access("administer panelizer $this->entity_type $panelizer->panelizer_key defaults") && ctools_access($panelizer->access, $this->get_contexts($panelizer));
  }

  /**
   * Implements a delegated hook_panelizer_defaults().
   *
   * This makes sure that all panelized entities configured to have a
   * default actually have one.
   */
  public function hook_panelizer_defaults(&$panelizers) {
    // For features integration, if they have modified a default and put
    // it into the database, we do not want to show one as a default.
    // Otherwise, features can't latch onto it.
    $default_names = &drupal_static('panelizer_defaults_in_database', NULL);
    if (!isset($default_names)) {
      $default_names = drupal_map_assoc(db_query("SELECT name FROM {panelizer_defaults} WHERE name LIKE '%:default&'")->fetchCol());
    }

    $entity_info = entity_get_info($this->entity_type);

    foreach ($this->plugin['bundles'] as $bundle => $info) {
      // Don't bother if there are no
      if (empty($info['status'])) {
        continue;
      }

      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        if (!empty($info['view modes'][$view_mode]['status']) && !empty($info['view modes'][$view_mode]['default'])) {
          $panelizer = $this->get_internal_default_panelizer($bundle, $view_mode);
          if (empty($default_names[$panelizer->name]) && !isset($panelizers[$panelizer->name])) {
            $panelizers[$panelizer->name] = $panelizer;
          }
        }
      }
    }
  }

  /**
   * An internal representation of a panelizer object, used to seed when
   * we have none and want something to get started.
   */
  public function get_internal_default_panelizer($bundle, $view_mode) {
    ctools_include('export');
    $load_name = implode(':', array($this->entity_type, $bundle, 'default'));
    $panelizer = ctools_export_crud_new('panelizer_defaults');
    $panelizer->name = $load_name;
    // Attach the view mode to the name, which is specially generated
    // to ignore the specialty "page_manager" view mode.
    if ($view_mode != 'page_manager') {
      $panelizer->name .= ':' . $view_mode;
    }

    $panelizer->panelizer_type = $this->entity_type;
    $panelizer->panelizer_key = $bundle;
    $panelizer->view_mode = $view_mode;
    $panelizer->display = $this->get_default_display($bundle, $view_mode);
    $panelizer->api_version = 1;
    $panelizer->title = t('Default');

    return $panelizer;
  }

  /**
   * Load the named default panel for the bundle.
   */
  public function get_default_panelizer_object($bundle, $name) {
    if (strpos($bundle, '.') !== FALSE) {
      list($bundle, $view_mode) = explode('.', $bundle);
    }
    else {
      $view_mode = 'page_manager';
    }

    // If the name is not in the format of entitytype:bundle:name which is
    // the machine name used, split that out automatically.
    if (strpos($name, ':') === FALSE) {
      $name = implode(':', array($this->entity_type, $bundle, $name));
      // This is the default view mode and older defaults won't have this,
      // so we don't enforce it.
      if ($view_mode != 'page_manager') {
        $name .= ':' . $view_mode;
      }
    }

    ctools_include('export');
    return ctools_export_crud_load('panelizer_defaults', $name);
  }

  /**
   * Provide a default display for newly panelized entities.
   *
   * This should be implemented by the entity plugin.
   */
  function get_default_display($bundle, $view_mode) {
    // This is a straight up empty display.
    $display = panels_new_display();
    $display->layout = 'flexible';

    $panes = array();
    foreach (field_info_instances($this->entity_type, $bundle) as $field_name => $instance) {
      $view_mode_settings = field_view_mode_settings($this->entity_type, $bundle);
      $actual_mode = (!empty($view_mode_settings[$view_mode]['custom_settings']) ? $view_mode : 'default');
      $field_display = $instance['display'][$actual_mode];

      $pane = panels_new_pane('entity_field', $this->entity_type . ':' . $field_name, TRUE);
      $pane->configuration['formatter'] = $field_display['type'];
      $pane->configuration['formatter_settings'] = $field_display['settings'];
      $pane->configuration['label'] = $field_display['label'];
      $pane->configuration['context'] = 'panelizer';
      $panes[] = array(
        '#pane' => $pane,
        '#weight' => $field_display['weight'],
      );
    }

    // Use our #weights to sort these so they appear in whatever order the
    // normal field configuration put them in.
    uasort($panes, 'element_sort');
    foreach ($panes as $pane) {
      $display->add_pane($pane['#pane'], 'center');
    }

    return $display;
  }

  /**
   * Get a panelizer object for the key.
   *
   * This must be implemented for each entity type.
   */
//  function get_panelizer_object($entity_id) {
//  }

  /**
   * Add entity specific form to the Panelizer settings form.
   *
   * This is primarily to allow bundle selection per entity type.
   */
  public function settings_form(&$form, &$form_state) {
    // Add entity settings
    // @todo change theme function name
    $form['entities'][$this->entity_type] = array(
      '#theme' => 'panelizer_settings_page_table',
      '#header' => array(
        array('data' => $this->entity_bundle_label(), 'width' => '15%'),
        t('Panelize'),
        t('Provide default panel'),
        t('Allow panel choice'),
        array('data' => t('Operations'), 'width' => '50%'),
      ),
      '#columns' => array('title', 'status', 'default', 'choice', 'links'),
    );

    $entity_info = entity_get_info($this->entity_type);

    $bundles = $entity_info['bundles'];

    drupal_alter('panelizer_default_types', $bundles, $this->entity_type);

    foreach ($bundles as $bundle => $bundle_info) {
      $base_url = 'admin/config/content/panelizer/' . $this->entity_type . '/' . $bundle;
      $bundle_id = str_replace(array('][', '_', ' '), '-', '#edit-entities-' . $this->entity_type . '-' . $bundle . '-0');

      // Add the widgets that apply only to the bundle.
      $form['entities'][$this->entity_type][$bundle][0]['title'] = array(
        '#markup' => '<strong>' . $bundle_info['label'] . '</strong>',
      );

      $form['entities'][$this->entity_type][$bundle][0]['status'] = array(
        '#type' => 'checkbox',
        '#default_value' => !empty($this->plugin['bundles'][$bundle]['status']),
      );

      // Set proper allowed content link for entire bundle based on status
      if (!empty($this->plugin['bundles'][$bundle]['status'])) {
        $links_array = array(
          'settings' => array(
            'title' => t('allowed content'),
            'href' => $base_url . '/allowed',
          ),
        );
        $links = theme('links', array(
          'links' => $links_array,
          'attributes' => array('class' => array('links', 'inline')),
        ));
      }
      else {
        $links = t('Save to access allowed content');
      }

      $form['entities'][$this->entity_type][$bundle][0]['links']['basic'] = array(
        '#type' => 'item',
        '#title' => $links,
        '#states' => array(
          'visible' => array(
            $bundle_id . '-status' => array('checked' => TRUE),
          ),
        ),
      );

      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        if (isset($this->plugin['view mode status'][$bundle][$view_mode]) && empty($this->plugin['view mode status'][$bundle][$view_mode])) {
          continue;
        }

        $base_id = str_replace(array('][', '_', ' '), '-', '#edit-entities-' . $this->entity_type . '-' . $bundle . '-' . $view_mode);
        $base_url = 'admin/config/content/panelizer/' . $this->entity_type . '/' . $bundle . '.' . $view_mode;

        if (!empty($this->plugin['bundles'][$bundle]['view modes'][$view_mode]) && is_array($this->plugin['bundles'][$bundle]['view modes'][$view_mode])) {
          $settings = $this->plugin['bundles'][$bundle]['view modes'][$view_mode];
        }
        else {
          $settings = array(
            'status' => FALSE,
            'default' => FALSE,
            'choice' => FALSE
          );
        }

        if (empty($view_mode_info['panelizer special'])) {
          $form['entities'][$this->entity_type][$bundle][$view_mode]['title'] = array(
            '#markup' => '&nbsp;&nbsp;&nbsp;&nbsp;' . $view_mode_info['label'],
          );
        }
        else {
          $form['entities'][$this->entity_type][$bundle][$view_mode]['title'] = array(
            '#markup' => '<strong>' . $bundle_info['label'] . '</strong>',
          );
        }

        $form['entities'][$this->entity_type][$bundle][$view_mode]['status'] = array(
          '#type' => 'checkbox',
          '#default_value' => !empty($settings['status']),
          '#states' => array(
            'visible' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
            ),
          ),
        );

        $form['entities'][$this->entity_type][$bundle][$view_mode]['default'] = array(
          '#type' => 'checkbox',
          '#default_value' => !empty($settings['default']),
          '#states' => array(
            'visible' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
            ),
          ),
        );

        $form['entities'][$this->entity_type][$bundle][$view_mode]['choice'] = array(
          '#type' => 'checkbox',
          '#default_value' => !empty($settings['choice']),
          '#states' => array(
            'visible' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
            ),
          ),
        );

        $form['entities'][$this->entity_type][$bundle][$view_mode]['links'] = array(
          '#prefix' => '<div class="container-inline">',
          '#suffix' => '</div>',
        );

        // Panelize is enabled and a default panel will be provided
        if (!empty($settings['status']) && !empty($settings['default']) && empty($settings['choice'])) {
          $links_array = array();
          foreach (panelizer_operations() as $path => $operation) {
            $links_array[$path] = array(
              'title' => $operation['link title'],
              'href' => $base_url . '/' . $path,
            );
          }

          $links = theme('links', array(
            'links' => $links_array,
            'attributes' => array('class' => array('links', 'inline')),
          ));
        }
        else {
          $links = t('Save to access default panel');
        }

        $form['entities'][$this->entity_type][$bundle][$view_mode]['links']['default'] = array(
          '#type' => 'item',
          '#title' => $links,
          '#states' => array(
            'visible' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-default' => array('checked' => TRUE),
              $base_id . '-choice' => array('checked' => FALSE),
            ),
          ),
        );

        if (!empty($settings['status']) && !empty($settings['choice'])) {
          $links_array = array(
            'list' => array(
              'title' => t('list'),
              'href' => $base_url . '/list',
            ),
          );

          $links = theme('links', array(
            'links' => $links_array,
            'attributes' => array('class' => array('links', 'inline')),
          ));
        }
        else {
          $links = t('Save to access panel list');
        }

        $form['entities'][$this->entity_type][$bundle][$view_mode]['links']['default2'] = array(
          '#type' => 'item',
          '#title' => $links,
          '#states' => array(
            'visible' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-choice' => array('checked' => TRUE),
            ),
          ),
        );
      }
    }
  }

  /**
   * Validate entity specific settings on the Panelizer settings form.
   */
  public function settings_form_validate(&$form, &$form_state) {

  }

  /**
   * Submit entity specific settings on the Panelizer settings form.
   */
  public function settings_form_submit(&$form, &$form_state) {
    if (empty($form_state['values']['entities'][$this->entity_type])) {
      return;
    }

    foreach ($form_state['values']['entities'][$this->entity_type] as $bundle => $values) {
      // Rewrite our settings because they're not quite in the right format in
      // the form.
      $settings = array('status' => $values[0]['status'], 'view modes' => array());
      foreach ($values as $view_mode => $data) {
        if ($view_mode) {
          $settings['view modes'][$view_mode] = $data;
        }
      }
      variable_set('panelizer_defaults_' . $this->entity_type . '_' . $bundle, $settings);
    }

    // @todo if we enable caching of the plugins, which we should, this
    // needs to clear that cache so they get reloaded.
  }

  /**
   * Render the panels display for a given panelizer entity.
   *
   * @param stdClass $entity
   *   A fully-loaded entity object controlled by panelizer.
   * @param array $args
   *   Optional array of arguments to pass to the panels display.
   * @param string $address
   *   An optional address to send to the renderer to use for addressable
   *   content.
   *
   * @return array
   *   If the entity isn't panelized, this returns NULL. Otherwise, it returns an
   *   associative array as meant for use with CTools with the following keys:
   *   - 'content': String containing the rendered panels display output.
   *   - 'no_blocks': Boolean defining if the panels display wants to hide core
   *      blocks or not when being rendered.
   */
  function render_entity($entity, $view_mode, $langcode = NULL, $args = array(), $address = NULL) {
    if (empty($entity->panelizer[$view_mode]) || empty($entity->panelizer[$view_mode]->display)) {
      return FALSE;
    }
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $panelizer = $entity->panelizer[$view_mode];
    $display = $panelizer->display;

    $display->context = $this->get_contexts($panelizer, $entity);
    $display->args = $args;
    $display->css_id = $panelizer->css_id;

    // This means the IPE will use our cache which means it will get appropriate
    // allowed content should it be selected.
    $display->cache_key = implode(':', array('panelizer', $this->entity_type, $entity_id, $view_mode));

    // Check to see if there is any CSS.
    if (!empty($panelizer->css)) {
      ctools_include('css');
      $filename = ctools_css_retrieve($display->cache_key);
      if (!$filename) {
        $filename = ctools_css_store($display->cache_key, $panelizer->css);
      }
      drupal_add_css($filename, array('group' => CSS_THEME));
    }

    if ($view_mode == 'page_manager') {
      // We think this is handled as a page, so set the current page display.
      panels_get_current_page_display($display);
    }

    // Allow applications to alter the panelizer and the display before rendering them.
    drupal_alter('panelizer_pre_render', $panelizer, $display, $entity);

    ctools_include('plugins', 'panels');
    $renderer = panels_get_renderer($panelizer->pipeline, $display);

    // If the IPE is enabled, but the user does not have access to edit
    // the entity, load the standard renderer instead.

    // use class_parents so we don't try to autoload the class we
    // are testing.
    $parents = class_parents($renderer);
    if (!empty($parents['panels_renderer_editor']) && (!$this->panelizer_access('content', $entity, $view_mode) || !$this->entity_access('update', $entity))) {
      $renderer = panels_get_renderer_handler('standard', $display);
    }

    $renderer->address = $address;

    $info = array(
      'content' => panels_render_display($display, $renderer),
      'no_blocks' => !empty($panelizer->no_blocks),
    );

    $info['classes_array'] = array();

    if (!empty($panelizer->css_class)) {
      ctools_include('cleanstring');
      foreach (explode(' ', $panelizer->css_class) as $class) {
        $class = ctools_context_keyword_substitute($class, array(), $display->context);
        if ($class) {
          $info['classes_array'][] = ctools_cleanstring($class);
        }
      }
    }

    if (!empty($parents['panels_renderer_editor'])) {
      ctools_add_css('panelizer-ipe', 'panelizer');
      ctools_add_js('panelizer-ipe', 'panelizer');
      drupal_add_js(drupal_get_path('module', 'panelizer') . "/js/panelizer-ipe.js", array('group' => JS_LIBRARY));
    }

    $info['title'] = $panelizer->display->get_title();
    return $info;
  }

  /**
   * Fetch an object array of CTools contexts from panelizer information.
   */
  public function get_contexts($panelizer, $entity = NULL) {
    ctools_include('context');
    if (empty($panelizer->base_contexts)) {
      $panelizer->base_contexts = $this->get_base_contexts($entity);
    }

    $contexts = ctools_context_load_contexts($panelizer);
    return $contexts;
  }

  /**
   * Callback to get the base context for a panelized entity
   */
  public function get_base_contexts($entity = NULL) {
    ctools_include('context');
    if ($entity) {
      $context = ctools_context_create('entity:' . $this->entity_type, $entity);
    }
    else {
      $context = ctools_context_create_empty('entity:' . $this->entity_type);
      // The placeholder is needed to create the form used for the live
      // preview.
      $context->placeholder = array(
        'type' => 'context',
        'conf' => array(
          'name' => $this->entity_type,
          'identifier' => $this->entity_identifier($entity),
          'keyword' => $this->entity_type,
          'context_settings' => array(),
        ),
      );
    }

    $context->identifier = $this->entity_identifier($entity);
    $context->keyword = $this->entity_type;
    return array('panelizer' => $context);
  }

  /**
   * Get the visible identifier if the identity.
   *
   * This is overridable because it can be a bit awkward using the
   * default label.
   */
  public function entity_identifier($entity) {
    $entity_info = entity_get_info($this->entity_type);
    return t('This @entity', array('@entity' => $entity_info['label']));
  }

  // Admin screens use a title callback for admin pages. This is used
  // to fill in that title.
  public function get_bundle_title($bundle) {
    $entity_info = entity_get_info($this->entity_type);

    return isset($entity_info['bundles'][$bundle]['label']) ? $entity_info['bundles'][$bundle]['label'] : '';
  }

  /**
   * Get the name of bundles on the entity.
   *
   * Entity API doesn't give us a way to determine this, so the class must
   * do this.
   *
   * @return
   *   A translated, safe string.
   */
  public function entity_bundle_label() {
    $entity_info = entity_get_info($this->entity_type);
    return t('@entity bundle', array('@entity' => $entity_info['label']));
  }

  /**
   * Fetch the entity out of a build for hook_entity_view.
   *
   * @param $build
   *   The render array that contains the entity.
   */
  public function get_entity_view_entity($build) {
    $element = '#' . $this->entity_type;
    if (isset($build[$element])) {
      return $build[$element];
    }
    else if (isset($build['#entity'])) {
      return $build['#entity'];
    }
  }

  /**
   * Implement views support for panelizer entity types.
   */
  public function hook_views_data_alter(&$items) {
    $entity_info = entity_get_info($this->entity_type);
    if (!empty($entity_info['base table'])) {
      $table = $entity_info['base table'];
      $items[$table]['panelizer_link'] = array(
        'field' => array(
          'title' => t('Panelizer link'),
          'help' => t('Provide a link to panelizer-related operations on the content.'),
          'handler' => 'panelizer_handler_field_link',
          'entity_type' => $this->entity_type,
        ),
      );
      $items[$table]['panelizer_status'] = array(
        'field' => array(
          'title' => t('Panelizer status'),
          'help' => t('Display whether an entity is panelized and which panelizer option it is using.'),
          'handler' => 'panelizer_handler_panelizer_status',
          'entity_type' => $this->entity_type,
        ),
      );

      // Join on revision id if possible or entity id if not.
      if (!empty($entity_info['entity keys']['revision'])) {
        $id_field = $entity_info['entity keys']['revision'];
        $field = 'revision_id';
      }
      else {
        $id_field = $entity_info['entity keys']['id'];
        $field = 'entity_id';
      }

      $items['panelizer_entity_' . $table]['table']['join'] = array(
        $table => array(
          'handler' => 'views_join',
          'table' => 'panelizer_entity',
          'left_table' => $table,
          'left_field' => $id_field,
          'field' => $field,
          'extra' => array(array(
            'field' => 'entity_type',
            'value' => $this->entity_type,
            'operator' => '=',
          )),
        ),
      );

      $items['panelizer_entity_' . $table]['table']['group'] = $items[$table]['table']['group'];

      $items['panelizer_entity_' . $table]['name'] = array(
        'filter' => array(
          'title' => t('Panelizer status'),
          'help' => t('Filter based upon panelizer status.'),
          'handler' => 'panelizer_handler_filter_panelizer_status',
          'entity_type' => $this->entity_type,
        ),
      );
    }
  }

  /**
   * Preprocess the entity view mode template.
   */
  public function preprocess_panelizer_view_mode(&$vars, $entity, $element, $panelizer, $info) {
    $vars['classes_array'][] = drupal_html_class($this->entity_type);
    $vars['classes_array'][] = drupal_html_class($this->entity_type . '-' . $element['#view_mode']);
    $vars['classes_array'][] = drupal_html_class($this->entity_type . '-' . $element['#panelizer_bundle']);
    $vars['classes_array'][] = drupal_html_class($this->entity_type . '-' . $element['#panelizer_entity_id']);
    if (!empty($entity->preview)) {
      $vars['classes_array'][] = drupal_html_class($this->entity_type . '-preview');
    }

    if (!empty($panelizer->title_element)) {
      $vars['title_element'] = $panelizer->title_element;
    }
    else {
      $vars['title_element'] = 'h2';
    }

    $vars['content'] = $info['content'];
    if (!empty($info['title'])) {
      $vars['title'] = $info['title'];
    }

    if (!empty($info['classes_array'])) {
      $vars['classes_array'] = array_merge($vars['classes_array'], $info['classes_array']);
    }

    if (!empty($panelizer->link_to_entity)) {
      list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

      $bits = explode('/', $this->plugin['entity path']);
      foreach ($bits as $count => $bit) {
        if (strpos($bit, '%') === 0) {
          $bits[$count] = $entity_id;
        }
      }
      $vars['entity_url'] = url(implode('/', $bits));
    }
  }
}

function panelizer_entity_default_bundle_form_submit($form, &$form_state) {
  $bundle = $form['panelizer']['#bundle'];
  $type_location = $form['panelizer']['#location'];
  $form_state['panelizer_entity_handler']->add_bundle_setting_form_submit($form, $form_state, $bundle, $type_location);
}
