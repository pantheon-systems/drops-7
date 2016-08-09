<?php
/**
 * @file
 * Class for the Panelizer comment entity plugin.
 */

/**
 * Panelizer Entity comment plugin class.
 *
 * Handles comment specific functionality for Panelizer.
 */
class PanelizerEntityComment extends PanelizerEntityDefault {
  public $views_table = 'comment';
  public $uses_page_manager = FALSE;

  public function entity_access($op, $entity) {
    if ($op == 'edit') {
      return comment_access($op, $entity);
    }

    // The view operation is not implemented by core.
    if ($op == 'view') {
      return TRUE;
    }

    return FALSE;
  }

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity) {
    comment_save($entity);
  }

  public function settings_form(&$form, &$form_state) {
    parent::settings_form($form, $form_state);
  }

  public function entity_identifier($entity) {
    return t('This comment');
  }

  public function entity_bundle_label() {
    return t('Comment node type');
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
    $handler->name = 'comment_view_panelizer';
    $handler->task = 'comment_view';
    $handler->subtask = '';
    $handler->handler = 'panelizer_node';
    $handler->weight = -100;
    $handler->conf = array(
      'title' => t('Comment panelizer'),
      'context' => 'argument_entity_id:comment_1',
      'access' => array(),
    );
    $handlers['comment_view_panelizer'] = $handler;

    return $handlers;
  }
}
