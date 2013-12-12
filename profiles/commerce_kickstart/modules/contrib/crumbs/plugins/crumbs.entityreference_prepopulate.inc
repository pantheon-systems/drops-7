<?php


/**
 * Implements hook_crumbs_plugins().
 */
function entityreference_prepopulate_crumbs_plugins($api) {
  $api->multiPlugin('node');
}

class entityreference_prepopulate_CrumbsMultiPlugin_node implements crumbs_MultiPlugin {

  function describe($api) {
    foreach (field_info_fields() as $field_name => $field_info) {
      if (1
        && $field_info['type'] === 'entityreference'
        && !empty($field_info['bundles']['node'])
      ) {
        foreach ($field_info['bundles']['node'] as $type) {
          $instance = field_info_instance('node', $field_name, $type);
          if (1
            && !empty($instance['default_value_function'])
            && $instance['default_value_function'] === 'entityreference_prepopulate_field_default_value'
          ) {
            $api->addRule("$field_name.$type");
          }
        }
      }
    }
  }

  function findParent($path, $item) {
    if (1
      // Start with a cheap-to-compute condition,
      // so the regex can be skipped in the average case.
      && substr($path, 0, 9) === 'node/add/'
      && preg_match('#^node/add/([^/]+)$#', $path, $m)
      && $path === $_GET['q']
    ) {
      $type = str_replace('-', '_', $m[1]);
      $info = field_info_fields();
      $candidates = array();
      foreach ($_GET as $k => $v) {
        if ($k === 'q') {
          continue;
        }
        if (1
          && 'q' !== $k
          && is_numeric($v) && !empty($v) && $v > 0
          && isset($info[$k]['settings']['target_type'])
        ) {
          $target_type = $info[$k]['settings']['target_type'];
          $instance = field_info_instance('node', $k, $type);
          if (1
            && !empty($instance['default_value_function'])
            && $instance['default_value_function'] === 'entityreference_prepopulate_field_default_value'
          ) {
            $target_entities = entity_load($target_type, array($v));
            if (!empty($target_entities[$v])) {
              $uri = entity_uri($target_type, $target_entities[$v]);
              if (!empty($uri['path'])) {
                $candidates["$k.$type"] = $uri['path'];
              }
            }
          }
        }
      }
      return $candidates;
    }
  }
}
