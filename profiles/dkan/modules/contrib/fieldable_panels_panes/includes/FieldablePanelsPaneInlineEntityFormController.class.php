<?php

/**
 * @file
 * Defines the inline entity form controller for Nodes.
 */

class FieldablePanelsPaneInlineEntityFormController extends EntityInlineEntityFormController {

  /**
  * Overrides EntityInlineEntityFormController::defaultLabels().
   */
  public function defaultLabels() {
    $labels = array(
      'singular' => t('Fieldable Panels Pane'),
      'plural' => t('Fieldable Panels Panes'),
    );
    return $labels;
  }

  /**
   * Overrides EntityInlineEntityFormController::tableFields().
   */
  public function tableFields($bundles) {
    $fields = parent::tableFields($bundles);

    return $fields;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   *
   * Copied from fieldable_panels_panes_entity_edit_form().
   */
  public function entityForm($entity_form, &$form_state) {
    // Make the other form items dependent upon it.
    ctools_include('dependent');
    ctools_add_js('dependent');

    $entity = $entity_form['#entity'];
    $entity_type = 'fieldable_panels_pane';
    list(,, $bundle) = entity_extract_ids($entity_type, $entity);

    // Map these properties for entity translations.
    $entity_form['#entity_type'] = array(
      '#type' => 'value',
      '#value' => $entity->bundle,
    );
    $form_state['fieldable_panels_pane'] = $entity_form['#entity'];

    $entity_form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#default_value' => $entity->title,
      '#weight' => -10,
    );

    $entity_form['language'] = array(
      '#type' => 'value',
      '#value' => $entity->language,
    );

    $entity_form['link'] = array(
      '#weight' => -10,
    );

    $entity_form['link']['link'] = array(
      '#title' => t('Make title a link'),
      '#type' => 'checkbox',
      '#default_value' => $entity->link,
      '#description' => t('Check here to make the title link to another page.'),
      '#id' => 'edit-link',
    );

    $entity_form['link']['path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path'),
      '#description' => t('The path for this link. This can be an internal Drupal path such as %add-node or an external URL such as %drupal. Enter %front to link to the front page.', array('%front' => '<front>', '%add-node' => 'node/add', '%drupal' => 'http://drupal.org')),
      '#dependency' => array('edit-link' => array(1)),
      '#default_value' => $entity->path,
    );

    $entity_form['reusable'] = array(
      '#weight' => 10,
    );

    $entity_form['revision'] = array(
      '#weight' => 11,
    );

    if (empty($entity->fpid)) {
      $entity_form['revision']['#access'] = FALSE;
    }

    $entity_form['reusable']['reusable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Make this entity reusable'),
      '#default_value' => $entity->reusable,
      '#id' => 'edit-reusable',
    );

    $entity_form['reusable']['category'] = array(
      '#type' => 'textfield',
      '#title' => t('Category'),
      '#description' => t('The category this content will appear in the "Add content" modal. If left blank the category will be "Miscellaneous".'),
      '#dependency' => array('edit-reusable' => array(1)),
      '#default_value' => $entity->category,
    );

    $entity_form['reusable']['admin_title'] = array(
      '#type' => 'textfield',
      '#title' => t('Administrative title'),
      '#description' => t('The name this content will appear in the "Add content" modal.'),
      '#dependency' => array('edit-reusable' => array(1)),
      '#default_value' => $entity->admin_title,
    );

    $entity_form['reusable']['admin_description'] = array(
      '#type' => 'textarea',
      '#title' => t('Administrative description'),
      '#description' => t('A description of what this content is, does or is for, for administrative use.'),
      '#dependency' => array('edit-reusable' => array(1)),
      '#default_value' => $entity->admin_description,
    );

    $entity_form['revision']['revision'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create new revision'),
      '#default_value' => 1,
      '#id' => 'edit-revision',
    );

    if (!user_access('administer fieldable panels panes') || empty($entity->fpid) || $entity->vid != $entity->current_vid) {
      $form['revision']['revision']['#disabled'] = TRUE;
      $form['revision']['revision']['#value'] = TRUE;
    }

    $entity_form['revision']['log'] = array(
      '#type' => 'textarea',
      '#title' => t('Log message'),
      '#description' => t('Provide an explanation of the changes you are making. This will help other authors understand your motivations.'),
      '#dependency' => array('edit-revision' => array(1)),
      '#default_value' => '',
    );

    $langcode = entity_language('fieldable_panels_pane', $entity);
    field_attach_form('fieldable_panels_pane', $entity, $entity_form, $form_state, $langcode);

    // _field_extra_fields_pre_render() doesn't execute properly, so manually
    // set the weights.
    $extra_fields = field_info_extra_fields($entity_type, $bundle, 'form');
    foreach ($extra_fields as $name => $settings) {
      if (isset($entity_form[$name])) {
        $entity_form[$name]['#weight'] = $settings['weight'];
      }
    }

    return $entity_form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   *
   * Fixes some of the custom entity values, similar to
   * fieldable_panels_panes_entity_edit_form_submit().
   */
  public function entityFormSubmit(&$entity_form, &$form_state) {
    $info = entity_get_info($this->entityType);
    list(, , $bundle) = entity_extract_ids($this->entityType, $entity_form['#entity']);
    $entity = $entity_form['#entity'];
    $entity_values = drupal_array_get_nested_value($form_state['values'], $entity_form['#parents']);

    // Some additional adjustments necessary for FPP to save correctly.
    if (!empty($entity_values['link']['path'])) {
      $entity_values['path'] = $entity_values['link']['path'];
    }
    if (isset($entity_values['link']['link'])) {
      $entity_values['link'] = $entity_values['link']['link'];
    }
    else {
      $entity_values['link'] = 0;
    }
    // The 'reusable' option contains several sub fields.
    if (isset($entity_values['reusable']['reusable'])) {
      $reusable = $entity_values['reusable'];
      $entity_values['reusable'] = FALSE;
      $entity_values['category'] = '';
      $entity_values['admin_title'] = '';
      $entity_values['admin_description'] = '';
      foreach (array('reusable', 'category', 'admin_title', 'admin_description') as $field) {
        if (isset($reusable[$field])) {
          $entity_values[$field] = $reusable[$field];
        }
      }
    }
    // Only fix the revision log if a revision is being saved.
    $entity_values['log'] = '';
    if (isset($entity_values['revision']['revision'])) {
      if (isset($entity_values['revision']['log'])) {
        $entity_values['log'] = $entity_values['revision']['log'];
      }
      $entity_values['revision'] = $entity_values['revision']['revision'];
    }
    else {
      $entity_values['revision'] = 0;
    }

    // Copy top-level form values that are not for fields to entity properties,
    // without changing existing entity properties that are not being edited by
    // this form. Copying field values must be done using field_attach_submit().
    $values_excluding_fields = $info['fieldable'] ? array_diff_key($entity_values, field_info_instances($this->entityType, $bundle)) : $entity_values;
    foreach ($values_excluding_fields as $key => $value) {
      $entity->$key = $value;
    }

    if ($info['fieldable']) {
      field_attach_submit($this->entityType, $entity, $entity_form, $form_state);
    }
  }
}
