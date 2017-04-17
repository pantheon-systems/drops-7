<?php

/**
 * @file
 * CiviCRM contact webform component.
 */

module_load_include('inc', 'webform_civicrm', 'includes/utils');
module_load_include('inc', 'webform_civicrm', 'includes/wf_crm_admin_help');

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_civicrm_contact() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'required' => 0,
    'extra' => array(
      'search_prompt' => '',
      'none_prompt' => '',
      'results_display' => array('display_name'),
      'allow_create' => 0,
      'widget' => 'autocomplete',
      'show_hidden_contact' => 0,
      'unique' => 0,
      'title_display' => 'before',
      'randomize' => 0,
      'description' => '',
      'no_autofill' => array(),
      'hide_fields' => array(),
      'hide_method' => 'hide',
      'no_hide_blank' => FALSE,
      'attributes' => array(),
      'private' => FALSE,
      'default' => '',
      'default_contact_id' => '',
      'default_relationship' => '',
      'allow_url_autofill' => TRUE,
      'dupes_allowed' => FALSE,
      'filters' => array(
        'contact_sub_type' => 0,
        'group' => array(),
        'tag' => array(),
        'check_permissions' => 1,
      ),
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_civicrm_contact($component) {
  civicrm_initialize();
  $form = array();
  $node = node_load($component['nid']);
  if (empty($node->webform_civicrm) && node_is_page($node)) {
    drupal_set_message(t('CiviCRM processing is not enabled for this webform.'), 'error');
    return $form;
  }
  list($contact_types, $sub_types) = wf_crm_get_contact_types();
  $data = $node->webform_civicrm['data'];
  $enabled = wf_crm_enabled_fields($node);
  list(, $c, ) = explode('_', $component['form_key'], 3);
  $contact_type = $data['contact'][$c]['contact'][1]['contact_type'];
  wf_crm_update_existing_component($component, $enabled, $data);
  $allow_create = $component['extra']['allow_create'];

  // Load scripts & css
  $form['#attached']['js'][] = wf_crm_tokeninput_path();
  $form['#attached']['js'][] = drupal_get_path('module', 'webform_civicrm') . '/js/webform_civicrm_admin.js';
  CRM_Core_Resources::singleton()->addCoreResources();
  $form['#suffix'] = wf_crm_admin_help::helpTemplate();
  $callback_path = '"' . url('webform-civicrm/js/' . $node->nid . '-' . $component['cid'], array('alias' => TRUE, 'query' => array('admin' => 1))) . '"';
  $settings = '{
    queryParam: "str",
    hintText: "' . t('Choose @type', array('@type' => $contact_types[$contact_type])) . '",
    noResultsText: "' . t('Not found') . '",
    searchingText: "' . t('Searching...') . '",
    tokenLimit: 1,
    prePopulate: prep
  }';
  $js = '
  jQuery(function($) {
    var prep = wfCiviContact.init(' . $callback_path . ');
    $("#default-contact-id").tokenInput(' . $callback_path . ', ' . $settings . ');
  });';
  $form['#attached']['js'][$js] = array('type' => 'inline');
  $form['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/css/token-input.css';
  $form['#attached']['js'][] = drupal_get_path('module', 'webform_civicrm') . '/js/contact_component.js';

  $form['display']['widget'] = array(
    '#type' => 'select',
    '#title' => t('Form Widget'),
    '#default_value' => $component['extra']['widget'],
    '#options' => array('autocomplete' => t('Autocomplete'), 'select' => t('Select List'), 'hidden' => t('Static'), 'textfield' => t('Enter Contact ID')),
    '#weight' => -9,
    '#parents' => array('extra', 'widget'),
  );
  $status = $allow_create ? t('<strong>Contact Creation: Enabled</strong> - this contact has name/email fields on the webform.') : t('<strong>Contact Creation: Disabled</strong> - no name/email fields for this contact on the webform.');
  $form['display']['#description'] = '<div class="messages ' . ($allow_create ? 'status' : 'warning') . '">' . $status . ' ' . wf_crm_admin_help::helpIcon('contact_creation', t('Contact Creation')) . '</div>';
  $form['display']['search_prompt'] = array(
    '#type' => 'textfield',
    '#title' => t('Search Prompt'),
    '#default_value' => $component['extra']['search_prompt'],
    '#description' => t('Text the user will see before selecting a contact.'),
    '#size' => 60,
    '#maxlength' => 1024,
    '#weight' => -7,
    '#parents' => array('extra', 'search_prompt'),
  );
  $form['display']['none_prompt'] = array(
    '#type' => 'textfield',
    '#title' => $allow_create ? t('Create Prompt') : t('Not Found Prompt'),
    '#default_value' => $component['extra']['none_prompt'],
    '#description' => $allow_create ? t('This text should prompt the user to create a new contact.') : t('This text should tell the user that no search results were found.'),
    '#size' => 60,
    '#maxlength' => 1024,
    '#weight' => -6,
    '#parents' => array('extra', 'none_prompt'),
  );
  $form['display']['results_display'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t("Contact Display Field(s)"),
    '#required' => TRUE,
    '#default_value' => $component['extra']['results_display'],
    '#options' => wf_crm_results_display_options($contact_type),
    '#parents' => array('extra', 'results_display'),
  );
  $form['display']['show_hidden_contact'] = array(
    '#type' => 'radios',
    '#title' => t('Display Contact Name'),
    '#description' => t('If enabled, this static element will show the contact that has been pre-selected (or else the Create/Not Found Prompt if set). Otherwise the element will not be visible.'),
    '#options' => array(t('No'), t('Yes')),
    '#default_value' => $component['extra']['show_hidden_contact'],
    '#parents' => array('extra', 'show_hidden_contact'),
    '#weight' => -5,
  );
  $form['field_handling'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t('Contact Field Handling'),
    '#description' => t('By default, all contact data will be autopopulated on the form for this existing contact. You can modify that behavior here.'),
  );
  $form['field_handling']['no_autofill'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Skip Autofilling of'),
    '#description' => t('Which fields should <em>not</em> be autofilled for this contact?'),
    '#default_value' => $component['extra']['no_autofill'],
    '#options' => array('' => '- ' . t('None') . ' -') + wf_crm_contact_fields($node, $c),
    '#parents' => array('extra', 'no_autofill'),
  );
  $form['field_handling']['hide_fields'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Fields to Lock'),
    '#description' => t('Prevent editing by disabling or hiding fields when a contact already exists.'),
    '#default_value' => $component['extra']['hide_fields'],
    '#options' => array('' => '- ' . t('None') . ' -') + wf_crm_contact_fields($node, $c),
    '#parents' => array('extra', 'hide_fields'),
  );
  $form['field_handling']['hide_method'] = array(
    '#type' => 'select',
    '#title' => t('Locked fields should be'),
    '#default_value' => $component['extra']['hide_method'],
    '#options' => array('hide' => t('Hidden'), 'disable' => t('Disabled')),
    '#parents' => array('extra', 'hide_method'),
  );
  $form['field_handling']['no_hide_blank'] = array(
    '#type' => 'checkbox',
    '#title' => t("Don't lock fields that are empty"),
    '#default_value' => $component['extra']['no_hide_blank'],
    '#parents' => array('extra', 'no_hide_blank'),
  );
  $form['validation']['unique'] = array(
    '#type' => 'checkbox',
    '#title' => t('Unique'),
    '#return_value' => 1,
    '#description' => t('Require this field to be unique for every submission. The same contact may not be entered twice.'),
    '#weight' => 1,
    '#default_value' => $component['extra']['unique'],
    '#parents' => array('extra', 'unique'),
  );
  $form['extra']['allow_create'] = array(
    '#type' => 'hidden',
    '#value' => $allow_create,
  );
  $form['defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default value'),
    '#description' => t('Should the form be pre-populated with an existing contact?<ul><li>Any filters set below will restrict this default.</li><li>If more than one contact meets the criteria, the first match will be picked. If multiple existing contact fields exist on the webform, each will select a different contact.</li></ul>'),
    '#collapsible' => TRUE,
  );
  $form['defaults']['default'] = array(
    '#type' => 'select',
    '#title' => t('Set default contact from'),
    '#options' => array('contact_id' => t('Specified Contact')),
    '#empty_option' => t('- None -'),
    '#default_value' => $component['extra']['default'],
    '#parents' => array('extra', 'default'),
  );
  if ($c == 1 && $contact_type == 'individual') {
    $form['defaults']['default']['#options']['user'] = t('Current User');
  }
  elseif ($c > 1) {
    $form['defaults']['default']['#options']['relationship'] = t('Relationship to !contact', array('!contact' => wf_crm_contact_label(1, $data)));
    $form['defaults']['default_relationship'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#title' => t('Specify Relationship(s)'),
      '#options' => array('' => '- ' . t('No relationship types defined for @a to @b', array('@a' => $contact_types[$contact_type], '@b' => $contact_types[$data['contact'][1]['contact'][1]['contact_type']])) . ' -'),
      '#default_value' => $component['extra']['default_relationship'],
      '#parents' => array('extra', 'default_relationship'),
    );
    $rtypes = wf_crm_get_contact_relationship_types($contact_type, $data['contact'][1]['contact'][1]['contact_type'], $data['contact'][$c]['contact'][1]['contact_sub_type'], $data['contact'][1]['contact'][1]['contact_sub_type']);
    foreach ($rtypes as $k => $v) {
      $form['defaults']['default_relationship']['#options'][$k] = $v . ' ' . wf_crm_contact_label(1, $data, 'plain');
      // We do this in the loop so the field is only required if it has some options
      $form['defaults']['default_relationship']['#required'] = TRUE;
      unset($form['defaults']['default_relationship']['#options']['']);
    }
  }
  $form['defaults']['default']['#options']['auto'] = t('Auto - From Filters');
  $form['defaults']['default_contact_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact'),
    '#id' => 'default-contact-id',
    '#parents' => array('extra', 'default_contact_id'),
  );
  $cid = $component['extra']['default_contact_id'];
  if ($cid) {
    if ($name = wf_crm_contact_access($component, array('check_permissions' => 1), $cid)) {
      $form['defaults']['default_contact_id']['#default_value'] = $cid;
      $form['defaults']['default_contact_id']['#attributes'] = array(
        'data-civicrm-name' => $name,
        'data-civicrm-id' => $cid,
      );
    }
  }
  $form['defaults']['allow_url_autofill'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use contact id from URL'),
    '#default_value' => $component['extra']['allow_url_autofill'],
    '#parents' => array('extra', 'allow_url_autofill'),
    '#description' => t('If the url contains e.g. %arg, it will be used to pre-populate this contact (takes precidence over other default values).', array('%arg' => "cid$c=123")),
  );
  if ($c > 1) {
    $form['defaults']['dupes_allowed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Allow Duplicate Autofill'),
      '#default_value' => $component['extra']['dupes_allowed'],
      '#parents' => array('extra', 'dupes_allowed'),
      '#description' => t('Check this box to allow a contact to be selected even if they already autofilled a prior field on the form. (For example, if contact 1 was autofilled with Bob Smith, should this field also be allowed to select Bob Smith or should it pick a different contact?)'),
    );
  }
  $form['defaults']['randomize'] = array(
    '#type' => 'checkbox',
    '#title' => t('Randomize'),
    '#default_value' => $component['extra']['randomize'],
    '#parents' => array('extra', 'randomize'),
    '#description' => t('Pick a contact at random if more than one meets criteria.'),
  );
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filters'),
    '#description' => t('Only contacts meeting filter criteria will be available as select options or default value.<br />Note: Filters only apply to how a contact is chosen on the form, they do not affect how a contact is saved.'),
    '#parents' => array('extra', 'filters'),
    '#tree' => TRUE,
    '#collapsible' => TRUE,
  );
  if (!empty($sub_types[$contact_type])) {
    $form['filters']['contact_sub_type'] = array(
      '#type' => 'select',
      '#title' => t('Type of @contact', array('@contact' => $contact_types[$contact_type])),
      '#options' => array(t('- Any -')) + $sub_types[$contact_type],
      '#default_value' => $component['extra']['filters']['contact_sub_type'],
    );
  }
  $form['filters']['group'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Groups'),
    '#options' => array('' => '- ' . t('None') . ' -') + wf_crm_apivalues('group_contact', 'getoptions', array('field' => 'group_id')),
    '#default_value' => $component['extra']['filters']['group'],
    '#description' => t('Listed contacts must be members of at least one of the selected groups (leave blank to not filter by group).'),
  );
  $tags = array();
  $form['filters']['tag'] = array(
    '#type' => 'select',
    '#multiple' => TRUE,
    '#title' => t('Tags'),
    '#options' => array('' => '- ' . t('None') . ' -') + CRM_Core_BAO_Tag::getTags("civicrm_contact", $tags, NULL, '- '),
    '#default_value' => $component['extra']['filters']['tag'],
    '#description' => t('Listed contacts must be have at least one of the selected tags (leave blank to not filter by tag).'),
  );
  $form['filters']['check_permissions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enforce Permissions'),
    '#default_value' => $component['extra']['filters']['check_permissions'],
    '#description' => t('Only show contacts the acting user has permission to see in CiviCRM.') . '<br />' . t('WARNING: Keeping this option enabled is highly recommended unless you are effectively controlling access by another method.'),
  );
  wf_crm_contact_component_form_help($form);
  return $form;
}

