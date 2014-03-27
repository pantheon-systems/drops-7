<?php
/**
 * @file
 * common.inc
 *
 * Contains common functionality for the entire base theme.
 */

/**
 * Return information from the .info file of a theme (and possible base themes).
 *
 * @param string $theme_key
 *   The machine name of the theme.
 * @param string $key
 *   The key name of the item to return from the .info file. This value can
 *   include "][" to automatically attempt to traverse any arrays.
 * @param bool $base_themes
 *   Recursively search base themes, defaults to TRUE.
 *
 * @return string|array|false
 *   A string or array depending on the type of value and if a base theme also
 *   contains the same $key, FALSE if no $key is found.
 */
function bootstrap_get_theme_info($theme_key = NULL, $key = NULL, $base_themes = TRUE) {
  // If no $theme_key is given, use the current theme if we can determine it.
  if (!isset($theme_key)) {
    $theme_key = !empty($GLOBALS['theme_key']) ? $GLOBALS['theme_key'] : FALSE;
  }
  if ($theme_key) {
    $themes = list_themes();
    if (!empty($themes[$theme_key])) {
      $theme = $themes[$theme_key];
      // If a key name was specified, return just that array.
      if ($key) {
        $value = FALSE;
        // Recursively add base theme values.
        if ($base_themes && isset($theme->base_themes)) {
          foreach (array_keys($theme->base_themes) as $base_theme) {
            $value = bootstrap_get_theme_info($base_theme, $key);
          }
        }
        if (!empty($themes[$theme_key])) {
          $info = $themes[$theme_key]->info;
          // Allow array traversal.
          $keys = explode('][', $key);
          foreach ($keys as $parent) {
            if (isset($info[$parent])) {
              $info = $info[$parent];
            }
            else {
              $info = FALSE;
            }
          }
          if (is_array($value)) {
            if (!empty($info)) {
              if (!is_array($info)) {
                $info = array($info);
              }
              $value = drupal_array_merge_deep($value, $info);
            }
          }
          else {
            if (!empty($info)) {
              if (empty($value)) {
                $value = $info;
              }
              else {
                if (!is_array($value)) {
                  $value = array($value);
                }
                if (!is_array($info)) {
                  $info = array($info);
                }
                $value = drupal_array_merge_deep($value, $info);
              }
            }
          }
        }
        return $value;
      }
      // If no info $key was specified, just return the entire info array.
      return $theme->info;
    }
  }
  return FALSE;
}

/**
 * Helper function for including theme files.
 *
 * @param string $theme
 *   Name of the theme to use for base path.
 * @param string $path
 *   Path relative to $theme.
 */
function bootstrap_include($theme, $path) {
  static $themes = array();
  if (!isset($themes[$theme])) {
    $themes[$theme] = drupal_get_path('theme', $theme);
  }
  if ($themes[$theme] && ($file = DRUPAL_ROOT . '/' . $themes[$theme] . '/' . $path) && file_exists($file)) {
    include_once $file;
  }
}

/**
 * Helper function for returning whether a theme is Bootstrap based.
 *
 * @param string $theme_key
 *   The machine name of the theme to check, if not set the active theme name
 *   will be used.
 * @param bool $include_theme_key
 *   Whether to append the returned list with $theme_key.
 *
 * @return array
 *   An indexed array of base themes.
 */
function _bootstrap_get_base_themes($theme_key = NULL, $include_theme_key = FALSE) {
  static $themes;
  if (!isset($theme_key)) {
    $theme_key = $GLOBALS['theme_key'];
  }
  if (!isset($themes[$theme_key])) {
    $themes[$theme_key] = array_unique(array_filter((array) bootstrap_get_theme_info($theme_key, 'base theme')));
  }
  if ($include_theme_key) {
    $themes[$theme_key][] = $theme_key;
  }
  return $themes[$theme_key];
}

