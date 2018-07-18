<?php

/**
 * Implements hook_preprocess_html().
 */
function expressadmin_preprocess_html(&$vars) {

  $element = array(
    '#tag' => 'link', // The #tag is the html tag - <link />
    '#attributes' => array( // Set up an array of attributes inside the tag
      'href' => '//fonts.googleapis.com/css?family=Open+Sans:400italic,700italic,400,700',
      'rel' => 'stylesheet',
      'type' => 'text/css',
    ),
  );
  drupal_add_html_head($element, 'web_fonts');

  // Turn off IE Compatibility Mode
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      'http-equiv' => 'X-UA-Compatible',
      'content' => 'IE=edge',
    ),
  );
  drupal_add_html_head($element, 'ie_compatibility_mode');

  // Attributes for html element.
  $vars['html_attributes_array'] = array(
    'lang' => $vars['language']->language,
    'dir' => $vars['language']->dir,
  );
  $vars['html_attributes'] = drupal_attributes($vars['html_attributes_array']);

  // Skip variables
  $vars['skip_link_text'] = 'Skip to content';
  $vars['skip_link_anchor'] = 'admin-top';
}

/**
 * Overrides theme_button().
 */
function expressadmin_button($variables) {
  $element = $variables['element'];
  $element['#attributes']['type'] = 'submit';
  element_set_attributes($element, array('id', 'name', 'value'));

  $element['#attributes']['class'][] = 'form-' . $element['#button_type'];
  if (!empty($element['#attributes']['disabled'])) {
    $element['#attributes']['class'][] = 'form-button-disabled';
  }
  $element['#attributes']['class'][] = 'btn';
  // Colorize button.
  _expressadmin_colorize_button($element);

  return '<input' . drupal_attributes($element['#attributes']) . ' />';
}

/**
 * Helper function for adding colors to button elements.
 *
 * @param array $element
 *   The form element, passed by reference.
 */
function _expressadmin_colorize_button(&$element) {
  if (_expressadmin_is_button($element)) {
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
      $class = _expressadmin_colorize_text($element['#value']);
      // If no particular class matched, use the default style.
      if (!$class) {
        $class = 'default';
      }
      $element['#attributes']['class'][] = 'btn-' . $class;
    }
  }
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
function _expressadmin_is_button($element) {
  return
    !empty($element['#type']) &&
    !empty($element['#value']) && (
      $element['#type'] === 'button' ||
      $element['#type'] === 'submit' ||
      $element['#type'] === 'image_button'
    );
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
function _expressadmin_colorize_text($string, $default = '') {
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
        t('Update style')       => 'info',

        // Default class.
        t('Add another item')   => 'default',
        t('Add existing block')   => 'default',
      ),

      // Text that contain these words anywhere in the string are checked last.
      'contains' => array(
        // Primary class.
        t('Confirm')            => 'primary',
        t('Filter')             => 'primary',
        t('Submit')             => 'primary',
        t('Search')             => 'primary',
        t('Add')                => 'primary',
        t('Create')             => 'primary',
        t('Send')               => 'primary',

        // Success class.
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
        t('Analyze')             => 'info',

        // Default class.
        t('Upload')             => 'default',
        t('submit')             => 'default',

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
 * Overrides theme_menu_local_tasks().
 */
function expressadmin_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    foreach($variables['primary'] as $menu_item_key => $menu_attributes) {
      $variables['primary'][$menu_item_key]['#link']['localized_options']['attributes']['class'][] = strip_tags(strtolower(str_replace(' ','-', $menu_attributes['#link']['title'])));
		}
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="tabs--primary nav nav-tabs clearfix">';
    $variables['primary']['#suffix'] = '</ul>';


    $output .= drupal_render($variables['primary']);
  }

  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs--secondary pagination pagination-sm clearfix">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }

  return $output;
}

/**
 * Override or insert variables into the page template.
 */
function expressadmin_preprocess_page(&$vars) {
  $vars['primary_local_tasks'] = $vars['tabs'];
  unset($vars['primary_local_tasks']['#secondary']);
  $vars['secondary_local_tasks'] = array(
    '#theme' => 'menu_local_tasks',
    '#secondary' => $vars['tabs']['#secondary'],
  );
}