function wf_crm_contact_component_form_help(&$form) {
  foreach(element_children($form) as $element) {
    if (is_array($form[$element])) {
      wf_crm_contact_component_form_help($form[$element]);
      if (method_exists('wf_crm_admin_help', "contact_component_$element")) {
        wf_crm_admin_help::addHelp($form[$element], "contact_component_$element");
      }
    }
  }
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_civicrm_contact($component, $value = NULL, $filter = TRUE) {
  $node = isset($component['nid']) ? node_load($component['nid']) : NULL;
  civicrm_initialize();
  list(, $c, ) = explode('_', $component['form_key'], 3);
  $hide_fields = array_filter($component['extra']['hide_fields']);
  $hide_method = wf_crm_aval($component['extra'], 'hide_method', 'hide');
  $no_hide_blank = (int) wf_crm_aval($component['extra'], 'no_hide_blank', 0);
  $js = '';
  $element = array(
    '#type' => $component['extra']['widget'] == 'autocomplete' ? 'textfield' : $component['extra']['widget'],
    '#weight' => $component['weight'],
    '#attributes' => $component['extra']['attributes'],
  );
  $element['#attributes']['data-hide-method'] = $hide_method;
  $element['#attributes']['data-no-hide-blank'] = $no_hide_blank;
  if ($component['extra']['widget'] != 'hidden' || $component['extra']['show_hidden_contact']) {
    $element += array(
      '#title' => $filter ? webform_filter_xss($component['name']) : $component['name'],
      '#title_display' => $component['extra']['title_display'] ? $component['extra']['title_display'] : 'before',
      '#required' => $component['required'],
      '#description' => $filter ? webform_filter_descriptions($component['extra']['description'], $node) : $component['extra']['description'],
      '#translatable' => array('title', 'description'),
      '#theme_wrappers' => array('webform_element'),
    );
    // See if there are any fields to fetch via ajax
    $other_fields = wf_crm_contact_fields($node, $c);
    if ($hide_method == 'hide') {
      foreach ($hide_fields as $h) {
        unset($other_fields[$h]);
      }
    }
    $ajax = $other_fields ? 'true' : 'false';
  }
  $callback_path = '"' . url('webform-civicrm/js/' . $node->nid . '-' . $component['cid'], array('alias' => TRUE)) . '"';

  // Render component based on type
  switch ($component['extra']['widget']) {

    case 'autocomplete':
      if ($node && isset($node->webform_civicrm)) {
        $element['#attached']['js'][] = wf_crm_tokeninput_path();
        $element['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/css/token-input.css';
        $settings = '{
          queryParam: "str",
          hintText: ' . json_encode(filter_xss($component['extra']['search_prompt'])) . ',
          noResultsText: ' . json_encode(filter_xss($component['extra']['none_prompt'])) . ',
          searchingText: ' . json_encode(t('Searching...')) . ',
          tokenLimit: 1,
          onAdd: function(item) {wfCivi.existingSelect(' . $c . ', ' . $node->nid . ', ' . $callback_path . ', toHide, "' . $hide_method . '", ' . $no_hide_blank . ', item.id, ' . $ajax . ');},
          onDelete: function(item) {wfCivi.existingSelect(' . $c . ', ' . $node->nid . ', ' . $callback_path . ', toHide, "' . $hide_method . '", ' . $no_hide_blank . ', "", false);},
          prePopulate: prep
        }';
        $js = "var prep = wfCivi.existingInit(field, $c, {$node->nid}, $callback_path, toHide);
                field.tokenInput($callback_path, $settings);";
      }
      break;

    case 'select':
      $element['#options'] = array();
      if ($node && isset($node->webform_civicrm)) {
        $filters = wf_crm_search_filters($node, $component);
        $element['#options'] = wf_crm_contact_search($node, $component, $filters);
        $js = "field.change(function() {
                  wfCivi.existingSelect($c, {$node->nid}, $callback_path, toHide, '$hide_method', $no_hide_blank, \$(this).val(), $ajax);
                });
                wfCivi.existingInit(field, $c, {$node->nid}, $callback_path, toHide);";
      }
      // Display empty option unless there are no results
      if (!$component['extra']['allow_create'] || count($element['#options']) > 1) {
        $element['#empty_option'] = filter_xss($component['extra'][$element['#options'] ? 'search_prompt' : 'none_prompt']);
      }
      break;

    case 'hidden':
      if (!empty($value[0])) {
        $element['#value'] = $value[0];
      }
      if ($node && isset($node->webform_civicrm)) {
        if ($component['extra']['show_hidden_contact']) {
          $element['#attached']['css'][] = drupal_get_path('module', 'webform_civicrm') . '/css/token-input.css';
        }
        if ($hide_fields) {
          $js = "wfCivi.existingInit(field, $c, {$node->nid}, $callback_path, toHide);";
        }
        $element['#theme_wrappers'] = array('static_contact_element');
        if ($component['required']) {
          $element['#element_validate'] = array('wf_crm_contact_component_required');
        }
      }
      break;

    case 'textfield':
      $element['#description'] = t('Lookup by Contact ID.');
      $element['#size'] = 8;
      if ($node && isset($node->webform_civicrm)) {
        $js = "field.change(function() {
                  wfCivi.existingSelect($c, {$node->nid}, $callback_path, toHide, '$hide_method', $no_hide_blank, \$(this).val(), $ajax);
                });
                wfCivi.existingInit(field, $c, {$node->nid}, $callback_path, toHide);";
      }
      // Display empty option unless there are no results
      if (!$component['extra']['allow_create'] || count($element['#options']) > 1) {
        $element['#empty_option'] = $component['extra'][$element['#options'] ? 'search_prompt' : 'none_prompt'];
      }

  }
  // Add inline javascript
  if ($js) {
    $js = '
      (function ($, D) {
        D.behaviors.webform_civicrm_' . $node->nid . '_' . $component['cid'] . ' = {
          attach: function (context) {
            var toHide = ' . json_encode(array_values($hide_fields)) . ';
            var field = $(\'.webform-client-form-' . $component['nid'] . ' :input.civicrm-enabled[name$="' . $component['form_key'] . ']"]\', context);
            ' . $js .'
          }
        };
      })(jQuery, Drupal);';
    $element['#attached']['js'][$js] = array('type' => 'inline');
  }

  // Enforce uniqueness.
  if ($component['extra']['unique']) {
    $element['#element_validate'][] = 'webform_validate_unique';
  }
  if ($cid = wf_crm_aval($value, 0)) {
    $element['#default_value'] = $cid;
  }
  wf_crm_fill_contact_value($node, $component, $element);

  return $element;
}

