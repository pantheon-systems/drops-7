<?php

/**
 * @file
 * Drush integration functions for the xmlsitemap module.
 *
 * @ingroup xmlsitemap
 */

/**
 * Implements hook_drush_command().
 */
function xmlsitemap_drush_command() {
  $items['xmlsitemap-regenerate'] = array(
    'description' => 'Regenerate the XML sitemap files.',
    'callback' => 'drush_xmlsitemap_regenerate',
    'drupal dependencies' => array('xmlsitemap'),
  );
  $items['xmlsitemap-rebuild'] = array(
    'description' => 'Dump and re-process all possible XML sitemap data, and then regenerate the files.',
    'callback' => 'drush_xmlsitemap_rebuild',
    'drupal dependencies' => array('xmlsitemap'),
    'options' => array(
      'types' => 'The types of links to rebuild, comma separated. If not provided, all link types will be used.',
    ),
  );
  $items['xmlsitemap-index'] = array(
    'description' => 'Process un-indexed XML sitemap links.',
    'callback' => 'drush_xmlsitemap_index',
    'drupal dependencies' => array('xmlsitemap'),
    'options' => array(
      'limit' => 'The limit of links of each type to process. Default value: ' . variable_get('xmlsitemap_batch_limit', 100),
    ),
  );
  $items['xmlsitemap-queue-rebuild'] = array(
    'description' => 'Dump and queues all possible XML sitemap data to be re-processed via the xmlsitemap_link_process queue. This command does not regenerate the sitemap files.',
    'options' => array(
      'types' => 'The types of links to queue for rebuild, comma separated. If not provided, all link types will be used.',
      'limit' => 'The number of links to be processed in each queue task.',
    ),
  );
  return $items;
}

/**
 * Regenerate the sitemap files from existing data.
 */
function drush_xmlsitemap_regenerate() {
  module_load_include('generate.inc', 'xmlsitemap');

  // Run the batch process.
  timer_start('xmlsitemap_regenerate');
  xmlsitemap_run_unprogressive_batch('xmlsitemap_regenerate_batch');

  $vars = array(
    '@timer' => timer_read('xmlsitemap_regenerate'),
    '@memory-peak' => format_size(memory_get_peak_usage(TRUE)),
  );
  drush_print(dt('XML sitemap files regenerated in @timer ms. Peak memory usage: @memory-peak.', $vars));
}

/**
 * Dump and rebuild all the sitemap data, then regenerate the files.
 */
function drush_xmlsitemap_rebuild() {
  module_load_include('generate.inc', 'xmlsitemap');

  // Build a list of rebuildable link types.
  $types = xmlsitemap_get_rebuildable_link_types();
  if ($option_types = drush_get_option('types', '')) {
    $option_types = explode(',', $option_types);
    if ($invalid_types = array_diff($option_types, $types)) {
      drush_set_error(dt('The following link types are invalid: @types', array('@types' => implode(', ', $invalid_types))));
    }
    $types = array_intersect($types, $option_types);
  }
  if (empty($types)) {
    return drush_set_error(dt('No link types are rebuildable.'));
  }

  // Run the batch process.
  timer_start('xmlsitemap_rebuild');
  xmlsitemap_run_unprogressive_batch('xmlsitemap_rebuild_batch', $types, TRUE);

  $vars = array(
    '@timer' => timer_read('xmlsitemap_rebuild'),
    '@memory-peak' => format_size(memory_get_peak_usage(TRUE)),
  );
  drush_print(dt('XML sitemap files rebuilt in @timer ms. Peak memory usage: @memory-peak.', $vars));
}

/**
 * Process un-indexed XML sitemap links.
 */
function drush_xmlsitemap_index() {
  $limit = (int) drush_get_option('limit', variable_get('xmlsitemap_batch_limit', 100));
  $count_before = db_query("SELECT COUNT(id) FROM {xmlsitemap}")->fetchField();

  module_invoke_all('xmlsitemap_index_links', $limit);
  $count_after = db_query("SELECT COUNT(id) FROM {xmlsitemap}")->fetchField();

  if ($count_after == $count_before) {
    drush_print(dt('No new XML sitemap links to index.'));
  }
  else {
    drush_print(dt('Indexed @count new XML sitemap links.', array('@count' => $count_after - $count_before)));
  }
}

/**
 * Dump and queue all the sitemap links to be rebuilt in a queue process.
 */
function drush_xmlsitemap_queue_rebuild() {
  module_load_include('generate.inc', 'xmlsitemap');

  $types = xmlsitemap_get_rebuildable_link_types();
  if ($option_types = drush_get_option('types', '')) {
    $option_types = explode(',', $option_types);
    if ($invalid_types = array_diff($option_types, $types)) {
      drush_set_error(dt('The following link types are invalid: @types', array('@types' => implode(', ', $invalid_types))));
    }
    $types = array_intersect($types, $option_types);
  }
  if (empty($types)) {
    return drush_set_error(dt('No link types are rebuildable.'));
  }

  xmlsitemap_rebuild_clear($types, TRUE);

  $link_count = 0;
  $chunk_count = 0;
  $chunk_size = (int) drush_get_option('limit', variable_get('xmlsitemap_batch_limit', 100));

  // @todo Figure out how to re-use this code with xmlsitemap_rebuild_batch_fetch()
  foreach ($types as $type) {
    $info = xmlsitemap_get_link_info($type);
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $type);
    $query->entityCondition('entity_id', 0, '>');
    $query->addTag('xmlsitemap_link_bundle_access');
    $query->addTag('xmlsitemap_rebuild');
    $query->addMetaData('entity', $type);
    $query->addMetaData('entity_info', $info);
    if ($bundles = xmlsitemap_get_link_type_enabled_bundles($type)) {
      $query->entityCondition('bundle', $bundles, 'IN');
    }
    else {
      // If no enabled bundle types, skip everything else.
      continue;
    }

    $results = $query->execute();
    if (!empty($results[$type])) {
      $ids = array_keys($results[$type]);
      $link_count += count($ids);
      $chunks = array_chunk($ids, $chunk_size);
      $chunk_count += count($chunks);
      foreach ($chunks as $chunk) {
        xmlsitemap_link_enqueue($type, $chunk);
      }
    }
  }

  if ($link_count) {
    drush_log(dt('Queued @link_count links for rebuild processing in the xmlsitemap_link_process (in @chunk_count chunks of up to @chunk_size links each).', array('@link_count' => $link_count, '@chunk_count' => $chunk_count, '@chunk_size' => $chunk_size)), 'success');
  }
  else {
    drush_log(dt('No links to queue for rebuild processing.'), 'ok');
  }
  variable_set('xmlsitemap_rebuild_needed', FALSE);
}
