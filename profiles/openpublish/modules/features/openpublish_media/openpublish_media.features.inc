<?php
/**
 * @file
 * openpublish_media.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_media_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function openpublish_media_image_default_styles() {
  $styles = array();

  // Exported image style: full_width.
  $styles['full_width'] = array(
    'name' => 'full_width',
    'effects' => array(
      1 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => '1200',
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => '1',
      ),
    ),
  );

  return $styles;
}

/**
 * Implements hook_node_info().
 */
function openpublish_media_node_info() {
  $items = array(
    'openpublish_photo_gallery' => array(
      'name' => t('Photo Gallery'),
      'base' => 'node_content',
      'description' => t('Create galleries from image media assets available on the site.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
    'openpublish_video' => array(
      'name' => t('Video'),
      'base' => 'node_content',
      'description' => t('Create content items with embedded video media assets.'),
      'has_title' => '1',
      'title_label' => t('Title'),
      'help' => '',
    ),
  );
  return $items;
}

/**
 * Implements hook_rdf_default_mappings().
 */
function openpublish_media_rdf_default_mappings() {
  $schemaorg = array();

  // Exported RDF mapping: openpublish_video
  $schemaorg['node']['openpublish_video'] = array(
    'rdftype' => array(
      0 => 'rnews:VideoObject',
      1 => 'schema:VideoObject',
    ),
    'title' => array(
      'predicates' => array(
        0 => 'rnews:name',
        1 => 'schema:name',
      ),
      'type' => 'property',
    ),
    'created' => array(
      'predicates' => array(
        0 => 'rnews:dateCreated',
        1 => 'schema:dateCreated',
      ),
      'datatype' => 'xsd:dateTime',
      'callback' => 'date_iso8601',
    ),
    'changed' => array(
      'predicates' => array(
        0 => 'rnews:dateModified',
        1 => 'schema:dateModified',
      ),
      'datatype' => 'xsd:dateTime',
      'callback' => 'date_iso8601',
    ),
    'body' => array(
      'predicates' => array(
        0 => 'rnews:description',
        1 => 'schema:description',
      ),
      'type' => 'property',
    ),
    'uid' => array(
      'predicates' => array(
        0 => 'sioc:has_creator',
      ),
      'type' => 'rel',
    ),
    'name' => array(
      'predicates' => array(
        0 => 'foaf:name',
      ),
    ),
    'comment_count' => array(
      'predicates' => array(
        0 => 'sioc:num_replies',
      ),
      'datatype' => 'xsd:integer',
    ),
    'last_activity' => array(
      'predicates' => array(
        0 => 'sioc:last_activity_date',
      ),
      'datatype' => 'xsd:dateTime',
      'callback' => 'date_iso8601',
    ),
    'field_op_related_nref' => array(
      'type' => 'rel',
      'predicates' => array(
        0 => 'rnews:associatedArticle',
        1 => 'schema:associatedArticle',
      ),
    ),
    'field_op_author' => array(
      'type' => 'rel',
      'predicates' => array(
        0 => 'rnews:creator',
        1 => 'schema:creator',
      ),
    ),
  );

  return $schemaorg;
}
