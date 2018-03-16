<?php
/**
 * @file
 *
 * An argument handler to load an entity using a Safeword field.
 */

/**
 * Plugins are described by creating a $plugin array which will be used by the
 * system that includes this file.
 */
$plugin = array(
  'title' => t("Entity by Safeword"),
  // keyword to use for %substitution
  'keyword' => 'safeword',
  'description' => t('Creates a single entity object from a placeholder.'),
  'context' => 'safeword_entity_by_safeword_context',
  'default' => array(
    'field' => NULL,
    'entity_type' => 'taxonomy_term',
    // 'breadcrumb' => TRUE,
  ),
  'settings form' => 'safeword_entity_by_safeword_settings_form',
  'placeholder form' => 'safeword_entity_by_safeword_ctools_argument_placeholder',
  // 'breadcrumb' => 'safeword_entity_by_safeword_breadcrumb',
);

/**
 * Discover if this argument gives us the term we crave.
 */
function safeword_entity_by_safeword_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('entity:' . $conf['entity_type']);
  }

  // There are three required values to perform this argument check.
  if (empty($arg) || empty($conf['entity_type']) || empty($conf['field'])) {
    return NULL;
  }

  // Search for entities with this machine name.
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $conf['entity_type']);
  $query->fieldCondition($conf['field'], 'machine', $arg);
  $result = $query->execute();

  // If no results were found, bail out.
  if (empty($result)) {
    return NULL;
  }

  // EFQ nests the entities two layers deep.
  $entity = NULL;
  foreach ($result as $entity_type => $record) {
    $record = reset($record);
    $record = reset($record);
    $entity = entity_load($entity_type, array($record));

    // If the entity couldn't be loaded, bail out.
    if (empty($entity)) {
      return NULL;
    }

    // The entity is nested one layer deep by entity_load().
    $entity = reset($entity);
  }

  // Compile the finished results.
  $context = ctools_context_create('entity:' . $entity_type, $entity);
  $context->original_argument = $arg;
  return $context;
}

/**
 * Settings form for the argument.
 */
function safeword_entity_by_safeword_settings_form(&$form, &$form_state, $conf) {
  // List of entity types to check against.
  $entity_types = array();
  foreach (entity_get_info() as $entity_type => $entity_spec) {
    if (!empty($entity_spec['fieldable'])) {
      $entity_types[$entity_type] = $entity_spec['label'];
    }
  }
  $form['settings']['entity_type'] = array(
    '#title' => t('Entity type to check'),
    '#type' => 'radios',
    '#options' => $entity_types,
    '#required' => TRUE,
    '#default_value' => !empty($conf['entity_type']) ? $conf['entity_type'] : array(),
  );

  // List all Safeword fields, one must be selected.
  $fields = array();
  foreach (field_info_field_map() as $field_name => $field_spec) {
    if (substr($field_spec['type'], 0, 8) == 'safeword') {
      $fields[$field_name] = $field_name;
    }
  }
  $form['settings']['field'] = array(
    '#title' => t('Field to compare against'),
    '#type' => 'radios',
    '#options' => $fields,
    '#required' => TRUE,
    '#default_value' => !empty($conf['field']) ? $conf['field'] : array(),
  );

  // $form['settings']['breadcrumb'] = array(
  //   '#title' => t('Inject hierarchy into breadcrumb trail'),
  //   '#type' => 'checkbox',
  //   '#default_value' => !empty($conf['breadcrumb']),
  //   '#description' => t('If checked, taxonomy term parents will appear in the breadcrumb trail.'),
  // );
}

/**
 * Form fragment to get an argument to convert a placeholder for preview.
 */
function safeword_entity_by_safeword_ctools_argument_placeholder($conf) {
  return array(
    '#type' => 'textfield',
    '#description' => t('Enter a name.'),
  );
}

/**
 * Inject the breadcrumb trail if necessary.
 */
// function safeword_entity_by_safeword_breadcrumb($conf, $context) {
//   if (empty($conf['breadcrumb']) || empty($context->data) || empty($context->data->tid)) {
//     return;
//   }
//
//   $breadcrumb = array();
//   $current = new stdClass();
//   $current->tid = $context->data->tid;
//   while ($parents = taxonomy_get_parents($current->tid)) {
//     $current = array_shift($parents);
//     $breadcrumb[] = l($current->name, 'taxonomy/term/' . $current->tid);
//   }
//
//   $breadcrumb = array_merge(drupal_get_breadcrumb(), array_reverse($breadcrumb));
//   drupal_set_breadcrumb($breadcrumb);
// }
