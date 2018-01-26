## IMPORTANT NOTE ##

This file contains installation instructions for the 7.x-1.x version of the
Drupal Memcache module. Configuration differs between 7.x and 6.x versions
of the module, so be sure to follow the 6.x instructions if you are configuring
the 6.x-1.x version of this module!

## REQUIREMENTS ##

- PHP 5.1 or greater
- Availability of a memcached daemon: http://memcached.org/
- One of the two PECL memcache packages:
  - http://pecl.php.net/package/memcache (recommended)
  - http://pecl.php.net/package/memcached (latest versions require PHP 5.2 or
    greater)

## INSTALLATION ##

These are the steps you need to take in order to use this software. Order
is important.

 1. Install the memcached binaries on your server and start the memcached
    service. Follow best practices for securing the service; for example,
    lock it down so only your web servers can make connections. Find community
    maintained documentation with a number of walk-throughs for various
    operating systems at https://www.drupal.org/node/1131458.
 2. Install your chosen PECL memcache extension -- this is the memcache client
    library which will be used by the Drupal memcache module to interact with
    the memcached server(s). Generally PECL memcache (3.0.6+) is recommended,
    but PECL memcached (2.0.1+) also works well for some people. There are
    known issues with older versions. Refer to the community maintained
    documentation referenced above for more information.
 3. Put your site into offline mode.
 4. Download and install the memcache module.
 5. If you have previously been running the memcache module, run update.php.
 6. Optionally edit settings.php to configure the servers, clusters and bins
    for memcache to use. If you skip this step the Drupal module will attempt to
    talk to the memcache server on port 11211 on the local host, storing all
    data in a single bin. This is sufficient for most smaller, single-server
    installations.
 7. Edit settings.php to make memcache the default cache class, for example:
      $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
      $conf['cache_default_class'] = 'MemCacheDrupal';
    The cache_backends path needs to be adjusted based on where you installed
    the module.
 8. Make sure the following line also exists, to ensure that the special
    cache_form bin is assigned to non-volatile storage:
      $conf['cache_class_cache_form'] = 'DrupalDatabaseCache';
 9. Optionally also add the following two lines to tell Drupal not to bootstrap
    the database when serving cached pages to anonymous visitors:
      $conf['page_cache_without_database'] = TRUE;
      $conf['page_cache_invoke_hooks'] = FALSE;
    If setting page_cache_without_database to TRUE, you also have to set
    page_cache_invoke_hooks to FALSE or you'll see an error like "Fatal error:
    Call to undefined function module_list()".
10. Bring your site back online.

## DRUSH ##

Enable the memcache module at admin/modules or with 'drush en memcache', then
rebuild the drush cache by running 'drush cc drush'. This will enable the
following drush commands:

  memcache-flush (mcf)  Flush all Memcached objects in a bin.
  memcache-stats (mcs)  Retrieve statistics from Memcached.

For more information about each command, use 'drush help'. For example:
  drush help mcf

Or:
  drush help mcs

## ADVANCED CONFIGURATION ##

This module is capable of working with one memcached instance or with multiple
memcached instances run across one or more servers. The default is to use one
server accessible on localhost port 11211. If that meets your needs, then the
configuration settings outlined above are sufficient for the module to work.
If you want to use multiple memcached instances, or if you are connecting to a
memcached instance located on a remote machine, further configuration is
required.

The available memcached servers are specified in $conf in settings.php. If you
do not specify any servers, memcache.inc assumes that you have a memcached
instance running on localhost:11211. If this is true, and it is the only
memcached instance you wish to use, no further configuration is required.

If you have more than one memcached instance running, you need to add two arrays
to $conf; memcache_servers and memcache_bins. The arrays follow this pattern:

'memcache_servers' => array(
  server1:port => cluster1,
  server2:port => cluster2,
  serverN:port => clusterN,
  'unix:///path/to/socket' => clusterS
)

'memcache_bins' => array(
   bin1 => cluster1,
   bin2 => cluster2,
   binN => clusterN,
   binS => clusterS
)

You can optionally assign a weight to each server, favoring one server more than
another. For example, to make it 10 times more likely to store an item on
server1 versus server2:

