<?php

function token_tweaks_settings_form($form, &$form_state) {
  $form['token_tree_recursion_limit'] = array(
    '#type' => 'select',
    '#title' => t('Maximum depth limit for the token UI'),
    '#description' => t('Lowering this value with limit the depth of the tokens built for the token tree. Increasing this value will allow more tokens to be visible but may affect performance. The default value is <em>4</em>.'),
    '#default_value' => variable_get('token_tree_recursion_limit', 3),
    '#options' => drupal_map_assoc(range(1, 9)),
  );

  /*$form['disable_tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Disable tokens from being visible in the UI'),
    '#collapsed' => TRUE,
  );

  variable_set('token_tweaks_alter_tokens', FALSE);
  $info = token_get_info();
  variable_del('token_tweaks_alter_tokens');

  $form['']

  foreach ($info['types'] as $type => $type_info) {
    $form['disable_tokens'][$type] = array(
      '#type' => 'container',
      '#title' => t('@type tokens', array('@type' => $type_info['name'])),
      '#collapsed' => TRUE,
    );
  }*/

  return system_settings_form($form);
}
