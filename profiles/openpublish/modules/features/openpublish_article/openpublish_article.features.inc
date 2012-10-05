<?php
/**
 * @file
 * openpublish_article.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_article_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "boxes" && $api == "box") {
    return array("version" => "1");
  }
  list($module, $api) = func_get_args();
  if ($module == "context" && $api == "context") {
    return array("version" => "3");
  }
  list($module, $api) = func_get_args();
  if ($module == "field_group" && $api == "field_group") {
    return array("version" => "1");
  }
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => "1");
  }
}

/**
 * Implements hook_image_default_styles().
 */
function openpublish_article_image_default_styles() {
  $styles = array();

  // Exported image style: 400px_wide.
  $styles['400px_wide'] = array(
    'name' => '400px_wide',
    'effects' => array(
      2 => array(
        'label' => 'Scale',
        'help' => 'Scaling will maintain the aspect-ratio of the original image. If only a single dimension is specified, the other dimension will be calculated.',
        'effect callback' => 'image_scale_effect',
        'dimensions callback' => 'image_scale_dimensions',
        'form callback' => 'image_scale_form',
        'summary theme' => 'image_scale_summary',
        'module' => 'image',
        'name' => 'image_scale',
        'data' => array(
          'width' => '400',
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: 620px_wide.
  $styles['620px_wide'] = array(
    'name' => '620px_wide',
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
          'width' => '620',
          'height' => '',
          'upscale' => 0,
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: author_thumbnail.
  $styles['author_thumbnail'] = array(
    'name' => 'author_thumbnail',
    'effects' => array(
      1 => array(
        'label' => 'Scale and crop',
        'help' => 'Scale and crop will maintain the aspect-ratio of the original image, then crop the larger dimension. This is most useful for creating perfectly square thumbnails without stretching the image.',
        'effect callback' => 'image_scale_and_crop_effect',
        'dimensions callback' => 'image_resize_dimensions',
        'form callback' => 'image_resize_form',
        'summary theme' => 'image_resize_summary',
        'module' => 'image',
        'name' => 'image_scale_and_crop',
        'data' => array(
          'width' => '50',
          'height' => '50',
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
function openpublish_article_node_info() {
  $items = array(
    'openpublish_article' => array(
      'name' => t('Article'),
      'base' => 'node_content',
      'description' => t('Used to create individual stories as would appear in News, Sport, or Business content areas.'),
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
function openpublish_article_rdf_default_mappings() {
  $schemaorg = array();

  // Exported RDF mapping: openpublish_article
  $schemaorg['node']['openpublish_article'] = array(
    'rdftype' => array(
      0 => 'rNews:Article',
      1 => 'schema:NewsArticle',
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
        0 => 'rnews:articlebody',
        1 => 'schema:articleBody',
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
    'field_op_author' => array(
      'type' => 'rel',
      'predicates' => array(
        0 => 'rnews:creator',
        1 => 'schema:creator',
      ),
    ),
    'field_op_main_image' => array(
      'type' => 'rel',
      'predicates' => array(
        0 => 'rnews:associatedMedia',
        1 => 'schema:associatedMedia',
      ),
    ),
  );

  return $schemaorg;
}
