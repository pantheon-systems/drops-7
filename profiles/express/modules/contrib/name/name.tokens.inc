<?php

/**
 * @file
 * Builds placeholder replacement tokens for country-related data.
 */

/**
 * Implements hook_token_info().
 */
function name_token_info() {

  $types['name'] = array(
    'name' => t('Name'),
    'description' => t('Name tokens.'),
    'needs-data' => 'name',
  );

  $name['default'] = array(
    'name' => t('Name'),
    'description' => t('Full default formatted name.'),
  );
  foreach (name_get_custom_format_options() as $machine_name => $formatter_title) {
    $name['formatter-' . $machine_name] = array(
      'name' => t('Name: %formatter formatted', array('%formatter' => $formatter_title)),
      'description' => t('Name using the %formatter formatter.', array('%formatter' => $formatter_title)),
    );
  }

  foreach (_name_translations() as $component => $component_label) {
    $name['component-' . $component] = array(
      'name' => t('Name component: %component', array('%component' => $component_label)),
      'description' => t('The %component component of the name.', array('%component' => $component_label)),
    );
  }
  $additions['realname'] = array(
    'name' => t('Real name'),
    'description' => t('The real name of the user account, from the RealName or Name modules.'),
  );
  return array(
    'types' => $types,
    'tokens' => array('name' => $name, 'user' => $additions),
  );
}

/**
 * Implements hook_tokens().
 */
function name_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $sanitize = !empty($options['sanitize']);
  $replacements = array();

  if ($type == 'user' && !empty($data['user'])) {
    $account = $data['user'];
    foreach ($tokens as $name => $original) {
      switch ($name) {
        case 'realname':
          $replacements[$original] = '';
          if (!empty($account->realname)) {
            $replacements[$original] = $sanitize ? check_plain($account->realname) : $account->realname;
          }
          break;
      }
    }
  }

  if ($type == 'name' && !empty($data['name'])) {
    $name = $data['name'];
    $name_components = array();
    foreach (_name_translations() as $key => $title) {
      if (!empty($item[$key])) {
        $name_components[$key] = $item[$key];
      }
      else {
        $name_components[$key] = '';
      }
    }

    foreach ($tokens as $key => $original) {
      if ($key == 'default') {
        // Full default formatted name.
        $default = name_format($name_components, name_settings('default_format'));
        $replacements[$original] = $sanitize ? check_plain($default) : $default;
      }
      elseif (strpos($key, 'formatter-') === 0) {
        list(,$machine_name) = explode('-', $key, 2);
        $format = name_get_format_by_machine_name($machine_name);
        $formated_name = name_format($name_components, $format);
        $replacements[$original] = $sanitize ? check_plain($formated_name) : $formated_name;
      }
      elseif (strpos($key, 'component-') === 0) {
        list(,$component) = explode('-', $key, 2);
        $replacements[$original] = $sanitize ? check_plain($name_components[$component]) : $name_components[$component];
      }
    }
  }
  return $replacements;
}