/**
 * Lookup contact name from ID, verify permissions, and attach as html data.
 * Used when rendering or altering a CiviCRM contact field
 *
 * @param $node
 *   Node object
 * @param $component
 *   Webform component
 * @param $element
 *   FAPI form element (reference)
 */
function wf_crm_fill_contact_value($node, $component, &$element) {
  $cid = wf_crm_aval($element, '#default_value', '');
  if ($element['#type'] == 'hidden') {
    // User may not change this value for hidden fields
    $element['#value'] = $cid;
    if (!$component['extra']['show_hidden_contact']) {
      return;
    }
  }
  if ($cid) {
    // Don't lookup same contact again
    if (wf_crm_aval($element, '#attributes:data-civicrm-id') != $cid) {
      $filters = wf_crm_search_filters($node, $component);
      $name = wf_crm_contact_access($component, $filters, $cid);
      if ($name !== FALSE) {
        $element['#attributes']['data-civicrm-name'] = $name;
        $element['#attributes']['data-civicrm-id'] = $cid;
      }
      else {
        unset($cid);
      }
    }
  }
  if (empty($cid) && $element['#type'] == 'hidden' && $component['extra']['none_prompt']) {
    $element['#attributes']['data-civicrm-name'] = filter_xss($component['extra']['none_prompt']);
  }
}

