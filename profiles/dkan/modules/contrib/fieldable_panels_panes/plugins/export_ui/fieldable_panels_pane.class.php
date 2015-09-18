<?php

/**
 * @file
 * Class for Export UI to manage Fieldable Panels Pane bundles.
 */

/**
 * Fieldable Panels Panes type Export UI plugin class.
 */
class fieldable_panels_pane extends ctools_export_ui {

  /**
   * Add some additional operations for handling entities.
   */
  function build_operations($item) {
    $base_path = ctools_export_ui_plugin_base_path($this->plugin);
    $name = $item->{$this->plugin['export']['key']};

    $operations['list'] = array(
      'title' => t('List'),
      'href' => $base_path . '/' . $name . '/list',
    );

    $operations['add_entity'] = array(
      'title' => t('Add Entity'),
      'href' => $base_path . '/' . $name . '/add',
    );

    $operations += parent::build_operations($item);

    $operations['field'] = array(
      'title' => t('Manage Fields'),
      'href' => $base_path . '/' . $name . '/fields',
    );

    $operations['display'] = array(
      'title' => t('Manage Display'),
      'href' => $base_path . '/' . $name . '/display',
    );

    return $operations;
  }

  /**
   * Allow users to jump right into adding fields.
   */
  function edit_form(&$form, &$form_state) {
    parent::edit_form($form, $form_state);

    if (module_exists('field_ui')) {
      $form['buttons']['save_continue'] = array(
        '#type' => 'submit',
        '#value' => t('Save and add fields'),
        '#access' => $form_state['op'] == 'add' || $form_state['op'] == 'clone',
      );
    }
  }

  /**
   * Update the form state "op" so we can properly redirect.
   */
  function edit_form_submit(&$form, &$form_state) {
    parent::edit_form_submit($form, $form_state);

    if ($form_state['triggering_element']['#parents'][0] == 'save_continue') {
      $form_state['op'] = 'save_continue';
    }
  }

  /**
   * Ensure menu gets rebuild after saving a new type.
   */
  function edit_save_form($form_state) {
    parent::edit_save_form($form_state);

    entity_info_cache_clear();
    menu_rebuild();

    if ($form_state['op'] === 'save_continue') {
      $this->plugin['redirect']['save_continue'] = $this->field_admin_path($form_state['values']['name'], 'fields');
    }
  }

  /**
   * Remove fields associated to bundles that are being deleted.
   */
  function delete_form_submit(&$form_state) {
    parent::delete_form_submit($form_state);

    if ($form_state['op'] == 'delete') {
      field_attach_delete_bundle('fieldable_panels_pane', $form_state['item']->name);
      entity_info_cache_clear();
    }
  }

  /**
   * List entities page.
   */
  function list_entities_page($js, $input, $item, $step = NULL) {
    drupal_set_title($this->get_page_title('list_entity', $item));

    return views_embed_view('fieldable_pane_entities', 'default', $item->name);
  }

  /**
   * Add entity page.
   */
  function add_entity_page($js, $input, $item, $step = NULL) {
    drupal_set_title($this->get_page_title('add_entity', $item));

    $form_state = array(
      'entity' => fieldable_panels_panes_create(array('bundle' => $item->name)),
      'add submit' => TRUE,
      'plugin' => $this->plugin,
      'object' => &$this,
      'ajax' => $js,
      'item' => $item,
      'op' => 'add_entity',
      'no_redirect' => TRUE,
      'rerender' => TRUE,
      'step' => $step,
      'function args' => func_get_args(),
    );

    // Default these to reusable.
    $form_state['entity']->reusable = TRUE;
    $output = drupal_build_form('fieldable_panels_panes_entity_edit_form', $form_state);
    if (!empty($form_state['executed'])) {
      $this->redirect($form_state['op'], $form_state['item']);
    }

    return $output;
  }

  /**
   * List footer.
   */
  function list_footer($form_state) {
    ctools_include('export');
    $items = ctools_export_crud_load_all('fieldable_panels_pane_type');
    $entity_info = entity_get_info('fieldable_panels_pane');

    $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => 2));
    $rows = array();

    if (!empty($entity_info['bundles'])) {
      foreach ($entity_info['bundles'] as $bundle => $info) {
        // Filter out bundles that already exist as ctools exportable objects.
        if (isset($items[$bundle])) {
          continue;
        }

        $row = array();

        $label = check_plain($info['label']);
        $label .= ' <small>' . t('(Machine name: @type)', array('@type' => $bundle)) . '</small>';

        $row[] = $label;

        $operations = array();

        $operations['list'] = array(
          'title' => t('list'),
          'href' => 'admin/structure/fieldable-panels-panes/manage/' . $bundle,
        );

        $operations['add'] = array(
          'title' => t('add'),
          'href' => 'admin/structure/fieldable-panels-panes/manage/' . $bundle . '/add',
        );

        $operations['fields'] = array(
          'title' => t('manage fields'),
          'href' => $this->field_admin_path($bundle, 'fields'),
        );

        $operations['display'] = array(
          'title' => t('manage display'),
          'href' => $this->field_admin_path($bundle, 'display'),
        );

        $ops = theme('links', array('links' => $operations, 'attributes' => array('class' => array('links', 'inline'))));

        $row[] = $ops;
        $rows[] = $row;
      }

      if (!empty($rows)) {
        $variables = array(
          'caption' => t('Legacy bundles that are not managed by the bundle administrative UI are listed here.'),
          'header' => $header,
          'rows' => $rows,
        );

        return theme('table', $variables);
      }
    }
  }

  /**
   * Helper method to derive paths to field ui operations.
   */
  function field_admin_path($name, $op) {
    return _field_ui_bundle_admin_path('fieldable_panels_pane', $name) . '/' . $op;
  }
}
