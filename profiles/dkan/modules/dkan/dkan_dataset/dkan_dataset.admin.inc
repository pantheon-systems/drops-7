<?php
/**
 * @file
 * Administration of DKAN Dataset
 */

/**
 * Menu Callback for DKAN Dataset teaser settings.
 */
function dkan_dataset_preview_settings() {
  $form = array();

  // Help text for formats
  $form['preview_list'] = array(
    '#type' => 'fieldset',
    '#title' => t('Available preview descriptions'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['preview_list']['preview_list_table'] = array(
    '#prefix' => '<div id="preview-list-table">',
    '#suffix' => '</div>',
    '#markup' => _dkan_dataset_preview_list_table(),
  );

  $form['external_previews'] = array(
    '#type' => 'fieldset',
    '#title' => t('External Preview Settings'),
    '#description' => t('Enable or disable external preview links for all resource formats. Modify the list of available resource formats !here.', array(
      '!here' => l(t('here'), 'admin/structure/taxonomy/format'),
    )),
    '#collapsible' => FALSE,
  );
   $form['external_previews']['formats'] = array(
    '#prefix' => '<div id="external-previews-providers">',
    '#suffix' => '</div>',
    '#theme' => 'dkan_dataset_preview_settings_external_table_form',
    '#caption' => t('Enabled Preview Providers'),
    '#header' => array(t('Format'), t('Enabled Previews')),
    'rows' => array(),
  );
  $format_vocabulary = taxonomy_vocabulary_machine_name_load('format');
  $formats = taxonomy_get_tree($format_vocabulary->vid);
  $previews = dkan_dataset_teaser_get_external_previews();


  // We will check to see if the preview is is already toggled in the variable.
  // If not, we chekck the defaults set by the preview definition.
  foreach ($formats as $format) {
    $default_value = array();
    $preview_options = array();
    foreach ($previews as $machine_name => $preview) {
      // The local preview is not optional.
      if ($machine_name == 'dkan') {
        continue;
      }
      // Restrict available options to sugested formats
      if (in_array($format->name, $preview['suggested_formats'])) {
        $preview_options[$machine_name] = $preview['name'];
      }
      if (in_array($format->name, $preview['format'])) {
        $default_value[] = $machine_name;
      }
    }
    $form['external_previews']['formats']['rows'][] = array(
      'format' => array(
        '#markup' => $format->name,
      ),
      "dkan_dataset_format_previews_tid{$format->tid}" => array(
        '#type' => 'checkboxes',
        '#tree' => TRUE,
        '#title' => 'whatever',
        '#title_display' => 'invisible',
        '#options' => $preview_options,
        '#attributes' => array(),
        '#default_value' => $default_value,
      ),
    );
  }

  $form['dkan_dataset_teaser_preview_label'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('dkan_dataset_teaser_preview_label', ''),
    '#title' => t('Label for local data preview'),
    '#description' => t('Any compatible format (like CSV) can be displayed in
      the local DKAN data preview. If you enter, for instance, "Data.gov" in
      this field, the link to the preview will read "Data.gov Preview." Leave
      blank to simply display the word "Preview".'),
  );

  return system_settings_form($form);
}

/**
 * Menu Callback for DKAN Dataset settings.
 */
function dkan_dataset_api_menu_settings() {
  drupal_goto('admin/config/services/odsm');
}

/**
 * Menu Callback for DKAN Dataset form settings.
 */
function dkan_dataset_form_settings() {
  $form = array();
  $form['dkan_dataset_form_ajax'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Ajax on dataset node.'),
    '#default_value' => variable_get('dkan_dataset_form_ajax', 0),
  );
  $form['dkan_resource_form_ajax'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Ajax on resource node.'),
    '#default_value' => variable_get('dkan_resource_form_ajax', 0),
  );
  $form['dkan_dataset_form_additional_info'] = array(
    '#type' => 'checkbox',
    '#title' => t('Divide Dataset form into main form and "Additional Data" (CKAN style).'),
    '#default_value' => variable_get('dkan_dataset_form_additional_info', 1),
  );
  return system_settings_form($form);
}

/**
 * Create table of available formats.
 * @todo This should be a theme function.
 * @return string Formatted table
 */
function _dkan_dataset_preview_list_table() {
  $vars = array(
    'header' => array(t('Preview Type'), t('Information')),
    'rows' => array(),
  );

  $previews = dkan_dataset_teaser_get_external_previews();
  foreach ($previews as $preview) {
    $info = $preview['description'] . '<br /><strong>' . t('Suggested formats:') . '</strong> <em>';
    $info .= implode(', ', $preview['suggested_formats']) . '</em>';
    $vars['rows'][] = array($preview['name'], $info);
  }
  return theme('table', $vars);
}
