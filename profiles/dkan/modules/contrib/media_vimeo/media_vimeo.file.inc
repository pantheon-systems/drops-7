<?php

/**
 * @file
 * File hooks implemented by the Media: Vimeo module.
 */

/**
 * Implements hook_file_operations().
 */
function media_vimeo_file_operations() {
  $operations = array(
    'media_vimeo_refresh' => array(
      'label' => t('Refresh Vimeo information from source'),
      'callback' => 'media_vimeo_cache_clear',
    ),
  );

  return $operations;
}

/**
 * Clear the cached Vimeo content for the selected files.
 */
function media_vimeo_cache_clear($fids) {
  $fids = array_keys($fids);

  $query = new EntityFieldQuery();
  $results = $query
    ->entityCondition('entity_type', 'file')
    ->propertyCondition('uri', 'vimeo:', 'STARTS_WITH')
    ->propertyCondition('fid', $fids)
    ->execute();

  $files = file_load_multiple(array_keys($results['file']));

  foreach ($files as $file) {
    $wrapper = file_stream_wrapper_get_instance_by_uri($file->uri);
    $local_path = $wrapper->getLocalThumbnailPath();
    file_unmanaged_delete($local_path);
  }
}