/**
 * Implements _webform_display_component().
 */
function _webform_display_civicrm_contact($component, $value, $format = 'html') {
  $display = empty($value[0]) ? '' : wf_crm_display_name($value[0]);
  if ($format == 'html' && $display && user_access('access CiviCRM')) {
    $display = l($display, 'civicrm/contact/view', array(
      'alias' => TRUE,
      'query' => array(
        'reset' => 1,
        'cid' => $value[0],
      ),
    ));
  }
  return array(
    '#title' => $component['name'],
    '#weight' => $component['weight'],
    '#theme' => 'display_civicrm_contact',
    '#theme_wrappers' => $format == 'html' ? array('webform_element') : array('webform_element_text'),
    '#field_prefix' => '',
    '#field_suffix' => '',
    '#format' => $format,
    '#value' => $display,
    '#translatable' => array('title'),
  );
}

/**
 * Theme function.
 * Format the output of data for this component.
 */
function theme_display_civicrm_contact($variables) {
  $element = $variables['element'];
  $prefix = $element['#format'] == 'html' ? '' : $element['#field_prefix'];
  $suffix = $element['#format'] == 'html' ? '' : $element['#field_suffix'];
  return $element['#value'] !== '' ? ($prefix . $element['#value'] . $suffix) : ' ';
}

