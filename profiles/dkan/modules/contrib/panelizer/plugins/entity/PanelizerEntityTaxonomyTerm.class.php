<?php
/**
 * @file
 * Class for the Panelizer taxonomy term entity plugin.
 */

/**
 * Panelizer Entity taxonomy term plugin class.
 *
 * Handles term specific functionality for Panelizer.
 */
class PanelizerEntityTaxonomyTerm extends PanelizerEntityDefault {
  public $entity_admin_root = 'admin/structure/taxonomy/%';
  public $entity_admin_bundle = 3;
  public $views_table = 'taxonomy_term_data';
  public $uses_page_manager = TRUE;

  public function entity_access($op, $entity) {
    // This must be implemented by the extending class.
    if ($op == 'update' || $op == 'delete') {
      return taxonomy_term_edit_access($entity);
    }

    if ($op == 'view') {
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity) {
    taxonomy_term_save($entity);
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
      $task = page_manager_get_task('term_view');
      if (!empty($task['disabled'])) {
        drupal_set_message('The taxonomy term template page is currently not enabled in page manager. You must enable this for Panelizer to be able to panelize taxonomy terms using the "Full page override" view mode.', 'warning');
      }

      $handler = page_manager_load_task_handler($task, '', 'term_view_panelizer');
      if (!empty($handler->disabled)) {
        drupal_set_message('The panelizer variant on the taxonomy term template page is currently not enabled in page manager. You must enable this for Panelizer to be able to panelize taxonomy terms using the "Full page override" view mode.', 'warning');
      }
    }
  }

  public function entity_identifier($entity) {
    return t('This taxonomy term');
  }

  public function entity_bundle_label() {
    return t('Taxonomy vocabulary');
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
    page_manager_get_task('term_view');

    $handler = new stdClass;
    $handler->disabled = FALSE; /* Edit this to true to make a default handler disabled initially */
    $handler->api_version = 1;
    $handler->name = 'term_view_panelizer';
    $handler->task = 'term_view';
    $handler->subtask = '';
    $handler->handler = 'panelizer_node';
    $handler->weight = -100;
    $handler->conf = array(
      'title' => t('Term panelizer'),
      'context' => page_manager_term_view_get_type() == 'multiple' ? 'argument_terms_1' : 'argument_term_1',
      'access' => array(),
    );
    $handlers['term_view_panelizer'] = $handler;

    return $handlers;
  }

  /**
   * Implements a delegated hook_form_alter.
   *
   * We want to add Panelizer settings for the bundle to the node type form.
   */
  public function hook_form_alter(&$form, &$form_state, $form_id) {
    if ($form_id == 'taxonomy_form_vocabulary') {
      if (isset($form['#vocabulary'])) {
        $bundle = $form['#vocabulary']->machine_name;
        $this->add_bundle_setting_form($form, $form_state, $bundle, array('machine_name'));
      }
    }
  }

  /**
   * Fetch the entity out of a build for hook_entity_view.
   *
   * @param $build
   *   The render array that contains the entity.
   */
  public function get_entity_view_entity($build) {
    $element = '#term';
    if (isset($build[$element])) {
      return $build[$element];
    }
  }

}
