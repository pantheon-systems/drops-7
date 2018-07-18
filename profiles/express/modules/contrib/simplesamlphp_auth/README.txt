-- SUMMARY --

The simplesamlphp_auth module makes it possible for Drupal to support SAML for
authentication of users. The module will auto-provision user accounts into
Drupal if you want it to. It can also dynamically assign Drupal roles based on
identity attribute values.


-- PREREQUISITES --

1) You must have SimpleSAMLphp installed and configured as a working service
   point (SP) as the module uses your local SimpleSAMLphp SP for the SAML
   support. For more information on installing and configuring SimpleSAMLphp as
   an SP visit: http://www.simplesamlphp.org.

   IMPORTANT: Your SP must be configured to use something other than phpsession
   for session storage (in config/config.php set store.type => 'memcache'
   or 'sql').

   To use memcache session handling you must have memcached installed on your
   server and PHP must have the memcache extension. For more information on
   installing the memcache extension for PHP visit:
   http://www.php.net/manual/en/memcache.installation.php

   If you are on a shared host or a machine that you cannot install memcache on
   then consider using the sql handler (store.type => 'sql').


-- INSTALLATION --

Assuming the prerequisites have been met, installation of this module is just
like any other Drupal module.

1) Download the module
2) Uncompress it
3) Move it to the appropriate modules directory (usually, sites/all/modules)
4) Go to the Drupal module administration page for your site
5) Enable the module
6) Configure the module (see below)


-- CONFIGURATION --

The configuration of the module is fairly straight forward. You will need to
know the names of the attributes that your SP will be making available to the
module in order to map them into Drupal.


-- TROUBLESHOOTING --

The most common reason for things not working is the SP session storage type
is still set to phpsession.


-- CONTACT --

Current Maintainers
* Steve Moitozo (geekwisdom) http://drupal.org/user/1662