/**
 * Helper function that wraps file_scan_directory().
 *
 * Finds all files that match a given mask in a given directory and then caches
 * the results. A general site cache clear will force new scans to be initiated
 * for already cached directories.
 *
 * @param string $dir
 *   The base directory or URI to scan, without trailing slash.
 * @param string $mask
 *   The preg_match() regular expression of the files to find.
 * @param array $options
 *   Additional options to pass to file_scan_directory().
 *
 * @return array
 *   An associative array (keyed on the chosen key) of objects with 'uri',
 *   'filename', and 'name' members corresponding to the matching files.
 *
 * @see file_scan_directory()
 */
function _bootstrap_file_scan_directory($dir, $mask, array $options = array()) {
  // Retrieve cached data.
  $cid = 'theme_registry:bootstrap:files';
  $files = array();
  if ($cache = cache_get($cid)) {
    $files = $cache->data;
  }
  // Generate a unique hash for all parameters passed as a change in any of
  // them would return different results.
  $hash = drupal_hash_base64(serialize(func_get_args()));
  if (!isset($files[$hash])) {
    $files[$hash] = file_scan_directory($dir, $mask, $options);
    cache_set($cid, $files);
  }
  return $files[$hash];
}

/**
 * Helper function for returning an array of Bootstrap Glyphicons.
 */
