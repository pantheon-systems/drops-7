CONTENTS OF THIS FILE
---------------------

 * Introduction
 * Requirements
 * Installation
 * Configuration
 * Maintainers


INTRODUCTION
------------

The Icon Select module allows the upload of font libraries into a central
repository, then exposes them as an option in a custom field type. It allows
control of available icons on either a font-wide basis or based on the
specific field instance.

 * For a full description of the module, visit the project page:
   https://www.drupal.org/sandbox/wolffereast/2319993

 * To submit bug reports and feature suggestions, or to track changes:
   https://www.drupal.org/project/issues/2319993


REQUIREMENTS
------------

No special requirements


INSTALLATION
------------

 * Install as you would normally install a contributed Drupal module. See:
   https://drupal.org/documentation/install/modules-themes/modules-7
   for further information.


CONFIGURATION
-------------

 * Add a new font:
 *  Configuration >> Content authoring >> Font Icon Select Options >>
 *  Upload New Library

   - All 4 file types (.eot, .svg, .ttf, and .woff) are required.

 * Configure an existing Font:
 *  Configuration >> Content authoring >> Font Icon Select Options

   - edit blacklist

     Select which Icons should be removed from all field instances.

   - edit font

     Upload different font files or change the Human Readable Name

 * Add an Icon Select field

   - Once you have uploaded a font library you can add an field of type Icon
     Select. Select the desired Font from the dropdown.

   - The field settings include instance specific Whitelist/Blacklist options.
     The global Blacklist selections are already removed from the available
     icons. As the names imply, a whitelist will allow only the selected
     options, while a blacklist allows all unselected icons.

MAINTAINERS
-----------
Current maintainers:
 * Stephen Wolff (wolffereast) - https://drupal.org/u/wolffereast

This project has been sponsored by:
 * Adworkshop
   An employee-owned integrated marketing agency providing strategic consulting
   and creative marketing solutions that help clients solve their evolving
   business challenges. Visit https://www.adworkshop.com for more information.
