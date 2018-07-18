<?php

/**
 * @file
 * Hooks provided by the XML sitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Provide information on the type of links this module provides.
 *
 * @see hook_entity_info()
 * @see hook_entity_info_alter()
 */
function hook_xmlsitemap_link_info() {
  return array(
    'mymodule' => array(
      'label' => 'My module',
      'base table' => 'mymodule',
      'entity keys' => array(
        // Primary ID key on {base table}
        'id' => 'myid',
        // Subtype key on {base table}
        'bundle' => 'mysubtype',
      ),
      'path callback' => 'mymodule_path',
      'bundle label' => t('Subtype name'),
      'bundles' => array(
        'mysubtype1' => array(
          'label' => t('My subtype 1'),
          'admin' => array(
            'real path' => 'admin/settings/mymodule/mysubtype1/edit',
            'access arguments' => array('administer mymodule'),
          ),
          'xmlsitemap' => array(
            'status' => XMLSITEMAP_STATUS_DEFAULT,
            'priority' => XMLSITEMAP_PRIORITY_DEFAULT,
          ),
        ),
      ),
      'xmlsitemap' => array(
        // Callback function to take an array of IDs and save them as sitemap
        // links.
        'process callback' => '',
        // Callback function used in batch API for rebuilding all links.
        'rebuild callback' => '',
        // Callback function called from the XML sitemap settings page.
        'settings callback' => '',
      )
    ),
  );
}

/**
 * Alter the data of a sitemap link before the link is saved.
 *
 * @param array $link
 *   An array with the data of the sitemap link.
 * @param array $context
 *   An optional context array containing data related to the link.
 */
function hook_xmlsitemap_link_alter(array &$link, array $context) {
  if ($link['type'] == 'mymodule') {
    $link['priority'] += 0.5;
  }
}

/**
 * Inform modules that an XML sitemap link has been created.
 *
 * @param $link
 *   Associative array defining an XML sitemap link as passed into
 *   xmlsitemap_link_save().
 * @param array $context
 *   An optional context array containing data related to the link.
 *
 * @see hook_xmlsitemap_link_update()
 */
function hook_xmlsitemap_link_insert(array $link, array $context) {
  db_insert('mytable')
    ->fields(array(
      'link_type' => $link['type'],
      'link_id' => $link['id'],
      'link_status' => $link['status'],
    ))
    ->execute();
}

/**
 * Inform modules that an XML sitemap link has been updated.
 *
 * @param $link
 *   Associative array defining an XML sitemap link as passed into
 *   xmlsitemap_link_save().
 * @param array $context
 *   An optional context array containing data related to the link.
 *
 * @see hook_xmlsitemap_link_insert()
 */
function hook_xmlsitemap_link_update(array $link, array $context) {
  db_update('mytable')
    ->fields(array(
      'link_type' => $link['type'],
      'link_id' => $link['id'],
      'link_status' => $link['status'],
    ))
    ->execute();
}

/**
 * Respond to XML sitemap link clearing and rebuilding.
 *
 * @param array $types
 *   An array of link types that are being rebuilt.
 * @param bool $save_custom
 *   If links with overridden status and/or priority are being removed or not.
 */
function hook_xmlsitemap_rebuild_clear(array $types, $save_custom) {
  db_delete('mytable')
    ->condition('link_type', $types, 'IN')
    ->execute();
}

/**
 * Index links for the XML sitemaps.
 */
function hook_xmlsitemap_index_links($limit) {
}

/**
 * Provide information about contexts available to XML sitemap.
 *
 * @see hook_xmlsitemap_context_info_alter().
 */
function hook_xmlsitemap_context_info() {
  $info['vocabulary'] = array(
    'label' => t('Vocabulary'),
    'summary callback' => 'mymodule_xmlsitemap_vocabulary_context_summary',
    'default' => 0,
  );
  return $info;
}

/**
 * Alter XML sitemap context info.
 *
 * @see hook_xmlsitemap_context_info().
 */
function hook_xmlsitemap_context_info_alter(&$info) {
  $info['vocabulary']['label'] = t('Site vocabularies');
}

/**
 * Provide information about the current context on the site.
 *
 * @see hook_xmlsitemap_context_alter()
 */