function _bootstrap_glyphicons() {
  static $icons = array(
    // Class => Name.
    'glyphicon-adjust' => 'adjust',
    'glyphicon-align-center' => 'align-center',
    'glyphicon-align-justify' => 'align-justify',
    'glyphicon-align-left' => 'align-left',
    'glyphicon-align-right' => 'align-right',
    'glyphicon-arrow-down' => 'arrow-down',
    'glyphicon-arrow-left' => 'arrow-left',
    'glyphicon-arrow-right' => 'arrow-right',
    'glyphicon-arrow-up' => 'arrow-up',
    'glyphicon-asterisk' => 'asterisk',
    'glyphicon-backward' => 'backward',
    'glyphicon-ban-circle' => 'ban-circle',
    'glyphicon-barcode' => 'barcode',
    'glyphicon-bell' => 'bell',
    'glyphicon-bold' => 'bold',
    'glyphicon-book' => 'book',
    'glyphicon-bookmark' => 'bookmark',
    'glyphicon-briefcase' => 'briefcase',
    'glyphicon-bullhorn' => 'bullhorn',
    'glyphicon-calendar' => 'calendar',
    'glyphicon-camera' => 'camera',
    'glyphicon-certificate' => 'certificate',
    'glyphicon-check' => 'check',
    'glyphicon-chevron-down' => 'chevron-down',
    'glyphicon-chevron-left' => 'chevron-left',
    'glyphicon-chevron-right' => 'chevron-right',
    'glyphicon-chevron-up' => 'chevron-up',
    'glyphicon-circle-arrow-down' => 'circle-arrow-down',
    'glyphicon-circle-arrow-left' => 'circle-arrow-left',
    'glyphicon-circle-arrow-right' => 'circle-arrow-right',
    'glyphicon-circle-arrow-up' => 'circle-arrow-up',
    'glyphicon-cloud' => 'cloud',
    'glyphicon-cloud-download' => 'cloud-download',
    'glyphicon-cloud-upload' => 'cloud-upload',
    'glyphicon-cog' => 'cog',
    'glyphicon-collapse-down' => 'collapse-down',
    'glyphicon-collapse-up' => 'collapse-up',
    'glyphicon-comment' => 'comment',
    'glyphicon-compressed' => 'compressed',
    'glyphicon-copyright-mark' => 'copyright-mark',
    'glyphicon-credit-card' => 'credit-card',
    'glyphicon-cutlery' => 'cutlery',
    'glyphicon-dashboard' => 'dashboard',
    'glyphicon-download' => 'download',
    'glyphicon-download-alt' => 'download-alt',
    'glyphicon-earphone' => 'earphone',
    'glyphicon-edit' => 'edit',
    'glyphicon-eject' => 'eject',
    'glyphicon-envelope' => 'envelope',
    'glyphicon-euro' => 'euro',
    'glyphicon-exclamation-sign' => 'exclamation-sign',
    'glyphicon-expand' => 'expand',
    'glyphicon-export' => 'export',
    'glyphicon-eye-close' => 'eye-close',
    'glyphicon-eye-open' => 'eye-open',
    'glyphicon-facetime-video' => 'facetime-video',
    'glyphicon-fast-backward' => 'fast-backward',
    'glyphicon-fast-forward' => 'fast-forward',
    'glyphicon-file' => 'file',
    'glyphicon-film' => 'film',
    'glyphicon-filter' => 'filter',
    'glyphicon-fire' => 'fire',
    'glyphicon-flag' => 'flag',
    'glyphicon-flash' => 'flash',
    'glyphicon-floppy-disk' => 'floppy-disk',
    'glyphicon-floppy-open' => 'floppy-open',
    'glyphicon-floppy-remove' => 'floppy-remove',
    'glyphicon-floppy-save' => 'floppy-save',
    'glyphicon-floppy-saved' => 'floppy-saved',
    'glyphicon-folder-close' => 'folder-close',
    'glyphicon-folder-open' => 'folder-open',
    'glyphicon-font' => 'font',
    'glyphicon-forward' => 'forward',
    'glyphicon-fullscreen' => 'fullscreen',
    'glyphicon-gbp' => 'gbp',
    'glyphicon-gift' => 'gift',
    'glyphicon-glass' => 'glass',
    'glyphicon-globe' => 'globe',
    'glyphicon-hand-down' => 'hand-down',
    'glyphicon-hand-left' => 'hand-left',
    'glyphicon-hand-right' => 'hand-right',
    'glyphicon-hand-up' => 'hand-up',
    'glyphicon-hd-video' => 'hd-video',
    'glyphicon-hdd' => 'hdd',
    'glyphicon-header' => 'header',
    'glyphicon-headphones' => 'headphones',
    'glyphicon-heart' => 'heart',
    'glyphicon-heart-empty' => 'heart-empty',
    'glyphicon-home' => 'home',
    'glyphicon-import' => 'import',
    'glyphicon-inbox' => 'inbox',
    'glyphicon-indent-left' => 'indent-left',
    'glyphicon-indent-right' => 'indent-right',
    'glyphicon-info-sign' => 'info-sign',
    'glyphicon-italic' => 'italic',
    'glyphicon-leaf' => 'leaf',
    'glyphicon-link' => 'link',
    'glyphicon-list' => 'list',
    'glyphicon-list-alt' => 'list-alt',
    'glyphicon-lock' => 'lock',
    'glyphicon-log-in' => 'log-in',
    'glyphicon-log-out' => 'log-out',
    'glyphicon-magnet' => 'magnet',
    'glyphicon-map-marker' => 'map-marker',
    'glyphicon-minus' => 'minus',
    'glyphicon-minus-sign' => 'minus-sign',
    'glyphicon-move' => 'move',
    'glyphicon-music' => 'music',
    'glyphicon-new-window' => 'new-window',
    'glyphicon-off' => 'off',
    'glyphicon-ok' => 'ok',
    'glyphicon-ok-circle' => 'ok-circle',
    'glyphicon-ok-sign' => 'ok-sign',
    'glyphicon-open' => 'open',
    'glyphicon-paperclip' => 'paperclip',
    'glyphicon-pause' => 'pause',
    'glyphicon-pencil' => 'pencil',
    'glyphicon-phone' => 'phone',
    'glyphicon-phone-alt' => 'phone-alt',
    'glyphicon-picture' => 'picture',
    'glyphicon-plane' => 'plane',
    'glyphicon-play' => 'play',
    'glyphicon-play-circle' => 'play-circle',
    'glyphicon-plus' => 'plus',
    'glyphicon-plus-sign' => 'plus-sign',
    'glyphicon-print' => 'print',
    'glyphicon-pushpin' => 'pushpin',
    'glyphicon-qrcode' => 'qrcode',
    'glyphicon-question-sign' => 'question-sign',
    'glyphicon-random' => 'random',
    'glyphicon-record' => 'record',
    'glyphicon-refresh' => 'refresh',
    'glyphicon-registration-mark' => 'registration-mark',
    'glyphicon-remove' => 'remove',
    'glyphicon-remove-circle' => 'remove-circle',
    'glyphicon-remove-sign' => 'remove-sign',
    'glyphicon-repeat' => 'repeat',
    'glyphicon-resize-full' => 'resize-full',
    'glyphicon-resize-horizontal' => 'resize-horizontal',
    'glyphicon-resize-small' => 'resize-small',
    'glyphicon-resize-vertical' => 'resize-vertical',
    'glyphicon-retweet' => 'retweet',
    'glyphicon-road' => 'road',
    'glyphicon-save' => 'save',
    'glyphicon-saved' => 'saved',
    'glyphicon-screenshot' => 'screenshot',
    'glyphicon-sd-video' => 'sd-video',
    'glyphicon-search' => 'search',
    'glyphicon-send' => 'send',
    'glyphicon-share' => 'share',
    'glyphicon-share-alt' => 'share-alt',
    'glyphicon-shopping-cart' => 'shopping-cart',
    'glyphicon-signal' => 'signal',
    'glyphicon-sort' => 'sort',
    'glyphicon-sort-by-alphabet' => 'sort-by-alphabet',
    'glyphicon-sort-by-alphabet-alt' => 'sort-by-alphabet-alt',
    'glyphicon-sort-by-attributes' => 'sort-by-attributes',
    'glyphicon-sort-by-attributes-alt' => 'sort-by-attributes-alt',
    'glyphicon-sort-by-order' => 'sort-by-order',
    'glyphicon-sort-by-order-alt' => 'sort-by-order-alt',
    'glyphicon-sound-5-1' => 'sound-5-1',
    'glyphicon-sound-6-1' => 'sound-6-1',
    'glyphicon-sound-7-1' => 'sound-7-1',
    'glyphicon-sound-dolby' => 'sound-dolby',
    'glyphicon-sound-stereo' => 'sound-stereo',
    'glyphicon-star' => 'star',
    'glyphicon-star-empty' => 'star-empty',
    'glyphicon-stats' => 'stats',
    'glyphicon-step-backward' => 'step-backward',
    'glyphicon-step-forward' => 'step-forward',
    'glyphicon-stop' => 'stop',
    'glyphicon-subtitles' => 'subtitles',
    'glyphicon-tag' => 'tag',
    'glyphicon-tags' => 'tags',
    'glyphicon-tasks' => 'tasks',
    'glyphicon-text-height' => 'text-height',
    'glyphicon-text-width' => 'text-width',
    'glyphicon-th' => 'th',
    'glyphicon-th-large' => 'th-large',
    'glyphicon-th-list' => 'th-list',
    'glyphicon-thumbs-down' => 'thumbs-down',
    'glyphicon-thumbs-up' => 'thumbs-up',
    'glyphicon-time' => 'time',
    'glyphicon-tint' => 'tint',
    'glyphicon-tower' => 'tower',
    'glyphicon-transfer' => 'transfer',
    'glyphicon-trash' => 'trash',
    'glyphicon-tree-conifer' => 'tree-conifer',
    'glyphicon-tree-deciduous' => 'tree-deciduous',
    'glyphicon-unchecked' => 'unchecked',
    'glyphicon-upload' => 'upload',
    'glyphicon-usd' => 'usd',
    'glyphicon-user' => 'user',
    'glyphicon-volume-down' => 'volume-down',
    'glyphicon-volume-off' => 'volume-off',
    'glyphicon-volume-up' => 'volume-up',
    'glyphicon-warning-sign' => 'warning-sign',
    'glyphicon-wrench' => 'wrench',
    'glyphicon-zoom-in' => 'zoom-in',
    'glyphicon-zoom-out' => 'zoom-out',
  );
  return $icons;
}

