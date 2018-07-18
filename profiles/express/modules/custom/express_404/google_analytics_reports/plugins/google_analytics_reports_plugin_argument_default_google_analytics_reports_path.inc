<?php

/**
 * @file
 * Contains current page path argument default plugin.
 */

/**
 * Default argument plugin to use current page path as argument.
 *
 * @ingroup views_argument_default_plugins
 */
class google_analytics_reports_plugin_argument_default_google_analytics_reports_path extends views_plugin_argument_default {

  /**
   * Get argument.
   */
  public function get_argument() {
    // Decode urls, that might appear due to browsers particularities.
    return urldecode(request_uri());
  }
}
