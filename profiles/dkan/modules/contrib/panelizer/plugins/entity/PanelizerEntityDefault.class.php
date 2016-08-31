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
   * Check if the necessary Page Manager display is enabled and the appropriate
   * variant has not been disabled.
   *
   * @return boolean
   *   Whether or not both the Page Manager display and the variant are enabled.
   */
  public function check_page_manager_status();

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
   * Determine the default name for the default object.
   */
  public function get_default_display_default_name($bundle, $view_mode = 'page_manager');

  /**
   * Determine the variable name used to identify the default display for the
   * given bundle/view mode combination.
   */
  public function get_default_display_variable_name($bundle, $view_mode = 'page_manager');

  /**
   * Determine the default display name for a given bundle & view mode
   * combination.
   */
  public function get_default_display_name($bundle, $view_mode = 'page_manager');

  /**
   * Determine whether a specific default display object exists.
   */
  public function default_display_exists($display_name);

  /**
   * Get a default display for a newly panelized entity.
   *
   * This is meant to give administrators a starting point when panelizing
   * new entities.
   */
  function get_default_display($bundle, $view_mode);

  /**
   * Identify the view modes that are available for use with this entity bundle.
   *
   * @param string $bundle
   *   The entity bundle to check. Defaults to '0', which will check for view
   *   modes that are available by default for all entities.
   *
   * @return array
   *   A list of view modes that are available to be panelized.
   */
  public function get_available_view_modes($bundle = 0);

  /**
   * Identify the view modes that are enabled for use with Panelizer.
   *
   * @param string $bundle
   *   The entity bundle to check.
   *
   * @return array
   *   A list of view modes that are panelized.
   */
  public function get_enabled_view_modes($bundle);

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
   * Confirm the view mode to be used, check if a substitute is assigned,
   * failover to 'default'.
   *
   * @param string $view_mode
   *   The original view mode to be checked.
   * @param string $bundle
   *   The entity bundle being used.
   *
   * @return string
   *   The final view mode that will be used.
   */
  public function get_view_mode($view_mode, $bundle);

  /**
   * Obtain the machine name of the Page Manager task.
   *
   * @return string
   *   The machine name for the Page Manager task; returns FALSE if this
   *   entity does not support Page Manager.
   */
  public function get_page_manager_task_name();

  /**
   * Identifies a substitute view mode for a given bundle.
   *
   * @param string $view_mode
   *   The original view mode to be checked.
   * @param string $bundle
   *   The entity bundle being checked.
   *
   * @return string
   *   The view mode that will be used.
   */
  public function get_substitute($view_mode, $bundle);

  /**
   * Obtain the system path to an entity bundle's display settings page for a
   * specific view mode.
   *
   * @param string $bundle
   * @param string $view_mode
   *
   * @return string
   *   The system path of the display settings page for this bundle/view mode
   *   combination.
   */
  public function admin_bundle_display_path($bundle, $view_mode);

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
   *   entity supports revisions, the second parameter is whether or not the
   *   user can control if a revision is created, the third states whether or
   *   not the revision is created by default.
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

  /**
   * Identify whether page manager is enabled for this entity type.
   *
   * @return bool
   */
  public function is_page_manager_enabled();
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
   * Storage for the display defaults already loaded by the system. Used in
   * default_display_exists().
   */
  private $displays = array();
  private $displays_loaded = array();

  /**
   *
   */
  private $enabled_view_modes = array();

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

      // Account for the choice permission when dealing with view modes.
      foreach ($settings['view modes'] as $view_mode => $view_mode_settings) {
        if (!empty($view_mode_settings['choice'])) {
          $items["administer panelizer $this->entity_type $bundle choice"] = array(
            'title' => t('%entity_name %bundle_name: Choose panels', array(
              '%entity_name' => $entity_info['label'],
              '%bundle_name' => $entity_info['bundles'][$bundle]['label'],
            )),
            'description' => t('Allows the user to choose which default display the entity uses.'),
          );
          // Break out of loop after finding one we just need to see if we should
          // enable the permission.
          break;
        }
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
        'page callback' => 'panelizer_entity_plugin_switcher_page',
        'type' => MENU_LOCAL_TASK,
      );

      $items[$this->plugin['entity path'] . '/panelizer'] = array(
        'title' => 'Customize display',
        // make sure this is accessible to panelize entities with no defaults.
        'page arguments' => array($this->entity_type, 'overview', $position),
        'weight' => 11,
        'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      ) + $base;

      $items[$this->plugin['entity path'] . '/panelizer/overview'] = array(
        'title' => 'Overview',
        'page arguments' => array($this->entity_type, 'overview', $position),
        'type' => MENU_DEFAULT_LOCAL_TASK,
        'weight' => -10,
      ) + $base;

      if ($this->supports_revisions) {
        $rev_base = $base;
        $rev_base['load arguments'] = array($position + 2);
        $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer'] = array(
          'title' => 'Customize display',
          // Make sure this is accessible to panelize entities with no defaults.
          'page arguments' => array($this->entity_type, 'overview', $position),
          'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
          'type' => MENU_LOCAL_TASK,
          'weight' => 11,
        ) + $rev_base;

        // Integration with Workbench Moderation.
        if (module_exists('workbench_moderation') && $this->entity_type == 'node') {
          $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer']['type'] = MENU_CALLBACK;
        }

        $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/overview'] = array(
          'title' => 'Overview',
          'page arguments' => array($this->entity_type, 'overview', $position),
          'type' => MENU_DEFAULT_LOCAL_TASK,
          'weight' => -100,
        ) + $rev_base;
      }

      // Put in all of our view mode based paths.
      $weight = 0;
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        $items[$this->plugin['entity path'] . "/panelizer/$view_mode"] = array(
          'title' => $view_mode_info['label'],
          'page arguments' => array($this->entity_type, 'settings', $position, $view_mode),
          'access arguments' => array($this->entity_type, 'access', 'admin', $position, 'settings', $view_mode),
          'weight' => $weight++,
        ) + $base;

        if ($this->supports_revisions) {
          $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/' . $view_mode] = array(
            'title' => $view_mode_info['label'],
            'page arguments' => array($this->entity_type, 'content', $position, $view_mode),
            'access arguments' => array($this->entity_type, 'access', 'admin', $position, 'content', $view_mode),
            'weight' => $weight++,
          ) + $base;
        }

        foreach (panelizer_operations() as $path => $operation) {
          $items[$this->plugin['entity path'] . '/panelizer/' . $view_mode . '/' . $path] = array(
            'title' => $operation['menu title'],
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
          'page arguments' => array($this->entity_type, 'reset', $position, $view_mode),
          'type' => MENU_CALLBACK,
        ) + $base;

        if ($this->supports_revisions) {
          $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/' . $view_mode . '/' . $path] = array(
            'title' => $operation['menu title'],
            'page arguments' => array($this->entity_type, $path, $position, $view_mode),
            'access arguments' => array($this->entity_type, 'access', 'admin', $position, $path, $view_mode),
            'weight' => $weight++,
          ) + $rev_base;

          if (isset($operation['file'])) {
            $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/' . $view_mode . '/' . $path]['file'] = $operation['file'];
          }
          if (isset($operation['file path'])) {
            $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/' . $view_mode . '/' . $path]['file path'] = $operation['file path'];
          }
        }

        // Make the 'content' URLs the local default tasks.
        $items[$this->plugin['entity path'] . '/panelizer/' . $view_mode . '/content']['type'] = MENU_DEFAULT_LOCAL_TASK;
        if ($this->supports_revisions && isset($items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/' . $view_mode . '/content']['type'])) {
          $items[$this->plugin['entity path'] . '/revisions/%panelizer_node_revision/panelizer/' . $view_mode . '/content']['type'] = MENU_DEFAULT_LOCAL_TASK;
        }
      }
    }

    if (!empty($items)) {
      ksort($items);
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
      // Load the $plugin information.
      ctools_include('export-ui');
      $plugin = ctools_get_export_ui('panelizer_defaults');

      $ui_items = $plugin['menu']['items'];

      // Change the item to a tab.
      $ui_items['list']['type'] = MENU_LOCAL_TASK;
      $ui_items['list']['weight'] = -6;
      $ui_items['list']['title'] = 'List';

      // Menu local actions are weird.
      if (isset($ui_items['add']['path'])) {
        $ui_items['add']['path'] = 'list/add';
      }
      if (isset($ui_items['import']['path'])) {
        $ui_items['import']['path'] = 'list/import';
      }

      // Edit is being handled elsewhere.
      unset($ui_items['edit callback']);
      unset($ui_items['access']);
      unset($ui_items['list callback']);
      foreach (panelizer_operations() as $path => $operation) {
        $location = isset($operation['ui path']) ? $operation['ui path'] : $path;
        if (isset($ui_items[$location])) {
          unset($ui_items[$location]);
        }
      }

      // Change the callbacks for everything.
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

        // Some of the page arguments attempt to pass the eight argument (item
        // #7, starting at 0) to the callback in order to work on the display
        // object. However, for some entities this will end up being the $op
        // instead of the object name, e.g. 'clone' instead of
        // 'taxonomy_term:tags:default'.
        if (!empty($ui_items[$key]['page arguments'][5]) && is_numeric($bundle)) {
          $ui_items[$key]['page arguments'][5] = $bundle + 3;
        }
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
   * Identify the view modes that are available for use with this entity bundle.
   *
   * @param string $bundle
   *   The entity bundle to identify. Defaults to '0', a placeholder for all
   *   bundles on this entity.
   *
   * @return array
   *   A list of view modes that are available to be panelized.
   */
  public function get_available_view_modes($bundle = 0) {
    if (!isset($this->enabled_view_modes[$bundle])) {
      $view_modes = array();
      $entity_info = entity_get_info($this->entity_type);
      $bundle_info = array();
      $view_mode_settings = array();
      if (!empty($bundle)) {
        $view_mode_settings = field_view_mode_settings($this->entity_type, $bundle);

        if (isset($entity_info['bundles'][$bundle])) {
          $bundle_info = $entity_info['bundles'][$bundle];
        }
      }

      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        // Automatically allow view modes that are part of Panels.
        if (isset($entity_info['view modes'][$view_mode])) {
          // Skip this view mode if it isn't enabled for this bundle.
          if (!empty($bundle)) {
            if (empty($view_mode_settings[$view_mode]['custom_settings'])) {
              continue;
            }
          }
          // When configuring a new bundle for an entity, the view modes that are by
          // default set to now have custom settings will be hidden, to avoid
          // confusion.
          else {
            if (isset($entity_info['view modes'][$view_mode]['custom settings']) && empty($entity_info['view modes'][$view_mode]['custom settings'])) {
              continue;
            }
          }
        }
        $this->enabled_view_modes[$bundle][$view_mode] = $view_mode_info['label'];
      }
    }

    return $this->enabled_view_modes[$bundle];
  }

  /**
   * Identify the view modes that are enabled for use with Panelizer.
   *
   * @param string $bundle
   *   The entity bundle to identify. Defaults to '0', a placeholder for all
   *   bundles on this entity.
   *
   * @return array
   *   A list of view modes that are panelized.
   */
  public function get_enabled_view_modes($bundle) {
    $enabled = array();
    $available = $this->get_available_view_modes($bundle);
  }

  /**
   * Identify the view mode that will actually be used for a specific request.
   *
   * @param string $view_mode
   *   The original view mode to be checked.
   * @param string $bundle
   *   The entity bundle being used.
   *
   * @return string
   *   The final view mode that will be used.
   */
  public function get_view_mode($view_mode, $bundle) {
    $settings = !empty($this->plugin['bundles'][$bundle]) ? $this->plugin['bundles'][$bundle] : array('status' => FALSE, 'choice' => FALSE);

    // Test to see if this view mode is actually panelizable at all.
    if (!isset($this->plugin['view modes'][$view_mode]) || (empty($this->plugin['view modes'][$view_mode]['custom settings']) && empty($this->plugin['view mode status'][$bundle][$view_mode]))) {
      $view_mode = 'default';
    }

    // See if a substitute should be used.
    $substitute = $this->get_substitute($view_mode, $bundle);
    if (!empty($substitute)) {
      $view_mode = $substitute;
    }

    return $view_mode;
  }

  /**
   * Obtain the machine name of the Page Manager task.
   *
   * @return string
   *   The machine name for the Page Manager task; returns FALSE if this
   *   entity does not support Page Manager.
   */
  public function get_page_manager_task_name() {
    if (empty($this->plugin['uses page manager'])) {
      return FALSE;
    }
    else {
      return $this->entity_type . '_view';
    }
  }

  /**
   * Identifies a substitute view mode for a given bundle.
   *
   * @param string $view_mode
   *   The original view mode to be checked.
   * @param string $bundle
   *   The entity bundle being checked.
   *
   * @return string
   *   The view mode that will be used.
   */
  public function get_substitute($view_mode, $bundle) {
    $substitute = '';

    // See if a substitute should be used.
    $settings = !empty($this->plugin['bundles'][$bundle]) ? $this->plugin['bundles'][$bundle] : array('status' => FALSE, 'choice' => FALSE);
    if (!empty($settings['view modes'][$view_mode]['substitute'])) {
      $substitute = $settings['view modes'][$view_mode]['substitute'];
    }

    return $substitute;
  }

  /**
   * Obtain the system path to an entity bundle's display settings page for a
   * specific view mode.
   *
   * @param string $bundle
   * @param string $view_mode
   *
   * @return string
   *   The system path of the display settings page for this bundle/view mode
   *   combination.
   */
  public function admin_bundle_display_path($bundle, $view_mode) {
    $path = $this->entity_admin_root;

    $pos = strpos($path, '%');
    if ($pos !== FALSE) {
      $path = substr($path, 0, $pos) . $bundle;
    }

    $path .= '/display';
    if ($view_mode != 'default') {
      $path .= '/' . $view_mode;
    }

    return $path;
  }

  /**
   * Check if the necessary Page Manager display is enabled and the appropriate
   * variant has not been disabled.
   *
   * @return boolean
   *   Whether or not both the Page Manager display and the variant are enabled.
   */
  public function check_page_manager_status() {
    $pm_links = array(
      '!pm' => l('Page Manager', 'admin/structure/pages'),
      '!panels' => l('Panels', 'admin/structure/panels'),
      '!task_name' => $this->get_page_manager_task_name(),
      '!entity_type' => $this->entity_type,
    );

    // The display in Page Manager must be enabled.
    if ($this->is_page_manager_enabled()) {
      drupal_set_message(t('Note: "!task_name" display must be enabled in !pm in order for the !entity_type full page display ("Full page override") to work correctly.', $pm_links), 'warning', FALSE);
      return FALSE;
    }
    // The Panelizer variant must also be enabled.
    else {
      $task = page_manager_get_task($pm_links['!task_name']);
      $handler = page_manager_load_task_handler($task, '', 'term_view_panelizer');
      if (!empty($handler->disabled)) {
        drupal_set_message(t('The "Panelizer" variant on the "!task_name" display is currently not enabled in !pm. This must be enabled for Panelizer to be able to display !entity_types using the "Full page override" view mode.', $pm_links), 'warning', FALSE);
        return FALSE;
      }
    }

    return TRUE;
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
    $settings = !empty($this->plugin['bundles'][$bundle]) ? $this->plugin['bundles'][$bundle] : array('status' => FALSE, 'choice' => FALSE);
    $entity_info = entity_get_info($this->entity_type);
    $perms_url = url('admin/people/permissions');
    $manage_display = t('Manage Display');
    $bundle_info = array();
    if (isset($entity_info['bundles'][$bundle])) {
      $bundle_info = $entity_info['bundles'][$bundle];
      if (!empty($bundle_info['admin']['real path'])) {
        $manage_display = l($manage_display, $bundle_info['admin']['real path'] . '/display');
      }
    }
    $view_mode_settings = array();
    if (!empty($bundle)) {
      $view_mode_settings = field_view_mode_settings($this->entity_type, $bundle);
    }

    $form['panelizer'] = array(
      '#type' => 'fieldset',
      '#title' => t('Panelizer'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#group' => 'additional_settings',
      '#attributes' => array(
        'class' => array('panelizer-entity-bundle'),
      ),
      '#bundle' => $bundle,
      '#location' => $type_location,
      '#tree' => TRUE,
      '#access' => panelizer_administer_entity_bundle($this, $bundle),
      '#attached' => array(
        'js' => array(ctools_attach_js('panelizer-entity-bundle', 'panelizer')),
      ),
    );

    // The master checkbox.
    $form['panelizer']['status'] = array(
      '#title' => t('Panelize'),
      '#type' => 'checkbox',
      '#default_value' => !empty($settings['status']),
      '#id' => 'panelizer-status',
      '#description' => t('Allow content of this type to have its display controlled by Panelizer. Once enabled, each individual view mode will have further options and will add <a href="!perm_url">several new permissions</a>.', array('!perm_url' => $perms_url)) . '<br />'
        . t('Other than "Full page override" and "Default", only view modes enabled through the Custom Display Settings section of the !manage_display tab will be available for use.', array('!manage_display' => $manage_display)) . '<br />'
        . t('Once enabled, a new tab named "Customize display" will show on pages for this content.'),
    );

    // Help, I need somebody.
    $form['panelizer']['help'] = array(
      '#title' => t('Optional help message to show above the display selector, if applicable'),
      '#type' => 'textarea',
      '#rows' => 3,
      '#default_value' => !empty($settings['help']) ? $settings['help'] : '',
      '#id' => 'panelizer-help',
      '#description' => t('Only used if one or more of the view modes has a display that allows multiple values and the "Customize display" tab is to be shown on the entity edit form. Allows HTML.'),
      '#states' => array(
        'visible' => array(
          '#panelizer-status' => array('checked' => TRUE),
        ),
      ),
    );

    $view_modes = $this->get_available_view_modes($bundle);
    foreach ($view_modes as $view_mode => $view_mode_label) {
      $view_mode_info = $this->plugin['view modes'][$view_mode];

      $form['panelizer']['view modes'][$view_mode] = array(
        '#type' => 'item',
        // '#title' => '<hr />' . $view_mode_info['label'],
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
          ),
        ),
      );

      // Show the optional view mode description.
      $pm_links = array(
        '!pm' => l('Page Manager', 'admin/structure/pages'),
        '!panels' => l('Panels', 'admin/structure/panels'),
        '!entity_type' => $this->entity_type,
        '!task_name' => $this->get_page_manager_task_name(),
      );

      $description = '';
      if ($view_mode == 'default') {
        $description = t('If a requested view mode for an entity was not enabled in the !manage_display tab page, this view mode will be used as a failover. For example, if "Teaser" was being used but it was not enabled.', array('!manage_display' => $manage_display));
      }
      elseif ($view_mode == 'page_manager') {
        $description = t("A custom view mode only used when !pm/!panels is used to control this entity's full page display, i.e. the '!task_name' display is enabled. Unlike the \"!full\" view mode, this one allows customization of the page title.",
          $pm_links + array(
            '!full' => !empty($entity_info['view modes']['full']['label']) ? $entity_info['view modes']['full']['label'] : 'Full',
          ));
      }
      elseif ($view_mode == 'full') {
        $description = t('Used when viewing !entity_type entities on their standalone page, does not allow customization of the page title.', array('!entity_type' => $this->entity_type));
      }
      elseif ($view_mode == 'teaser') {
        $description = t('Used in content lists by default, e.g. on the default homepage and on taxonomy term pages.');
      }
      elseif ($view_mode == 'rss') {
        $description = t('Used by the default RSS content lists.');
      }
      $form['panelizer']['view modes'][$view_mode]['status'] = array(
        '#title' => $view_mode_info['label'],
        '#type' => 'checkbox',
        '#default_value' => !empty($settings['view modes'][$view_mode]['status']),
        '#id' => 'panelizer-' . $view_mode . '-status',
        '#prefix' => '<hr />',
        '#description' => $description,
        '#attributes' => array(
          'title' => $view_mode_info['label'],
        ),
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
          ),
        ),
      );
      if ($view_mode == 'page_manager') {
        if (!$this->is_page_manager_enabled()) {
          $form['panelizer']['view modes'][$view_mode]['status']['#title'] .= ' (<em>'
            . t('!pm is enabled correctly', $pm_links)
            . '</em>)';
        }
        else {
          $form['panelizer']['view modes'][$view_mode]['status']['#title'] .= ' (<em>'
            . t('"!task_name" must be enabled in !pm', $pm_links)
            . '</em>)';
          // Only display this message if the form has not been submitted, the
          // bundle has been panelized and the view mode is panelized.
          if (empty($form_state['input']) && !empty($settings['status']) && !empty($settings['view modes'][$view_mode]['status'])) {
            drupal_set_message(t('Note: "!task_name" display must be enabled in !pm in order for the !entity_type full page display ("Full page override") to work correctly.', $pm_links), 'warning', FALSE);
          }
        }
      }

      $options = array('' => t('- Ignore this option -')) + $view_modes;
      unset($options[$view_mode]);
      $form['panelizer']['view modes'][$view_mode]['substitute'] = array(
        '#title' => t('Substitute a different view mode in place of this one'),
        '#description' => t("Allows this view mode to be enabled but for the actual display to be handled by another view mode. This can save on configuration effort should multiple view modes need to look the same."),
        '#type' => 'select',
        '#options' => $options,
        '#default_value' => $this->get_substitute($view_mode, $bundle),
        '#id' => 'panelizer-' . $view_mode . '-substitute',
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
          ),
        ),
      );

      $form['panelizer']['view modes'][$view_mode]['default'] = array(
        '#title' => t('Provide an initial display named "Default"'),
        '#type' => 'checkbox',
        '#default_value' => !empty($settings['view modes'][$view_mode]['status']) && !empty($settings['view modes'][$view_mode]['default']),
        '#id' => 'panelizer-' . $view_mode . '-initial',
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-substitute' => array('value' => ''),
          ),
        ),
      );

      // Obtain a list of all available panels for this view mode / bundle.
      $panelizers = $this->get_default_panelizer_objects($bundle . '.' . $view_mode);
      $options = array();
      if (!empty($panelizers)) {
        foreach ($panelizers as $name => $panelizer) {
          // Don't show disabled displays.
          if (empty($panelizer->disabled)) {
            $options[$name] = $panelizer->title;
          }
        }
      }
      if (!empty($options)) {
        ksort($options);
      }

      // The default display to be used if nothing found.
      $default_name = implode(':', array($this->entity_type, $bundle, 'default'));
      $variable_name = 'panelizer_' . $this->entity_type . ':' . $bundle . ':' . $view_mode . '_selection';
      if ($view_mode != 'page_manager') {
        $default_name .= ':' . $view_mode;
      }
      // If this has not been set previously, use the 'default' as the default
      // selection.
      $default_value = variable_get($variable_name, FALSE);
      if (empty($default_value)) {
        $default_value = $default_name;
      }
      // Indicate which item is actually the default.
      if (count($options) > 1 && isset($options[$default_value])) {
        $options[$default_value] .= ' (' . t('default') . ')';
      }

      if (!empty($bundle_info) && count($options) > 0) {
        $form['panelizer']['view modes'][$view_mode]['selection'] = array(
          '#title' => t('Default panel'),
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $default_value,
          '#id' => 'panelizer-' . $view_mode . '-default',
          '#states' => array(
            'visible' => array(
              '#panelizer-status' => array('checked' => TRUE),
              '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
              '#panelizer-' . $view_mode . '-substitute' => array('value' => ''),
            ),
          ),
          '#required' => count($options),
          '#disabled' => count($options) == 0,
          '#description' => t('The default display to be used for new %bundle records. If "Allow panel choice" is not enabled, the item selected will be used for any new %bundle record. All existing %bundle records will have to be manually updated to the new selection.', array('%bundle' => $bundle)),
        );

        $form['panelizer']['view modes'][$view_mode]['default revert'] = array(
          '#type' => 'checkbox',
          '#title' => t('Update existing entities to use this display'),
          '#states' => array(
            'visible' => array(
              '#panelizer-status' => array('checked' => TRUE),
              '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
              '#panelizer-' . $view_mode . '-substitute' => array('value' => ''),
            ),
          ),
          '#description' => t('Will update all %bundle records to use the newly selected display, unless they have been customized. Note: only takes effect when the display is changed, and will not work if the default was not assigned previously.', array('%bundle' => $bundle)),
          '#field_prefix' => '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;',
        );
      }

      // Control whether the default can be selected.
      $form['panelizer']['view modes'][$view_mode]['choice'] = array(
        '#title' => t('Allow per-record display choice'),
        '#type' => 'checkbox',
        '#default_value' => !empty($settings['view modes'][$view_mode]['status']) && !empty($settings['view modes'][$view_mode]['choice']),
        '#id' => 'panelizer-' . $view_mode . '-choice',
        '#states' => array(
          'visible' => array(
            '#panelizer-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-status' => array('checked' => TRUE),
            '#panelizer-' . $view_mode . '-substitute' => array('value' => ''),
          ),
        ),
        '#description' => t("Allows multiple displays to be created for this view mode. Once created, a selector will be provided on the %bundle record's edit form allowing the display of this view mode to be chosen. Additionally, any customizations made will be based upon the selected display. Note: the selector will not be shown if there is only one display, instead the default will be automatically selected.", array('%bundle' => $bundle)),
      );
      if (!empty($bundle)) {
        $form['panelizer']['view modes'][$view_mode]['choice']['#description'] .= '<br />'
          . t('This option adds a <a href="!perm_url">new permission</a>: !perm',
            array(
              '!perm_url' => $perms_url,
              '!perm' => t('%entity_name %bundle_name: Choose panels',
                array(
                  '%entity_name' => $entity_info['label'],
                  '%bundle_name' => $entity_info['bundles'][$bundle]['label'],
                )
              ),
            )
          );
      }
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

    // Check to see if the bundle has changed. If so, we need to move stuff
    // around.
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
      if (!empty($panelizer_defaults)) {
        foreach ($panelizer_defaults as $panelizer) {
          list($entity_type, $old_bundle, $name) = explode(':', $panelizer->name);
          $panelizer->name = implode(':', array($entity_type, $new_bundle, $name));
          if ($panelizer->view_mode != 'page_manager') {
            $panelizer->name .= ':' . $panelizer->view_mode;
          }

          // The default display selection.
          $old_variable_name = 'panelizer_' . $this->entity_type . ':' . $bundle . ':' . $panelizer->view_mode . '_selection';
          $new_variable_name = 'panelizer_' . $this->entity_type . ':' . $new_bundle . ':' . $panelizer->view_mode . '_selection';
          $default_layout = variable_get($old_variable_name, NULL);
          if (!is_null($default_layout)) {
            variable_set($new_variable_name, $default_layout);
            variable_del($old_variable_name);
          }

          $panelizer->panelizer_key = $new_bundle;
          // If there's a pnid this should change the name and retain the pnid.
          // If there is no pnid this will create a new one in the database
          // because exported panelizer defaults attached to a bundle will have
          // to be moved to the database in order to follow along and then be
          // re-exported.
          // @todo Should we warn the user about this?
          ctools_export_crud_save('panelizer_defaults', $panelizer);
        }
      }
    }

    // Fix the configuration.
    // If the main configuration is disabled then everything gets disabled.
    if (empty($form_state['values']['panelizer']['status'])) {
      $form_state['values']['panelizer']['view modes'] = array();
    }
    elseif (!empty($form_state['values']['panelizer']['view modes'])) {
      // Make sure each setting is disabled if the view mode is disabled.
      foreach ($form_state['values']['panelizer']['view modes'] as $view_mode => &$config) {
        if (empty($config['status'])) {
          foreach ($config as $key => $val) {
            $config[$key] = 0;
          }
        }
      }
    }

    // Save the default display for this bundle to a variable so that it may be
    // controlled separately.
    foreach ($this->get_default_panelizer_objects($new_bundle) as $panelizer) {
      if (isset($form_state['values']['panelizer']['view modes'][$panelizer->view_mode]['selection'])) {
        $variable_name = 'panelizer_' . $this->entity_type . ':' . $new_bundle . ':' . $panelizer->view_mode . '_selection';
        $old_value = variable_get($variable_name, NULL);
        $new_value = $form_state['values']['panelizer']['view modes'][$panelizer->view_mode]['selection'];

        // Save the variable.
        variable_set($variable_name, $new_value);

        // Cleanup.

        // Additional cleanup if the default display was changed.
        if (!is_null($old_value) && $old_value != $new_value) {
          // The user specifically requested that existing entities are to be
          // updated to the new display.
          if (!empty($form_state['values']['panelizer']['view modes'][$panelizer->view_mode]['default revert'])) {
            $updated_count = db_update('panelizer_entity')
              ->fields(array('name' => $new_value))
              ->condition('name', $old_value)
              ->execute();
            drupal_set_message(t('@count @entity records were updated to the new Panelizer display for the @mode view mode.', array('@count' => $updated_count, '@entity' => $this->entity_type, '@mode' => $panelizer->view_mode)));

            // If EntityCache is enabled, clear all records of this type. This
            // is a little heavy-handed, but I don't believe there's an easy way
            // to clear only entities of certain types without querying for them
            // first, which could trigger an execution timeout.
            if (module_exists('entitycache')) {
              cache_clear_all('*', 'cache_entity_' . $this->entity_type, TRUE);
            }
          }
        }
      }
    }

    // Remove some settings that shouldn't be saved with the others.
    if (!empty($form_state['values']['panelizer']['view modes'])) {
      foreach ($form_state['values']['panelizer']['view modes'] as $view_mode => $settings) {
        unset($form_state['values']['panelizer']['view modes'][$view_mode]['selection']);
        unset($form_state['values']['panelizer']['view modes'][$view_mode]['default revert']);
      }
    }

    variable_set('panelizer_defaults_' . $this->entity_type . '_' . $new_bundle, $form_state['values']['panelizer']);

    // Verify the necessary Page Manager prerequisites are ready.
    if (!empty($form_state['values']['panelizer']['status'])
      && !empty($form_state['values']['panelizer']['view modes']['page_manager']['status'])
      && $this->is_page_manager_enabled()) {
      $this->check_page_manager_status();
    }

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

  public function get_default_display_default_name($bundle, $view_mode = 'page_manager') {
    $default_name = implode(':', array($this->entity_type, $bundle, 'default'));

    if ($view_mode != 'page_manager') {
      $default_name .= ':' . $view_mode;
    }

    return $default_name;
  }

  public function get_default_display_name($bundle, $view_mode = 'page_manager') {
    $variable_name = $this->get_default_display_variable_name($bundle, $view_mode);
    // If this has not been set previously, use the 'default' as the default
    // selection.
    $default_value = variable_get($variable_name, FALSE);

    if (empty($default_value)) {
      $default_value = $this->get_default_display_default_name($bundle, $view_mode);
    }

    return $default_value;
  }

  public function default_display_exists($display_name) {
    // If the display name is empty then the display doesn't exist.
    if (empty($display_name)) {
      return FALSE;
    }

    $parts = explode(':', $display_name);
    // If the display name doesn't contain the entity_type, the bundle and the
    // display machine name, then it's an invalid name.
    if (count($parts) <= 2) {
      return FALSE;
    }

    // The entity bundle is the second part of the $display_name string.
    $bundle = $parts[1];

    // If no check was performed already to see if displays exist for this
    // bundle, try loading them.
    if (empty($this->displays_loaded[$bundle])) {
      $this->displays_loaded[$bundle] = TRUE;
      $displays = $this->get_default_panelizer_objects($bundle);
      $this->displays = array_merge($this->displays, $displays);
    }

    return isset($this->displays[$display_name]);
  }

  public function get_default_display_variable_name($bundle, $view_mode = 'page_manager') {
    return 'panelizer_' . $this->entity_type . ':' . $bundle . ':' . $view_mode . '_selection';
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

    if (empty($ids)) {
      return;
    }

    // Load all the panelizers associated with the list of entities.
    if ($this->supports_revisions) {
      $result = db_query("SELECT * FROM {panelizer_entity} WHERE entity_type = :entity_type AND revision_id IN (:vids)", array(':entity_type' => $this->entity_type, ':vids' => $vids));
    }
    else {
      $result = db_query("SELECT * FROM {panelizer_entity} WHERE entity_type = :entity_type AND entity_id IN (:ids)", array(':entity_type' => $this->entity_type, ':ids' => $ids));
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
        // Skip disabled view modes.
        $check_needed = array_key_exists($view_mode, $this->plugin['bundles'][$bundles[$entity_id]]['view modes']);
        $view_mode_disabled = empty($this->plugin['bundles'][$bundles[$entity_id]]['view modes'][$view_mode]['status']);
        if ($check_needed === FALSE || $view_mode_disabled) {
          continue;
        }

        // Load the default display for this entity bundle / view_mode.
        $name = $this->get_default_display_name($bundles[$entity_id], $view_mode);

        // If no panelizer was loaded for the view mode, queue up defaults.
        if (empty($panelizers[$entity_id][$view_mode]) && $this->has_default_panel($bundles[$entity_id] . '.' . $view_mode)) {
          $defaults[$name] = $name;
        }
        // Otherwise unpack the loaded panelizer.
        else if (!empty($panelizers[$entity_id][$view_mode])) {
          $entity->panelizer[$view_mode] = ctools_export_unpack_object('panelizer_entity', $panelizers[$entity_id][$view_mode]);
          // If somehow we have no name AND no did, fill in the default.
          // This can happen if use of defaults has switched around maybe?
          if (empty($entity->panelizer[$view_mode]->did) && empty($entity->panelizer[$view_mode]->name)) {
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
    if (!empty($defaults)) {
      $panelizer_defaults = $this->load_default_panelizer_objects($defaults);
    }

    // if any panelizers were loaded, get their attached displays.
    if (!empty($dids)) {
      $displays = panels_load_displays($dids);
    }

    // Now, go back through our entities and assign dids and defaults
    // accordingly.
    foreach ($entities as $entity_id => $entity) {
      // Skip not panelized bundles.
      if (empty($bundles[$entity_id])) {
        continue;
      }

      // Reload these.
      list(, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

      // Check for each view mode.
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        if (empty($entity->panelizer[$view_mode])) {
          // Load the configured default display.
          $default_value = $this->get_default_display_name($bundle, $view_mode);

          if (!empty($panelizer_defaults[$default_value])) {
            $entity->panelizer[$view_mode] = clone $panelizer_defaults[$default_value];
            // Make sure this entity can't write to the default display.
            $entity->panelizer[$view_mode]->did = NULL;
            $entity->panelizer[$view_mode]->entity_id = 0;
            $entity->panelizer[$view_mode]->revision_id = 0;
          }
        }
        elseif (empty($entity->panelizer[$view_mode]->display) || empty($entity->panelizer[$view_mode]->did)) {
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
              // Reload the settings from the default configuration.
              $entity->panelizer[$view_mode] = clone $panelizer_defaults[$entity->panelizer[$view_mode]->name];
              $entity->panelizer[$view_mode]->did = NULL;
              $entity->panelizer[$view_mode]->entity_id = $entity_id;
              $entity->panelizer[$view_mode]->revision_id = $revision_id;
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

    // Check to see if this entity is a translation of another entity.
    // If so, use the source entity's panelizer information to clone it.
    if (isset($entity->translation_source) && isset($entity->translation_source->panelizer)) {
      $entity->panelizer = $entity->translation_source->panelizer;
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
      // Don't write out empty records.
      if (empty($panelizer)) {
        continue;
      }

      // Just a safety check to make sure we can't have a missing view mode.
      if (empty($view_mode)) {
        $view_mode = 'page_manager';
      }

      // In certain circumstances $panelizer will be the default's name rather
      // than a full object.
      if (!is_object($panelizer) && is_array($panelizer) && !empty($panelizer['name'])) {
        $panelizer = $this->get_default_panelizer_object($bundle . '.' . $view_mode, $panelizer['name']);
        $panelizer->did = NULL;

        // Ensure original values are maintained.
        $panelizer->entity_id = $entity_id;
        $panelizer->revision_id = $revision_id;
      }

      // If this is a default display, skip saving it.
      $default_display = $this->get_default_display_default_name($bundle, $view_mode);
      if (!empty($panelizer->name) && $panelizer->name == $default_display) {
        continue;
      }

      // On entity insert, we only write the display if it is not a default.
      // That probably means it came from an export or deploy or something
      // along those lines.
      if (empty($panelizer->name) && !empty($panelizer->display)) {
        // Ensure we don't accidentally overwrite existing display
        // data or anything silly like that.
        $panelizer = $this->clone_panelizer($panelizer, $entity);
        // Ensure that Panels storage is set correctly.
        $panelizer->display->storage_type = 'panelizer_entity';
        $panelizer->display->storage_id = implode(':', array($this->entity_type, $entity_id, $view_mode));
        // First write the display
        panels_save_display($panelizer->display);

        // Make sure we have the new did.
        $panelizer->did = $panelizer->display->did;
      }

      // To prevent overwriting a cloned entity's $panelizer object, clone it.
      else {
        // Store $panelizer->name as  it is removed by clone_panelizer().
        $stored_name = $panelizer->name;

        // Clone the $panelizer object.
        $panelizer = $this->clone_panelizer($panelizer, $entity);

        // Restore the original $panelizer->name.
        $panelizer->name = $stored_name;
      }

      // Make sure there is a view mode.
      if (empty($panelizer->view_mode)) {
        $panelizer->view_mode = $view_mode;
      }

      // And write the new record.
      drupal_write_record('panelizer_entity', $panelizer);
    }
  }

  public function hook_entity_update($entity) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    if (!$this->is_panelized($bundle)) {
      return;
    }

    // If there's no panelizer information on the entity then there is nothing
    // to do.
    if (empty($entity->panelizer)) {
      return;
    }

    // Allow exports or older data to be deployed successfully.
    if (is_object($entity->panelizer)) {
      $entity->panelizer = array('page_manager' => $entity->panelizer);
    }

    // When updating many/most objects, make sure the previous revision's
    // configuration is loaded too, as they won't be automatically loaded.
    // @todo There may be another way of handling this.
    if (isset($entity->original, $entity->original->panelizer)) {
      foreach ($entity->original->panelizer as $view_mode => $panelizer) {
        if (!isset($entity->panelizer[$view_mode])) {
          $entity->panelizer[$view_mode] = clone $panelizer;
        }
      }
    }

    // Update each panelizer configuration.
    foreach ($entity->panelizer as $view_mode => $panelizer) {
      // Don't write out empty records.
      if (empty($panelizer)) {
        continue;
      }

      // In some cases $panelizer is array, convert it to an object.
      if (is_array($panelizer)) {
        $panelizer = (object) $panelizer;
      }

      // Just a safety check to make sure we can't have a missing view mode.
      if (empty($view_mode)) {
        $view_mode = 'page_manager';
      }

      // In certain circumstances $panelizer will be the default's name rather
      // than a full object.
      if (!is_object($panelizer) && is_array($panelizer) && !empty($panelizer['name'])) {
        $panelizer = $this->get_default_panelizer_object($bundle . '.' . $view_mode, $panelizer['name']);
        $panelizer->did = NULL;

        // Ensure original values are maintained.
        $panelizer->entity_id = $entity_id;
        $panelizer->revision_id = $revision_id;
      }

      // If this is a default display, and a change wasn't made, skip saving it.
      $default_display = $this->get_default_display_default_name($bundle, $view_mode);
      if (empty($panelizer->display_is_modified)
          && !empty($panelizer->name) && $panelizer->name == $default_display) {
        // Delete the existing record for this revision/entity if one existed
        // before and a new revision was not being saved.
        if (empty($entity->revision)) {
          // Only delete the display for this specific revision.
          $this->delete_entity_panelizer($entity, $view_mode, TRUE);
        }
        continue;
      }

      // Determine whether an existing Panelizer record needs to be updated or
      // a new one created.
      $update = array();

      // This entity supports revisions.
      if ($this->supports_revisions) {
        // If no revision value is assigned, indicating that no record was
        // previously saved for this entity/view_mode combination, or a new
        // revision is being created, create a new {panelizer_entity} record.
        if (empty($panelizer->revision_id) || $panelizer->revision_id != $revision_id) {
          $panelizer->revision_id = $revision_id;
          // If this has a custom display, flag the system that the display
          // needs to be saved as a new record.
          if (!empty($panelizer->did)) {
            $panelizer->display_is_modified = TRUE;
          }
        }
        // This entity is being updated.
        else {
          $update = array('entity_type', 'entity_id', 'revision_id', 'view_mode');
        }
      }
      // This entity does not support revisions.
      else {
        // There is no entity_id set yet, the record was never saved before.
        if (empty($panelizer->entity_id)) {
          // Nothing to do.
        }
        // This record is being updated.
        else {
          $update = array('entity_type', 'entity_id', 'view_mode');
        }
      }

      // The editor will set this flag if the display is modified. This lets
      // us know if we need to clone a new display or not.
      // NOTE: This means that when exporting or deploying, we need to be sure
      // to set the display_is_modified flag to ensure this gets written.
      if (!empty($panelizer->display_is_modified)) {
        // Check if this display is shared and avoid changing other revisions
        // displays.
        $has_shared_display_args = array(
          ':entity_type' => $this->entity_type,
          ':entity_id' => $entity_id,
          ':revision_id' => $revision_id,
          ':did' => $panelizer->did,
        );
        $has_shared_display = db_query('SELECT COUNT(did) FROM {panelizer_entity} WHERE entity_type = :entity_type AND entity_id = :entity_id AND revision_id <> :revision_id AND did = :did', $has_shared_display_args)->fetchField();

        // If this is a new entry or the entry is using a display from a
        // default, or revision is enabled and this is a shared display, clone
        // the display.
        if (!$update || empty($panelizer->did) || !empty($has_shared_display)) {
          $entity->panelizer[$view_mode] = $panelizer = $this->clone_panelizer($panelizer, $entity);

          // Update the cache key since we are adding a new display
          $panelizer->display->cache_key = implode(':', array_filter(array('panelizer', $panelizer->entity_type, $panelizer->entity_id, $view_mode, $revision_id)));
        }

        // Ensure that Panels storage is set correctly.
        $panelizer->display->storage_type = 'panelizer_entity';
        $panelizer->display->storage_id = implode(':', array($this->entity_type, $entity_id, $view_mode));

        // First write the display.
        panels_save_display($panelizer->display);

        // Make sure we have the did.
        $panelizer->did = $panelizer->display->did;

        // Ensure that we always write this as NULL when we have our own panel:
        $panelizer->name = NULL;
      }
      else {
        $panelizer->entity_type = $this->entity_type;
        $panelizer->entity_id = $entity_id;
        // The (int) ensures that entities that do not support revisions work
        // since the revision_id cannot be NULL.
        $panelizer->revision_id = (int) $revision_id;

        // Make sure we keep the same did as the original if the layout wasn't
        // changed.
        if (empty($panelizer->name) && empty($panelizer->did) && !empty($entity->original->panelizer[$view_mode]->did)) {
          $panelizer->did = $entity->original->panelizer[$view_mode]->did;
          $update = array('entity_type', 'entity_id', 'revision_id', 'view_mode');
        }
      }

      // Make sure there is a view mode.
      if (empty($panelizer->view_mode)) {
        $panelizer->view_mode = $view_mode;
      }

      // Make sure there is a 'did' value. This can happen when the value is
      // passed via inline_entity_form.
      if (!isset($panelizer->did)) {
        $panelizer->did = 0;
      }

      // Save the record.
      drupal_write_record('panelizer_entity', $panelizer, $update);

      // If there was a CSS value saved before, update the exported file. This
      // is done after the entity is updated to ensure that the next page load
      // gets the new file.
      ctools_include('css');
      $cache_key = implode(':', array_filter(array('panelizer', $this->entity_type, $entity_id, $view_mode, $revision_id)));
      $filename = ctools_css_retrieve($cache_key);
      if ($filename) {
        ctools_css_clear($cache_key);
      }
      if (!empty($panelizer->css)) {
        ctools_css_store($cache_key, $panelizer->css);
      }
    }
  }

  public function hook_entity_delete($entity) {
    $this->delete_entity_panelizer($entity);
  }

  public function hook_field_attach_delete_revision($entity) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    // Locate and delete all displays associated with the entity.
    $revisions = db_query("SELECT revision_id, did FROM {panelizer_entity} WHERE entity_type = :type AND entity_id = :id", array(':type' => (string) $this->entity_type, ':id' => $entity_id))->fetchAllAssoc('revision_id');

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
    // Need to track the number of actual visible widgets because
    // element_get_visible_children doesn't handle nested fields.
    $visible_widgets = 0;

    foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
      $view_bundle = $bundle . '.' . $view_mode;

      $panelizers = $this->get_default_panelizer_objects($view_bundle);

      // Ignore view modes that don't have a choice, have no displays defined,
      // or already have their own custom display set up.
      if (!$this->has_panel_choice($view_bundle) || empty($panelizers) || !empty($entity->panelizer[$view_mode]->did)) {
        continue;
      }

      $options = array();
      foreach ($panelizers as $name => $panelizer) {
        if (empty($panelizer->disabled)) {
          $options[$name] = $panelizer->title ? $panelizer->title : t('Default');
        }
      }

      // Load the configured default display.
      $default_value = $this->get_default_display_name($bundle, $view_mode);

      // The selected value.
      $selected = $default_value;
      if (!empty($entity->panelizer[$view_mode]->name)) {
        $selected = $entity->panelizer[$view_mode]->name;
      }

      // Only display the selector if options were available.
      if (!empty($options)) {
        // Indicate which item is the default.
        if (isset($options[$default_value])) {
          $options[$default_value] .= ' (' . t("default for '@bundle'", array('@bundle' => $bundle)) . ')';
        }

        // If only one option is available, don't show the selector.
        if (count($options) === 1) {
          $widgets[$view_mode]['name'] = array(
            '#title' => $view_mode_info['label'],
            '#type' => 'value',
            '#value' => $selected,
            // Put these here because submit does not get a real entity with the
            // actual *(&)ing panelizer.
            '#revision_id' => isset($entity->panelizer[$view_mode]->revision_id) ? $entity->panelizer[$view_mode]->revision_id : NULL,
            '#entity_id' => isset($entity->panelizer[$view_mode]->entity_id) ? $entity->panelizer[$view_mode]->entity_id : NULL,
          );
        }
        else {
          $widgets[$view_mode]['name'] = array(
            '#title' => $view_mode_info['label'],
            '#type' => 'select',
            '#options' => $options,
            '#default_value' => $selected,
            '#required' => TRUE,
            // Put these here because submit does not get a real entity with the
            // actual *(&)ing panelizer.
            '#revision_id' => isset($entity->panelizer[$view_mode]->revision_id) ? $entity->panelizer[$view_mode]->revision_id : NULL,
            '#entity_id' => isset($entity->panelizer[$view_mode]->entity_id) ? $entity->panelizer[$view_mode]->entity_id : NULL,
          );
          $visible_widgets++;
        }
      }
    }

    // Only display this if the entity has visible options available.
    if (!empty($widgets)) {
      $form_state['panelizer has choice'] = TRUE;
      $form['panelizer'] = array(
        '#type' => 'fieldset',
        '#access' => $this->panelizer_access('choice', $entity, $view_mode),
        '#title' => t('Customize display'),
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

      // Optional fieldset description.
      if (!empty($this->plugin['bundles'][$bundle]['help'])) {
        $form['panelizer']['#description'] = $this->plugin['bundles'][$bundle]['help'];
      }

      // If there are no visible widgets, don't display the fieldset.
      if ($visible_widgets == 0) {
        $form['panelizer']['#access'] = FALSE;
      }
    }
  }

  public function hook_field_attach_submit($entity, &$form, &$form_state) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);
    if (!empty($form_state['panelizer has choice'])) {
      foreach ($this->plugin['view modes'] as $view_mode => $view_mode_info) {
        if (isset($form_state['values']['panelizer'][$view_mode]['name'])) {
          $entity->panelizer[$view_mode] = $this->get_default_panelizer_object($bundle . '.' . $view_mode, $form_state['values']['panelizer'][$view_mode]['name']);
          if (!empty($entity->panelizer[$view_mode])) {
            $entity->panelizer[$view_mode]->did = NULL;

            // Ensure original values are maintained, if they exist.
            if (isset($form['panelizer'][$view_mode]['name'])) {
              $entity->panelizer[$view_mode]->entity_id = $form['panelizer'][$view_mode]['name']['#entity_id'];
              $entity->panelizer[$view_mode]->revision_id = $form['panelizer'][$view_mode]['name']['#revision_id'];
            }
          }
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
   *   An array. The first parameter is a boolean as to whether or not the
   *   entity supports revisions, the second parameter is whether or not the
   *   user can control if a revision is created, the third states whether or
   *   not the revision is created by default.
   */
  public function entity_allows_revisions($entity) {
    return array(
      // Whether or not the entity supports revisions.
      FALSE,

      // Whether or not the user can control if a revision is created.
      FALSE,

      // Whether or not the revision is created by default.
      FALSE,
    );
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
    $og_access = FALSE;
    if (is_object($bundle)) {
      $entity = $bundle;
      list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

      // Additional support for Organic Groups.
      // @todo move to og_panelizer_access();
      if (module_exists('og')) {
        if (og_is_group($this->entity_type, $entity)) {
          $og_access = og_user_access($this->entity_type, $entity_id, "administer panelizer og_group $op");
        }
        else {
          $og_groups = og_get_entity_groups($this->entity_type, $entity);
          foreach ($og_groups as $og_group_type => $og_gids) {
            foreach ($og_gids as $og_gid) {
              if (og_user_access($og_group_type, $og_gid, "administer panelizer $this->entity_type $bundle $op")) {
                $og_access = TRUE;
              }
            }
          }
        }
      }

      // If there is an $op, this must actually be panelized in order to pass.
      // If there is no $op, then the settings page can provide us a "panelize
      // it!" page even if there is no display.
      if ($op && $op != 'overview' && $op != 'settings' && $op != 'choice' && empty($entity->panelizer[$view_mode])) {
        return FALSE;
      }
    }

    // Invoke hook_panelizer_access().
    $panelizer_access = module_invoke_all('panelizer_access', $op, $this->entity_type, $bundle, $view_mode);
    array_unshift($panelizer_access, user_access('administer panelizer'), user_access("administer panelizer {$this->entity_type} {$bundle} {$op}"));
    $panelizer_access[] = $og_access;

    // Trigger hook_panelizer_access_alter().
    // We can't pass this many parameters to drupal_alter, so stuff them into
    // an array.
    $options = array(
      'op' => $op,
      'entity_type' => $this->entity_type,
      'bundle' => $bundle,
      'view_mode' => $view_mode
    );
    drupal_alter('panelizer_access', $panelizer_access, $options);

    foreach ($panelizer_access as $access) {
      if ($access) {
        return $access;
      }
    }
    return FALSE;
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
      $context = array(
        'entity' => $entity,
        'view_mode' => $view_mode,
        'status' => $status,
        'panelized' => $panelized,
      );
      drupal_alter('panelizer_overview_links', $links_array, $this->entity_type, $context);

      $links = theme('links', array(
        'links' => $links_array,
        'attributes' => array('class' => array('links', 'inline')),
      ));

      $row[] = $links;
      $rows[] = $row;
    }

    return array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#prefix' => '<p>'
        . t('Changes made here will override the default (Panelizer) displays and will only affect this @entity.', array('@entity' => $this->entity_type))
        . "</p>\n",
    );
  }

  /**
   * Provides the base panelizer URL for an entity.
   */
  function entity_base_url($entity, $view_mode = NULL) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $path_elements[] = $entity_id;

    $path = $this->plugin['entity path'];
    if ($this->supports_revisions) {
      $current_entities = entity_load($this->entity_type, array($entity_id));
      $current_entity = array_pop($current_entities);
      if ($revision_id !== $current_entity->vid) {
        $path_elements[] = $revision_id;
        $path .= '/revisions/%';
      }
    }

    $bits = explode('/', $path);
    foreach ($bits as $count => $bit) {
      if (strpos($bit, '%') === 0) {
        $bits[$count] = array_shift($path_elements);
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
    // Integration with Workbench Moderation: these local tabs will be
    // automatically added via the menu system.
    if (module_exists('workbench_moderation') && isset($bundle->workbench_moderation) && $bundle->workbench_moderation['my_revision']->vid == $bundle->workbench_moderation['current']->vid) {
      return $output;
    }

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
    $links = '<div class="clearfix">' . theme('links', array(
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
      'revision info' => $this->entity_allows_revisions($entity),
      'panelizer' => $panelizer,
      'view_mode' => $view_mode,
      'no_redirect' => TRUE,
    );

    ctools_include('common', 'panelizer');
    $output = drupal_build_form('panelizer_reset_entity_form', $form_state);

    if (!empty($form_state['executed'])) {
      $this->reset_entity_panelizer($entity, $view_mode);
      drupal_set_message(t('Panelizer display information has been reset.'));
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
      $entity->panelizer[$view_mode] = $form_state['panelizer'];

      // Make sure that entity_save knows that the panelizer settings are
      // modified and must be made local to the entity.
      if (empty($panelizer->did) || !empty($panelizer->name)) {
        $panelizer->display_is_modified = TRUE;
      }

      // Update the entity.
      $this->entity_save($entity);

      drupal_set_message(t('The settings have been updated.'));

      // Redirect.
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
      'entity' => $entity,
      'revision info' => $this->entity_allows_revisions($entity),
      'panelizer' => &$panelizer,
      'panelizer type' => $this->entity_type,
      'cache key' => $cache_key,
      'no_redirect' => TRUE,
    );

    ctools_include('common', 'panelizer');
    $output = drupal_build_form('panelizer_default_context_form', $form_state);
    if (!empty($form_state['executed'])) {
      if (!empty($form_state['clicked_button']['#write'])) {
        $entity->panelizer[$view_mode] = $form_state['panelizer'];

        // Make sure that entity_save knows that the panelizer settings are
        // modified and must be made local to the entity.
        if (empty($panelizer->did) || !empty($panelizer->name)) {
          $panelizer->display_is_modified = TRUE;
        }

        // Update the entity.
        $this->entity_save($entity);

        drupal_set_message(t('The settings have been updated.'));
      }
      else {
        drupal_set_message(t('Changes have been discarded.'));
      }

      // Clear the context cache.
      panelizer_context_cache_clear($this->entity_type, $cache_key);

      // Redirect.
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
      'allowed_layouts' => panelizer_get_allowed_layouts_option($this->entity_type, $bundle),
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
      'display cache' => panels_edit_cache_get(implode(':', array_filter(array('panelizer', $this->entity_type, $entity_id, $view_mode, $revision_id)))),
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
   * @param string $view_mode
   *   The view mode to delete. If not specified, all view modes will be
   *   deleted.
   * @param bool $one_revision
   *   Whether to delete all revisions for this entity, or a specific one.
   */
  function delete_entity_panelizer($entity, $view_mode = NULL, $one_revision = FALSE) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    // Locate any displays associated with the entity.
    $query = db_select('panelizer_entity', 'pe')
      ->fields('pe', array('did'))
      ->condition('entity_type', $this->entity_type)
      ->condition('entity_id', $entity_id);
    if (!empty($view_mode)) {
      $query->condition('view_mode', $view_mode);
    }
    if (!empty($revision_id) && !empty($one_revision)) {
      $query->condition('revision_id', $revision_id);
    }
    $dids = $query->execute()
      ->fetchCol();

    // Delete the Panels displays.
    foreach (array_unique(array_filter($dids)) as $did) {
      panels_delete_display($did);
    }

    // Delete the {panelizer_entity} records.
    $delete = db_delete('panelizer_entity')
      ->condition('entity_type', $this->entity_type)
      ->condition('entity_id', $entity_id);
    if (!empty($view_mode)) {
      $delete->condition('view_mode', $view_mode);
    }
    if (!empty($revision_id) && !empty($one_revision)) {
      $delete->condition('revision_id', $revision_id);
    }
    $delete->execute();

    // Reset the entity's cache. If the EntityCache module is enabled, this also
    // resets its permanent cache.
    entity_get_controller($this->entity_type)->resetCache(array($entity_id));
  }

  /**
   * Reset displays so that the defaults can be used instead.
   *
   * @param object $entity
   *   The entity.
   * @param $view_mode
   *   The view mode to delete. If not specified, all view modes will be
   *   deleted.
   */
  function reset_entity_panelizer($entity, $view_mode = NULL) {
    // Only proceed if the view mode was customized for this entity.
    if (empty($entity->panelizer[$view_mode])) {
      drupal_set_message(t('Unable to reset this view mode'));
    }
    else {
      // Build a list of displays to delete.
      $dids = array();

      // Identify this entity's bundle.
      list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

      // Add the custom display to the list of displays to delete.
      if (!empty($entity->panelizer[$view_mode]->did)) {
        $dids[] = $entity->panelizer[$view_mode]->did;
      }

      // Update the {panelizer_entity} record.
      $entity->panelizer[$view_mode]->did = NULL;
      $entity->panelizer[$view_mode]->name = NULL;

      // Update the entity.
      $this->entity_save($entity);

      // If a new revision was not created, delete any unused displays.
      if (empty($entity->revision)) {
        // Work out which view modes to use.
        if (!empty($view_mode)) {
          $view_modes = array($view_mode);
        }
        else {
          $entity_info = entity_get_info($this->entity_type);
          $view_modes = array_keys($entity_info['view modes']);
        }

        // Locate all displays associated with the entity.
        $new_dids = db_select('panelizer_entity', 'p')
          ->fields('p', array('did'))
          ->condition('entity_type', $this->entity_type)
          ->condition('revision_id', $revision_id)
          ->condition('view_mode', $view_modes, 'IN')
          ->condition('did', '0', '>')
          ->execute()
          ->fetchCol();
        if (!empty($new_dids)) {
          $dids = array_merge($dids, $new_dids);
        }

        // Delete the display records if they are not still in use.
        foreach (array_unique($dids) as $did) {
          panels_delete_display($did);
        }

        // Delete the {panelizer_entity} records.
        db_delete('panelizer_entity')
          ->condition('entity_type', $this->entity_type)
          ->condition('revision_id', $revision_id)
          ->condition('view_mode', $view_modes, 'IN')
          ->condition('did', $dids, 'IN')
          ->execute();

        // Reset the entity's cache. If the EntityCache module is enabled, this
        // also resets its permanent cache.
        entity_get_controller($this->entity_type)->resetCache(array($entity_id));
      }
    }
  }

  /**
   * Determine if a bundle is panelized.
   */
  public function is_panelized($bundle) {
    if (strpos($bundle, '.') === FALSE) {
      $has_bundle = !empty($this->plugin['bundles'][$bundle]);
      $bundle_enabled = !empty($this->plugin['bundles'][$bundle]['status']);
      return $has_bundle && $bundle_enabled;
    }
    else {
      list($bundle, $view_mode) = explode('.', $bundle);
      $has_bundle = !empty($this->plugin['bundles'][$bundle]);
      $bundle_enabled = !empty($this->plugin['bundles'][$bundle]['status']);
      $view_mode_enabled = !empty($this->plugin['bundles'][$bundle]['view modes'][$view_mode]['status']);
      return $has_bundle && $bundle_enabled && $view_mode_enabled;
    }
  }

  /**
   * Determine if a bundle has a default display.
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

    // Is this display panelized?
    $is_panelized = $this->is_panelized($bundle);

    // Load the default setting name.
    $default = $this->get_default_display_name($bundle, $view_mode);

    // Verify the display exists.
    $display_exists = $this->default_display_exists($default);

    return $is_panelized && !empty($default) && $display_exists;
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

    // If the entity bundle is not panelized, nothing to do here.
    if (!$this->is_panelized($bundle)) {
      return array();
    }

    if (!empty($view_mode)) {
      // If this view mode is not panelized, nothing to do here.
      if (!$this->is_panelized($bundle . '.' . $view_mode)) {
        return array();
      }

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
   * Load the named default display for the bundle.
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
      $name = implode(':', array($this->entity_type, $bundle, 'default'));
      // This is the default view mode and older defaults won't have this,
      // so we don't enforce it.
      if ($view_mode != 'page_manager') {
        $name .= ':' . $view_mode;
      }
    }

    ctools_include('export');
    $panelizer = ctools_export_load_object('panelizer_defaults', 'names', array($name));
    return reset($panelizer);
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
        array('data' => $this->entity_bundle_label(), 'style' => 'white-space:nowrap;'),
        t('Panelize'),
        t('Substitute view mode'),
        t('Provide initial display'),
        t('Allow panel choice'),
        t('Default panel'),
        t('Update existing entities to use this display'),
        array('data' => t('Operations'), 'style' => 'white-space:nowrap;'),
      ),
      '#columns' => array(
        'title',
        'status',
        'substitute',
        'default',
        'choice',
        'selection',
        'default revert',
        'links',
      ),
    );

    $entity_info = entity_get_info($this->entity_type);
    $bundles = $entity_info['bundles'];

    drupal_alter('panelizer_default_types', $bundles, $this->entity_type);

    foreach ($bundles as $bundle => $bundle_info) {
      $view_mode_settings = array();
      if (!empty($bundle)) {
        $view_mode_settings = field_view_mode_settings($this->entity_type, $bundle);
      }
      $base_url = 'admin/structure/panelizer/' . $this->entity_type . '/' . $bundle;
      $bundle_id = str_replace(array('][', '_', ' '), '-', '#edit-entities-' . $this->entity_type . '-' . $bundle . '-0');

      // Add the widgets that apply only to the bundle.
      $form['entities'][$this->entity_type][$bundle][0]['title'] = array(
        '#markup' => '<strong>' . $bundle_info['label'] . '</strong>',
      );

      $form['entities'][$this->entity_type][$bundle][0]['status'] = array(
        '#type' => 'checkbox',
        '#title' => t('Panelize: @label', array('@label' => $bundle_info['label'])),
        '#title_display' => 'invisible',
        '#default_value' => !empty($this->plugin['bundles'][$bundle]['status']),
      );
      $form['entities'][$this->entity_type][$bundle][0]['help'] = array(
        '#type' => 'hidden',
        '#default_value' => !empty($this->plugin['bundles'][$bundle]['help']) ? $this->plugin['bundles'][$bundle]['help'] : '',
      );

      // Set proper allowed content link for entire bundle based on status
      if (!empty($this->plugin['bundles'][$bundle]['status'])) {
        $links_array = array();
        if (!empty($bundle_info['admin']['real path'])) {
          $links_array['displays'] = array(
            'title' => t('manage display'),
            'href' => $bundle_info['admin']['real path'] . '/display',
          );
        }
        $links_array['settings'] = array(
          'title' => t('allowed content'),
          'href' => $base_url . '/allowed',
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
          'show' => array(
            $bundle_id . '-status' => array('checked' => TRUE),
          ),
        ),
      );

      $view_modes = $this->get_available_view_modes($bundle);
      foreach ($view_modes as $view_mode => $view_mode_label) {
        $view_mode_info = $this->plugin['view modes'][$view_mode];

        $base_id = str_replace(array('][', '_', ' '), '-', '#edit-entities-' . $this->entity_type . '-' . $bundle . '-' . $view_mode);
        $base_url = 'admin/structure/panelizer/' . $this->entity_type . '/' . $bundle . '.' . $view_mode;

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
            '#markup' => '&nbsp;&nbsp;&nbsp;&nbsp;' . $view_mode_label,
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
          '#title' => t(
            'Panelize: @label, @bundle',
            array(
              '@label' => $bundle_info['label'],
              '@bundle' => $view_mode_label
            )
          ),
          '#title_display' => 'invisible',
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
            ),
          ),
        );

        $options = array('' => t('- Ignore this option -')) + $view_modes;
        unset($options[$view_mode]);
        $form['entities'][$this->entity_type][$bundle][$view_mode]['substitute'] = array(
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $this->get_substitute($view_mode, $bundle),
          '#title' => t(
            'Substitute view mode: @label, @bundle',
            array(
              '@label' => $bundle_info['label'],
              '@bundle' => $view_mode_label
            )
          ),
          '#title_display' => 'invisible',
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
            ),
          ),
        );

        $form['entities'][$this->entity_type][$bundle][$view_mode]['default'] = array(
          '#type' => 'checkbox',
          '#default_value' => !empty($settings['default']),
          '#title' => t(
            'Provide initial display: @label, @bundle',
            array(
              '@label' => $bundle_info['label'],
              '@bundle' => $view_mode_label
            )
          ),
          '#title_display' => 'invisible',
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-substitute' => array('value' => ''),
            ),
          ),
        );

        $form['entities'][$this->entity_type][$bundle][$view_mode]['choice'] = array(
          '#type' => 'checkbox',
          '#default_value' => !empty($settings['choice']),
          '#title' => t(
            'Allow panel choice: @label, @bundle',
            array(
              '@label' => $bundle_info['label'],
              '@bundle' => $view_mode_label
            )
          ),
          '#title_display' => 'invisible',
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-substitute' => array('value' => ''),
            ),
          ),
        );

        // Obtain a list of all available panels for this view mode / bundle.
        $panelizers = $this->get_default_panelizer_objects($bundle . '.' . $view_mode);
        $options = array();
        if (!empty($panelizers)) {
          foreach ($panelizers as $name => $panelizer) {
            // Don't show disabled displays.
            if (empty($panelizer->disabled)) {
              $options[$name] = $panelizer->title;
            }
          }
        }
        if (!empty($options)) {
          ksort($options);
        }

        // The default display to be used if nothing found.
        $default_name = implode(':', array($this->entity_type, $bundle, 'default'));
        $variable_name = 'panelizer_' . $this->entity_type . ':' . $bundle . ':' . $view_mode . '_selection';
        if ($view_mode != 'page_manager') {
          $default_name .= ':' . $view_mode;
        }
        // If this has not been set previously, use the 'default' as the default
        // selection.
        $default_value = variable_get($variable_name, FALSE);
        if (empty($default_value)) {
          $default_value = $default_name;
        }

        // First time this is displayed there won't be any defaults assigned, so
        // show a placeholder indicating the page needs to be saved before they
        // will show.
        if (count($options) == 0) {
          if ($default_value == $default_name) {
            $options = array('' => t('Save to access selector'));
          }
          else {
            $options = array('' => t('No displays created yet'));
          }
        }
        // Indicate which item is actually the default.
        if (count($options) > 1 && isset($options[$default_value])) {
          $options[$default_value] .= ' (' . t('default') . ')';
        }
        $form['entities'][$this->entity_type][$bundle][$view_mode]['selection'] = array(
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $default_value,
          '#title' => t(
            'Default panel: @label, @bundle',
            array(
              '@label' => $bundle_info['label'],
              '@bundle' => $view_mode_label
            )
          ),
          '#title_display' => 'invisible',
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-substitute' => array('value' => ''),
            ),
          ),
          '#disabled' => count($options) == 1,
        );
        $form['entities'][$this->entity_type][$bundle][$view_mode]['default revert'] = array(
          '#type' => 'checkbox',
          '#default_value' => FALSE,
          '#title' => t(
            'Update existing entities to use this display: @label, @bundle',
            array(
              '@label' => $bundle_info['label'],
              '@bundle' => $view_mode_label
            )
          ),
          '#title_display' => 'invisible',
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-substitute' => array('value' => ''),
            ),
          ),
          '#disabled' => count($options) == 1,
        );

        $form['entities'][$this->entity_type][$bundle][$view_mode]['links'] = array(
          '#prefix' => '<div class="container-inline">',
          '#suffix' => '</div>',
        );

        // Panelize is enabled and a default display will be provided.
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
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-default' => array('checked' => TRUE),
              $base_id . '-choice' => array('checked' => FALSE),
              $base_id . '-substitute' => array('value' => ''),
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
          $links = t('Save to access display list');
        }

        $form['entities'][$this->entity_type][$bundle][$view_mode]['links']['default2'] = array(
          '#type' => 'item',
          '#title' => $links,
          '#states' => array(
            'show' => array(
              $bundle_id . '-status' => array('checked' => TRUE),
              $base_id . '-status' => array('checked' => TRUE),
              $base_id . '-choice' => array('checked' => TRUE),
              $base_id . '-substitute' => array('value' => ''),
            ),
          ),
        );

        // Additional messages if this display is enabled.
        if (empty($form_state['input']) && $view_mode == 'page_manager' && !empty($settings['status'])) {
          $this->check_page_manager_status();
        }
      }
    }
    $form['#attached'] = array(
      'js' => array(ctools_attach_js('states-show')),
    );
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
      $settings = array(
        'status' => $values[0]['status'],
        'view modes' => array(),
      );
      if (!empty($values[0]['status'])) {
        // This field is optional so should not always be applied.
        if (isset($values[0]['help']) && !empty($values[0]['help'])) {
          $settings['help'] = $values[0]['help'];
        }

        foreach ($values as $view_mode => $config) {
          if (!empty($view_mode) && !empty($config)) {
            // Fix the configuration.
            // Make sure each setting is disabled if the view mode is disabled.
            if (empty($config['status'])) {
              foreach ($config as $key => $val) {
                $config[$key] = 0;
              }
            }

            // Save the default display for this bundle to a variable so that it
            // may be controlled separately.
            if (!empty($config['selection'])) {
              $variable_name = 'panelizer_' . $this->entity_type . ':' . $bundle . ':' . $view_mode . '_selection';
              $old_value = variable_get($variable_name, NULL);
              $new_value = $config['selection'];
              variable_set($variable_name, $config['selection']);

              // Cleanup.

              // Additional cleanup if the default display was changed.
              if (!is_null($old_value) && $old_value != $new_value) {
                // The user specifically requested that existing entities are
                // to be updated to the new display.
                if (!empty($config['default revert'])) {
                  $updated_count = db_update('panelizer_entity')
                    ->fields(array('name' => $new_value))
                    ->condition('name', $old_value)
                    ->execute();
                  drupal_set_message(t('@count @entity records were updated to the new Panelizer display for the @mode view mode.', array('@count' => $updated_count, '@entity' => $this->entity_type, '@mode' => $view_mode)));

                  // If EntityCache is enabled, clear all records of this type.
                  // This is a little heavy-handed, but I don't believe there's
                  // an easy way to clear only entities of certain types
                  // without querying for them first, which could trigger an
                  // execution timeout.
                  if (module_exists('entitycache')) {
                    cache_clear_all('*', 'cache_entity_' . $this->entity_type, TRUE);
                  }
                }
              }
            }

            // Don't save some settings with the rest of the settings bundle.
            unset($config['selection']);
            unset($config['default revert']);

            $settings['view modes'][$view_mode] = $config;
          }
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
   * @param array $extra_contexts
   *   An optional array of extra context objects that will be added to the
   *   display.
   *
   * @return array
   *   If the entity isn't panelized, this returns NULL. Otherwise, it returns an
   *   associative array as meant for use with CTools with the following keys:
   *   - 'content': String containing the rendered panels display output.
   *   - 'no_blocks': Boolean defining if the panels display wants to hide core
   *      blocks or not when being rendered.
   */
  function render_entity($entity, $view_mode, $langcode = NULL, $args = array(), $address = NULL, $extra_contexts = array()) {
    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    // Optionally substitute the view mode with another one.
    $substitute = $this->get_substitute($view_mode, $bundle);
    if (!empty($substitute)) {
      $view_mode = $substitute;
    }

    // Nothing configured for this view mode.
    if (empty($entity->panelizer[$view_mode]) || empty($entity->panelizer[$view_mode]->display)) {
      return FALSE;
    }

    $panelizer = $entity->panelizer[$view_mode];
    $display = $panelizer->display;

    $display->context = $this->get_contexts($panelizer, $entity) + $extra_contexts;
    $display->args = $args;
    $display->css_id = $panelizer->css_id;

    // This means the IPE will use our cache which means it will get appropriate
    // allowed content should it be selected.
    $display->cache_key = implode(':', array_filter(array('panelizer', $this->entity_type, $entity_id, $view_mode, $revision_id)));

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

    // Allow applications to alter the panelizer and the display before
    // rendering them.
    drupal_alter('panelizer_pre_render', $panelizer, $display, $entity);

    ctools_include('plugins', 'panels');
    $renderer = panels_get_renderer($panelizer->pipeline, $display);

    // If the IPE is enabled, but the user does not have access to edit
    // the entity, load the standard renderer instead.

    // Use class_parents so we don't try to autoload the class we are testing.
    $parents = class_parents($renderer);
    if (!empty($parents['panels_renderer_editor']) && (!$this->panelizer_access('content', $entity, $view_mode) && !$this->entity_access('update', $entity))) {
      $renderer = panels_get_renderer_handler('standard', $display);
    }

    $renderer->address = $address;

    $info = array(
      'title' => $panelizer->display->get_title(),
      'content' => panels_render_display($display, $renderer),
      'no_blocks' => !empty($panelizer->no_blocks),
    );

    $info['classes_array'] = array();

    if (!empty($panelizer->css_class)) {
      foreach (explode(' ', $panelizer->css_class) as $class) {
        $class = ctools_context_keyword_substitute($class, array(), $display->context);
        if ($class) {
          $info['classes_array'][] = drupal_html_class($class);
        }
      }
    }

    if (!empty($parents['panels_renderer_editor'])) {
      $path = drupal_get_path('module', 'panelizer');
      ctools_add_js('panelizer-ipe', 'panelizer');
      drupal_add_js($path . "/js/panelizer-ipe.js", array('group' => JS_LIBRARY));
      drupal_add_css($path . "/css/panelizer-ipe.css");
    }

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

  /**
   * Identify whether page manager is enabled for this entity type.
   */
  public function is_page_manager_enabled() {
    return variable_get('page_manager_' . $this->entity_type . '_view_disabled', TRUE);
  }
}

function panelizer_entity_default_bundle_form_submit($form, &$form_state) {
  $bundle = $form['panelizer']['#bundle'];
  $type_location = $form['panelizer']['#location'];
  $form_state['panelizer_entity_handler']->add_bundle_setting_form_submit($form, $form_state, $bundle, $type_location);
}
