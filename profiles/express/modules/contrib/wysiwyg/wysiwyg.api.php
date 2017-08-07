<?php

/**
 * @file
 * API documentation for Wysiwyg module.
 *
 * To implement a "Drupal plugin" button, you need to write a Wysiwyg plugin:
 * - Implement hook_wysiwyg_include_directory() to register the directory
 *   containing plugin definitions.
 * - In each plugin definition file, implement hook_INCLUDE_plugin().
 * - For each plugin button, implement a JavaScript integration and an icon for
 *   the button.
 *
 * @todo Icon: Recommended size and type of image.
 *
 * For example implementations you may want to look at
 * - Image Assist (img_assist)
 * - Teaser break plugin (plugins/break; part of WYSIWYG)
 * - IMCE (imce_wysiwyg)
 */

/**
 * Return an array of native editor plugins.
 *
 * Only to be used for native (internal) editor plugins.
 *
 * @see hook_wysiwyg_include_directory()
 *
 * @param $editor
 *   The internal name of the currently processed editor.
 * @param $version
 *   The version of the currently processed editor.
 *
 * @return
 *   An associative array having internal plugin names as keys and an array of
 *   plugin meta-information as values.
 */
function hook_wysiwyg_plugin($editor, $version) {
  switch ($editor) {
    case 'tinymce':
      if ($version > 3) {
        return array(
          'myplugin' => array(
            // A URL to the plugin's homepage.
            'url' => 'http://drupal.org/project/img_assist',
            // The full path to the native editor plugin, no trailing slash.
            // Ignored when 'internal' is set to TRUE below.
            'path' => drupal_get_path('module', 'img_assist') . '/drupalimage',
            // The name of the plugin's main JavaScript file.
            // Ignored when 'internal' is set to TRUE below.
            // Default value depends on which editor the plugin is for.
            'filename' => 'editor_plugin.js',
            // A list of buttons provided by this native plugin. The key has to
            // match the corresponding JavaScript implementation. The value is
            // is displayed on the editor configuration form only.
            // CKEditor-specific note: The internal button name/key is
            // capitalized, i.e. Img_assist.
            'buttons' => array(
              'img_assist' => t('Image Assist'),
            ),
            // A list of editor extensions provided by this native plugin.
            // Extensions are not displayed as buttons and touch the editor's
            // internals, so you should know what you are doing.
            'extensions' => array(
              'imce' => t('IMCE'),
            ),
            // A list of global, native editor configuration settings to
            // override. To be used rarely and only when required.
            'options' => array(
              'file_browser_callback' => 'imceImageBrowser',
              'inline_styles' => TRUE,
            ),
            // Boolean whether the editor needs to load this plugin. When TRUE,
            // the editor will automatically load the plugin based on the 'path'
            // variable provided. If FALSE, the plugin either does not need to
            // be loaded or is already loaded by something else on the page.
            // Most plugins should define TRUE here.
            'load' => TRUE,
            // Boolean whether this plugin is a native plugin, i.e. shipped with
            // the editor. Definition must be omitted for plugins provided by
            // other modules. TRUE means 'path' and 'filename' above are ignored
            // and the plugin is instead loaded from the editor's plugin folder.
            'internal' => TRUE,
            // TinyMCE-specific: Additional HTML elements to allow in the markup.
            'extended_valid_elements' => array(
              'img[class|src|border=0|alt|title|width|height|align|name|style]',
            ),
          ),
        );
      }
      break;
  }
}

/**
 * Register a directory containing Wysiwyg plugins.
 *
 * @param $type
 *   The type of objects being collected: either 'plugins' or 'editors'.
 * @return
 *   A sub-directory of the implementing module that contains the corresponding
 *   plugin files. This directory must only contain integration files for
 *   Wysiwyg module.
 */
function hook_wysiwyg_include_directory($type) {
  switch ($type) {
    case 'plugins':
      // You can just return $type, if you place your Wysiwyg plugins into a
      // sub-directory named 'plugins'.
      return $type;
  }
}

/**
 * Define a Wysiwyg plugin.
 *
 * Supposed to be used for "Drupal plugins" (cross-editor plugins) only.
 *
 * @see hook_wysiwyg_plugin()
 *
 * Each plugin file in the specified plugin directory of a module needs to
 * define meta information about the particular plugin provided.
 * The plugin's hook implementation function name is built out of the following:
 * - 'hook': The name of the module providing the plugin.
 * - 'INCLUDE': The basename of the file containing the plugin definition.
 * - 'plugin': Static.
 *
 * For example, if your module's name is 'mymodule' and
 * mymodule_wysiwyg_include_directory() returned 'plugins' as plugin directory,
 * and this directory contains an "awesome" plugin file named 'awesome.inc', i.e.
 *   sites/all/modules/mymodule/plugins/awesome.inc
 * then the corresponding plugin hook function name is:
 *   mymodule_awesome_plugin()
 *
 * @see hook_wysiwyg_include_directory()
 *
 * @return
 *   Meta information about the buttons provided by this plugin.
 */
