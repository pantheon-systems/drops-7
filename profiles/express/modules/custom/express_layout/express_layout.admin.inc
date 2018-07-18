<?php

/**
 * @file
 * Holds admin forms and functionality for the layout entity
 */
function express_layout_form($form, &$form_state, $op = 'edit', $express_layout = NULL) {

  // Set title of page
  drupal_set_title('Edit Layout');

  $form = array();

  // Add region layout diagram
  $form['region_layout']['#weight'] = 0;
  $form['region_layout']['#markup'] = _express_layout_region_diagram();

  // Load layout entity and attach fields to it
  $express_layout = entity_load_single('express_layout', arg(1));
  field_attach_form('express_layout', $express_layout, $form, $form_state);

  // Hide regions if they aren't allowed to be included by the settings form
  if ($express_layout) {
    $form_state['#express_layout_node_type'] = $express_layout->node_type;
    $settings = variable_get('express_layout_settings_' . $express_layout->node_type, NULL);
    foreach ($settings as $key => $setting) {
      if (!$setting) {
        $form[$key]['#access'] = FALSE;
      }
    }
  }

  // Get layout field title
  $layout_field_title = $form['field_feature_layout_toggle'][LANGUAGE_NONE]['#title'];
  // Unset field title
  unset($form['field_feature_layout_toggle'][LANGUAGE_NONE]['#title']);

  // Add fieldset and title
  $form['field_feature_layout_toggle']["#prefix"] = "<fieldset class=\"form-wrapper\"><legend><span class=\"fieldset-legend\">$layout_field_title</span></legend><div class=\"fieldset-wrapper\">";
  // Close fieldset
  $form['field_feature_layout_toggle']["#suffix"] = '</div></fieldset>';

  // Get feature image field title
  $layout_field_image = $form['field_feature_title_image'][LANGUAGE_NONE][0]['#title'];
  // Unset field title
  unset($form['field_feature_title_image'][LANGUAGE_NONE][0]['#title']);

  // Add fieldset and title
  $form['field_feature_title_image']["#prefix"] = "<fieldset class=\"form-wrapper\"><legend><span class=\"fieldset-legend\">$layout_field_image</span></legend><div class=\"fieldset-wrapper\">";
  // Close fieldset
  $form['field_feature_title_image']["#suffix"] = '</div></fieldset>';

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Update layout'),
    ),
    '#weight' => 500,
  );
  $form['#validate'][] = '_express_layout_validate';
  return $form;
}

/**
 * Validate fonction for express_layout_form.
 * Add #access alters from original form on validation.
 */
function _express_layout_validate($form, &$form_state) {
  // Load layout enity

  // Hide regions if they aren't allowed to be included by the settings form
  $type = $form_state['#express_layout_node_type'];
  $settings = variable_get('express_layout_settings_' . $type, NULL);
  foreach ($settings as $key => $setting) {
    if (!$setting) {
      $form_state['complete form'][$key]['#access'] = FALSE;
    }
  }


}
/**
 * Submit handler for Express Layout form
 */
function express_layout_form_submit($form, &$form_state) {

  // Loading layout entity again since IEF resets the form values
  // @todo Figure out the proper controller to not have to reload layout
  $express_layout = entity_load_single('express_layout', arg(1));

  // Make a new layout entity if node doesn't have one already.
  if ($express_layout == FALSE) {
    $node = node_load(arg(1));
    $values = array(
      'layout_id' => $node->nid,
      'title' => 'Layout: ' . $node->title,
      'node_type' => $node->type,
    );

    $express_layout = entity_create('express_layout', $values);
  }

  entity_form_submit_build_entity('express_layout', $express_layout, $form, $form_state);
  entity_save('express_layout', $express_layout);

  drupal_set_message(t('"@layout_title" has been updated.', array(
    '@layout_title' => $express_layout->title,
  )));
  $form_state['redirect'] = 'node/' . arg(1);

  if (module_exists('varnish')) {
    // Purge this node's path to reflect layout changes
    $path = base_path() . drupal_get_path_alias('node/' . arg(1));
    $host = _varnish_get_host();
    varnish_purge($host, $path);
  }
}

/**
 * Block layout settings form.
 */
function express_layout_settings_form($form, &$form_state) {
  $form = array();
  $form['description'] = array(
    '#markup' => 'Select which regions are available to place blocks for each content type.',
  );

  // Get fields on block_layout bundle and remove node id field.
  $fields = field_info_instances('express_layout', 'express_layout');
  unset($fields['field_layout_node_id']);
  // Build available fields array.
  $regions = array();
  foreach ($fields as $key => $field) {
    $regions[$key] = $field['label'];
  }

  // Get node types.
  $types = node_type_get_types();
  ksort($types);

  // Build region fields for each content type
  foreach ($types as $key => $type) {
    // Get default settings for node type
    $default = variable_get('express_layout_settings_' . $key,'');
    $type_key = 'layout-regions-' . $key;
    $form[$type_key . '-available'] = array(
      '#type' => 'fieldset',
      '#title' => $type->name,
    );
    $form[$type_key . '-available'][$key] = array(
      '#type' => 'checkboxes',
      '#options' => $regions,
      '#default_value' => $default,
    );
  }
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save Layout Settings'));
  $form['#submit'][] = 'express_layout_settings_form_submit';

  return $form;
}

/**
 * Submit function for express_layout_settings_form.
 */
