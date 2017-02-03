<?php

/**
 * @file
 * Define a new ctools content type.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Visualization embed'),
  'description' => t('Creates an new embed using either a local or remote visualization'),
  'category' => t('Visualizations'),
  'edit form' => 'visualization_entity_embed_edit_form',
  'render callback' => 'visualization_entity_embed_render',
  'admin info' => 'visualization_entity_embed_admin_info',
  'defaults' => array(
    'local_source' => '',
    'remote_source' => '',
    'height' => '480',
    'width' => '100%',
  ),
);