function hook_INCLUDE_plugin() {
  $plugins['awesome'] = array(
    // The plugin's title; defaulting to its internal name ('awesome').
    'title' => t('Awesome plugin'),
    // The (vendor) homepage of this plugin; defaults to ''.
    'vendor url' => 'http://drupal.org/project/wysiwyg',
    // The path to the button's icon; defaults to
    // '/[path-to-module]/[plugins-directory]/[plugin-name]/images'.
    'icon path' => 'path to icon',
    // The button image filename; defaults to '[plugin-name].png'.
    'icon file' => 'name of the icon file with extension',
    // The button title to display on hover.
    'icon title' => t('Do something'),
    // An alternative path to the integration JavaScript; defaults to
    // '[path-to-module]/[plugins-directory]/[plugin-name]'.
    'js path' => drupal_get_path('module', 'mymodule') . '/awesomeness',
    // An alternative filename of the integration JavaScript; defaults to
    // '[plugin-name].js'.
    'js file' => 'awesome.js',
    // An alternative path to the integration stylesheet; defaults to
    // '[path-to-module]/[plugins-directory]/[plugin-name]'.
    'css path' => drupal_get_path('module', 'mymodule') . '/awesomeness',
    // An alternative filename of the integration stylesheet; defaults to
    // '[plugin-name].css'.
    'css file' => 'awesome.css',
    // An array of settings for this button. Required, but API is still in flux.
    'settings' => array(
    ),
    // TinyMCE-specific: Additional HTML elements to allow in the markup.
    'extended_valid_elements' => array(
      'tag1[attribute1|attribute2]',
      'tag2[attribute3|attribute4]',
    ),
  );
  return $plugins;
}

/**
 * Define a Wysiwyg editor library.
 *
 * @todo Complete this documentation.
 */
function hook_INCLUDE_editor() {
  $editor['ckeditor'] = array(
    // The official, human-readable label of the editor library.
    'title' => 'CKEditor',
    // The URL to the library's homepage.
    'vendor url' => 'http://ckeditor.com',
    // The URL to the library's download page.
    'download url' => 'http://ckeditor.com/download',
    // A definition of available variants for the editor library.
    // The first defined is used by default.
    'libraries' => array(
      '' => array(
        'title' => 'Default',
        'files' => array(
          'ckeditor.js' => array('preprocess' => FALSE),
        ),
      ),
      'src' => array(
        'title' => 'Source',
        'files' => array(
          'ckeditor_source.js' => array('preprocess' => FALSE),
        ),
      ),
    ),
    // (optional) A callback to invoke to return additional notes for installing
    // the editor library in the administrative list/overview.
    'install note callback' => 'wysiwyg_ckeditor_install_note',
    // The minimum and maximum versions the implementation has been tested with.
    // Users will be notified if installing a version not within this range.
    'verified version range' => array('1.2.3', '3.4.5'),
    // (optional) A callback to perform migrations of the settings stored in a
    // profile when a library change has been detected. Takes a reference to a
    // settings object, the processed editor definition, the profile version and
    // the installed library version. Migrations should be performed in the
    // order changes were introduced by library versions, and the last version
    // migrated to should be returned, or FALSE if no migration was possible.
    // The returned version should be less than or equal to the highest version
    // ( and >= the lowest version) defined in 'verified version range' and
    // be as close as possible to, without passing, the installed version.
    'migrate settings callback' => 'wysiwyg_ckeditor_migrate_settings',
    // A callback to determine the library's version.
    'version callback' => 'wysiwyg_ckeditor_version',
    // A callback to return available themes/skins for the editor library.
    'themes callback' => 'wysiwyg_ckeditor_themes',
    // (optional) A callback to perform editor-specific adjustments or
    // enhancements for the administrative editor profile settings form.
    'settings form callback' => 'wysiwyg_ckeditor_settings_form',
    // (optional) A callback to return an initialization JavaScript snippet for
    // this editor library, loaded before the actual library files. The returned
    // JavaScript is executed as inline script in a primitive environment,
    // before the DOM is loaded; typically used to prime a base path and other
    // global window variables for the editor library before it is loaded.
    // All implementations should verbosely document what they are doing and
    // why that is required.
    'init callback' => 'wysiwyg_ckeditor_init',
    // A callback to convert administrative profile/editor settings into
    // JavaScript settings.
    'settings callback' => 'wysiwyg_ckeditor_settings',
    // A callback to supply definitions of available editor plugins.
    'plugin callback' => 'wysiwyg_ckeditor_plugins',
    // A callback to supply global metadata for a single native external plugin.
    'plugin meta callback' => 'wysiwyg_ckeditor_plugin_meta',
    // A callback to convert administrative plugin settings for an editor
    // profile into JavaScript settings per profile.
    'plugin settings callback' => 'wysiwyg_ckeditor_plugin_settings',
    // (optional) Defines the proxy plugin that handles plugins provided by
    // Drupal modules, which work in all editors that support proxy plugins.
    'proxy plugin' => array(
      'drupal' => array(
        'load' => TRUE,
        'proxy' => TRUE,
      ),
    ),
    // (optional) A callback to convert proxy plugin settings into JavaScript
    // settings.
    'proxy plugin settings callback' => 'wysiwyg_ckeditor_proxy_plugin_settings',
    // Defines the list of supported (minimum) versions of the editor library,
    // and the respective Drupal integration files to load.
    'versions' => array(
      '3.0.0.3665' => array(
        'js files' => array('ckeditor-3.0.js'),
      ),
    ),
  );
  return $editor;
}

