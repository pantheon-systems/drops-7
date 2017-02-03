<?php
/**
* @file
* Views integration for Manual Crop.
*/

/**
 * Implements hook_views_data().
*/
function manualcrop_views_data() {
  $data['manualcrop']['table']['group'] = t('Manual Crop');

  // File id.
  $data['manualcrop']['fid'] = array(
    'title' => t('File id'),
    'help' => t('The id of the file.'),
    'field' => array(
      'handler' => 'views_handler_field_file',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_file_fid',
      'numeric' => TRUE,
    ),
    'filter' => array(
       'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
       'handler' => 'views_handler_sort',
    ),
  );

  // Style name.
  $data['manualcrop']['style_name'] = array(
    'title' => t('Style name'),
    'help' => t('The image style of the cropped image.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Numeric field defaults.
  $numeric_default = array(
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // X coordinate.
  $data['manualcrop']['x'] = array(
    'title' => t('Crop area: X offset'),
    'help' => t('X coordinate of the crop area.'),
  ) + $numeric_default;

  // Y coordinate.
  $data['manualcrop']['y'] = array(
    'title' => t('Crop area: Y offset'),
    'help' => t('Y coordinate of the crop area.'),
  ) + $numeric_default;

  // Width.
  $data['manualcrop']['width'] = array(
    'title' => t('Crop area: width'),
    'help' => t('Width of the crop area.'),
  ) + $numeric_default;

  // Height.
  $data['manualcrop']['height'] = array(
    'title' => t('Crop area: height'),
    'help' => t('Height of the crop area.'),
  ) + $numeric_default;

  // Join the file_managed table.
  $data['manualcrop']['table']['join'] = array(
    'file_managed' => array(
      'field' => 'fid',
      'left_field' => 'fid',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function manualcrop_views_data_alter(&$data) {
  if (isset($data['file_managed'])) {
    $data['file_managed']['file_to_manualcrop'] = array(
      'title' => t('Manual crop'),
      'help' => t('Manualcrop data for this file.'),
      'relationship' => array(
         'title' => t('Manual Crop'),
         'label' => t('manualcrop'),
         'base' => 'manualcrop',
         'base field' => 'fid',
         'relationship field' => 'fid',
      ),
    );
  }
}
