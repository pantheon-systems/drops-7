<?php

/**
 * @file
 * Defines the inline entity form controller for Nodes.
 */

class BeanInlineEntityFormController2 extends EntityInlineEntityFormController {

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

    $fields['title'] = array(
      'type' => 'property',
      'label' => t('Title'),
      'weight' => 2,
    );

    return $fields;
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

    return parent::entityForm($entity_form, $form_state);
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   */
  public function entityFormSubmit(&$entity_form, &$form_state) {
    parent::entityFormSubmit($entity_form, $form_state);
  }

  /**
   * Overrides EntityInlineEntityFormController::save().
   */
  public function save($entity, $context) {
    bean_save($entity);
  }
}
