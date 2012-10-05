<?php
/**
 * @file
 * phase2_profile.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function phase2_profile_ctools_plugin_api() {
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
function phase2_profile_image_default_styles() {
  $styles = array();

  // Exported image style: list-page-thumbnail
  $styles['list-page-thumbnail'] = array(
    'name' => 'list-page-thumbnail',
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
          'width' => '95',
          'height' => '95',
        ),
        'weight' => '1',
      ),
    ),
  );

  // Exported image style: right-sidebar-thumbnails
  $styles['right-sidebar-thumbnails'] = array(
    'name' => 'right-sidebar-thumbnails',
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
          'width' => '80',
          'height' => '80',
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
function phase2_profile_node_info() {
  $items = array(
    'profile' => array(
      'name' => t('Profile'),
      'base' => 'node_content',
      'description' => t('Create a profile for a person, which may be an author, a staff member, or even a business.'),
      'has_title' => '1',
      'title_label' => t('Full Name'),
      'help' => t('This is the name used as the title of this content. This may be comprised of the First and Last names, or of the Organization name, or any other name that you choose.'),
    ),
  );
  return $items;
}

/**
 * Implements hook_rdf_default_mappings().
 */
function phase2_profile_rdf_default_mappings() {
  $schemaorg = array();

  // Exported RDF mapping: profile
  $schemaorg['node']['profile'] = array(
    'rdftype' => array(
      0 => 'rnews:Person',
      1 => 'schema:Person',
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
        0 => 'dc:date',
        1 => 'dc:created',
      ),
      'datatype' => 'xsd:dateTime',
      'callback' => 'date_iso8601',
    ),
    'changed' => array(
      'predicates' => array(
        0 => 'dc:modified',
      ),
      'datatype' => 'xsd:dateTime',
      'callback' => 'date_iso8601',
    ),
    'body' => array(
      'predicates' => array(
        0 => 'content:encoded',
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
    'field_profile_first_name' => array(
      'type' => 'property',
      'predicates' => array(
        0 => 'rnews:givenName',
        1 => 'schema:givenName',
      ),
    ),
    'field_profile_last_name' => array(
      'type' => 'property',
      'predicates' => array(
        0 => 'rnews:familyName',
        1 => 'schema:familyName',
      ),
    ),
  );

  return $schemaorg;
}

/**
 * Implements hook_rdf_namespaces().
 */
function phase2_profile_rdf_namespaces() {
  return array(
    'rnews'  => 'http://iptc.org/std/rNews/2011-10-07#',
  );
}
