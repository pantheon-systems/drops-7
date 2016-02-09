<?php
/**
 * @file
 * cu_content_administration_override.features.inc
 */

/**
 * Implements hook_views_api().
 */
function cu_content_administration_override_views_api($module = NULL, $api = NULL) {
  return array("api" => "3.0");
}
