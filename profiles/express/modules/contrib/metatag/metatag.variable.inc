<?php

/**
 * @file
 * Definition of variables for Variable API module.
 */

/**
 * Implements hook_variable_info().
 */
function metatag_variable_info($options) {
  $variables['metatag_pager_string'] = array(
    'title' => t('Custom pager string', array(), $options),
    'type' => 'string',
    'description' => t('When the current page includes a pager, e.g. the URL contains "?page=42", the [current-page:pager] token may be added to a meta tag to differentiate between two pages that would otherwise have the same meta tags. The value "PAGER" (must be in uppercase) will be replaced by the actual page count. Note: the pager will only output if the page number is 2 or above and the string "page=NUMBER" is in the URL.<br />For best use, it may be worthwhile to add the [current-page:pager] to the page title to the left of the site name, to the start of description tags, etc.', array(), $options),
    'default' => 'Page PAGER | ',
    'group' => 'metatag',
    'localize' => TRUE,
    'multidomain' => TRUE,
  );

  $info = metatag_get_info();
  $groups = $info['groups'];
  $tags = $info['tags'];
  foreach ($tags as $tag => $info) {
    if (isset($info['maxlength'])) {
      $default = $info['maxlength'];
      $var_name = 'metatag_maxlength_' . $tag;
      $title = $info['label'];
      if (!empty($info['group']) && isset($groups[$info['group']])) {
        $title = $groups[$info['group']]['label'] . ': ' . $title;
      }
      $variables[$var_name] = array(
        'title' => t('Maximum length for @tag', array('@tag' => $title), $options),
        'type' => 'string',
        'description' => t('Controls the maximum length of the @tag meta tag, values longer than this will be truncated. If set to "0" the maximum length will be ignored.', array('@tag' => $title), $options),
        'default' => intval(variable_get($var_name, $default)),
        'group' => 'metatag',
        'localize' => TRUE,
        'multidomain' => TRUE,
      );
    }
  }

  return $variables;
}

/**
 * Implements hook_variable_group_info().
 */
function metatag_variable_group_info() {
  $groups['metatag'] = array(
    'title' => t('Metatag'),
    'description' => t('Configure meta tags on your website.'),
    'access' => 'administer meta tags',
    'path' => array('admin/config/search/metatags/settings'),
  );

  return $groups;
}