/**
 * Implements _webform_table_component().
 */
function _webform_table_civicrm_contact($component, $value) {
  return empty($value[0]) ? '' : check_plain(wf_crm_display_name($value[0]));
}

/**
 * Implements _webform_csv_headers_component().
 */
function _webform_csv_headers_civicrm_contact($component, $export_options) {
  $header = array();
  $header[0] = '';
  $header[1] = '';
  $header[2] = $component['name'];
  return $header;
}

/**
 * Implements _webform_csv_data_component().
 */
function _webform_csv_data_civicrm_contact($component, $export_options, $value) {
  return empty($value[0]) ? '' : wf_crm_display_name($value[0]);
}

/**
 * Returns a list of contacts based on component settings.
 *
 * @param $node
 *   Node object
 * @param $component
 *   Webform component
 * @param $params
 *   Contact get params (filters)
 * @param $str
 *   Search string (used during autocomplete)
 *
 * @return array
 */
function wf_crm_contact_search($node, $component, $params, $str = NULL) {
  if (empty($node->webform_civicrm)) {
    return array();
  }
  $limit = $str ? 12 : 500;
  $ret = array();
  $display = $component['extra']['results_display'];
  $params += array(
    'rowCount' => $limit,
    'sort' => 'sort_name',
  );
  if ($str) {
    $params['display_name'] = str_replace(' ', '%', CRM_Utils_Type::escape($str, 'String'));
  }
  $result = wf_civicrm_api('contact', 'get', $params);
  // Autocomplete results
  if ($str) {
    foreach (wf_crm_aval($result, 'values', array()) as $contact) {
      if ($name = wf_crm_format_contact($contact, $display)) {
        $ret[] = array('id' => $contact['id'], 'name' => $name);
      }
    }
    if (count($ret) < $limit && $component['extra']['allow_create']) {
      // HTML hack to get prompt to show up different than search results
      $ret[] = array('id' => "-$str", 'name' => '<em><i>' . filter_xss($component['extra']['none_prompt']) . '</i></em>');
    }
  }
  // Select results
  else {
    if ($component['extra']['allow_create']) {
      $ret['-'] = filter_xss($component['extra']['none_prompt']);
    }
    foreach (wf_crm_aval($result, 'values', array()) as $contact) {
      if ($name = wf_crm_format_contact($contact, $display)) {
        $ret[$contact['id']] = $name;
      }
    }
    // If we get exactly $limit results, there are probably more - warn that the list is truncated
    if (wf_crm_aval($result, 'count') >= $limit) {
      watchdog(
        'webform_civicrm',
        'Maximum contacts exceeded, list truncated on the webform "@title". The webform_civicrm "@field" field cannot display more than !limit contacts because it is a select list. Recommend switching to autocomplete widget in component settings.',
        array('!limit' => $limit, '@field' => $component['name'], '@title' => $node->title),
        WATCHDOG_WARNING,
        l(t('Edit component'), "node/{$node->nid}/webform/components/{$component['cid']}")
      );
      if (wf_crm_admin_access($node) && node_is_page($node)) {
        drupal_set_message(
          '<strong>' . t('Maximum contacts exceeded, list truncated.') .'</strong><br>' .
          t('The field "@field" cannot show more than !limit contacts because it is a select list. Recommend switching to autocomplete widget in <a !link>component settings</a>.',
            array('!limit' => $limit, '@field' => $component['name'], '!link' => 'href="' . url("node/{$node->nid}/webform/components/{$component['cid']}") . '"')),
          'warning'
        );
      }
    }
  }
  return $ret;
}