/**
 * Helper function for returning a Bootstrap Glyphicon.
 *
 * @param string $name
 *   The icon name, minus the "glyphicon-" prefix.
 * @param string $default
 *   (Optional) The default value to return.
 *
 * @return string
 *   The HTML markup containing the icon defined by $name, $default value if
 *   icon does not exist or returns empty output for whatever reason.
 */
function _bootstrap_icon($name, $default = NULL) {
  $output = NULL;
  // Ensure the icon specified is a valid Bootstrap Glyphicon.
  if (_bootstrap_glyphicons_supported() && in_array($name, _bootstrap_glyphicons())) {
    // Attempt to use the Icon API module, if enabled and it generates output.
    if (module_exists('icon')) {
      $output = theme('icon', array('bundle' => 'bootstrap', 'icon' => 'glyphicon-' . $name));
    }
    if (empty($output)) {
      // Mimic the Icon API markup.
      $attributes = array(
        'class' => array('icon', 'glyphicon', 'glyphicon-' . $name),
        'aria-hidden' => 'true',
      );
      $output = '<span' . drupal_attributes($attributes) . '></span>';
    }
  }
  return empty($output) && isset($default) ? $default : $output;
}

/**
 * Helper function for determining if Bootstrap Glyphicons can be used.
 */
function _bootstrap_glyphicons_supported() {
  // Use the advanced drupal_static() pattern, since this has the potential to
  // be called very often by _bootstrap_icon().
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['supported'] = &drupal_static(__FUNCTION__);
    // Get the active theme.
    $drupal_static_fast['theme'] = variable_get('theme_default', $GLOBALS['theme']);
  }

  // Get static data.
  $supported = &$drupal_static_fast['supported'];
  $theme = &$drupal_static_fast['theme'];

  // Retrieve supported themes.
  if (!isset($supported)) {
    $supported = array();

    // Retrieve cached data.
    $cid = 'theme_registry:bootstrap:icon_support';
    if (($cache = cache_get($cid)) && !empty($cache->data)) {
      $supported = $cache->data;
    }

    if (!isset($supported[$theme])) {
      // Bootstrap based themes are enabled by default to use CDN. Check if
      // that is the case here so no file discovery is necessary. If the active
      // theme does not have this setting, it falls back to the base theme that
      // does.
      $supported[$theme] = (bool) theme_get_setting('bootstrap_cdn', $theme);

      // CDN not used, iterate over all of the active (base) themes to determine
      // if they contain glyphicon font files.
      if (!$supported[$theme]) {
        foreach (_bootstrap_get_base_themes($theme, TRUE) as $_theme) {
          // Scan the theme for files.
          $fonts = _bootstrap_file_scan_directory(drupal_get_path('theme', $_theme), '/glyphicons-halflings-regular\.(eot|svg|ttf|woff)$/');

          // Fonts found, stop the search.
          if (!empty($fonts)) {
            $supported[$theme] = TRUE;
            break;
          }
        }
      }

      // Cache all supported themes now that this theme is added to the array.
      cache_set($cid, $supported);
    }
  }
  return $supported[$theme];
}

