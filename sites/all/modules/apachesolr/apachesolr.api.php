<?php
/**
 * Exposed Hooks in 7.x:
 */

/**
 * Prepare the query by adding parameters, sorts, etc.
 *
 * This hook is invoked before the query is cached.  The cached query
 * is used after the search such as for building facet and sort blocks,
 * so parameters added during this hook may be visible to end users.
 *
 * This is otherwise the same as HOOK_apachesolr_query_alter(), but runs
 * before it.
 *
 * @param $query
 *  An object implementing DrupalSolrQueryInterface. No need for &.
 */
function HOOK_apachesolr_query_prepare($query) {
  // Add a sort on the node ID.
  $query->setAvailableSort('entity_id', array(
    'title' => t('Node ID'),
    'default' => 'asc',
  ));
}

/**
 * Alter the query after it's prepared and cached.
 *
 * Any module performing a search should call
 * drupal_alter('apachesolr_query', $query). That function then invokes
 * this hook. It allows modules to modify the query object and its params.
 *
 * A module implementing HOOK_apachesolr_query_alter() may set
 * $query->abort_search to TRUE to flag the query to be aborted.
 *
 * @param $query
 *  An object implementing DrupalSolrQueryInterface. No need for &.
 */
function HOOK_apachesolr_query_alter($query) {
  // I only want to see articles by the admin!
  $query->addFilter("is_uid", 1);

  // Only search titles.
  $query->replaceParam('qf', 'label');
}

/**
 * Alter hook for apachesolr_field_mappings().
 *
     Add or alter index mappings for Field API types.  The default mappings array handles just
    list fields and taxonomy term reference fields, such as:

    $mappings['list_text'] = array(
      'display_callback' => 'apachesolr_fields_list_display_callback',
      'indexing_callback' => 'apachesolr_fields_list_indexing_callback',
      'index_type' => 'string',
      'facets' => TRUE,
    ),

    In your _alter hook implementation you can add additional field types such as:

      $mappings['number_integer']['number'] = array('indexing_callback' => '', 'index_type' => 'integer', 'facets' => TRUE);

    You can allso add a mapping for a specific field.  This will take precedence over any
    mapping for a general field type. A field-specific mapping would look like:

      $mappings['per-field']['field_model_name'] = array('indexing_callback' => '', 'index_type' => 'string', 'facets' => TRUE);

    or

      $mappings['per-field']['field_model_price'] = array('indexing_callback' => '', 'index_type' => 'float', 'facets' => TRUE);

    If a custom field needs to be searchable but does not need to be faceted you can change the 'facets'
    parameter to FALSE, like:

      $mappings['number_integer']['number'] = array('callback' => '', 'index_type' => 'integer', 'facets' => FALSE);
 */
function HOOK_apachesolr_field_mappings_alter(&$mappings) {

}


/**
 *  Invoked by apachesolr.module when generating a list of nodes to index for a given
 * namespace.  Return an array of node types to be excluded from indexing for that namespace
 * (e.g. 'apachesolr_search'). This is used by apachesolr_search module to exclude
 * certain node types from the index.
 */
function HOOK_apachesolr_types_exclude($namespace) {
}

/**
 * This is invoked by apachesolr.module for each node to be added to the index.
 * If any module returns TRUE, the node is skipped for indexing. Note that nodes
 * which are already present in the index and subsequently qualify to be
 * excluded will not be removed from the index automatically. This hook can be
 * used to remove them prior to returning TRUE.
 */
function HOOK_apachesolr_node_exclude($node, $namespace) {
  // Exclude nodes from uid 1.
  if ($node->uid == 1) {
    apachesolr_delete_node_from_index($node);
    return TRUE;
  }
}

/**
 * Allows a module to change the contents of the $document object before it is sent to the Solr Server.
 * To add a new field to the document, you should generally use one of the pre-defined dynamic fields.
 * Follow the naming conventions for the type of data being added based on the schema.xml file.
 */
function HOOK_apachesolr_update_index($document, $node) {
}

/**
 * The is invoked by apachesolr_search.module for each document returned in a search - new in 6.x-beta7
 * as a replacement for the call to HOOK_nodeapi().
 */
function HOOK_apachesolr_search_result_alter($doc) {
}

/**
 *   Called by the sort link block code. Allows other modules to modify, add or remove sorts.
 */
function HOOK_apachesolr_sort_links_alter(&$sort_links) {
}

/**
 * Respond to search environment deletion.
 *
 * This hook is invoked from apachesolr_environment_delete() after the environment is removed
 * from the database.
 *
 * @param $environment
 *   The environment object that is being deleted.
 */
function HOOK_apachesolr_environment_delete($environment) {
}