/**
 * Alter editor definitions defined by other modules.
 *
 * @param array $editors
 *   The Editors to alter.
 */
function hook_wysiwyg_editor_alter(&$editors) {
  $editors['editor']['version callback'] = 'my_own_version_callback';
}

/**
 * Act on editor profile settings.
 *
 * This hook is invoked from wysiwyg_get_editor_config() after the JavaScript
 * settings have been generated for an editor profile and before the settings
 * are added to the page. The settings may be customized or enhanced; typically
 * with options that cannot be controlled through Wysiwyg module's
 * administrative UI currently.
 *
 * Modules implementing this hook to enforce settings that can also be
 * controlled through the UI should also implement
 * hook_form_wysiwyg_profile_form_alter() to adjust or at least indicate on the
 * editor profile configuration form that certain/affected settings cannot be
 * changed.
 *
 * @param $settings
 *   An associative array of JavaScript settings to pass to the editor.
 * @param $context
 *   An associative array containing additional context information:
 *   - editor: The plugin definition array of the editor.
 *   - profile: The editor profile object, as loaded from the database.
 *   - theme: The name of the editor theme/skin.
 */
function hook_wysiwyg_editor_settings_alter(&$settings, $context) {
  // Each editor has its own collection of native settings that may be extended
  // or overridden. Please consult the respective official vendor documentation
  // for details.
  if ($context['profile']->editor == 'tinymce') {
    // Supported values to JSON data types.
    $settings['cleanup_on_startup'] = TRUE;
    // Function references (callbacks) need special care.
    // @see wysiwyg_wrap_js_callback()
    $settings['file_browser_callback'] = wysiwyg_wrap_js_callback('myFileBrowserCallback');
    // Regular Expressions need special care.
    // @see wysiwyg_wrap_js_regexp()
    $settings['stylesheetParser_skipSelectors'] = wysiwyg_wrap_js_regexp('(^body\.|^caption\.|\.high|^\.)', 'i');
  }
}

/**
 * Act on stylesheets used in WYSIWYG mode.
 *
 * This hook acts like a pre-render callback to the style element normally
 * output in the document header. It is invoked before Core has
 * sorted/grouped/aggregated stylesheets and changes made here will only have
 * an effect on the stylesheets used in an editor's WYSIWYG mode.
 * Wysiwyg will only keep items if their type is 'file' or 'inline' and only if
 * they are in the group CSS_THEME.
 *
 * This hook may be invoked several times in a row with slightly different or
 * altered stylesheets if something like Color module is used by a theme.
 * Wysiwyg will cache the final list of stylesheets so this hook will only be
 * called while the cache is being rebuilt.
 *
 * Messages set in this hook will not be displayed because the processing is
 * done in an internal HTTP request and the page output is ignored.
 *
 * @param $elements
 *   The style element which will be rendered. Added stylesheets are found in
 *   $element['#items']['path/to/stylesheet.css'].
 * @param $context
 *   An array with the following keys:
 *   - theme: The name of the theme which was used when the list of stylesheets
 *     was generated.
 */
function hook_wysiwyg_editor_styles_alter(&$element, $context) {
  if ($context['theme'] == 'alpha') {
    unset($element['#items']['sites/all/themes/omega/alpha/css/alpha-debug.css']);
  }
}