/**
 * Helper function for determining whether an element is a button.
 *
 * @param array $element
 *   A renderable element.
 *
 * @return bool
 *   TRUE or FALSE.
 */
function _bootstrap_is_button($element) {
  return
    !empty($element['#type']) &&
    !empty($element['#value']) && (
      $element['#type'] === 'button' ||
      $element['#type'] === 'submit' ||
      $element['#type'] === 'image_button'
    );
}

/**
 * Helper function for adding colors to button elements.
 *
 * @param array $element
 *   The form element, passed by reference.
 */
function _bootstrap_colorize_button(&$element) {
  if (_bootstrap_is_button($element)) {
    // Do not add the class if one is already present in the array.
    $button_classes = array(
      'btn-default',
      'btn-primary',
      'btn-success',
      'btn-info',
      'btn-warning',
      'btn-danger',
      'btn-link',
    );
    $class_intersection = array_intersect($button_classes, $element['#attributes']['class']);
    if (empty($class_intersection)) {
      // Get the matched class.
      $class = _bootstrap_colorize_text($element['#value']);
      // If no particular class matched, use the default style.
      if (!$class) {
        $class = 'default';
      }
      $element['#attributes']['class'][] = 'btn-' . $class;
    }
  }
}

/**
 * Helper function for associating Bootstrap classes based on a string's value.
 *
 * @param string $string
 *   The string to match classes against.
 * @param string $default
 *   The default class to return if no match is found.
 *
 * @return string
 *   The Bootstrap class matched against the value of $haystack or $default if
 *   no match could be made.
 */
