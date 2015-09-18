<?php

/**
 * @file
 * Drush support for ECK.
 *
 * Gives you ability to see entities, their bundles, and the attached fields
 * from drush.
 */

/**
 * Implements hook_drush_help().
 */
function eck_drush_help($section) {
  $output = "";
  switch ($section) {
    case 'drush:entity-construction-kit':
      $output = dt('Show fields attached to a selected entity and bundle created by entity construction kit.');
      break;

    case 'drush:entity-construction-kit-all':
      $output = dt('List all of the entities, bundles, and attached fields created by entity construction kit.');
      break;
  }
  return $output;
}

/**
 * Implements hook_drush_command().
 */
function eck_drush_command() {
  $items = array();

  $items['entity-construction-kit'] = array(
    'description' => "Show fields attached to a selected entity and bundle created by entity construction kit.",
    'drupal dependencies' => array('eck'),
    'aliases' => array('eck'),
  );
  $items['entity-construction-kit-all'] = array(
    'description' => "List all of the entities, bundles, and attached fields created by entity construction kit.",
    'drupal dependencies' => array('eck'),
    'aliases' => array('eck-all'),
  );

  return $items;
}

/**
 * Implements drush_hook_command().
 *
 * Allows the user to select which entity, and bundle to show fields.
 */
function drush_eck_entity_construction_kit() {

  $entity_objs = EntityType::loadAll();

  if (empty($entity_objs)) {
    return drush_log('Entity Constuction Kit does not have any entities.', 'warning');
  }

  if (count($entity_objs) > 1) {
    $options = array();
    foreach ($entity_objs as $et) {
      $options[] = $et->label . ", " . $et->name;
    }

    $choice = drush_choice($options, dt("\nPick an entity type:\n"));
    if ($choice === FALSE) {
      return drush_log(dt('User cancelled'), 'ok');
    }
    $entity_type = $entity_objs[$choice];
  }
  else {
    $entity_type = $entity_objs[0];
    drush_log(dt('Only one entity type detected, proceeding with @entity_label, @entity_name, ', array('@entity_label' => $entity_type->label, '@entity_name' => $entity_type->name)), 'ok');
  }

  $bundle_objs = Bundle::loadByEntityType($entity_type);
  if (count($bundle_objs) > 1) {
    $options = array();

    foreach ($bundle_objs as $bundle) {
      $options[] = $bundle->label . ', ' . $bundle->name;
    }

    $choice = drush_choice($options, dt("\nPick a bundle:\n"));

    if ($choice === FALSE) {
      return drush_log(dt('User cancelled'), 'ok');
    }

    $bundle = $bundle_objs[$choice];
  }
  else {
    $bundle = $bundle_objs[0];
    drush_log(dt('Only one bundle detected, proceeding with @bundle_label, @bundle_name, ', array('@bundle_label' => $bundle->label, '@bundle_name' => $bundle->name)), 'ok');
  }

  $rows[] = array('');
  $rows[] = array(
    dt('Entity: '),
    $entity_type->label . ', ' . $entity_type->name,
  );
  $rows[] = array('');

  $rows = array_merge($rows, _eck_get_bundle($entity_type, $bundle));

  _eck_fix_underscores($rows);

  drush_print_table($rows, TRUE);
}

/**
 * Implements drush_hook_command().
 *
 * Shows all entities, bundles, and fields.
 */
function drush_eck_entity_construction_kit_all() {

  $entity_types = EntityType::loadAll();
  if (empty($entity_types)) {
    return drush_log('Entity Constuction Kit does not have any entities.', 'warning');
  }

  $rows[] = array('');
  foreach ($entity_types as $entity_type) {

    $etlabel = $entity_type->label;
    $etname = $entity_type->name;

    $rows[] = array('');

    $rows[] = array(
      dt('Entity: '),
      $etlabel . ', ' . $etname,
    );
    $rows[] = array('');
    $rows[] = array('');

    foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
      $rows = array_merge($rows, _eck_get_bundle($entity_type, $bundle));
    }
  }

  _eck_fix_underscores($rows);

  drush_print_table($rows, TRUE);
}

/**
 * Fix the table.
 *
 * Replaces special separators in the table with lines that match the width of
 * the column.
 */
function _eck_fix_underscores(&$rows) {
  $widths = array_fill(0, count($rows[0]), 0);
  $c = 0;
  foreach ($rows as $row) {
    $c = 0;
    foreach ($row as $col) {
      $widths[$c] = max($widths[$c], drupal_strlen($col));
      $c++;
    }
  }

  $tmp = json_encode($rows);
  $c = 0;
  foreach ($widths as $width) {
    $tmp = str_replace("--$c--", str_repeat('-', $width), $tmp);
    $c++;
  }

  $rows = json_decode($tmp);
}

/**
 * Bundle visualization.
 */
function _eck_get_bundle($entity_type, $bundle) {
  $rows = array();
  $blabel = $bundle->label;
  $bname = $bundle->name;
  $instances = field_info_instances($entity_type->name, $bname);
  $tmp = array(
    array(
      t('Bundle:'),
      $blabel . ', ' . $bname,
      'Field Machine Name',
      'Widget',
    ),
    array(
      '--0--',
      '--1--',
      '--2--',
      '--3--',
    ),
  );
  $rows = array_merge($rows, $tmp);

  $first = TRUE;
  foreach ($instances as $key => $field) {
    $rows[] = array(
      $first ? 'Fields:' : '',
      $field['label'],
      $key,
      $field['widget']['type'],
    );
    $first = FALSE;
  }
  $rows[] = array('');

  return $rows;
}
