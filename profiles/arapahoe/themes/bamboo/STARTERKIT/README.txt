BUILD A SUB-THEME WITH Bamboo
----------------------

The base Bamboo theme is designed to be easily extended by a sub-theme. You
shouldn't modify any of the CSS or PHP files in the bamboo/ folder unless you've
specified to use local.css; It's preferred instead you should create a sub-theme
of Bamboo which is located in a folder outside of the root Bamboo/ folder.
The examples below assume Bamboo and your sub-theme will be
installed in sites/all/themes/, but any valid theme directory is acceptable
(read the sites/default/default.settings.php for more info.)

  Why? To learn why you shouldn't modify any of the files in the bamboo/ folder,
  see https://drupal.org/node/245802


*** IMPORTANT NOTE ***
*
* In Drupal 7, the theme system caches which template files and which theme
* functions should be called. This means that if you add a new theme,
* preprocess or process function to your template.php file or add a new template
* (.tpl.php) file to your sub-theme, you will need to rebuild the "theme
* registry." See https://drupal.org/node/173880#theme-registry
*
* Drupal 7 also stores a cache of the data in .info files. If you modify any
* lines in your sub-theme's .info file, you MUST refresh Drupal 7's cache by
* simply visiting the Appearance page at admin/appearance.
*

 1. Setup the location for your new sub-theme.

    Copy the STARTERKIT folder out of the Bamboo/ folder and rename it to be your
    new sub-theme. IMPORTANT: The name of your sub-theme must start with an
    alphabetic character and can only contain lowercase letters, numbers and
    underscores.

    For example, copy the sites/all/themes/Bamboo/STARTERKIT folder and rename it
    as sites/all/themes/bamboo_subtheme.

      Why? Each theme should reside in its own folder. To make it easier to
      upgrade Bamboo, sub-themes should reside in a folder separate from the base
      theme.

 2. Setup the basic information for your sub-theme.

    In your new sub-theme folder, rename the STARTERKIT.info.txt file to include
    the name of your new sub-theme and remove the ".txt" extension. Then edit
    the .info file by editing the name and description field.

    For example, rename the STARTERKIT.info file to bamboo_subtheme.info in your
    newly created sub-theme folder. bamboo_subtheme/bamboo_subtheme.info

    Edit the bamboo_subtheme.info file and change "name = Bamboo Sub-theme
    Starter Kit" to "name = Bamboo Sub-theme" and "description = Read..."
    to "description = A Bamboo sub-theme".

      Why? The .info file describes the basic things about your theme: its
      name, description, features, template regions, CSS files, and JavaScript
      files. See the Drupal 7 Theme Guide for more info:
      https://drupal.org/node/171205

    Then, visit your site's Appearance page at admin/appearance to refresh
    Drupal 7's cache of .info file data.


 3. Set your website's default theme.

    Log in as an administrator on your Drupal site, go to the Appearance page at
    admin/appearance and click the "Enable and set default" link next to your
    new sub-theme.


Optional steps:

 4. Modify the markup in Bamboo core's template files.

    If you decide you want to modify any of the .tpl.php template files in the
    Bamboo folder, copy them to your sub-theme's folder before making any changes.
    And then rebuild the theme registry.

    For example, copy bamboo/templates/page.tpl.php
    to bamboo_subtheme/templates/page.tpl.php.


 5. Further extend your sub-theme.

    Discover further ways to extend your sub-theme by reading
    Drupal 7's Theme Guide online at:
   https://drupal.org/theme-guide
