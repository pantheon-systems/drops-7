<?php
/**
 * @file
 * Class for the Panelizer node entity plugin.
 */

/**
 * Panelizer Entity node plugin class.
 *
 * Handles node specific functionality for Panelizer.
 */
class PanelizerEntityNode extends PanelizerEntityDefault {
  /**
   * True if the entity supports revisions.
   */
  public $supports_revisions = TRUE;
  public $entity_admin_root = 'admin/structure/types/manage/%panelizer_node_type';
  public $entity_admin_bundle = 4;
  public $views_table = 'node';
  public $uses_page_manager = TRUE;

  public function entity_access($op, $entity) {
    // This must be implemented by the extending clas.
    return node_access($op, $entity);
  }

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity) {
    node_save($entity);
  }

  public function entity_identifier($entity) {
    return t('This node');
  }

  public function entity_bundle_label() {
    return t('Node type');
  }

  /**
   * Determine if the entity allows revisions.
   */
  public function entity_allows_revisions($entity) {
    $retval = array();

    list($entity_id, $revision_id, $bundle) = entity_extract_ids($this->entity_type, $entity);

    $node_options = variable_get('node_options_' . $bundle, array('status', 'promote'));
    $retval[0] = in_array('revision', $node_options);
    $retval[1] = user_access('administer nodes');

    return $retval;
  }

  public function settings_form(&$form, &$form_state) {
    parent::settings_form($form, $form_state);

    $warn = FALSE;
    foreach ($this->plugin['bundles'] as $info) {
      if (!empty($info['status']) && !empty($info['view modes']['page_manager']['status'])) {
        $warn = TRUE;
        break;
      }
    }

    if ($warn) {
      $task = page_manager_get_task('node_view');
      if (!empty($task['disabled'])) {
        drupal_set_message('The node template page is currently not enabled in page manager. You must enable this for Panelizer to be able to panelize nodes using the "Full page override" view mode.', 'warning');
      }

      $handler = page_manager_load_task_handler($task, '', 'node_view_panelizer');
      if (!empty($handler->disabled)) {
        drupal_set_message('The panelizer variant on the node template page is currently not enabled in page manager. You must enable this for Panelizer to be able to panelize nodes using the "Full page override" view mode.', 'warning');
      }
    }
  }

  function get_default_display($bundle, $view_mode) {
    $display = parent::get_default_display($bundle, $view_mode);
    // Add the node title to the display since we can't get that automatically.
    $display->title = '%node:title';

    // Add the node links, they probably would like these.
    $pane = panels_new_pane('node_links', 'node_links', TRUE);
    $pane->css['css_class'] = 'link-wrapper';
    $pane->configuration['build_mode'] = $view_mode;
    $pane->configuration['context'] = 'panelizer';

    // @todo -- submitted by does not exist as a pane! That's v. sad.
    $display->add_pane($pane, 'center');

    return $display;
  }

  /**
   * Implements a delegated hook_page_manager_handlers().
   *
   * This makes sure that all panelized entities have the proper entry
   * in page manager for rendering.
   */
  public function hook_default_page_manager_handlers(&$handlers) {
    $handler = new stdClass;
    $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
    $handler->api_version = 1;
    $handler->name = 'node_view_panelizer';
    $handler->task = 'node_view';
    $handler->subtask = '';
    $handler->handler = 'panelizer_node';
    $handler->weight = -100;
    $handler->conf = array(
      'title' => t('Node panelizer'),
      'context' => 'argument_entity_id:node_1',
      'access' => array(),
    );
    $handlers['node_view_panelizer'] = $handler;

    return $handlers;
  }

  /**
   * Implements a delegated hook_form_alter.
   *
   * We want to add Panelizer settings for the bundle to the node type form.
   */
  public function hook_form_alter(&$form, &$form_state, $form_id) {
    if ($form_id == 'node_type_form') {
      if (isset($form['#node_type'])) {
        $bundle = $form['#node_type']->type;
        $this->add_bundle_setting_form($form, $form_state, $bundle, array('type'));
      }
    }
  }

  public function hook_page_alter(&$page) {
    if ($_GET['q'] == 'admin/structure/types' && !empty($page['content']['system_main']['node_table'])) {
      // shortcut
      $table = &$page['content']['system_main']['node_table'];
      // Modify the header.
      $table['#header'][1]['colspan'] = 5;

      // Since we can't tell what row a type is for, but we know that they
      // were generated in this order, go through the original types
      // list:
      $types = node_type_get_types();
      $names = node_type_get_names();
      $row_index = 0;
      foreach ($names as $bundle => $name) {
        $type = $types[$bundle];
        if (node_hook($type->type, 'form')) {
          $type_url_str = str_replace('_', '-', $type->type);
          if ($this->is_panelized($bundle) && panelizer_administer_entity_bundle($this, $bundle)) {
            $table['#rows'][$row_index][] = array('data' => l(t('panelizer'), 'admin/structure/types/manage/' . $type_url_str . '/panelizer'));
          }
          else {
            $table['#rows'][$row_index][] = array('data' => '');
          }
          // Update row index for next pass:
          $row_index++;
        }
      }
    }
  }

  public function preprocess_panelizer_view_mode(&$vars, $entity, $element, $panelizer, $info) {
    parent::preprocess_panelizer_view_mode($vars, $entity, $element, $panelizer, $info);

    if (!empty($entity->promote)) {
      $vars['classes_array'][] = 'node-promoted';
    }
    if (!empty($entity->sticky)) {
      $vars['classes_array'][] = 'node-sticky';
    }
    if (empty($entity->status)) {
      $vars['classes_array'][] = 'node-unpublished';
    }
  }

  /**
   * Implements hook_views_plugins_alter().
   */
  function hook_views_plugins_alter(&$plugins) {
    // While it would be nice to genericize this plugin, there is no
    // generic entity view. This means that to genericize it we'll still
    // need to have each entity know how to do the view individually.
    // @todo make this happen.
    $path = drupal_get_path('module', 'panelizer') . '/plugins/views';
    $plugins['row']['panelizer_node_view'] = array(
      'title' => t('Panelizer display'),
      'help' => t('Render entities using the panels display for any that have been panelized.'),
      'handler' => 'panelizer_plugin_row_panelizer_node_view',
      'parent' => 'node',
      'base' => array('node'),
      'path' => $path,
      'uses options' => TRUE,
      'type' => 'normal',
      'register theme' => FALSE,
      'name' => 'panelizer_node_view',
    );
  }
}
