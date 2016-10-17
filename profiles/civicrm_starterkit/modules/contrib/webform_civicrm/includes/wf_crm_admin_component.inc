<?php

/**
 * @file
 * Webform CiviCRM module's alterations to webform component form.
 */
module_load_include('inc', 'webform_civicrm', 'includes/utils');

// FIXME: Temporary workaround for https://www.drupal.org/node/2389537
variable_set('webform_table', TRUE);

class wf_crm_admin_component {
  private $form;
  private $form_state;
  private $node;
  private $data;
  private $component;
  private $field;
  private $hasOptionsTable;

  function __construct(&$form, &$form_state) {
    civicrm_initialize();
    $this->form = &$form;
    $this->form_state = &$form_state;
    $this->node = node_load($form['nid']['#value']);
    $this->data = wf_crm_aval($this->node, 'webform_civicrm:data');
    $this->component = wf_crm_aval($this->node, 'webform:components:' . $this->form['cid']['#value']);
  }

  /**
   * Alter back-end webform component edit forms.
   * Called by hook_form_alter() whenever editing a webform component.
   */
  public function alterForm() {
    $fid = $this->form['form_key']['#default_value'];
    $pieces = wf_crm_explode_key($fid);
    // Is this a crm component of a CiviCRM-enabled webform?
    if (!$this->data || !$pieces) {
      return;
    }
    list(, $i, $ent, $n, $table, $name) = $pieces;
    $this->field = wf_crm_aval(wf_crm_get_fields(), $table . '_' . $name);
    // Is this a crm field or fieldset?
    if (!$this->field && $table != 'fieldset') {
      return;
    }
    $this->form['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/css/webform_civicrm_admin.css';

    if (empty($this->form['clone']['#value']) || $name == 'fieldset') {
      // Prevent users from editing the form_key and breaking things
      $this->form['form_key']['#disabled'] = TRUE;
      $this->form['form_key']['#value'] = $fid;
      $this->form['form_key']['#description'] = NULL;
      $this->form['form_key']['#attributes']['title'] = t('Automatically set for use by CiviCRM processing.');

      // Clone an entire contact when cloning their fieldset
      if (!empty($this->form['clone']['#value'])) {
        $this->form['actions']['submit']['#value'] = t('Clone Contact');
        $new = count($this->data['contact']) + 1;
        $this->form['form_key']['#value'] = implode('_', array('civicrm', $new, $ent, $n, $table, $name));
        $this->form['name']['#default_value'] = wf_crm_contact_label($new, array(), 'plain');
        ++$this->form['position']['weight']['#default_value'];
        array_unshift($this->form['#submit'], 'wf_crm_contact_clone');
        if (empty($this->form_state['input'])) {
          $types = wf_crm_get_contact_types();
          $type = $types[0][$this->data['contact'][$i]['contact'][1]['contact_type']];
          drupal_set_message(t('Press the "Clone Contact" button below and a new @type will be added to the form with all the settings for !contact. All fields within this fieldset will be cloned (be careful, that may include non-contact fields).', array('@type' => $type, '!contact' => wf_crm_contact_label($i, $this->data))), 'status', FALSE);
        }
      }
    }
    elseif (empty($this->form_state['input'])) {
      // Clone a single CiviCRM field
      drupal_set_message(t('You are cloning a CiviCRM component. Refer to the Webform CiviCRM instructions for how to set the key if you want the new field to be processed by CiviCRM. You can also clone an entire contact by clicking the clone button by their fieldset.'), 'status', FALSE);
    }

    // Show dropdown list of states
    if ($table === 'address' && $name === 'state_province_id') {
      $this->form['validation']['maxlength']['#type'] = 'hidden';
      $this->form['validation']['maxlength']['#value'] = 5;
      $this->form['value']['#type'] = 'select';
      unset($this->form['value']['#size'], $this->form['value']['#maxlength'], $this->form['value']['#description']);
      $this->form['value']['#options'] = wf_crm_get_states();
      $this->form['value']['#empty_value'] = '';
    }

    elseif ($name == 'contribution_page_id') {
      $this->form['value']['#access'] = FALSE;
      $this->form['#prefix'] = '<p>' . t('This is a placeholder for a Contribution Page.') . '</p>';
    }

    // Interface for managing options
    elseif (($this->component['type'] === 'hidden' || $this->component['type'] === 'select')) {
      $this->hasOptionsTable = $this->buildOptionsTable();
    }

    // Customize options_element a bit
    if (!$this->hasOptionsTable && $this->component['type'] === 'select') {
      $this->alterOptionsElement();
    }

    // Money fields
    if (!$this->hasOptionsTable && wf_crm_aval($this->field, 'data_type') == 'Money') {
      $this->moneyOptions();
    }

    // Simplify form for hidden fields
    elseif (($name === 'contact_id' || $name === 'external_identifier') && $this->component['type'] === 'hidden') {
      $this->form['value']['#value'] = '';
      $this->form['value']['#access'] = FALSE;
      $this->form['#prefix'] = '<p>'
        . t('There are no configuration options for this hidden field. You can use it for post processing, for example to include a link to the CiviCRM contact in an email.')
        . '</p><p>'
        . t('The webform token for this field is:')
        . '<br /><strong>' . $this->componentToken($this->form['cid']['#value'])
        . '</strong><br /><em>'
        . t('Note: this token will change if you move the field in or out of a fieldset.')
        . '</em></p>';
    }
    // Customize form for checksum element
    elseif ($name == 'cs') {
      $this->form['value']['#title'] = t('Checksum Lifespan');
      $this->form['value']['#required'] = TRUE;
      $this->form['value']['#type'] = 'textfield';
      $this->form['value']['#description'] = t('Enter the number of days for which this checksum will be valid. Enter 0 to never expire.');
      $this->form['#validate'][] = 'wf_crm_cs_validate';
      $enabled = wf_crm_enabled_fields($this->node);
      $this->form['#prefix'] = '<p>'
        . t('This field will generate a checksum for !contact after the form is submitted. You could use its webform token to create a personalized link for anonymous users.', array('!contact' => wf_crm_contact_label($i, $this->data)))
        . '</p><p>';
      if ($cid_field = wf_crm_aval($enabled, 'civicrm_' . $i . '_contact_1_contact_contact_id')) {
        $this->form['#prefix'] .= t('Example: to link users back to this form, place this line in a webform-generated email:')
          . '<br /><code>'
          . url('node/' . $this->node->nid, array('absolute' => TRUE)) . '?cid' . $i . '=';
        $this->form['#prefix'] .= $this->componentToken($cid_field)
          . '&amp;cs' . $i . '=' . $this->componentToken($this->form['cid']['#value'])
          . '</code></p>';
        $this->form['#prefix'] .= '<p>' . t('Example: to redirect to a contribution page, paste this into the redirect field on the Webform "Form Settings" tab (change id from 1 to the real id of your contribution page):')
          . '<br /><code>'
          . 'civicrm/contribute/transact?reset=1&amp;id=1'
          . '&amp;cid=' . $this->componentToken($cid_field)
          . '&amp;cs=' . $this->componentToken($this->form['cid']['#value'])
          . '</code></p>';
      }
      else {
        $this->form['#prefix'] .= t('Consider enabling the hidden contact_id field - hashed links usually require this value.') . '</p>';
      }
    }
    // Alter weights for contact component
    if ($this->form['type']['#value'] == 'civicrm_contact') {
      $this->form['display']['#weight'] = -1;
    }
    // Allow widget to be altered, unless it's a contact or file
    elseif ($this->field && !in_array($this->field['type'], array('civicrm_contact', 'file')) && empty($_GET['type'])) {
      $unusable_widgets = array('civicrm_contact', 'pagebreak', 'markup', 'layout_box', 'fieldset');
      $widgets = array_diff_key(webform_component_options(), array_flip($unusable_widgets));
      if ($this->form['type']['#value'] == 'textfield' && $table === 'address') {
        if ($name === 'state_province_id' || $name === 'county_id') {
          $widgets['textfield'] = t('Textfield / AJAX Select');
        }
      }
      $this->form['widget'] = array(
        '#type' => 'fieldset',
        '#title' => t('Widget: %type', array('%type' => wf_crm_aval($widgets, $this->form['type']['#value'], t('Unknown')))),
        '#description' => t('The default widget for this %type field is %widget. You may change it if you wish - for example, a hidden field allows you to set the value without exposing this field to the form.<br />Note: Not all widgets may be compatible with this CiviCRM field - test your form after changing widgets.', array('%type' => str_replace('#', '', $this->field['name']), '%widget' => wf_crm_aval($widgets, $this->field['type'], t('Disabled')))),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $this->form['widget']['type'] = array(
        '#type' => 'select',
        '#options' => $widgets,
        '#default_value' => $this->form['type']['#value'],
      );
      $this->form['widget']['change_widget'] = array(
        '#type' => 'submit',
        '#value' => t('Change Widget'),
        '#submit' => array('webform_component_edit_form_submit', 'wf_crm_change_widget'),
        '#states' => array(
          'invisible' => array(
            'select[name="widget[type]"]' => array('value' => $this->form['type']['#value']),
          ),
          'disabled' => array(
            'select[name="widget[type]"]' => array('value' => $this->form['type']['#value']),
          ),
        )
      );
    }
  }

  /**
   * Ensure page breaks are not placed after contribution page
   */
  public function adjustPageBreak() {
    $components = $this->node->webform['components'];
    foreach ($components as $cid => $component) {
      if ($component['form_key'] == 'civicrm_1_contribution_1_contribution_contribution_page_id') {
        // Iterate up through parents
        while ($component['pid']) {
          $component = $components[$component['pid']];
        }
        $min = $component['weight'];
      }
    }
    if (isset($min)) {
      $weight = $this->form['position']['weight']['#default_value'];
      $this->form['position']['weight'] = array('#type' => 'value');
      if ($weight >= $min) {
        $weight = $min - 1;
      }
      $this->form['position']['weight']['#value'] = $weight;
    }
  }

  /**
   * Interface similar to options_element module
   * but with the important difference that options already exist in the civi db
   * so this does not allow create/delete of options
   *
   * @return bool
   */
  private function buildOptionsTable() {
    $options = $sort = wf_crm_field_options($this->component, 'component_edit', $this->data);
    if (!$options) {
      return FALSE;
    }
    $this->form['#attached']['js'] = array(drupal_get_path('module', 'webform_civicrm') . '/js/webform_civicrm_options.js');

    $defaults_selected = array();
    if (isset($this->component['value']) && strlen($this->component['value'])) {
      foreach (explode(',', trim($this->component['value'])) as $v) {
        $defaults_selected[] = '_web_civi_option_selected_' . $v;
      }
    }
    // Get rid of stuff related to options_element module
    unset($this->form['items']);

    $this->form['value']['#type'] = 'hidden';
    $this->form['civicrm_options_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => t('Options'),
      '#id' => 'wf-crm-options-fieldset',
      '#theme' => 'webform_civicrm_options_table',
    );
    $option_keys = array();
    foreach ($options as $k => $v) {
      $option_keys['_web_civi_option_selected_' . $k] = '';
      $this->form['civicrm_options_fieldset']['civicrm_option_name_' . $k] = array(
        '#markup' => '<span class="civicrm-option-name">' . $v . '</span>',
      );
    }
    if ($this->component['type'] === 'select') {
      $this->form['civicrm_options_fieldset']['civicrm_live_options'] = array(
        '#type' => 'radios',
        '#options' => array(t('<strong>Static Options</strong> (fully configurable)'), t('<strong>Live Options</strong> (update automatically)')),
        '#default_value' => (int) !empty($this->component['extra']['civicrm_live_options']),
        '#parents' => array('extra', 'civicrm_live_options'),
      );
      $this->form['civicrm_options_fieldset']['intro'] = array(
        '#markup' => '<p><div class="live-options-hide">' .
          t('Drag the arrows to re-order these options. Click the "enabled" checkbox to show/remove an item from the form. Set the label as you want it to appear on the form.') .
          '</div><div class="live-options-show">' .
          t('You cannot control the presentation of live options. They will be loaded from the CiviCRM database every time the form is displayed.') .
          '</div><div>' .
          t('Check the "default" box for an option to be selected by default when a user views the form.') .
          '</div></p>',
      );
      // Special instructions for contact reference fields
      if (wf_crm_aval($this->field, 'data_type') == 'ContactReference') {
        $this->form['civicrm_options_fieldset']['intro'] = array(
          '#markup' => '<p>' . t('This is a contact reference field. It points to another contact on the webform. You can configure how that contact is presented by editing their "Existing Contact" field.') . '</p>' .
            '<p>' . t("Note: In most cases it is not desirable to have the selection of webform contacts exposed to the end-user so you may wish to set this field's value on the CiviCRM tab instead.") . '</p>',
        );
      }
      $options_selected = wf_crm_str2array($this->component['extra']['items']);
      // Sort weights. Unselected options will be at the bottom.
      $option_keys = $option_selected_keys = array();
      foreach ($options_selected as $k => $v) {
        if (isset($options[$k])) {
          $option_keys['_web_civi_option_selected_' . $k] = '';
          $option_selected_keys[] = '_web_civi_option_selected_' . $k;
          unset($sort[$k]);
        }
      }
      foreach ($sort as $k => $v) {
        $option_keys['_web_civi_option_selected_' . $k] = '';
      }
      $this->form['extra']['items']['#type'] = 'hidden';
      $this->form['extra']['items']['#required'] = FALSE;
      $this->form['extra']['items']['#value'] = $this->component['extra']['items'];
      $this->form['extra']['options_source']['#access'] = FALSE;
      $this->form['civicrm_options_fieldset']['civicrm_options'] = array(
        '#type' => 'checkboxes',
        '#required' => TRUE,
        '#options' => $option_keys,
        '#default_value' => $option_selected_keys,
      );
      $w = 0;
      foreach ($option_keys as $k => $v) {
        $k = str_replace('_web_civi_option_selected_', '', $k);
        $this->form['civicrm_options_fieldset']['civicrm_option_label_' . $k] = array(
          '#type' => 'textfield',
          '#size' => 30,
          '#default_value' => !empty($options_selected[$k]) ? $options_selected[$k] : $options[$k],
        );
        $this->form['civicrm_options_fieldset']['civicrm_option_weight_' . $k] = array(
          '#type' => 'textfield',
          '#size' => 3,
          '#default_value' => ++$w,
        );
      }
    }
    $this->form['civicrm_options_fieldset']['civicrm_defaults'] = array(
      '#type' => 'checkboxes',
      '#options' => array('' => '') + $option_keys,
      '#default_value' => $defaults_selected,
    );
    // Auto set multi-value option for single-valued entities
    if (empty($this->field['extra']['multiple']) && $this->component['type'] === 'select') {
      $this->form['extra']['multiple']['#type'] = 'hidden';
      $this->form['extra']['multiple']['#value'] = 0;
    }
    // Restore multiple checkbox in case options_element module removed it
    elseif ($this->component['type'] === 'select') {
      $this->form['extra']['multiple']['#type'] = 'checkbox';
      $this->form['extra']['multiple']['#title'] = t('Multiple');
      $this->form['extra']['multiple']['#description'] = t('Check this option if the user should be allowed to choose multiple values.');
      $this->form['extra']['multiple']['#default_value'] = !empty($this->component['extra']['multiple']);
    }
    else {
      $this->form['extra']['multiple']['#type'] = 'hidden';
      $this->form['extra']['multiple']['#value'] = (int) !empty($this->field['extra']['multiple']);
    }
    array_unshift($this->form['#submit'], 'wf_crm_process_options_selection');
    return TRUE;
  }

  /**
   * When a CiviCRM field has no options but is rendered as a select
   * Customize the options_element form
   */
  private function alterOptionsElement() {
    $this->form['items']['options']['#key_type_toggle'] = t('Show CiviCRM values (keys)');
    $this->form['items']['options']['#key_type_toggled'] = TRUE;
  }

  /**
   * options_element alterations for when a currency field (e.g. contribution amount, event fee) is rendered as a select
   */
  private function moneyOptions() {
    form_load_include($this->form_state, 'inc', 'webform_civicrm', 'includes/wf_crm_admin_component');
    $this->form['#validate'][] = 'wf_crm_money_validate';
    if ($this->component['type'] === 'grid') {
      $this->form['options']['#title'] = t('Amounts (columns)');
      $this->form['options']['#description'] = t('Options to select across the top. Usually these are quantities. Keys must be numeric');
      $this->form['questions']['#title'] = t('Items (rows)');
      $this->form['questions']['#description'] = t('Items down the side of the grid. Usually the keys represent item price, which will be multiplied with the amount column.') .
        '<br />' . t('Use non-numeric keys to use the price from the amount column as-is with no multiplication.');
    }
    elseif ($this->component['type'] === 'select') {
      $this->form['items']['options']['option_settings']['options_source']['#access'] = FALSE;
      $this->form['items']['#description'] = t('Enter item labels and prices. If you allow the user to select multiple items their amounts will be added together.');
    }
    foreach (array('items', 'options', 'questions') as $field) {
      if (!empty($this->form[$field]['options']['#key_type_toggle'])) {
        $this->form[$field]['options']['#key_type_toggle'] = t('Show Prices');
        $this->form[$field]['options']['#key_type_toggled'] = TRUE;
      }
    }
    // CSS trick to add currency symbol to options element keys
    $symbol = wf_crm_aval(CRM_Core_Config::singleton(), 'defaultCurrencySymbol', '$');
    drupal_add_css('
      .option-key-cell:before {
        content:"' . $symbol . '";
      }
      .options-widget .form-text.option-key {
        width: 50%;
        margin-left: .3em;
      }',
      'inline');
  }

  /**
   * Custom Processing for CiviCRM webform component option lists
   */
  public function postProcess() {
    $vals = &$this->form_state['values'];
    $vals['value'] = '';
    foreach ($vals['civicrm_options_fieldset']['civicrm_defaults'] as $k) {
      if ($k) {
        $vals['value'] .= ($vals['value'] ? ',' : '') . str_replace('_web_civi_option_selected_', '', $k);
      }
    }
    if (isset($vals['civicrm_options_fieldset']['civicrm_options'])) {
      if (empty($vals['extra']['civicrm_live_options'])) {
        $items = array();
        foreach ($vals['civicrm_options_fieldset']['civicrm_options'] as $k) {
          if ($k) {
            $v = str_replace('_web_civi_option_selected_', '', $k);
            if (!($label = $vals['civicrm_options_fieldset']['civicrm_option_label_' . $v])) {
              $label = $this->form['civicrm_options_fieldset']['civicrm_option_name_' . $v]['#value'];
            }
            $items[$vals['civicrm_options_fieldset']['civicrm_option_weight_' . $v]] = $v . '|' . $label;
          }
        }
        ksort($items);
        $vals['extra']['items'] = implode("\n", $items);

        // A single static radio should be shown as a checkbox
        if (count($items) == 1 && empty($vals['extra']['aslist'])) {
          $vals['extra']['multiple'] = 1;
        }
      }
      else {
        $items = wf_crm_field_options($vals, 'live_options', $this->data);
        $items += wf_crm_str2array($vals['extra']['items']);
        $vals['extra']['items'] = wf_crm_array2str($items);
      }
    }
  }

  /**
   * Look-up the webform token for a field
   * @param $cid
   * @return string
   */
  private function componentToken($cid) {
    module_load_include('inc', 'webform', 'includes/webform.components');
    $component = $this->node->webform['components'][$cid];
    $parents = webform_component_parent_keys($this->node, $component);

    return '[submission:values:' . implode(':', $parents) . ':nolabel]';
  }

  /**
   * Add CiviCRM info and theming to webform components form.
   * @param array $form
   * @param array $rows
   * @param stdClass $node
   */
  static function preprocessComponentsForm(&$form, &$rows, $node) {
    // Prepare for CiviCRM processing
    $enabled = $data = array();
    if (!empty($node->webform_civicrm)) {
      civicrm_initialize();
      $enabled = wf_crm_enabled_fields($node);
      $data = $node->webform_civicrm['data'];
      $form['components']['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/css/webform_civicrm_admin.css';
    }
    foreach ($rows as &$row) {
      // Empty message when no components have been added
      if (!empty($row[0]['colspan'])) {
        if (empty($node->webform['components']) && wf_crm_admin_access($node)) {
          $form['components']['#attached']['js'][] = drupal_get_path('module', 'webform_civicrm') . '/js/webform_tab.js';
          $row[0]['data'] = t(
            'Add a webform component below, or click the <a class="webform-civicrm-tab-link" href="!url">CiviCRM tab</a> to add CRM fields.',
            (array('!url' => url("node/{$node->nid}/civicrm")))
          );
        }
      }
      // Add-component row - re-render select element without civicrm_contact option
      elseif (in_array('webform-add-form', $row['class'])) {
        unset($form['add']['type']['#options']['civicrm_contact']);
        $form['add']['type']['#printed'] = FALSE;
        $row['data'][1] = drupal_render($form['add']['type']);
      }
      // Component row
      elseif (!empty($row['data-cid']) && isset($node->webform['components'][$row['data-cid']])) {
        $cid = $row['data-cid'];
        $component = $node->webform['components'][$cid];
        $type = &$row['data'][2]['data'];
        if ($component['type'] == 'civicrm_contact') {
          $types = array(
            'autocomplete' => t('Contact - Autocomplete'),
            'select' => t('Contact - Select List'),
            'hidden' => $component['extra']['show_hidden_contact'] ? t('Contact - Static') : t('Contact - Hidden'),
          );
          $type = $types[$component['extra']['widget']];
        }
        elseif ($component['type'] == 'select') {
          if ($component['extra']['aslist']) {
            $type = $component['extra']['multiple'] ? t('Multi-select') : t('Select');
          }
          else {
            $type = $component['extra']['multiple'] ? t('Checkboxes') : t('Radio buttons');
          }
        }
        if (in_array($cid, $enabled)) {
          $fields = wf_crm_get_fields();
          $sets = wf_crm_get_fields('sets');
          $class = 'civi-icon';
          list( , $c, $ent, $n, $table, $name) = explode('_', $component['form_key'], 6);
          $field = wf_crm_aval($fields, $table . '_' . $name, array('type' => 'fieldset'));
          // Don't allow CiviCRM fields to be cloned
          $row['data'][7]['data'] = '';
          if ($component['type'] == 'fieldset') {
            $title = t('Contact !num', array('!num' => $c));
            $type = t('Fieldset for !contact', array('!contact' => wf_crm_contact_label($c, $data)));
            $row['data'][7]['data'] = l(t('Clone Contact'), "node/{$node->nid}/webform/components/$cid/clone", array(
              'attributes' => array('title' => t('Add a new contact to the form with the same fields and settings')
              )));
            $class .= ' fieldset';
          }
          elseif ($ent == 'contact') {
            $field_type = ($table == 'contact' || $table == 'other') ? $field['name'] : $sets[$table]['label'];
            $title = t('!type Field for !contact', array('!contact' => wf_crm_contact_label($c, $data), '!type' => $field_type));
          }
          else {
            $title = t('Field for !type', array('!type' => $sets[$table]['label']));
          }
          if ($table === 'address' && $component['type'] === 'textfield') {
            if ($name === 'state_province_id' || $name === 'county_id') {
              $type = t('AJAX Select');
            }
          }
          if ($component['type'] == 'civicrm_contact' || $component['type'] == 'fieldset') {
            $class .= ' ' . $node->webform_civicrm['data']['contact'][$c]['contact'][1]['contact_type'];
          }
          if ($component['type'] == 'select') {
            $type .= ' (' . (empty($component['extra']['civicrm_live_options']) ? t('static') : t('live')) . ')';
          }
          // Show defaults with labels instead of keys
          if ($component['type'] == 'civicrm_contact') {
            if ($component['extra']['default'] == 'contact_id') {
              $row['data'][3]['data'] = check_plain(wf_crm_display_name($component['extra']['default_contact_id']));
            }
            elseif ($component['extra']['default'] == 'user') {
              $row['data'][3]['data'] = t('Current User');
            }
            elseif ($component['extra']['default'] == 'auto') {
              $row['data'][3]['data'] = t('Auto - From Filters');
            }
            elseif ($component['extra']['default'] == 'relationship' && $component['extra']['default_relationship']) {
              $row['data'][3]['data'] = t('Relationship to !contact', array('!contact' => wf_crm_contact_label(1, $data)));
            }
          }
          elseif (isset($component['value']) && strlen($component['value']) && ($field['type'] == 'select' || !empty($field['expose_list']))) {
            if ($component['type'] == 'select') {
              $items = wf_crm_str2array($component['extra']['items']);
            }
            else {
              $items = wf_crm_field_options($component, 'components_form', $node->webform_civicrm['data']);
            }
            $val = '';
            foreach (explode(',', $component['value']) as $v) {
              if (isset($items[trim($v)])) {
                $val .= ($val ? ', ' : '') . $items[trim($v)];
              }
            }
            $row['data'][3]['data'] = $val;
          }
          // Contribution page - link to civicrm config form instead of component edit form
          if ($name == 'contribution_page_id') {
            $type = t('CiviCRM Billing Fields');
            $class .= ' contribution';
            $row['data'][6]['data'] = l(t('Configure'), 'civicrm/admin/contribute/settings', array(
              'query' => array(
                'reset' => 1,
                'action' => 'update',
                'id' => $component['value'],
              ),
              'attributes' => array(
                'title' => t('Edit Contribution Page in CiviCRM'),
              )
            ));
          }
          $type = '<span class="' . $class . '"> </span>' . $type;
          $row['data'][2]['title'] = $title;
        }
      }
    }
  }

  /**
   * Ensure billing fields are not on same page as event/member amounts
   * @param stdClass $node
   */
  static function checkBillingPagination($node) {
    civicrm_initialize();
    $enabled = wf_crm_enabled_fields($node);
    $field_ids = array(
      'membership_membership_type_id',
      'membership_num_terms',
      'membership_fee_amount',
      'participant_fee_amount',
      'participant_event_id',
    );
    foreach ($node->webform['components'] as $cid => $component) {
      if (in_array($cid, $enabled)) {
        list( , , , , $key) = explode('_', $component['form_key'], 5);
        if (in_array($key, $field_ids)) {
          $payment_fields[$cid] = $component;
        }
        elseif ($key == 'contribution_contribution_page_id') {
          $contribution_page_page = $component['page_num'];
        }
      }
    }
    // Warning if payment fields are on same page as billing fields
    $message = '';
    if (isset($contribution_page_page) && !empty($payment_fields)) {
      foreach ($payment_fields as $component) {
        if ($component['page_num'] >= $contribution_page_page) {
          $message .= '<li>' . $component['name'] . '</li>';
        }
      }
    }
    if ($message) {
      $message = t('In order for line-items to be displayed correctly to the user, it is recommended to add a page break in-between Contribution Billing Fields and the following:') .
        '<ul>' . $message . '</ul>';
      drupal_set_message($message, 'warning', FALSE);
    }
  }
}

/**
 * Drupal theme callback
 * Format civicrm options form as a table
 */
function theme_webform_civicrm_options_table($variables) {
  $element = $variables['element'];
  $element['civicrm_defaults']['']['#attributes']['class'][] = 'select-all-civi-defaults';
  $default_box = drupal_render($element['civicrm_defaults']['']);
  $select_box = '<input class="select-all-civi-options" type="checkbox" checked="checked" title="' . t('Select All') . '"> ';
  $table = array(
    'rows' => array(),
    'attributes' => array('id' => 'civicrm-options-table'),
    'sticky' => FALSE,
  );
  if (empty($element['civicrm_options'])) {
    $table['header'] = array(t('Item'), $default_box . t('Selected'));
  }
  else {
    $table['header'] = array(
      t('Item'),
      t('Weight'),
      array('data' => $select_box . t('Enabled'), 'class' => array('live-options-hide')),
      array('data' => t('Label'), 'class' => array('live-options-hide')),
      $default_box . t('Default'),
    );
    drupal_add_tabledrag('civicrm-options-table', 'order', 'self', 'civicrm-option-weight');
  }
  foreach (element_children($element['civicrm_defaults']) as $k) {
    if ($k) {
      $v = str_replace('_web_civi_option_selected_', '', $k);
      $row = array(drupal_render($element['civicrm_option_name_' . $v]));
      if (!empty($element['civicrm_options'])) {
        $element['civicrm_option_weight_' . $v]['#attributes']['class'] = array('civicrm-option-weight');
        $element['civicrm_options'][$k]['#attributes']['class'] = array('civicrm-enabled');
        $element['civicrm_option_label_' . $v]['#attributes']['class'] = array('civicrm-label');
        $row[] = drupal_render($element['civicrm_option_weight_' . $v]);
        $row[] = array('data' => drupal_render($element['civicrm_options'][$k]), 'class' => array('live-options-hide'));
        $row[] = array('data' => drupal_render($element['civicrm_option_label_' . $v]), 'class' => array('live-options-hide'));
      }
      $element['civicrm_defaults'][$k]['#attributes']['class'] = array('civicrm-default');
      $row[] = drupal_render($element['civicrm_defaults'][$k]);
      $table['rows'][] = array(
        'data' => $row,
        'class' => array('draggable'),
      );
    }
  }
  return drupal_render_children($element) . theme('table', $table);
}

/**
 * Drupal FAPI validate callback
 * Validate checksum lifespan
 */
function wf_crm_cs_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['value']) || $form_state['values']['value'] < 0) {
    form_error($form['value'], t('Please enter a valid number of days.'));
  }
}

