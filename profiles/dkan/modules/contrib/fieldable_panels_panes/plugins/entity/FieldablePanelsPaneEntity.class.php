<?php
/**
 * @file
 * Class for the Panelizer fieldable_panels_pane term entity plugin.
 */

/**
 * Panelizer Entity fieldable_panels_pane term plugin class.
 *
 * Handles term specific functionality for Panelizer.
 */
class FieldablePanelsPaneEntity extends PanelizerEntityDefault {
  public $entity_admin_root = 'admin/structure/fieldable-panels-panes/%fieldable_panels_pane_type';
  public $entity_admin_bundle = 3;
  public $supports_revisions = TRUE;
  public $views_table = 'fieldable_panels_panes';

  public function entity_access($op, $entity) {
    return fieldable_panels_panes_access($op, $entity);
  }

  /**
   * Implement the save function for the entity.
   */
  public function entity_save($entity) {
    return fieldable_panels_panes_save($entity);
  }

  public function entity_identifier($entity) {
    return t('This pane');
  }

  public function entity_bundle_label() {
    return t('Pane bundle');
  }

  function get_default_display($bundle, $view_mode) {
    return parent::get_default_display($bundle, $view_mode);
  }
}
