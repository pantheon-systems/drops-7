By Khalid Baheyeldin

Copyright 2008 http://2bits.com

Description
-----------
This module provides performance statistics logging for a site, such as page
generation times, and memory usage, for each page load.

This module is useful for developers and site administrators alike to identify
pages that are slow to generate or use excessive memory.

Features include:
* Settings to enable detailed logging or summary logging. The module defaults to
  no logging at all.

* Detailed logging causes one database row to be written for each page load of
  the site. The data includes page generation time in milliseconds, and the
  number of bytes allocated to PHP, time stamp, etc.

* Summary logging logs the average and maximum page generation time, average and
  maximum memory usage, last access time, and number of accesses for each path.

* Summary can be logged to any cache for which a Drupal caching module is
  available that transparently integrates with the drupal cache layer. Some
  examples:
    http://drupal.org/project/apc
    http://drupal.org/project/memcache
    http://drupal.org/project/filecache

* A settings option is available when using summary mode, to exclude
  pages with less than a certain number of accesses. Useful for large sites.

* Support for normal page cache.

Note that detailed logging is only suitable for a site that is in development or
testing. Do NOT enable detailed logging on a live site.

The memory measurement feature of this module depends on the
memory_get_peak_usage() function, available only in PHP 5.2.x or later.

Only summary logging with Memcache, APC or similar mechanisms are the
recommended mode for live sites, with a threshold of 2 or more.

Note on Completeness:
---------------------
Please note that when summary logging to APC or Memcache, the data captured in
the summary will not be comprehensive reflecting every single page view for
every URL.

The reason for this is that there is no atomic locking when updating the data
structures that store per-URL statistics in this module.

This means that the values you get when using these storage caches are only
samples, and would miss some page views, depending on how busy the site is.

For memcache, there is way to implement locking using the $mc->increment and/or
$mc->add as well. However, there is a risk if these are implemented, that there
will be less concurrency and we can cause a site to slow down.

Configuration:
--------------
To configure the Performance Logging and Monitoring module, navigate to
/admin/config/development/performance-logging. By default, this module creates a
key for each entry based off of the hostname of the site being accessed. If you
have a site with multiple domains, it is recommended to specify a shared key
between all sites in your settings.php file:

  $conf['performance_key'] = 'example_key';

If you are using memcache, then you need to configure an extra bin for
performance. If you have multiple web server boxes, then it is best to
centralize this bin for all the boxes, so you get combined statistics.

Your settings.php looks like this:

  $conf['cache_backends'][] = './sites/all/modules/memcache/memcache.inc';
  $conf['cache_default_class'] = 'MemCacheDrupal';
  // Prevent special cache_form bin from being assigned to a volatile cache
  // storage implementation
  $conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

  $conf['memcache_servers'] = array(
    '127.0.0.1:11211' => 'default',
    // More bins here ....
    '127.0.0.1:11311' => 'performance',
  );
  $conf['memcache_bins'] = array(
    'cache_performance' => 'performance',
  );

Note that since version 2.x, you can use any Drupal caching module available
that transparently integrates with the drupal cache layer (like the apc or
filecache modules).

Statistics:
-----------
You can view the recorded performance statistics (summary and details) at
/admin/reports/performance-logging

Custom detailed logging implementation
--------------------------------------
As mentioned before, detailed logging is NOT recommended on production environ-
ments. If you, for whatever reason, DO wish detailed logging on production, you
should create a custom detailed logging mechanism that will NOT interfere with
your live site. You can do this by creating your own versions of the following
functions:

  - performance_log_details($params)
    => function that is called to store the performance data
  - performance_view_details()
    => function that is called to view the stored detail log. This function is
       called from hook_menu() and should return content that Drupal can render
      as a page.
  - performance_clear_details()
    => function that is called to delete the entire detail log

Have a look at includes/performance.details.inc for more details about these
functions.

When you have created those functions, add the location of the file containing
your custom implementation to settings.php like so:

  $conf['performance_detail_logging'] = './sites/all/path/to/your/file';

NOTE: there is NO drush support for your custom detail logging implementation!

Drush support
-------------
Drush support has been integrated as well. You can check the summary and detail
logs using the performance-summary (aliased as perf-sm) and performance-detail
(aliased as perf-dt) commands. Some examples:

  Retrieve last 15 entries from the detail log:
    drush performance-detail 15

  Retrieve last 20 summary log entries sorted by the number of queries,
  descending:
    drush performance-summary 20 query_count

  Retrieve last 35 entries from the detail log sorted by size, ascending:
    drush performance-detail 35 bytes asc

Use drush perf-sm --help or drush perf-dt --help to see a full explanation.

Bugs/Features/Patches:
----------------------
If you want to report bugs, feature requests, or submit a patch, please do so at
the project page on the Drupal web site at http://drupal.org/project/performance

Author
------
Khalid Baheyeldin (http://baheyeldin.com/khalid and http://2bits.com)

If you use this module, find it useful, and want to send the author a thank you
note, then use the Feedback/Contact page at the URL above.

The author can also be contacted for paid customizations of this and other
modules.