/**
 * Display the list of available node types for node creation.
 */
function expressadmin_node_add_list($variables) {
  $content = $variables['content'];
  $output = '';
  if ($content) {
    $output = '<ul class="admin-list">';
    foreach ($content as $item) {
      $output .= '<li class="clearfix">';
      $output .= '<span class="label">' . l($item['title'], $item['href'], $item['localized_options']) . '</span>';
      $output .= '<div class="description">' . filter_xss_admin($item['description']) . '</div>';
      $output .= '</li>';
    }
    $output .= '</ul>';
  }
  else {
    $output = '<p>' . t('You have not created any content types yet. Go to the <a href="@create-content">content type creation page</a> to add a new content type.', array('@create-content' => url('admin/structure/types/add'))) . '</p>';
  }
  return $output;
}

function expressadmin_bean_add_list($variables) {
  $content = $variables['content'];
  $blocks = array();
  $i = 0;
  if ($content) {
    foreach ($content as $item) {
      $title = l(t('<span class="icon"></span>@label', array('@label' => $item->getLabel())), 'block/add/' . $item->buildURL(), array('html' => TRUE));
      $title = '<span class="label">' . $title . '</span>';
      $description = (!is_array($item->getDescription())) ? '<div class="description">' . $item->getDescription() . '</div>' : '';
      //creative way to setup sorting by label; add number to prevent duplicate keys
      $blocks[str_replace(' ', '', $item->getLabel()) . '_' . $i] = '<li>' . $title . $description . '</li>';
      $i++;
    }
    ksort($blocks);
    $output = '<ul class="bean-type-list admin-list">' . implode('', $blocks) . '</ul>';
  }
  else {
    $output = '<p>' . t('You have not created any block types yet.') . '</p>';
  }
  return $output;
}

/**
 * Overrides theme_admin_block_content().
 *
 * Use unordered list markup in both compact and extended mode.
 */
function expressadmin_admin_block_content($variables) {
  $content = $variables['content'];
  $output = '';
  if (!empty($content)) {
    $output = system_admin_compact_mode() ? '<ul class="admin-list compact">' : '<ul class="admin-list">';
    foreach ($content as $item) {
      $output .= '<li class="leaf">';
      $output .= '<span class="label">' . l($item['title'], $item['href'], $item['localized_options']) . '</span>';
      if (isset($item['description']) && !system_admin_compact_mode()) {
        $output .= '<div class="description">' . filter_xss_admin($item['description']) . '</div>';
      }
      $output .= '</li>';
    }
    $output .= '</ul>';
  }
  return $output;
}

/**
 * Override of theme_tablesort_indicator().
 *
 * Use our own image versions, so they show up as black and not gray on gray.
 */
function expressadmin_tablesort_indicator($variables) {
  $style = $variables['style'];
  $theme_path = drupal_get_path('theme', 'seven');
  if ($style == 'asc') {
    return theme('image', array('path' => $theme_path . '/images/arrow-asc.png', 'alt' => t('sort ascending'), 'width' => 13, 'height' => 13, 'title' => t('sort ascending')));
  }
  else {
    return theme('image', array('path' => $theme_path . '/images/arrow-desc.png', 'alt' => t('sort descending'), 'width' => 13, 'height' => 13, 'title' => t('sort descending')));
  }
}

/**
 * Implements hook_css_alter().
 */
function expressadmin_css_alter(&$css) {
  // Use Seven's vertical tabs style instead of the default one.
  if (isset($css['misc/vertical-tabs.css'])) {
    $css['misc/vertical-tabs.css']['data'] = drupal_get_path('theme', 'seven') . '/vertical-tabs.css';
    $css['misc/vertical-tabs.css']['type'] = 'file';
  }
  if (isset($css['misc/vertical-tabs-rtl.css'])) {
    $css['misc/vertical-tabs-rtl.css']['data'] = drupal_get_path('theme', 'seven') . '/vertical-tabs-rtl.css';
    $css['misc/vertical-tabs-rtl.css']['type'] = 'file';
  }
  // Use Seven's jQuery UI theme style instead of the default one.
  if (isset($css['misc/ui/jquery.ui.theme.css'])) {
    $css['misc/ui/jquery.ui.theme.css']['data'] = drupal_get_path('theme', 'seven') . '/jquery.ui.theme.css';
    $css['misc/ui/jquery.ui.theme.css']['type'] = 'file';
  }
}

