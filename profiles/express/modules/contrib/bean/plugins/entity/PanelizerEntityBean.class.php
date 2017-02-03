<?php

/**
 * @file
 * Class for the Panelizer bean entity plugin.
 */

/**
 * Panelizer Entity bean plugin class.
 *
 * Handles bean specific functionality for Panelizer.
 */
class PanelizerEntityBean extends PanelizerEntityDefault {
  public $supports_revisions = TRUE;
  public $entity_admin_root = 'admin/structure/block-types/manage/%bean_type_panelizer';
  public $entity_admin_bundle = 4;
  public $views_table = 'bean';
  public $uses_page_manager = FALSE;

  /**
   * Access callback.
   */
  public function entity_access($op, $entity) {
    // This must be implemented by the extending class.
    return bean_access($op, $entity);
  }

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity) {
    bean_save($entity);
  }

  /**
   * Define the visible identifier of the identity.
   */
  public function entity_identifier($entity) {
    return t('This bean');
  }

  /**
   * Define the name of bundles on the entity.
   */
  public function entity_bundle_label() {
    return t('bean type');
  }

  /**
   * Determine if the entity allows revisions.
   */
  public function entity_allows_revisions($entity) {
    $bean_type_name = $entity->type;
    $retval = array();

    $retval[0] = TRUE;
    $retval[1] = user_access("edit any $bean_type_name bean");
    $retval[2] = TRUE;

    return $retval;
  }

  /**
   * Implements a delegated hook_form_alter.
   *
   * We want to add Panelizer settings for the bundle to the bean type form.
   */
  public function hook_form_alter(&$form, &$form_state, $form_id) {
    if ($form_id == 'bean_admin_ui_type_form') {
      if (isset($form['bean_type'])) {
        $bundle = $form['bean_type']['#value']->type;
        $this->add_bundle_setting_form($form, $form_state, $bundle, array('type'));
      }
    }
  }

  /**
   * Implements a delegated hook_page_alter.
   *
   * Add panelizer links to the block types page.
   */
  public function hook_page_alter(&$page) {
    if ($_GET['q'] == 'admin/structure/block-types' && !empty($page['content']['system_main']['bean_table'])) {
      // shortcut
      $table = &$page['content']['system_main']['bean_table'];
      // Modify the header.
      $table['#header'][2]['colspan'] = 5;

      $bean_info = bean_entity_info();
      $names = $bean_info['bean']['bundles'];
      foreach ($names as $bundle => $name) {

        // @see bean_admin_ui_admin_page() for information on why we have to
        // append '_0'.
        $type_url_str = str_replace(' ', '', $name['label'] . '_0');
        if ($this->is_panelized($bundle) && panelizer_administer_entity_bundle($this, $bundle)) {
          $table['#rows'][$type_url_str][] = array('data' => l(t('panelizer'), 'admin/structure/block-types/manage/' . $bundle . '/panelizer'));
        }
        else {
          $table['#rows'][$type_url_str][] = array('data' => '');
        }
      }
    }
  }

  /**
   * Provides the base panelizer URL for a bean entity.
   *
   * We override the parent function in order to use the delta rather than raw
   * bean id.
   */
  function entity_base_url($entity, $view_mode = NULL) {
    $bits = explode('/', $this->plugin['entity path']);
    foreach ($bits as $count => $bit) {
      if (strpos($bit, '%') === 0) {
        $bits[$count] = $entity->delta;
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
   * Implements hook_views_plugins_alter().
   */
  function hook_views_plugins_alter(&$plugins) {
    $path = drupal_get_path('module', 'panelizer') . '/plugins/views';
    $plugins['row']['panelizer_bean_view'] = array(
      'title' => t('Panelizer display'),
      'help' => t('Render entities using the panels display for any that have been panelized.'),
      'handler' => 'panelizer_plugin_row_panelizer_bean_view',
      'parent' => 'bean',
      'base' => array('bean'),
      'path' => $path,
      'uses options' => TRUE,
      'type' => 'normal',
      'register theme' => FALSE,
      'name' => 'panelizer_bean_view',
    );
  }
}
