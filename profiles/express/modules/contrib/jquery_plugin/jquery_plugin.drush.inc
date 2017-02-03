<?php
// $Id: jquery_plugin.drush.inc,v 1.1.2.2 2010/09/30 19:37:35 mfb Exp $

/**
 * Implements hook_drush_command().
 */
function jquery_plugin_drush_command() {
  $items['jquery-plugin-update'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_FULL,
    'description' => 'Updates jQuery plugins',
  );
  return $items;
}

/**
 * Drush callback; update jQuery plugins.
 */
function drush_jquery_plugin_update() {
  // Update jQuery Tools plugins.
  $ch = curl_init('http://flowplayer.org/tools/download/index.html');
  // Support gzip-encoded response.
  curl_setopt($ch, CURLOPT_ENCODING, '');
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  $source = curl_exec($ch);
  curl_close($ch);
  $doc = new DOMDocument();
  // Ignore parse warnings.
  @$doc->loadHTML($source);
  $xpath = new DOMXpath($doc);
  $anchors = $xpath->query('//table//a');
  foreach ($anchors as $anchor) {
    $href = $anchor->getAttribute('href');
    if (strpos($href, '.min.js') && substr_count($href, '/') == 5) {
      $ch = curl_init('http://flowplayer.org' . $href);
      $handle = fopen(drupal_get_path('module', 'jquery_plugin') . '/' . strtok(basename($href), '?'), 'w');
      // Support gzip-encoded response.
      curl_setopt($ch, CURLOPT_ENCODING, '');
      curl_setopt($ch, CURLOPT_FILE, $handle);
      curl_exec($ch);
      curl_close($ch);
      fclose($handle);
    }
  }
}
