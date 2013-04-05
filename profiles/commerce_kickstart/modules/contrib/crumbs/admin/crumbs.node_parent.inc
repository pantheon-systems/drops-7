<?php


/**
 * Form builder callback
 */
function crumbs_admin_node_parent_form() {
  $text = <<<EOT
<p>You may specify a pattern for the parent path for each node type.</p>
EOT;
  $doc = t($text);

  $text = <<<EOT
<p>Whatever you configure here, will be available on the Crumbs settings page as<br/>
crumbs.nodeParent.[type]</p>
EOT;
  $doc .= t($text);

  if (!module_exists('token')) {
    $text = <<<EOT
<p>You will get more dynamic possibilities, if you install the !token_module_link module.</p>
EOT;
    $doc .= t($text, array(
      '!token_module_link' => l('Token', 'http://drupal.org/project/token', array('external' => TRUE)),
    ));
  }
  elseif (!module_exists('entity_token')) {
    $text = <<<EOT
<p>You will get more interesting tokens, if you install the entity_token module.<br/>
(included in !entity_module_link module)</p>
EOT;
    $doc .= t($text, array(
      '!entity_module_link' => l('Entity', 'http://drupal.org/project/entity', array('external' => TRUE)),
    ));
  }

  $form = array();
  $form['intro'] = array(
    '#markup' => $doc,
  );

  $form['types'] = array(
    '#title' => t('Node types'),
    '#tree' => TRUE,
  );

  $settings = variable_get('crumbs_node_parent_patterns', array());

  foreach (node_type_get_types() as $type_name => $type) {
    $element = array(
      '#type' => 'textfield',
      '#title' => t($type->name),
      '#description' => t('Crumbs parent path for nodes of type "!type_name" (!type)', array(
        '!type_name' => $type->name,
        '!type' => "$type_name",
      )),
      // The maximum length is quite arbitrary.
      // Pathauto has 1280, but we think that 512 is enough.
      '#maxlength' => 512,
      '#default_value' => @$settings[$type_name],
    );
    if (module_exists('token')) {
      $element['#element_validate'][] = 'token_element_validate';
      $element['#token_types'][] = 'node';
    }
    $form['types'][$type_name] = $element;
  }

  if (module_exists('token')) {
    $form['token_help'] = array(
      '#title' => t('Replacement patterns'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['token_help']['help'] = array(
      '#theme' => 'token_tree',
      '#token_types' => array('node'),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}


function crumbs_admin_node_parent_form_submit(&$form, &$form_state) {
  $settings = $form_state['values']['types'];
  foreach ($settings as $type_name => $setting) {
    if (empty($setting)) {
      unset($settings[$type_name]);
    }
  }
  variable_set('crumbs_node_parent_patterns', $settings);
}