/**
 * Load contact name if user has permission. Else return FALSE.
 *
 * @param $component
 *   Webform component of type 'civicrm_contact'
 * @param $filters
 *   Contact get params
 * @param $cid
 *   Contact id
 *
 * @return bool|string
 */
function wf_crm_contact_access($component, $filters, $cid) {
  // Create new contact doesn't require lookup
  $cid = "$cid";
  list(, $c, ) = explode('_', $component['form_key'], 3);
  if ($cid && $cid[0] === '-' && !empty($component['extra']['allow_create']) && !empty($component['extra']['none_prompt'])) {
    return filter_xss($component['extra']['none_prompt']);
  }
  if (!is_numeric($cid)) {
    return FALSE;
  }
  $filters['id'] = $cid;
  $filters['is_deleted'] = 0;
  // A contact always has permission to view self
  if ($cid == wf_crm_user_cid()) {
    $filters['check_permissions'] = FALSE;
  }
  if (!empty($filters['check_permissions'])) {
    // If we have a valid checksum for this contact, bypass other permission checks
    // For legacy reasons we support "cid" param as an alias of "cid1"
    if (wf_crm_aval($_GET, "cid$c") == $cid || $c == 1 && wf_crm_aval($_GET, "cid") == $cid) {
      // For legacy reasons we support "cs" param as an alias of "cs1"
      if (!empty($_GET['cs']) && $c == 1 && CRM_Contact_BAO_Contact_Utils::validChecksum($cid, $_GET['cs'])) {
        $filters['check_permissions'] = FALSE;
      }
      elseif (!empty($_GET["cs$c"]) && CRM_Contact_BAO_Contact_Utils::validChecksum($cid, $_GET["cs$c"])) {
        $filters['check_permissions'] = FALSE;
      }
    }
  }
  // Fetch contact name with filters applied
  $result = wf_civicrm_api('contact', 'get', $filters);
  return wf_crm_format_contact(wf_crm_aval($result, "values:$cid"), $component['extra']['results_display']);
}