'memcache_servers' => array(
  server1:port => array('cluster' => cluster1, 'weight' => 10),
  server2:port => array('cluster' => cluster2, 'weight' => 1'),
)

The bin/cluster/server model can be described as follows:

- Servers are memcached instances identified by host:port.

- Clusters are groups of servers that act as a memory pool. Each cluster can
  contain one or more servers.

- Bins are groups of data that get cached together and map 1:1 to the $table
  parameter of cache_set(). Examples from Drupal core are cache_filter and
  cache_menu. The default is 'cache'.

- Multiple bins can be assigned to a cluster.

- The default cluster is 'default'.

## LOCKING ##

The memcache-lock.inc file included with this module can be used as a drop-in
replacement for the database-mediated locking mechanism provided by Drupal
core. To enable, define the following in your settings.php:

  $conf['lock_inc'] = 'sites/all/modules/memcache/memcache-lock.inc';

Locks are written in the 'semaphore' table, which will map to the 'default'
memcache cluster unless you explicitly configure a 'semaphore' cluster.

## STAMPEDE PROTECTION ##

Memcache includes stampede protection for rebuilding expired and invalid cache
items.  To enable stampede protection, define the following in settings.php:

  $conf['memcache_stampede_protection'] = TRUE;

To avoid lock stampedes, it is important that you enable the memcache lock
implementation when enabling stampede protection -- enabling stampede protection
without enabling the Memcache lock implementation can cause worse performance and
can result in dropped locks due to key-length truncation.

Memcache stampede protection is primarily designed to benefit the following
caching pattern: a miss on a cache_get() for a specific cid is immediately
followed by a cache_set() for that cid. Of course, this is not the only caching
pattern used in Drupal, so stampede protection can be selectively disabled for
optimal performance.  For example, a cache miss in Drupal core's
module_implements() won't execute a cache_set until drupal_page_footer()
calls module_implements_write_cache() which can occur much later in page
generation.  To avoid long hanging locks, stampede protection should be
disabled for these delayed caching patterns.

Memcache stampede protection can be disabled for entire bins, specific cid's in
specific bins, or cid's starting with a specific prefix in specific bins. For
example:

  $conf['memcache_stampede_protection_ignore'] = array(
    // Ignore some cids in 'cache_bootstrap'.
    'cache_bootstrap' => array(
      'module_implements',
      'variables',
      'lookup_cache',
      'schema:runtime:*',
      'theme_registry:runtime:*',
      '_drupal_file_scan_cache',
    ),
    // Ignore all cids in the 'cache' bin starting with 'i18n:string:'
    'cache' => array(
      'i18n:string:*',
    ),
    // Disable stampede protection for the entire 'cache_path' and 'cache_rules'
    // bins.
    'cache_path',
    'cache_rules',
  );

Only change the following stampede protection tunables if you're sure you know
what you're doing, which requires first reading the memcache.inc code.

The value passed to lock_acquire, defaults to '15':
  $conf['memcache_stampede_semaphore'] = 15;

The value passed to lock_wait, defaults to 5:
  $conf['memcache_stampede_wait_time'] = 5;

The maximum number of calls to lock_wait() due to stampede protection during a
single request, defaults to 3:
  $conf['memcache_stampede_wait_limit'] = 3;

When adjusting these variables, be aware that:
 - there is unlikely to be a good use case for setting wait_time higher
   than stampede_semaphore;
 - wait_time * wait_limit is designed to default to a number less than
   standard web server timeouts (i.e. 15 seconds vs. apache's default of
   30 seconds).

## CACHE LIFETIME ##

Memcache respects Drupal core's minimum cache lifetime configuration. This
setting affects all cached items, not just pages. In some cases, it may
be desirable to cache different types of items for different amounts of time.
You can override the minimum cache lifetime on a per-bin basis in settings.php.
For example:

  // Cache pages for 60 seconds.
  $conf['cache_lifetime_cache_page'] = 60;
  // Cache menus for 10 minutes.
  $conf['cache_lifetime_menu'] = 600;

## CACHE HEADER ##

Drupal core indicates whether or not a page was served out of the cache by
setting the 'X-Drupal-Cache' response header with a value of HIT or MISS. If
you'd like to confirm whether pages are actually being retreived from Memcache
and not another backend, you can enable the following option:

  $conf['memcache_pagecache_header'] = TRUE;

When enabled, the Memcache module will add its own 'Drupal-PageCache-Memcache'
header. When cached pages are served out of the cache the header will include an
'age=' value indicating how many seconds ago the page was stored in the cache.

## PERSISTENT CONNECTIONS ##

As of 7.x-1.6, the memcache module uses peristent connections by default. If
this causes you problems you can disable persistent connections by adding the
following to your settings.php:

  $conf['memcache_persistent'] = FALSE;

## EXAMPLES ##

Example 1:

First, the most basic configuration which consists of one memcached instance
running on localhost port 11211 and all caches except for cache_form being
stored in memcache. We also enable stampede protection, and the memcache
locking mechanism. Finally, we tell Drupal to not bootstrap the database when
serving cached pages to anonymous visitors.

  $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
  $conf['lock_inc'] = 'sites/all/modules/memcache/memcache-lock.inc';
  $conf['memcache_stampede_protection'] = TRUE;
  $conf['cache_default_class'] = 'MemCacheDrupal';

  // The 'cache_form' bin must be assigned to non-volatile storage.
  $conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

  // Don't bootstrap the database when serving pages from the cache.
  $conf['page_cache_without_database'] = TRUE;
  $conf['page_cache_invoke_hooks'] = FALSE;

Note that no servers or bins are defined.  The default server and bin
configuration which is used in this case is equivalant to setting:

  $conf['memcache_servers'] = array('localhost:11211' => 'default');


Example 2:

In this example we define three memcached instances, two accessed over the
network, and one on a Unix socket -- please note this is only an illustration of
what is possible, and is not a recommended configuration as it's highly unlikely
you'd want to configure memcache to use both sockets and network addresses like
this, instead you'd consistently use one or the other.

The instance on port 11211 belongs to the 'default' cluster where everything
gets cached that isn't otherwise defined. (We refer to it as a "cluster", but in
this example our "clusters" involve only one instance.) The instance on port
11212 belongs to the 'pages' cluster, with the 'cache_page' table mapped to
it -- so the Drupal page cache is stored in this cluster.  Finally, the instance
listening on a socket is part of the 'blocks' cluster, with the 'cache_block'
table mapped to it -- so the Drupal block cache is stored here. Note that
sockets do not have ports.

  $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
  $conf['lock_inc'] = 'sites/all/modules/memcache/memcache-lock.inc';
  $conf['memcache_stampede_protection'] = TRUE;
  $conf['cache_default_class'] = 'MemCacheDrupal';

  // The 'cache_form' bin must be assigned no non-volatile storage.
  $conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

  // Don't bootstrap the database when serving pages from the cache.
  $conf['page_cache_without_database'] = TRUE;
  $conf['page_cache_invoke_hooks'] = FALSE;

  // Important to define a default cluster in both the servers
  // and in the bins. This links them together.
  $conf['memcache_servers'] = array('10.1.1.1:11211' => 'default',
                                    '10.1.1.1:11212' => 'pages',
                                    'unix:///path/to/socket' => 'blocks');
  $conf['memcache_bins'] = array('cache' => 'default',
                                 'cache_page' => 'pages',
                                 'cache_block' => 'blocks');