function express_layout_settings_form_submit($form, &$form_state) {
  // Save regions of node types
  foreach ($form_state['values'] as $key => $value) {
    if (is_array($value)) {
      // Save settings for each content type
      variable_set('express_layout_settings_' . $key, $value);
      // Save node types that have > 0 regions
      foreach ($value as $field) {
        if ($field) {
          variable_set('express_layout_type_' . $key, $key);
        }
      }
    }
  }
  drupal_set_message(t('Layout region settings saved.'), 'status', FALSE);
}

/**
 * Manual form for attaching layouts to nodes that don't have them.
 */
function express_layout_attach_layouts_form($form, &$form_state) {
  $fields = field_info_instances('express_layout', 'express_layout');
  $form = array();
  $form['description'] = array(
    '#markup' => '<p>By clicking the button below, you\'ll add a layout entity to each node that doesn\'t have one yet.</p>',
  );
  $form['submit'] = array('#type' => 'submit', '#value' => t(' Attach Layouts to Nodes'));
  $form['#submit'][] = 'express_attach_layouts';
  return $form;
}
/**
 * Submit function for express_layout_attach_layouts_form.
 */
function express_attach_layouts() {
  // Make sure all nodes have layout entities
  $nodes = entity_load('node');
  foreach ($nodes as $node) {
    express_layout_create_layout($node);
  }
  // Migrate fields to layout entities
  // The following code currently creates new records instead of updating
  // the current records. This is so we can go back to block_selector,
  // just in case things go wrong during update. We can delete them later.
  $fields = field_info_instances('express_layout', 'express_layout');
  unset($fields['field_layout_node_id']);
  unset($fields['field_feature_layout_toggle']);
  unset($fields['field_feature_title_image']);
  foreach ($fields as $key => $field) {
    $data_field = 'field_data_' . $key;
    $result = db_query("SELECT * FROM {$data_field}");
    foreach ($result as $record) {
      $record->entity_type = 'express_layout';
      $record->bundle = 'express_layout';
      //$record->revision_id = $record->entity_id;
      drupal_write_record ($data_field, $record);
      drupal_set_message('Layout reference migrated');
    }
    $revision_field = 'field_revision_' . $key;
    $result = db_query("SELECT * FROM {$revision_field}");
    foreach ($result as $record) {
      $record->entity_type = 'express_layout';
      $record->bundle = 'express_layout';
      //$record->revision_id = $record->entity_id;
      drupal_write_record ($revision_field, $record);
      drupal_set_message('Layout reference revision migrated');
    }
  }

  // Clear the caches so the migrated content shows.
  cache_clear_all();
  drupal_set_message('Layout migration completed.');
}

/**
 * Migrate function to create layouts for each node.
 */
function express_layout_create_layout($node) {
  // Restrict content types that don't need layouts.
  if (in_array($node->type, variable_get('express_layout_node_type_ignore', array()))) {
    return;
  }
  $nid = $node->nid;
  if (!entity_load_single('express_layout', $nid)) {
    $values = array(
      'layout_id' => $nid,
      'title' => 'Layout: ' . $node->title,
      'node_type' => $node->type,
    );
    $layout = entity_create('express_layout', $values);
    entity_save('express_layout', $layout);
    drupal_set_message('Layout: ' . $node->title . ' created. Layout ID:' . $nid);
  }
}

/**
 * Make a diagram of layout regions.
 */
function _express_layout_region_diagram() {
  $html = '<p><strong>Content Regions</strong></p><table id="region-diagram">
  <tr>
    <td colspan="5">Intro Wide</td>
  </tr>
  <tr>
    <td class="fill">&nbsp;</td>
    <td colspan="3">Slider</td>
    <td class="fill">&nbsp;</td>
  </tr>
  <tr>
    <td colspan="5">Post Title Wide</td>
  </tr>
  <tr>
    <td class="fill">&nbsp;</td>
    <td colspan="3">Post Title</td>
    <td class="fill">&nbsp;</td>
  </tr>
  <tr>
    <td class="fill">&nbsp;</td>
    <td width="25%">Left Sidebar</td>
    <td>
      <table>
        <tr>
          <td>Above Content</td>
        </tr>
        <tr>
          <td>
            <table>
              <td width="33%" class="inner-left">Inner Left</td>
              <td width="33%"><strong>Main Content</strong></td>
              <td width="33%" class="inner-right">Inner Right</td>
            </table>
          </td>
        </tr>
        <tr>
          <td>Below Content</td>
        </tr>
      </table>
    </td>
    <td width="25%">Right Sidebar</td>
    <td class="fill">&nbsp;</td>
  </tr>
  <tr>
    <td colspan="5">Post Wide</td>
  </tr>
</table>

<style>
  table#region-diagram
  table#region-diagram table {
    border-collapse: collapse;
    table-layout:fixed;
  }
  table#region-diagram td {
    text-align: center;
    padding: 10px;
    border:1px solid #e7e7e7;
  }
  table#region-diagram td.fill {
    background-color:#e7e7e7;
  }
</style>';
  return $html;
}

/**
 * Form callback for advanced settings.
 */
function express_layout_advanced_configuration_form() {
  $form = array();

  $node_types = node_type_get_names();
  $form['express_layout_node_type_ignore_text'] = array(
    '#markup' => 'Exclude the content types that don\'t need layout entites.',
  );
  $form['express_layout_node_type_ignore'] = array(
    '#type' => 'checkboxes',
    '#options' => $node_types,
    '#default_value' => variable_get('express_layout_node_type_ignore', array()),
  );
  return system_settings_form($form);
}