/**
 * Display a contact based on chosen fields
 *
 * @param array $contact
 * @param array $display_fields
 * @return bool|string
 */
function wf_crm_format_contact($contact, $display_fields) {
  if (!$contact) {
    return FALSE;
  }
  $display = array();
  foreach ($display_fields as $field) {
    if ($field && !empty($contact[$field])) {
      $display[] = check_plain($contact[$field]);
    }
  }
  return implode(' :: ', $display);
}

/**
 * Find exposed field groups for a contact
 *
 * @param $node
 *   Node object
 * @param $con
 *   Contact #
 *
 * @return array
 */
function wf_crm_contact_fields($node, $con) {
  $ret = array();
  $sets = wf_crm_get_fields('sets');
  $sets['name'] = array('label' => t('Name'));
  foreach ($node->webform['components'] as $f) {
    if ($pieces = wf_crm_explode_key($f['form_key'])) {
      list( , $c, $ent, , $table, $field) = $pieces;
      if ($ent == 'contact' && $c == $con && isset($sets[$table])) {
        // Separate name from other contact fields
        if ($table == 'contact' && strpos($field, 'name')) {
          $table = 'name';
        }
        if ($field != 'existing') {
          $ret[$table] = $sets[$table]['label'];
        }
      }
    }
  }
  return $ret;
}

/**
 * Update existing component if other fields have been added or removed
 *
 * @param $component
 *   Webform component of type 'civicrm_contact' (reference)
 * @param $enabled
 *   Array of enabled fields
 * @param $data
 *   Array of crm entity data
 */
function wf_crm_update_existing_component(&$component, $enabled, $data) {
  list(, $c, ) = explode('_', $component['form_key'], 3);
  if (!empty($data['contact'][$c])) {
    $contact_type = $data['contact'][$c]['contact'][1]['contact_type'];
    $allow_create = wf_crm_name_field_exists($enabled, $c, $contact_type);
    if ($allow_create != $component['extra']['allow_create']) {
      $component['extra']['none_prompt'] = $allow_create ? t('+ Create new +') : t('- None Found -');
      $component['extra']['allow_create'] = $allow_create;
      webform_component_update($component);
    }
  }
}

/**
 * Get a contact's relations of certain types
 *
 * @param cid
 *   Contact id
 * @param types
 *   Array of relationship_type_ids
 * @param $current
 *   Limit to current & enabled relations?
 *
 * @return array
 */
function wf_crm_find_relations($cid, $types, $current = TRUE) {
  $found = $allowed = array();
  $cid = (int) $cid;
  static $employer_type = 0;
  if ($cid) {
    if (!$employer_type && $current) {
      $employer_type = CRM_Core_DAO::singleValueQuery("SELECT id FROM civicrm_relationship_type WHERE name_a_b = 'Employee of'");
    }
    $type_ids = '';
    foreach ($types as $t) {
      list($type, $a_b) = explode('_', $t);
      // Put current employer first in the list
      if ($type == $employer_type && $current) {
        $sql = "SELECT id, employer_id
        FROM civicrm_contact
        WHERE id = $cid OR employer_id = $cid";
        $dao = CRM_Core_DAO::executeQuery($sql);
        while ($dao->fetch()) {
          $found[] = $dao->id == $cid ? $dao->employer_id : $dao->id;
        }
        $dao->free();
      }
      $type_ids .= ($type_ids ? ',' : '') . $type;
      if ($a_b == 'a' || $a_b == 'r') {
        $allowed[] = $type . '_a';
      }
      if ($a_b == 'b' || $a_b == 'r') {
        $allowed[] = $type . '_b';
      }
    }
    if ($type_ids) {
      $sql = "SELECT relationship_type_id, contact_id_a, contact_id_b
        FROM civicrm_relationship
        WHERE relationship_type_id IN ($type_ids) AND (contact_id_a = $cid OR contact_id_b = $cid)";
      if ($current) {
        $sql .= " AND is_active = 1 AND (end_date > CURDATE() OR end_date IS NULL)";
      }
      $dao = CRM_Core_DAO::executeQuery($sql);
      while ($dao->fetch()) {
        $a_b = $dao->contact_id_a == $cid ? 'b' : 'a';
        if (in_array($dao->relationship_type_id . '_' . $a_b, $allowed)) {
          $c = $dao->{"contact_id_$a_b"};
          $found[$c] = $c;
        }
      }
      $dao->free();
    }
  }
  return $found;
}