function hook_xmlsitemap_context() {
  $context = array();
  if ($vid = mymodule_get_current_vocabulary()) {
    $context['vocabulary'] = $vid;
  }
  return $context;
}

/**
 * Alter the current context information.
 *
 * @see hook_xmlsitemap_context()
 */
function hook_xmlsitemap_context_alter(&$context) {
  if (user_access('administer taxonomy')) {
    unset($context['vocabulary']);
  }
}

/**
 * Provide options for the url() function based on an XML sitemap context.
 */
function hook_xmlsitemap_context_url_options(array $context) {
}

/**
 * Alter the url() options based on an XML sitemap context.
 */
function hook_xmlsitemap_context_url_options_alter(array &$options, array $context) {
}

/**
 * Alter the content added to an XML sitemap for an individual element.
 *
 * This hooks is called when the module is generating the XML content for the
 * sitemap and allows other modules to alter existing or add additional XML data
 * for any element by adding additional key value paris to the $element array.
 *
 * The key in the element array is then used as the name of the XML child
 * element to add and the value is the value of that element. For example:
 *
 * @code $element['video:title'] = 'Big Ponycorn'; @endcode
 *
 * Would result in a child element like <video:title>Big Ponycorn</video:title>
 * being added to the sitemap for this particular link.
 *
 * @param array $element
 *   The element that will be converted to XML for the link.
 * @param array $link
 *   An array of properties providing context about the link that we are
 *   generating an XML element for.
 * @param object $sitemap
 *   The sitemap that is currently being generated.
 */
function hook_xmlsitemap_element_alter(array &$element, array $link, $sitemap) {
  if ($link['subtype'] === 'video') {
    $node = node_load($link['id']);
    $element['video:video'] = array(
      'video:title' => check_plain($node->title),
      'video:description' => isset($node->body[LANGUAGE_NONE][0]['summary']) ? check_plain($node->body[LANGUAGE_NONE][0]['summary']) : check_plain($node->body[LANGUAGE_NONE][0]['value']),
      'video:live' => 'no',
    );
  }
}

/**
 * Alter the attributes used for the root element of the XML sitemap.
 *
 * For example add an xmlns:video attribute:
 * <urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9" xmlns:video="http://www.google.com/schemas/sitemap-video/1.1">
 *
 * @param array $attributes
 *   An associative array of attributes to use in the root element of an XML
 *   sitemap.
 * @param object $sitemap
 *   The sitemap that is currently being generated.
 */
function hook_xmlsitemap_root_attributes_alter(&$attributes, $sitemap) {
  $attributes['xmlns:video'] = 'http://www.google.com/schemas/sitemap-video/1.1';
}

/**
 * Alter the query selecting data from {xmlsitemap} during sitemap generation.
 *
 * @param $query
 *   A Query object describing the composite parts of a SQL query.
 *
 * @see hook_query_TAG_alter()
 */
function hook_query_xmlsitemap_generate_alter(QueryAlterableInterface $query) {
  $sitemap = $query->getMetaData('sitemap');
  if (!empty($sitemap->context['vocabulary'])) {
    $node_condition = db_and();
    $node_condition->condition('type', 'taxonomy_term');
    $node_condition->condition('subtype', $sitemap->context['vocabulary']);
    $normal_condition = db_and();
    $normal_condition->condition('type', 'taxonomy_term', '<>');
    $condition = db_or();
    $condition->condition($node_condition);
    $condition->condition($normal_condition);
    $query->condition($condition);
  }
}

/**
 * Provide information about XML sitemap bulk operations.
 */
function hook_xmlsitemap_sitemap_operations() {
}

/**
 * Respond to XML sitemap deletion.
 *
 * This hook is invoked from xmlsitemap_sitemap_delete_multiple() after the XML
 * sitemap has been removed from the table in the database.
 *
 * @param $sitemap
 *   The XML sitemap object that was deleted.
 */
function hook_xmlsitemap_sitemap_delete(stdClass $sitemap) {
  db_query("DELETE FROM {mytable} WHERE smid = '%s'", $sitemap->smid);
}

/**
 * @} End of "addtogroup hooks".
 */
