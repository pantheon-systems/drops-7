<?php
/**
 * @file
 * openpublish_blog_post.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function openpublish_blog_post_ctools_plugin_api() {
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
 * Implements hook_node_info().
 */
function openpublish_blog_post_node_info() {
  $items = array(
    'openpublish_blog_post' => array(
      'name' => t('Blog Post'),
      'base' => 'node_content',
      'description' => t('Used to create a single post to a column or blog.'),
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
function openpublish_blog_post_rdf_default_mappings() {
  $schemaorg = array();

  // Exported RDF mapping: openpublish_blog_post
  $schemaorg['node']['openpublish_blog_post'] = array(
    'rdftype' => array(
      0 => 'rnews:NewsItem',
      1 => 'schema:CreativeWork',
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

/**
 * Implements hook_rdf_namespaces().
 */
function openpublish_blog_post_rdf_namespaces() {
  return array(
    'rnews'  => 'http://iptc.org/std/rNews/2011-10-07#',
  );
}
