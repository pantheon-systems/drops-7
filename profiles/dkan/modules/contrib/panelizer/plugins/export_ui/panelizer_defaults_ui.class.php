<?php

/**
 * @file
 * Contains the administrative UI for selectable panelizer defaults.
 */
class panelizer_defaults_ui extends ctools_export_ui {
  function init($plugin) {
    ctools_include('export');

    $this->plugin = $plugin;
    // Get rid of the list parent:
    unset($this->plugin['menu']['items']['list callback']);
  }

  function hook_menu(&$items) {
    // Change the item to a tab.
    $this->plugin['menu']['items']['list']['type'] = MENU_LOCAL_TASK;
    $this->plugin['menu']['items']['list']['weight'] = -6;
    $this->plugin['menu']['items']['list']['title'] = 'List defaults';

    // menu local actions are weird.
    $this->plugin['menu']['items']['add']['path'] = 'list/add';
    $this->plugin['menu']['items']['import']['path'] = 'list/import';

    // Edit is being handled elsewhere:
    unset($this->plugin['menu']['items']['edit callback']);
    unset($this->plugin['menu']['items']['access']);
    foreach (panelizer_operations() as $path => $operation) {
      $location = isset($operation['ui path']) ? $operation['ui path'] : $path;
      if (isset($this->plugin['menu']['items'][$location])) {
        unset($this->plugin['menu']['items'][$location]);
      }
    }

    // Change the callbacks for everything:
    foreach ($this->plugin['menu']['items'] as $key => $item) {
      $this->plugin['menu']['items'][$key]['access callback'] = 'panelizer_has_choice_callback';
      $this->plugin['menu']['items'][$key]['access arguments'] = array(4, 5, '');
      $this->plugin['menu']['items'][$key]['page callback'] = 'panelizer_export_ui_switcher_page';
      array_unshift($this->plugin['menu']['items'][$key]['page arguments'], 5);
      array_unshift($this->plugin['menu']['items'][$key]['page arguments'], 4);
    }

    parent::hook_menu($items);
  }

  function list_page($js, $input) {
    drupal_set_title($this->entity_handler->get_bundle_title($this->entity_bundle));
    return parent::list_page($js, $input);
  }

  function list_filter($form_state, $item) {
    // Reminder: This returns TRUE to exclude the item.
    if ($this->entity_handler->entity_type != $item->panelizer_type) {
      return TRUE;
    }
    if ($this->entity_bundle != $item->panelizer_key) {
      return TRUE;
    }

    if ($this->entity_view_mode != $item->view_mode) {
      return TRUE;
    }

    if (!$this->entity_handler->access_default_panelizer_object($item)) {
      return TRUE;
    }

    if (empty($item->title) && $item->name == implode(':', array($this->entity_handler->entity_type, $this->entity_bundle, 'default'))) {
      $item->title = t('Default');
    }

    return parent::list_filter($form_state, $item);
  }

  function edit_execute_form_standard(&$form_state) {
    if ($form_state['form type'] == 'clone') {
      list($x, $y, $name) = explode(':', $form_state['original name']);
      $form_state['item']->title = t('Clone of') . ' ' . $form_state['item']->title;
      $form_state['item']->name = 'clone_of_' . $name;
    }
    else if ($form_state['op'] == 'add') {
      $form_state['item']->panelizer_type = $this->entity_handler->entity_type;
      $form_state['item']->panelizer_key = $this->entity_bundle;
      $form_state['item']->view_mode = $this->entity_view_mode;
      $form_state['item']->display = $this->entity_handler->get_default_display($this->entity_bundle, $this->entity_view_mode);
    }
    return parent::edit_execute_form_standard($form_state);
  }

  function edit_form_validate(&$form, &$form_state) {
    $export_key = $this->plugin['export']['key'];
    // When adding a machine name, the entity/bundle are left off so the user
    // does not have to deal with it. We put it back here behind the scenes.
    $name = $form_state['values'][$export_key];

    form_set_value($form['info'][$export_key], implode(':', array($this->entity_handler->entity_type, $this->entity_bundle, $name)), $form_state);
  }

  // Simplest way to override the drupal_goto from parent.
  // Why isn't delete using the redirect system everything else is?
  function delete_page($js, $input, $item) {
    $clone = clone($item);
    // Change the name into the title so the form shows the right
    // value. @todo file a bug against CTools to use admin title if
    // available.
    $clone->name = $clone->title;
    $form_state = array(
      'plugin' => $this->plugin,
      'object' => &$this,
      'ajax' => $js,
      'item' => $clone,
      'op' => $item->export_type & EXPORT_IN_CODE ? 'revert' : 'delete',
      'rerender' => TRUE,
      'no_redirect' => TRUE,
    );

    $output = drupal_build_form('ctools_export_ui_delete_confirm_form', $form_state);
    if (!empty($form_state['executed'])) {
      ctools_export_crud_delete($this->plugin['schema'], $item);
      $export_key = $this->plugin['export']['key'];
      $message = str_replace('%title', check_plain($item->title), $this->plugin['strings']['confirmation'][$form_state['op']]['success']);
      drupal_set_message($message);
      drupal_goto(ctools_export_ui_plugin_base_path($this->plugin) . '/list');
    }

    return $output;
  }
}