/**
 * Format filters for the contact get api
 *
 * @param $node
 *   Webform node object
 * @param $component
 *   Webform component of type 'civicrm_contact'
 *
 * @return array
 *   Api params
 */
function wf_crm_search_filters($node, $component) {
  $params = array('is_deleted' => 0);
  list(, $c, ) = explode('_', $component['form_key'], 3);
  $params['contact_type'] = ucfirst($node->webform_civicrm['data']['contact'][$c]['contact'][1]['contact_type']);
  foreach ($component['extra']['filters'] as $key => $val) {
    if (is_array($val)) {
      $val = array_filter($val);
    }
    if (!empty($val)) {
      if ($key === 'tag' || $key === 'group') {
        $val = array_fill_keys($val, 1);
      }
      $params[$key] = $val;
    }
  }
  return $params;
}

/**
 * Theme the wrapper for a static contact element
 * Includes normal webform theme wrappers plus a tokeninput-style name field
 */
function theme_static_contact_element($vars) {
  $element = $vars['element'];
  $component = $element['#webform_component'];

  // All elements using this for display only are given the "display" type.
  $type = wf_crm_aval($element, '#format') == 'html' ? 'display' : 'civicrm_contact';

  // Convert the parents array into a string, excluding the "submitted" wrapper.
  $nested_level = $element['#parents'][0] == 'submitted' ? 1 : 0;
  $parents = str_replace('_', '-', implode('--', array_slice($element['#parents'], $nested_level)));

  $wrapper_classes = array(
    'form-item',
    'webform-component',
    'webform-component-' . $type,
    'static',
    'webform-component--' . $parents,
  );

  if ($component['extra']['title_display'] === 'inline') {
    $wrapper_classes[] = 'webform-container-inline';
  }

  $output = '<div class="' . implode(' ', $wrapper_classes) . '">' . "\n";

  // Display static value in addition to hidden field
  if ($type == 'civicrm_contact' && !empty($element['#attributes']['data-civicrm-name'])) {

    if ($component['extra']['title_display'] != 'none') {
      $output .= theme('form_element_label', array('element' => $element));
    }

    $output .= '<ul class="token-input-list"><li class="token-input-token"><p>' . $element['#attributes']['data-civicrm-name'] . "</p></li></ul>\n";

    if (!empty($component['extra']['description'])) {
      $output .= ' <div class="description">' . filter_xss($element['#description']) . "</div>\n";
    }
  }

  return $output . $element['#children'] . "\n</div>\n";
}

/**
 * Returns a list of fields that can be shown in an "Existing Contact" field display
 * In the future we could use api.getfields for this, but that also returns a lot of stuff we don't want
 *
 * @return array
 */
function wf_crm_results_display_options($contact_type) {
  $options = array(
    'display_name' => t("Display Name"),
    'sort_name' => t("Sort Name"),
  );
  if ($contact_type == 'individual') {
    $options += array(
      'first_name' => t("First Name"),
      'middle_name' => t("Middle Name"),
      'last_name' => t("Last Name"),
      'current_employer' => t("Current Employer"),
      'job_title' => t("Job Title"),
    );
  }
  else {
    $options[$contact_type . '_name'] = $contact_type == 'organization' ? t("Organization Name") : t("Household Name");
  }
  $options += array(
    'nick_name' => t("Nick Name"),
    'id' => t("Contact ID"),
    'external_identifier' => t("External ID"),
    'source' => t("Source"),
    'email' => t("Email"),
    'city' => t("City"),
    'county' => t("District/County"),
    'state_province' => t("State/Province"),
    'country' => t("Country"),
    'postal_code' => t("Postal Code"),
    'phone' => t("Phone"),
  );
  return $options;
}

/**
 * Get the path to the jquery.tokeninput plugin
 * @return string
 */
function wf_crm_tokeninput_path() {
  static $ret;
  if (!$ret) {
    // First see if the library is installed in Drupal
    $path = libraries_get_path('tokeninput');
    if ($path && is_file(DRUPAL_ROOT . '/' . $path . '/src/jquery.tokeninput.js')) {
      $ret = $path . '/src/jquery.tokeninput.js';
    }
    // If not, fall back on using the copy shipped with CiviCRM
    else {
      $ret = drupal_get_path('module', 'civicrm') . '/../packages/jquery/plugins/jquery.tokeninput.js';
    }
  }
  return $ret;
}

/**
 * Validation callback
 *
 * @param array $element
 * @param array $form_state
 */
function wf_crm_contact_component_required($element, &$form_state) {
  if (empty($element['#value'])) {
    form_error($element, t('!name field is required.', array('!name' => $element['#title'])));
  }
}