/**
 * Preprocessor for theme('admin_block').
 */
function expressadmin_preprocess_admin_block(&$vars) {

  $titles = array(
    'people' => 'user',
    'content-authoring' => 'pencil',
    'date-api' => 'calendar',
    'media' => 'picture-o',
    'search' => 'search',
    'regional-and-language' => 'map-marker',
    'system' => 'cog',
    'user-interface' => 'laptop',
    'development' => 'code',
    'web-services' => 'globe',
    'news' => 'newspaper-o',
    'site-configurations' => 'cog',
    'site-status' => 'rocket',
    'bundles' => 'gift',
    'forms' => 'check-square-o',
    'url-management' => 'link',
    'advanced-content' => 'th-list',
    'search-engine-optimization' => 'line-chart',
    'social-media' => 'share-alt',
    'cache' => 'refresh',
    'menus' => 'bars',
  );
  $key = strtolower(str_replace(' ','-', $vars['block']['link_title']));
  if (array_key_exists($key, $titles)) {
    $vars['block']['title'] = '<i class="fa fa-' . $titles[$key] . '"></i> ' . $vars['block']['link_title'];
  }
}

/**
 * Implements of hook_form_alter().
 * Add drupal /unique path to node forms.
 */

function expressadmin_form_alter(&$form, &$form_state, $form_id) {
  if (isset($form['#node_edit_form']) && ($form['#node_edit_form'] == TRUE) && isset($form['nid']['#value'])) {
    $form['title']['#description'] = '<div class="unique-node-path">Node path: node/' . $form['nid']['#value'] . '</div>';
  }
}

function expressadmin_page_alter(&$page) {
  // Get form checkbox/radio column settings
  $express_field_columns = variable_get('express_field_columns_minimum', 10);
  $settings['express_field_columns'] = $express_field_columns;
  drupal_add_js($settings, 'setting');
}


/**
 * Implements theme_preprocess_username().
 *
 * Copy of template_preprocess_username except removing the truncation.
 */
function expressadmin_preprocess_username(&$variables) {
  $account = $variables['account'];

  $variables['extra'] = '';
  if (empty($account->uid)) {
    $variables['uid'] = 0;
    if (theme_get_setting('toggle_comment_user_verification')) {
      $variables['extra'] = ' (' . t('not verified') . ')';
    }
  }
  else {
    $variables['uid'] = (int) $account->uid;
  }

  // Set the name to a formatted name that is safe for printing and
  // that won't break tables by being too long. Keep an unshortened,
  // unsanitized version, in case other preprocess functions want to implement
  // their own shortening logic or add markup. If they do so, they must ensure
  // that $variables['name'] is safe for printing.
  $name = $variables['name_raw'] = format_username($account);
  if (drupal_strlen($name) > 20) {
    // Remove name truncation
    //$name = drupal_substr($name, 0, 15) . '...';
  }
  $variables['name'] = check_plain($name);

  $variables['profile_access'] = user_access('access user profiles');
  $variables['link_attributes'] = array();
  // Populate link path and attributes if appropriate.
  if ($variables['uid'] && $variables['profile_access']) {
    // We are linking to a local user.
    $variables['link_attributes'] = array('title' => t('View user profile.'));
    $variables['link_path'] = 'user/' . $variables['uid'];
  }
  elseif (!empty($account->homepage)) {
    // Like the 'class' attribute, the 'rel' attribute can hold a
    // space-separated set of values, so initialize it as an array to make it
    // easier for other preprocess functions to append to it.
    $variables['link_attributes'] = array('rel' => array('nofollow'));
    $variables['link_path'] = $account->homepage;
    $variables['homepage'] = $account->homepage;
  }
  // We do not want the l() function to check_plain() a second time.
  $variables['link_options']['html'] = TRUE;
  // Set a default class.
  $variables['attributes_array'] = array('class' => array('username'));
}
