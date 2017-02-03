<?php

/**
 * @file
 * Defines the inline entity form controller for Nodes.
 */

class BeanInlineEntityFormController extends EntityInlineEntityFormController {

  /**
  * Overrides EntityInlineEntityFormController::labels().
   */
  public function labels() {
    $labels = array(
      'singular' => t('block'),
      'plural' => t('blocks'),
    );
    return $labels;
  }

  /**
   * Overrides EntityInlineEntityFormController::tableFields().
   */
  public function tableFields($bundles) {
    $fields = array();

    $fields['label'] = array(
      'type' => 'property',
      'label' => t('Label'),
      'weight' => 1,
    );

    return $fields;
  }

  /**
   * Overrides EntityInlineEntityFormController::defaultSettings().
   */
  public function defaultSettings() {
    $defaults = parent::defaultSettings();
    $defaults['parent_sync_label'] = FALSE;
    $defaults['parent_sync_title'] = FALSE;

    return $defaults;
  }

  /**
   * Overrides EntityInlineEntityFormController::settingsForm().
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);

    $form['parent_sync_label'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto set the block label to the parent @entity entity label', array('@entity' => $instance['entity_type'])),
      '#description' => t('This will hide the label input field and set the label to the @entity entity label.', array('@entity' => $instance['entity_type'])),
      '#default_value' => $this->settings['parent_sync_label'],
    );

    $form['parent_sync_title'] = array(
      '#type' => 'checkbox',
      '#title' => t('Auto set the block title to the parent @entity entity label', array('@entity' => $instance['entity_type'])),
      '#description' => t('This will hide the title input field and set the title to the @entity entity label.', array('@entity' => $instance['entity_type'])),
      '#default_value' => $this->settings['parent_sync_title'],
    );

    return $form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   */
  public function entityForm($entity_form, &$form_state) {
    $bean = $entity_form['#entity'];
    $type = bean_type_load($bean->type);
    $extra_fields = field_info_extra_fields('bean', $bean->type, 'form');

    if (!isset($bean->label)) {
      $bean->label = NULL;
    }

    $entity_form['label'] = array(
      '#type' => 'textfield',
      '#title' => t('@type Label', array('@type' => check_plain($type->getLabel()))),
      '#required' => TRUE,
      '#default_value' => $bean->label,
      '#maxlength' => 255,
      // Check if extra fields define a weight.
      '#weight' => !empty($extra_fields['label']) ? $extra_fields['label']['weight'] : -5,
    );

    $entity_form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('@type Title', array('@type' => check_plain($type->getLabel()))),
      '#default_value' => $bean->title,
      '#maxlength' => 255,
      // Check if extra fields define a weight.
      '#weight' => !empty($extra_fields['title']) ? $extra_fields['title']['weight'] : -5,
    );

    $entity_form['revision'] = array(
      '#weight' => 10,
    );

    if (isset($bean->is_new)) {
      $entity_form['revision']['#access'] = FALSE;
    }

    ctools_include('dependent');
    ctools_add_js('dependent');

    $entity_form['revision']['is_new_revision'] = array(
      '#type' => 'checkbox',
      '#title' => t('Create new revision'),
      '#default_value' => 1,
      '#id' => 'edit-revision',
    );

    if (isset($bean->is_new)) {
      $entity_form['default_revision'] = array(
        '#type' => 'value',
        '#value' => TRUE,
      );
    }
    else {
      $entity_form['revision']['default_revision'] = array(
        '#type' => 'checkbox',
        '#title' => t('Set Revision as default'),
        '#default_value' => $bean->isDefaultRevision(),
        '#states' => array(
          // Hide if the option above is disabled, to avoid potential dataloss.
          'invisible' => array(
            ':input[name="is_new_revision"]' => array('checked' => FALSE),
          ),
        ),
      );
    }

    $entity_form['revision']['log'] = array(
      '#type' => 'textarea',
      '#title' => t('Log message'),
      '#description' => t('Provide an explanation of the changes you are making. This will help other authors understand your motivations.'),
      '#dependency' => array('edit-revision' => array(1)),
      '#default_value' => '',
    );