Example 3:

Here is an example configuration that has two clusters, 'default' and
'cluster2'. Five memcached instances running on four different servers are
divided up between the two clusters. The 'cache_filter' and 'cache_menu' bins
go to 'cluster2'. All other bins go to 'default'.

  $conf['cache_backends'][] = 'sites/all/modules/memcache/memcache.inc';
  $conf['lock_inc'] = 'sites/all/modules/memcache/memcache-lock.inc';
  $conf['memcache_stampede_protection'] = TRUE;
  $conf['cache_default_class'] = 'MemCacheDrupal';

  // The 'cache_form' bin must be assigned no non-volatile storage.
  $conf['cache_class_cache_form'] = 'DrupalDatabaseCache';

  // Don't bootstrap the database when serving pages from the cache.
  $conf['page_cache_without_database'] = TRUE;
  $conf['page_cache_invoke_hooks'] = FALSE;

  $conf['memcache_servers'] = array('10.1.1.6:11211' => 'default',
                                    '10.1.1.6:11212' => 'default',
                                    '10.1.1.7:11211' => 'default',
                                    '10.1.1.8:11211' => 'cluster2',
                                    '10.1.1.9:11211' => 'cluster2');

  $conf['memcache_bins'] = array('cache' => 'default',
                                 'cache_filter' => 'cluster2',
                                 'cache_menu' => 'cluster2');
  );

## PREFIXING ##

If you want to have multiple Drupal installations share memcached instances,
you need to include a unique prefix for each Drupal installation in the $conf
array of settings.php. This can be a single string prefix, or a keyed array of
bin => prefix pairs:

   $conf['memcache_key_prefix'] = 'something_unique';

Using a per-bin prefix:

   $conf['memcache_key_prefix'] = array(
     'default' => 'something_unique',
     'cache_page' => 'something_else_unique'
   );

