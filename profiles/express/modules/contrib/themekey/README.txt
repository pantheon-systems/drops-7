
ThemeKey
========

Name: themekey
Authors: Markus Kalkbrenner | Cocomore AG
         Carsten Müller | Cocomore AG
         Christian Spitzlay | Cocomore AG
         Thilo Wawrzik <drupal at profix898 dot de>
Drupal: 7.x
Sponsor: Cocomore AG - http://www.cocomore.com
                     - http://drupal.cocomore.com

Description
===========

ThemeKey is meant to be a generic theme switching module. It
allows you to switch the theme for different paths and based
on object properties (e.g. node field values). It can also be
easily extended to support additional paths or properties as
exposed by other modules.

Documentation for users and developers is very sparse at the
moment. I hope to complete the docs in the next few weeks.
Thanks for your patience :)


Installation
============

1. Place whole themekey folder into your Drupal modules/ or better
   sites/x/modules/ directory.

2. Enable the ThemeKey module by navigating to
     Configuration > Modules

3. Bring up themekey configuration screens by navigating to
     Configuration > User Interface / ThemeKey


ThemeKey UI
===========

1. Enable the ThemeKey UI module by navigating to
     Configuration > Modules

2. Bring up ThemeKey configuration screens by navigating to
     Configuration > User Interface / ThemeKey > Settings > User Interface


For Developers
==============

HOOK_themekey_properties()
  Attributes
    Key:    namespace:property
    Value:  array()
            - description => Readable name of property (required)
            - validator   => Callback function to validate a rule starting with that property (optional)
                             TODO: describe validator arguments and return value 
            - file        => File that provides the validator function (optional)
            - path        => Alternative path relative to dupal's doc root to load the file (optional)
            - static      => true/false, static properties don't occur in properties drop down
                             and have fixed operator and value (optional)
            - page cache  => Level of page caching support:
                             - THEMEKEY_PAGECACHE_SUPPORTED
                             - THEMEKEY_PAGECACHE_UNSUPPORTED
                             - THEMEKEY_PAGECACHE_TIMEBASED
                             Default is THEMEKEY_PAGECACHE_UNSUPPORTED (optional)

  Maps
    Key:    none (indexed)
    Value:  array()
            - src       => Source property path (required)
            - dst       => Destination property path (required)
            - callback  => Mapping callback (required)
            - file      => File that provides the callback function (optional)
            - path      => Alternative path relative to dupal's doc root to load the file (optional)

HOOK_themekey_global()
  Global properties
    Key:    namespace:property
    Value:  property value (scalar value or array of scalar values)

HOOK_themekey_paths()
  Paths
    Key:    none (indexed)
    Value:  array()
            - path      => Router path to register (required)
            - callbacks => Load (and/or match) callback (optional)
              (the callback function can set the 'theme' element in $params array directly, which will be applied)
              Callback arguments:
              - $item:    array of elements associated with the path/callback
              - $params:  array of parameters available for load callback

