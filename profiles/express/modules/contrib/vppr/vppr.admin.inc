<?php
/**
 * @file
 * Vocabulary Permissions Per Role - UI.
 */

/**
 * Replacement page callback for the taxonomy overview page.
 *
 * Either return the default form or our own page.
 */
function vppr_taxonomy_overview_vocabularies() {
  if (user_access('administer taxonomy')) {
    module_load_include('inc', 'taxonomy', 'taxonomy.admin');
    return drupal_get_form('taxonomy_overview_vocabularies');
  }

  $vocabularies = taxonomy_get_vocabularies();
  $rows = array();
  foreach ($vocabularies as $vocabulary) {
    if (user_access('administer ' . $vocabulary->machine_name . ' vocabulary terms')) {
      $row = array();
      $row[] = check_plain($vocabulary->name);
      $row[] = l(t('list terms'), "admin/structure/taxonomy/$vocabulary->machine_name");
      $row[] = l(t('add terms'), "admin/structure/taxonomy/$vocabulary->machine_name/add");
      $rows[] = $row;
    }
  }
  $header = array(t('Vocabulary name'));
  $header[] = array('data' => t('Operations'), 'colspan' => '2');

  // TODO: Return a render array instead; this is a page callback, at last.
  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'empty' => t('No vocabularies available.'),
    'attributes' => array('id' => 'vppr-vocabularies'),
  ));
}
