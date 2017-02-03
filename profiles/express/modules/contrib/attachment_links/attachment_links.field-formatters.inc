<?php

/**
 * @file
 * Field formatters for attachment_links module
 */

/**
 * Implements hook_field_formatter_info().
 */
function attachment_links_field_formatter_info() {
  $hooks['attachment_links_preferred'] = array(
    'label' => t("Preferred link: Filename [TYPE SIZE]"),
    'field types' => array('node_reference'),
    'multiple values' => FIELD_BEHAVIOR_DEFAULT,
  );
  $hooks['attachment_links_newest'] = array(
    'label' => t("Newest link: Filename [TYPE SIZE]"),
    'field types' => array('node_reference'),
    'multiple values' => FIELD_BEHAVIOR_DEFAULT,
  );

  return $hooks;
}

/**
 * Implements hook_field_formatter_view().
 */
function attachment_links_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $elements = array();
  foreach ($items as $delta => $item) {
    $elements[$delta] = array(
      '#markup' => theme('attachment_links_formatter_'. $display['type'], array('element' => $item, 'field' => $instance)),
    );
  }
  return $elements;
}

/**
 * Theme function to generate 'AL: Preferred' field formatter.
 */
function theme_attachment_links_formatter_attachment_links_preferred($element) {
  $output = '';
  if (!empty($element)) {
    $files = array();
    foreach($element as $key => $value) {
      if (!empty($value['node']->nid) && is_numeric($value['node']->nid)) {
        $node = node_load($value['node']->nid);
        $files[] = attachment_links_render_formatter($node,'preferred');
      }
    }
    $output = implode(', ', $files);
  }
  return $output;
}

/**
 * Theme function to generate 'AL: Newest' field formatter.
 */
function theme_attachment_links_formatter_attachment_links_newest($element) {
  $output = '';
  if (!empty($element)) {
    $files = array();
    foreach($element as $key => $value) {
      if (!empty($value['node']->nid) && is_numeric($value['node']->nid)) {
        $node = node_load($value['node']->nid);
        $files[] = attachment_links_render_formatter($node,'newest');
      }
    }
    $output = implode(', ', $files);
  }
  return $output;
}

/**
 * Helper to the theme function: Renders AL field formatters.
 */
function attachment_links_render_formatter($node, $type='preferred'){
  drupal_add_css(drupal_get_path('module','attachment_links') .'/attachment-links.base.css');
  $types = array(
    'preferred' => '',
    'newest'    => '/newest',
  );

  $file_field_name = variable_get('attachment_links_selection_' . $node->type, 0);
  if ($file_field_name) {
    $file_field = field_info_field($file_field_name);
    $files = field_get_items('node', $node, $file_field['field_name']);

    // If this is a 'newest' file, re-order array to sort by newest upload
    if ($files && $type == 'newest') {
      $files = _attachment_links_array_sort($files, 'timestamp', SORT_DESC);
    }

    // Format the data
    $file = reset($files);
    $filesize = $file['filesize'];
    $fileparts = explode('.',$file['filename']);
    $fileext = strtoupper(array_pop($fileparts));
    $filename = implode('.',$fileparts);
    $fileclass = str_replace('/', '-', $file['filemime']);

    // Final output
    $formatted  = '<span class="al-file '.$fileclass.'">';
    $formatted .= l($filename, "node/$node->nid/attachment". $types[$type]) .' ['. $fileext .', '. ceil($filesize/1000) .'kB]';
    $formatted .= '</span>';
    
    return $formatted;
  }

  return 'attachment_links can\'t format this field with the current settings :(';
}

/**
 * Sorts an array by an arbitrary key
 *
 * @url
 *  http://www.php.net/manual/en/function.sort.php#99419
 */
function _attachment_links_array_sort($array, $on, $order=SORT_ASC) {
  $new_array = array();
  $sortable_array = array();
  
  if (count($array) > 0) {
    foreach ($array as $k => $v) {
      if (is_array($v)) {
        foreach ($v as $k2 => $v2) {
          if ($k2 == $on) {
            $sortable_array[$k] = $v2;
          }
        }
      }
      else {
        $sortable_array[$k] = $v;
      }
    }
    
    switch ($order) {
      case SORT_ASC:
        asort($sortable_array);
        break;
      
      case SORT_DESC:
        arsort($sortable_array);
        break;
    }
    
    foreach ($sortable_array as $k => $v) {
      $new_array[$k] = $array[$k];
    }
  }
  
  return $new_array;
}