function _bootstrap_colorize_text($string, $default = '') {
  static $texts;
  if (!isset($texts)) {
    $texts = array(
      // Text that match these specific strings are checked first.
      'matches' => array(
        // Primary class.
        t('Download feature')   => 'primary',

        // Success class.
        t('Add effect')         => 'success',
        t('Add and configure')  => 'success',

        // Info class.
        t('Save and add')       => 'info',
        t('Add another item')   => 'info',
        t('Update style')       => 'info',
      ),

      // Text that contain these words anywhere in the string are checked last.
      'contains' => array(
        // Primary class.
        t('Confirm')            => 'primary',
        t('Filter')             => 'primary',
        t('Submit')             => 'primary',
        t('Search')             => 'primary',

        // Success class.
        t('Add')                => 'success',
        t('Create')             => 'success',
        t('Save')               => 'success',
        t('Write')              => 'success',

        // Warning class.
        t('Export')             => 'warning',
        t('Import')             => 'warning',
        t('Restore')            => 'warning',
        t('Rebuild')            => 'warning',

        // Info class.
        t('Apply')              => 'info',
        t('Update')             => 'info',

        // Danger class.
        t('Delete')             => 'danger',
        t('Remove')             => 'danger',
      ),
    );

    // Allow sub-themes to alter this array of patterns.
    drupal_alter('bootstrap_colorize_text', $texts);
  }

  // Iterate over the array.
  foreach ($texts as $pattern => $strings) {
    foreach ($strings as $value => $class) {
      switch ($pattern) {
        case 'matches':
          if ($string === $value) {
            return $class;
          }
          break;

        case 'contains':
          if (strpos(drupal_strtolower($string), drupal_strtolower($value)) !== FALSE) {
            return $class;
          }
          break;
      }
    }
  }

  // Return the default if nothing was matched.
  return $default;
}

/**
 * Helper function for adding icons to button elements.
 *
 * @param array $element
 *   The form element, passed by reference.
 */
function _bootstrap_iconize_button(&$element) {
  if (_bootstrap_is_button($element) && ($icon = _bootstrap_iconize_text($element['#value']))) {
    $element['#icon'] = $icon;
  }
}

/**
 * Helper function for associating Bootstrap icons with text.
 *
 * @param string $string
 *   The string to match classes against.
 * @param string $default
 *   The default icon to return if no match is found.
 *
 * @return string
 *   The Bootstrap icon matched against the value of $haystack or $default if
 *   no match could be made.
 */