/**
 * Drupal FAPI validate callback
 * Validate money options & default value
 */
function wf_crm_money_validate($form, &$form_state) {
  $vals = $form_state['values'];
  if (!empty($vals['value']) && !is_numeric($vals['value'])) {
    form_error($form['value'], t('This is a CiviCRM currency field. @field must be a number.', array('@field' => $form['value']['#title'])));
  }
  foreach (array('items', 'options') as $field) {
    if (!empty($vals['extra'][$field])) {
      foreach (wf_crm_str2array($vals['extra'][$field]) as $key => $val) {
        if (!is_numeric($key)) {
          form_error($form, t('This is a CiviCRM currency field. @field keys must be numeric.', array('@field' => $form[$field]['#title'])));
          break;
        }
      }
    }
  }
}

/**
 * Drupal FAPI submit callback
 * Alter a webform component type.
 */
function wf_crm_change_widget($form, &$form_state) {
  // Get rid of default message
  unset($_SESSION['messages']['status']);
  drupal_set_message(t('Click "Save component" to change this field to %type (or go back to cancel). Test your form to ensure that the new widget works with this CiviCRM field.', array('%type' => $form['widget']['type']['#options'][$form_state['values']['widget']['type']])));
  // Set redirect to current form with 'type' arg to trigger type change
  // @see webform_civicrm_node_load
  $form_state['redirect'] = array(
    $_GET['q'],
    array('query' => array(
      'type' => $form_state['values']['widget']['type'])
    )
  );
  // Prevent 'destination' arg from overriding redirect
  unset($_GET['destination']);
}

/**
 * Drupal FAPI submit callback for single-component form
 */
function wf_crm_process_options_selection($form, &$form_state) {
  $admin_form = new wf_crm_admin_component($form, $form_state);
  $admin_form->postProcess();
}

/**
 * Drupal FAPI validation callback for all-components form
 */
function wf_crm_components_form_validate($form, &$form_state) {
  $components = wf_crm_aval($form_state, 'values:components', array());
  foreach ($components as $cid => $component) {
    $component += $form['#node']->webform['components'][$cid];
    if ($component['form_key'] == 'civicrm_1_contribution_1_contribution_contribution_page_id') {
      // Iterate up through parents
      while ($component['pid']) {
        $component = $components[$component['pid']];
      }
      $weight = $component['weight'];
    }
  }
  if (isset($weight)) {
    foreach ($components as $cid => $component) {
      $component += $form['#node']->webform['components'][$cid];
      if ($component['type'] == 'pagebreak' && $component['weight'] > $weight) {
        form_error($form['components'], t('CiviCRM Contribution Billing Fields <em>must</em> be on the last page of the form.'));
      }
    }
  }
}
