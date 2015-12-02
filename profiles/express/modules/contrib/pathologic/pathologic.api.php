<?php

/**
 * @file
 * Hooks provided by Pathologic.
 *
 * @ingroup pathologic
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Allow modules to alter a URL Pathologic is about to create.
 *
 * This hook is invoked after Pathologic has torn apart a URL it thinks it can
 * alter properly and is just about to call the url() function to construct the
 * new URL. Modules can alter the values that Pathologic is about to send to
 * url(), or even stop Pathologic from altering a URL entirely.
 *
 * @param $url_params
 *   An array with 'path' and 'options' values, which correspond to the $path
 *   and $options parameters of the url() function. The 'options' array has an
 *   extra parameter labeled 'use_original' which is set to FALSE by default.
 *   This parameter is ignored by url(), but if its value is set to TRUE after
 *   all alter hook invocations, Pathologic will return the original, unaltered
 *   path it found in the content instead of calling url() and generating a new
 *   one. Thus, it provides a way for modules to halt the alteration of paths
 *   which Pathologic has incorrectly decided should be altered.
 * @param $parts
 *   This array contains the result of running parse_url() on the path that
 *   Pathologic found in content, though Pathologic likely altered some of the
 *   values in this array since. It contains another parameter, 'original',
 *   which contains the original URL Pathologic found in the content, unaltered.
 *   You should not alter this value in any way; to alter how Pathologic
 *   constructs the new URL, alter $url_params instead.
 * @param $settings
 *   This contains the settings Pathologic is using to decide how to alter the
 *   URL; some settings are from the graphical filter form and alterable by the
 *   user, while others are determined programmatically. If you're looking for
 *   the filter settings which Pathologic is currently using (if you've altered
 *   your own field onto the filter settings form, for example), try looking in
 *   $settings['current_settings'].
 *
 * @see url()
 * @see parse_url()
 * @see pathologic_replace()
 * @see http://drupal.org/node/1762022
 */
function hook_pathologic_alter(&$url_params, $parts, $settings) {
  // If we're linking to the "bananas" subdirectory or something under it, then
  // have Pathologic pass through the original URL, without altering it.
  if (preg_match('~^bananas(/.*)?$~', $url_params['path'])) {
    $url_params['options']['use_original'] = TRUE;
  }

  // If we're linking to a path like "article/something.html", then prepend
  // "magazine" to the path, but remove the ".html". The end result will look
  // like "magazine/article/something".
  if (preg_match('~^article/(.+)\.html$~', $url_params['path'], $matches)) {
    $url_params['path'] = 'magazine/article/' . $matches[1];
  }

  // If the URL doesn't have a "foo" query parameter, then add one.
  if (!is_array($url_params['options']['query'])) {
    $url_params['options']['query'] = array();
  }
  if (empty($url_params['options']['query']['foo'])) {
    $url_params['options']['query']['foo'] = 'bar';
  }

  // If it's a path to a local image, make sure it's using our CDN server.
  if (preg_match('~\.(png|gif|jpe?g)$~', $url_params['path'])) {
    $url_params['path'] = 'http://cdn.example.com/' . $url_params['path'];
    $url_params['options']['external'] = TRUE;
  }
}

/**
 * @} End of "addtogroup hooks".
 */
