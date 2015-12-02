<?php
/**
 * @file
 * cu_block_selector.features.field_base.inc
 */

/**
 * Implements hook_field_default_field_bases().
 */
function cu_block_selector_field_default_field_bases() {
  $field_bases = array();

  // Exported field_base: 'field_footer'
  $field_bases['field_footer'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_footer',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Exported field_base: 'field_header'
  $field_bases['field_header'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_header',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Exported field_base: 'field_inner_content_left'
  $field_bases['field_inner_content_left'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_inner_content_left',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Exported field_base: 'field_inner_content_right'
  $field_bases['field_inner_content_right'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_inner_content_right',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Exported field_base: 'field_override_sidebar_first'
  $field_bases['field_override_sidebar_first'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_override_sidebar_first',
    'foreign keys' => array(),
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        0 => 'Inherit Sidebar Left from Parent',
        1 => 'Customize Sidebar Left',
      ),
      'allowed_values_function' => '',
      'allowed_values_php' => '',
    ),
    'translatable' => 0,
    'type' => 'list_boolean',
  );

  // Exported field_base: 'field_override_sidebar_second'
  $field_bases['field_override_sidebar_second'] = array(
    'active' => 1,
    'cardinality' => 1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_override_sidebar_second',
    'foreign keys' => array(),
    'indexes' => array(
      'value' => array(
        0 => 'value',
      ),
    ),
    'locked' => 0,
    'module' => 'list',
    'settings' => array(
      'allowed_values' => array(
        0 => 'Inherit Sidebar Right from Parent',
        1 => 'Customize Sidebar Right',
      ),
      'allowed_values_function' => '',
      'allowed_values_php' => '',
    ),
    'translatable' => 0,
    'type' => 'list_boolean',
  );

  // Exported field_base: 'field_sidebar_first'
  $field_bases['field_sidebar_first'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sidebar_first',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );

  // Exported field_base: 'field_sidebar_second'
  $field_bases['field_sidebar_second'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_sidebar_second',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );


  // Exported field_base: 'field_intro'
  $field_bases['field_intro'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_intro',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );
  
  // Exported field_base: 'field_slider'
  $field_bases['field_slider'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_slider',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );


  // Exported field_base: 'field_wide_2'
  $field_bases['field_wide_2'] = array(
    'active' => 1,
    'cardinality' => -1,
    'deleted' => 0,
    'entity_types' => array(),
    'field_name' => 'field_wide_2',
    'foreign keys' => array(
      'bean' => array(
        'columns' => array(
          'target_id' => 'bid',
        ),
        'table' => 'bean',
      ),
    ),
    'indexes' => array(
      'target_id' => array(
        0 => 'target_id',
      ),
    ),
    'locked' => 0,
    'module' => 'entityreference',
    'settings' => array(
      'handler' => 'views',
      'handler_settings' => array(
        'behaviors' => array(
          'views-select-list' => array(
            'status' => 0,
          ),
        ),
        'view' => array(
          'args' => array(),
          'display_name' => 'entityreference_1',
          'view_name' => 'cu_block_selector_beans',
        ),
      ),
      'target_type' => 'bean',
    ),
    'translatable' => 0,
    'type' => 'entityreference',
  );



  return $field_bases;
}
