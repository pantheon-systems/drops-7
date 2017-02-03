<?php
/**
 * @file
 * Class for the Panelizer taxonomy term entity plugin.
 */

/**
 * Panelizer Entity user plugin class.
 *
 * Handles user specific functionality for Panelizer.
 */
class PanelizerEntityUser extends PanelizerEntityDefault {
  public $entity_admin_root = 'admin/config/people/accounts';
  // No bundle support so we hardcode the default bundle.
  public $entity_admin_bundle = 'user';
  public $views_table = 'users';
  public $uses_page_manager = TRUE;

  public function entity_access($op, $entity) {
    // This must be implemented by the extending class.
    if ($op == 'update' || $op == 'delete') {
      return user_edit_access($entity);
    }

    if ($op == 'view') {
      return user_view_access($entity);
    }

    return FALSE;
  }

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity) {
    // IMPORTANT NOTE: this can *only* update panelizer items!
    user_save($entity, array('panelizer' => $entity->panelizer));
  }

  public function settings_form(&$form, &$form_state) {
    parent::settings_form($form, $form_state);

    if (!empty($this->plugin['bundles']['user']['status']) && !empty($this->plugin['bundles']['user']['view modes']['page_manager']['status'])) {
      $task = page_manager_get_task('user_view');
      if (!empty($task['disabled'])) {
        drupal_set_message('The user template page is currently not enabled in page manager. You must enable this for Panelizer to be able to panelize users using the "Full page override" view mode.', 'warning');
      }

      $handler = page_manager_load_task_handler($task, '', 'user_view_panelizer');
      if (!empty($handler->disabled)) {
        drupal_set_message('The panelizer variant on the user template page is currently not enabled in page manager. You must enable this for Panelizer to be able to panelize users using the "Full page override" view mode.', 'warning');
      }
    }
  }

  public function entity_identifier($entity) {
    return t('This user');
  }

  public function entity_bundle_label() {
    return t('User');
  }

  function get_default_display($bundle, $view_mode) {
    // For now we just go with the empty display.
    // @todo come up with a better default display.
    return parent::get_default_display($bundle, $view_mode);
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
    $handler->name = 'user_view_panelizer';
    $handler->task = 'user_view';
    $handler->subtask = '';
    $handler->handler = 'panelizer_node';
    $handler->weight = -100;
    $handler->conf = array(
      'title' => t('User panelizer'),
      'context' => 'argument_entity_id:user_1',
      'access' => array(),
    );
    $handlers['user_view_panelizer'] = $handler;

    return $handlers;
  }

  /**
   * Implements a delegated hook_form_alter.
   *
   * We want to add Panelizer settings for the bundle to the node type form.
   */
  public function hook_form_alter(&$form, &$form_state, $form_id) {
    if ($form_id == 'user_admin_settings') {
      $this->add_bundle_setting_form($form, $form_state, 'user', NULL);
    }
  }

  /**
   * Fetch the entity out of a build for hook_entity_view.
   *
   * @param $build
   *   The render array that contains the entity.
   */
  public function get_entity_view_entity($build) {
    $element = '#account';
    if (isset($build[$element])) {
      return $build[$element];
    }
  }

}