    // The view mode.
    if (user_access('edit bean view mode')) {
      $bean_info = $bean->entityInfo();
      foreach ($bean_info['view modes'] as $view_mode_name => $data) {
        $view_modes[$view_mode_name] = $data['label'];
      }

      $entity_form['view_mode'] = array(
        '#title' => t('View Mode'),
        '#description' => t('Edit the view mode of the Bean'),
        '#default_value' => $bean->view_mode,
        '#type' => 'select',
        '#required' => TRUE,
        '#options' => $view_modes,
        // Check if extra fields define a weight.
        '#weight' => !empty($extra_fields['view_mode']) ? $extra_fields['view_mode']['weight'] : -8,
      );
    }
    else {
      $entity_form['view_mode'] = array(
        '#type' => 'value',
        '#value' => $bean->view_mode,
      );
    }

    // Get the Bean type form
    $entity_form += $bean->getForm($entity_form, $form_state);

    // Hide the title field if it is auto-generated.
    if ($this->settings['parent_sync_label']) {
      $entity_form['label']['#required'] = FALSE;
      $entity_form['label']['#access'] = FALSE;
    }

    // Hide the title field if it is auto-generated.
    if ($this->settings['parent_sync_title']) {
      $entity_form['title']['#required'] = FALSE;
      $entity_form['title']['#access'] = FALSE;
      // Hide the replacement field added by the Title module as well.
      if (module_exists('title')) {
        $title_field = title_field_replacement_info('bean', 'title');
        if ($title_field) {
          $title_field_name = $title_field['field']['field_name'];
          if (isset($entity_form[$title_field_name])) {
            $entity_form[$title_field_name]['#access'] = FALSE;
            $entity_form[$title_field_name]['#required'] = FALSE;
          }
        }
      }
    }

    return parent::entityForm($entity_form, $form_state);
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   */
  public function entityFormSubmit(&$entity_form, &$form_state) {
    if (isset($form_state['redirect'])) {
      $before = $form_state['redirect'];
    }

    // The parents array tells us exactly what part of the form was
    // submitted. We're going to mock out a subsection of $form_state to
    // pass to bean_form_submit instead of the whole thing.
    $parents = $entity_form['#parents'];
    $ief_form_state['bean'] = $entity_form['#entity'];
    $ief_form_state['values'] = $form_state['values'];
    foreach ($parents as $parent) {
      $ief_form_state['values'] = $ief_form_state['values'][$parent];
    }

    // Get lables to set temporary label and title.
    $display_labels = $this->labels();

    // Set label since it will be auto generated based on the parent entity.
    if (empty($ief_form_state['bean']->label) && $this->settings['parent_sync_label']) {
      $ief_form_state['values']['label'] = $display_labels['singular'];
    }

    // Set title since it will be auto generated based on the parent entity.
    if (empty($ief_form_state['bean']->title) && $this->settings['parent_sync_title']) {
      $ief_form_state['values']['title'] = $display_labels['singular'];
    }

    // Load bean.pages.inc so we can call bean_form_submit
    module_load_include('inc', 'bean', 'includes/bean.pages');

    // Saves initial bean via bean_form_submit().
    bean_form_submit($entity_form, $ief_form_state);

    // Undo the redirect set by bean_form_submit().
    if (isset($form_state['redirect'])) {
      if (isset($before)) {
        $form_state['redirect'] = $before;
      }
      else {
        unset($form_state['redirect']);
      }
    }
  }

  /**
   * Overrides EntityInlineEntityFormController::save().
   */
  public function save($entity, $context) {
    if ($this->settings['parent_sync_label'] || $this->settings['parent_sync_title']) {
      $parent_label = entity_label($context['parent_entity_type'], $context['parent_entity']);

      // Set label to the parent label.
      if ($this->settings['parent_sync_label']) {
        $entity->label = $parent_label;
      }

      // Set title to the parent label.
      if ($this->settings['parent_sync_title']) {
        $entity->title = $parent_label;
      }
    }

    bean_save($entity);
  }
}