In the above example, the 'something_unique' prefix will be used for all bins
except for the 'cache_page' bin which will use the 'something_else_unique'
prefix. Note that if using a keyed array for specifying prefix, you must specify
the 'default' prefix.

It is also possible to specify multiple prefixes per bin. Only the first prefix
will be used when setting/getting cache items, but all prefixes will be cleared
when deleting cache items. This provides support for more complicated
configurations such as a live instance and an administrative instance each with
their own prefixes and therefore their own unique caches. Any time a cache item
is deleted on either instance, it gets flushed on both -- thus, should an admin
do something that flushes the page cache, it will appropriately get flushed on
both instances. (For more discussion see the issue where support was added,
https://www.drupal.org/node/1084448.) This feature is enabled when you configure
prefixes as arrays within arrays. For example:

  // Live instance.
  $conf['memcache_key_prefix'] = array(
    'default' => array(
      'live_unique', // live cache prefix
      'admin_unique', // admin cache prefix
    ),
  );

The above would be the configuration of your live instance. Then, on your
administrative instance you would flip the keys:

  // Administrative instance.
  $conf['memcache_key_prefix'] = array(
    'default' => array(
      'admin_unique', // admin cache prefix
      'live_unique', // live cache prefix
    ),
  );

## EXPERIMENTAL - ALTERNATIVE SERIALIZE ##

This is a new experimental feature added to the memcache module in version
7.x-1.6 and should be tested carefully before utilizing in production.

To optimize how data is serialized before it is written to memcache, you can
enable either the igbinary or msgpack PECL extension. Both switch from using
PHP's own human-readable serialized data strucutres to more compact binary
formats.

No specicial configuration is required.  If both extensions are enabled,
memcache will automatically use the igbinary extension. If only one extension
is enabled, memcache will automatically use that extension.

You can optionally specify which extension is used by adding one of the
following to your settings.php:

  // Force memcache to use PHP's core serialize functions
  $conf['memcache_serialize'] = 'serialize';

  // Force memcache to use the igbinary serialize functions (if available)
  $conf['memcache_serialize'] = 'igbinary';

  // Force memcache to use the msgpack serialize functions (if available)
  $conf['memcache_serialize'] = 'msgpack';

To review which serialize function is being used, enable the memcache_admin
module and visit admin/reports/memcache.

IGBINARY:

The igbinary project is maintained on GitHub:
 - https://github.com/phadej/igbinary

The official igbinary PECL extension can be found at:
 - https://pecl.php.net/package/igbinary

Version 2.0.1 or greater is recommended.

MSGPACK:

The msgpack project is maintained at:
  - https://msgpack.org

The official msgpack PECL extension can be found at:
  - https://pecl.php.net/package/msgpack

Version 2.0.2 or greater is recommended.

## MAXIMUM LENGTHS ##

If the length of your prefix + key + bin combine to be more than 250 characters,
they will be automatically hashed. Memcache only supports key lengths up to 250
bytes. You can optionally configure the hashing algorithm used, however sha1 was
selected as the default because it performs quickly with minimal collisions.

Visit http://www.php.net/manual/en/function.hash-algos.php to learn more about
which hash algorithms are available.

$conf['memcache_key_hash_algorithm'] = 'sha1';

You can also tune the maximum key length BUT BE AWARE this doesn't affect
memcached's server-side limitations -- this value is primarily exposed to allow
you to further shrink the length of keys to optimize network performance.
Specifying a length larger than 250 will almost certainly lead to problems
unless you know what you're doing.

$conf['memcache_key_max_length'] = 250;

By default, the memcached server can store objects up to 1 MiB in size. It's
possible to increase the memcached page size to support larger objects, but this
can also lead to wasted memory. Alternatively, the Drupal memcache module splits
these large objects into smaller pieces. By default, the Drupal memcache module
splits objects into 1 MiB sized pieces. You can modify this with the following
tunable to match any special server configuration you may have. NOTE: Increasing
this value without making changes to your memcached server can result in
failures to cache large items.

(Note: 1 MiB = 1024 x 1024 = 1048576.)

$conf['memcache_data_max_length'] = 1048576;

It is generally undesirable to store excessively large objects in memcache as
this can result in a performance penalty. Because of this, by default the Drupal
memcache module logs any time an object is cached that has to be split into
multiple pieces. If this is generating too many watchdog logs, you should first
understand why these objects are so large and if anything can be done to make
them smaller. If you determine that the large size is valid and is not causing
you any unnecessary performance penalty, you can tune the following variable to
minimize or disable this logging. Set the value to a positive integer to only
log when an object is split into this many or more pieces. For example, if
memcache_data_max_length is set to 1048576 and memcache_log_data_pieces is set
to 5, watchdog logs will only be written when an object is split into 5 or more
pieces (objects >4 MiB in size). Or, to to completely disable logging set
memcache_log_data_pieces to 0 or FALSE.

$conf['memcache_log_data_pieces'] = 2;

## MULTIPLE SERVERS ##

To use this module with multiple memcached servers, it is important that you set
the hash strategy to consistent. This is controlled in the PHP extension, not
the Drupal module.

If using PECL memcache:
Edit /etc/php.d/memcache.ini (path may changed based on package/distribution)
and set the following:
memcache.hash_strategy=consistent

You need to reload apache httpd after making that change.

If using PECL memcached:
Memcached options can be controlled in settings.php.  The following setting is
needed:
$conf['memcache_options'] = array(
  Memcached::OPT_DISTRIBUTION => Memcached::DISTRIBUTION_CONSISTENT,
);

## DEBUG LOGGING ##

You can optionally enable debug logging by adding the following to your
settings.php:
  $conf['memcache_debug_log'] = '/path/to/file.log';

By default, only the following memcache actions are logged: 'set', 'add',
'delete', and 'flush'. If you'd like to also log 'get' and 'getMulti' actions,
enble verbose logging:
  $conf['memcache_debug_verbose'] = TRUE;

This file needs to be writable by the web server (and/or by drush) or you will
see lots of watchdog errors. You are responsible for ensuring that the debug log
doesn't get too large. By default, enabling debug logging will write logs
looking something like:

  1484719570|add|semaphore|semaphore-memcache_system_list%3Acache_bootstrap|1
  1484719570|set|cache_bootstrap|cache_bootstrap-system_list|1
  1484719570|delete|semaphore|semaphore-memcache_system_list%3Acache_bootstrap|1

The default log format is pipe delineated, containing the following fields:
  timestamp|action|bin|cid|return code

You can specify a custom log format by setting the memcache_debug_log_format
variable. Supported variables that will be replaced in your format are:
'!timestamp', '!action', '!bin', '!cid', and '!rc'.
For example, the default log format (note that it includes a new line at the
end) is:
  $conf['memcache_debug_log_format'] = "!timestamp|!action|!bin|!cid|!rc\n";

You can change the timestamp format by specifying a PHP date() format string in
the memcache_debug_time_format variable. PHP date() formats are documented at
http://php.net/manual/en/function.date.php. By default timestamps are written as
a unix timestamp. For example:
  $conf['memcache_debug_time_format'] = 'U';

## TROUBLESHOOTING ##

PROBLEM:
 Error:
  Failed to load required file memcache/dmemcache.inc
 Or:
 cache_backends not properly configured in settings.php, failed to load
 required file memcache.inc

SOLUTION:
You need to enable memcache in settings.php. Search for "Example 1" above
for a basic configuration example.

PROBLEM:
 Error:
  PECL !extension version %version is unsupported. Please update to
  %recommended or newer.

SOLUTION:
Upgrade to the latest available PECL extension release. Older PECL extensions
have known bugs and cause a variety of problems when using the memcache module.

PROBLEM:
 Error:
  Failed to connect to memcached server instance at <IP ADDRESS>.

SOLUTION:
Verify that the memcached daemon is running at the specified IP and PORT. To
debug you can try to telnet directly to the memcache server from your web
servers, example:
   telnet localhost 11211

PROBLEM:
 Error:
  Failed to store to then retrieve data from memcache.

SOLUTION:
Carefully review your settings.php configuration against the above
documentation. This error simply does a cache_set followed by a cache_get
and confirms that what is written to the cache can then be read back again.
This test was added in the 7.x-1.1 release.

The following code is what performs this test -- you can wrap this in a <?php
tag and execute as a script with 'drush scr' to perform further debugging.

        $cid = 'memcache_requirements_test';
        $value = 'OK';
        // Temporarily store a test value in memcache.
        cache_set($cid, $value);
        // Retreive the test value from memcache.
        $data = cache_get($cid);
        if (!isset($data->data) || $data->data !== $value) {
          echo t('Failed to store to then retrieve data from memcache.');
        }
        else {
          // Test a delete as well.
          cache_clear_all($cid, 'cache');
        }

PROBLEM:
 Error:
  Unexpected failure when testing memcache configuration.

SOLUTION:
Be sure the memcache module is properly installed, and that your settings.php
configuration is correct. This error means an exception was thrown when
attempting to write to and then read from memcache.

PROBLEM:
 Error:
  Failed to set key: Failed to set key: cache_page-......

SOLUTION:
Upgrade your PECL library to PECL package (2.2.1) (or higher).

WARNING:
Zlib compression at the php.ini level and Memcache conflict.
See http://drupal.org/node/273824

## MEMCACHE ADMIN ##

A module offering a UI for memcache is included. It provides aggregated and
per-page statistics for memcache.

## Memcached PECL Extension Support

We also support the Memcached PECL extension. This extension backends
to libmemcached and allows you to use some of the newer advanced features in
memcached 1.4.

NOTE: It is important to realize that the memcache php.ini options do not impact
the memcached extension, this new extension doesn't read in options that way.
Instead, it takes options directly from Drupal. Because of this, you must
configure memcached in settings.php. Please look here for possible options:

http://us2.php.net/manual/en/memcached.constants.php

An example configuration block is below, this block also illustrates our
default options (selected through performance testing). These options will be
set unless overridden in settings.php.

  $conf['memcache_options'] = array(
    Memcached::OPT_COMPRESSION => FALSE,
    Memcached::OPT_DISTRIBUTION => Memcached::DISTRIBUTION_CONSISTENT,
  );

These are as follows:

 * Turn off compression, as this takes more CPU cycles than it's worth for most
   users
 * Turn on consistent distribution, which allows you to add/remove servers
   easily

Other options you could experiment with:
 + Memcached::OPT_BINARY_PROTOCOL => TRUE,
    * This enables the Memcache binary protocol (only available in Memcached
      1.4 and later). Note that some users have reported SLOWER performance
      with this feature enabled. It should only be enabled on extremely high
      traffic networks where memcache network traffic is a bottleneck.
      Additional reading about the binary protocol:
        http://code.google.com/p/memcached/wiki/MemcacheBinaryProtocol

 + Memcached::OPT_TCP_NODELAY => TRUE,
    * This enables the no-delay feature for connecting sockets; it's been
      reported that this can speed up the Binary protocol (see above). This
      tells the TCP stack to send packets immediately and without waiting for
      a full payload, reducing per-packet network latency (disabling "Nagling").

It's possible to enable SASL authentication as documented here:
  http://php.net/manual/en/memcached.setsaslauthdata.php
  https://code.google.com/p/memcached/wiki/SASLHowto

SASL authentication requires a memcached server with SASL support (version 1.4.3
or greater built with --enable-sasl and started with the -S flag) and the PECL
memcached client version 2.0.0 or greater also built with SASL support. Once
these requirements are satisfied you can then enable SASL support in the Drupal
memcache module by enabling the binary protocol and setting
memcache_sasl_username and memcache_sasl_password in settings.php. For example:

  $conf['memcache_options'] = array(
    Memcached::OPT_BINARY_PROTOCOL => TRUE,
  );
  $conf['memcache_sasl_username'] = 'yourSASLUsername';
  $conf['memcache_sasl_password'] = 'yourSASLPassword';

## Amazon Elasticache

You can use the Drupal Memcache module to talk with Amazon Elasticache, but to
enable Automatic Discovery you must use Amazon's forked version of the PECL
Memcached extension with Dynamic Client Mode enabled.

Their PECL Memcached fork is maintained on GitHub:
 - https://github.com/awslabs/aws-elasticache-cluster-client-memcached-for-php

If you are using PHP 7 you need to select the php7 branch of their project.

Once the extension is installed, you can enable Dynamic Client Mode as follows:

  $conf['memcache_options'] = array(
    Memcached::OPT_DISTRIBUTION => Memcached::DISTRIBUTION_CONSISTENT,
    Memcached::OPT_CLIENT_MODE  => Memcached::DYNAMIC_CLIENT_MODE,
  );

You then configure the module normally. Amazon explains:
  "If you use Automatic Discovery, you can use the cluster's Configuration
   Endpoint to configure your Memcached client."

The Configuration Endpoint must have 'cfg' in the name or it won't work. Further
documentation can be found here:
http://docs.aws.amazon.com/AmazonElastiCache/latest/UserGuide/Endpoints.html

If you don't want to use Automatic Discovery you don't need to install the
forked PECL extension, Amazon explains:
  "If you don't use Automatic Discovery, you must configure your client to use
   the individual node endpoints for reads and writes. You must also keep track
   of them as you add and remove nodes."