function _bootstrap_iconize_text($string, $default = '') {
  static $texts;
  if (!isset($texts)) {
    $texts = array(
      // Text that match these specific strings are checked first.
      'matches' => array(),

      // Text that contain these words anywhere in the string are checked last.
      'contains' => array(
        t('Manage')     => 'cog',
        t('Configure')  => 'cog',
        t('Download')   => 'download',
        t('Export')     => 'export',
        t('Filter')     => 'filter',
        t('Import')     => 'import',
        t('Save')       => 'ok',
        t('Update')     => 'ok',
        t('Edit')       => 'pencil',
        t('Add')        => 'plus',
        t('Write')      => 'plus',
        t('Cancel')     => 'remove',
        t('Delete')     => 'trash',
        t('Remove')     => 'trash',
        t('Upload')     => 'upload',
      ),
    );

    // Allow sub-themes to alter this array of patterns.
    drupal_alter('bootstrap_iconize_text', $texts);
  }

  // Iterate over the array.
  foreach ($texts as $pattern => $strings) {
    foreach ($strings as $value => $icon) {
      switch ($pattern) {
        case 'matches':
          if ($string === $value) {
            return _bootstrap_icon($icon, $default);
          }
          break;

        case 'contains':
          if (strpos(drupal_strtolower($string), drupal_strtolower($value)) !== FALSE) {
            return _bootstrap_icon($icon, $default);
          }
          break;
      }
    }
  }

  // Return a default icon if nothing was matched.
  return _bootstrap_icon($default);
}

/**
 * Helper function for invoking a specific suggestion's preprocess functions.
 *
 * @param array $variables
 *   The theme implementation variables array.
 */
function _bootstrap_preprocess_theme_suggestion(&$variables) {
  $registry = theme_get_registry();
  if (!empty($variables['theme_hook_suggestion']) && !empty($registry[$variables['theme_hook_suggestion']]['preprocess functions'])) {
    // Save the suggestion as the hook to pass to the function.
    $hook = $variables['theme_hook_suggestion'];

    // Unset the theme_hook_suggestion so the suggestion's preprocess functions
    // can provide theme_hook_suggestions if needed.
    unset($variables['theme_hook_suggestion']);

    // Iterate over the preprocess functions.
    foreach ($registry[$hook]['preprocess functions'] as $function) {
      if (function_exists($function)) {
        $function($variables, $hook);
      }
    }
  }
}

/**
 * Helper function for invoking a specific suggestion's process functions.
 *
 * @param array $variables
 *   The theme implementation variables array.
 */
function _bootstrap_process_theme_suggestion(&$variables) {
  $registry = theme_get_registry();
  if (!empty($variables['theme_hook_suggestion']) && !empty($registry[$variables['theme_hook_suggestion']]['process functions'])) {
    // Save the suggestion as the hook to pass to the function.
    $hook = $variables['theme_hook_suggestion'];

    // Unset the theme_hook_suggestion so the suggestion's process functions
    // can provide theme_hook_suggestions if needed.
    unset($variables['theme_hook_suggestion']);

    // Iterate over the process functions.
    foreach ($registry[$hook]['process functions'] as $function) {
      if (function_exists($function)) {
        $function($variables, $hook);
      }
    }
  }
}


/**
 * Helper function for determining if a tooltip can be used for a description.
 *
 * @param string $description
 *   The string of text to process.
 *
 * @return bool
 *   Returns TRUE if the $description can be used as a tooltip, FALSE otherwise.
 */
function _bootstrap_tooltip_description($description) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['descriptions'] = &drupal_static(__FUNCTION__);
  }
  // Get static data.
  $descriptions = &$drupal_static_fast['descriptions'];
  $tooltip_descriptions = &$drupal_static_fast['tooltip_descriptions'];

  // Determine if tooltips are enabled.
  if (!isset($tooltip_descriptions)) {
    $tooltip_descriptions = theme_get_setting('bootstrap_tooltip_enabled') && theme_get_setting('bootstrap_tooltip_descriptions');
  }

  // Go ahead and return FALSE if not enabled.
  if (!$tooltip_descriptions) {
    return FALSE;
  }

  $id = drupal_hash_base64($description);
  if (!isset($descriptions[$id])) {
    $descriptions[$id] = ($description === strip_tags($description) && strlen($description) <= theme_get_setting('bootstrap_tooltip_descriptions_length'));
  }
  return $descriptions[$id];
}
