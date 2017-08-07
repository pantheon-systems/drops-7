<?php

/**
 * @file
 * This file is empty by default because the base theme chain (Alpha & Omega) provides
 * all the basic functionality. However, in case you wish to customize the output that Drupal
 * generates through Alpha & Omega this file is a good place to do so.
 *
 * Alpha comes with a neat solution for keeping this file as clean as possible while the code
 * for your subtheme grows. Please read the README.txt in the /preprocess and /process subfolders
 * for more information on this topic.
 */

/**
 * Implements hook_css_alter().
 */
function cu_omega_css_alter(&$css) {
  // Remove jquery ui stylesheet.
  unset($css['misc/ui/jquery.ui.theme.css']);

  // Set all non-print css to screen so print css is cleaner
  foreach ($css as $key => $stylesheet) {
    if ($stylesheet['media'] == 'all') {
      //swap these lines for browser debugging
      $css[$key]['media'] = 'screen';
      //unset($css[$key]);
    } elseif ($stylesheet['media'] == 'print') {
      // uncomment line below for browser debugging
      //$css[$key]['media'] = 'screen';
    }
  }
}

/**
 * Implements hook_preprocess_html().
 */
function cu_omega_preprocess_html(&$vars) {
  // Remove page-node- body class
  foreach ($vars['attributes_array']['class'] as $key => $value) {
    if ($value == 'page-node-') {
      unset($vars['attributes_array']['class'][$key]);
    }
  }
  $vars['head_title_array']['slogan'] = 'University of Colorado Boulder';
  $vars['head_title'] = implode(' | ', $vars['head_title_array']);

  //$vars['attributes_array']['class'][]='banner-white';
  $headings = theme_get_setting('headings') ? theme_get_setting('headings') : 'headings-bold';
  $vars['attributes_array']['class'][]=$headings;
  $page_title_image_background = theme_get_setting('page_title_image_background') ? theme_get_setting('page_title_image_background') : 'page-title-image-background-white';
  $vars['attributes_array']['class'][]=$page_title_image_background;
  $icon_color = theme_get_setting('block_icons_color') ? theme_get_setting('block_icons_color') : 'block-icons-inherit';
  $vars['attributes_array']['class'][]=$icon_color;

  drupal_add_js(drupal_get_path('theme','cu_omega') .'/js/mobile-menu-toggle.js');

  drupal_add_js(drupal_get_path('theme','cu_omega') .'/js/track-focus.js', array('scope' => 'footer'));

  $element = array(
    '#tag' => 'link', // The #tag is the html tag - <link />
    '#attributes' => array( // Set up an array of attributes inside the tag
      'href' => '//fast.fonts.net/cssapi/86696b99-fb1a-4964-9676-9233fb4fca8f.css',
      'rel' => 'stylesheet',
      'type' => 'text/css',
    ),
  );
  if (variable_get('use_fonts', TRUE)) {
    drupal_add_html_head($element, 'web_fonts');
  }

  // Turn off IE Compatibility Mode
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      'http-equiv' => 'X-UA-Compatible',
      'content' => 'IE=edge',
    ),
  );
  drupal_add_html_head($element, 'ie_compatibility_mode');

  // Check to see if site is responsive or not...
  $responsive = theme_get_setting('alpha_responsive') ? 'is-responsive' : 'is-not-responsive';
  $vars['attributes_array']['class'][] = $responsive;
}

/**
 * Implements hook_preprocess_page().
 */

function cu_omega_preprocess_page(&$vars) {

  $vars['site_slogan'] = 'University of Colorado <strong>Boulder</strong>';

  if($vars['is_front'] == TRUE) {
    $vars['title_hidden'] = 1;
  }

  foreach ($vars['main_menu'] as $key => $value) {
    if($vars['main_menu'][$key]['href'] == '<front>') {
      $vars['main_menu'][$key]['html'] = TRUE;
      $vars['main_menu'][$key]['title'] = '<i class="fa fa-home"></i><span class="element-invisible">Home</span>';
      $vars['main_menu'][$key]['attributes']['id'] = 'home-link';
    }
  }
  if (!empty($vars['footer_menu'])) {
    foreach ($vars['footer_menu'] as $key => $value) {
      if($vars['footer_menu'][$key]['href'] == '<front>') {
        $vars['footer_menu'][$key]['html'] = TRUE;
        $vars['footer_menu'][$key]['title'] = '<i class="fa fa-home"></i><span class="element-invisible">Home</span>';
      }
    }
  }
  drupal_add_css(drupal_get_path('theme','cu_omega') . '/jquery-ui/css/jquery.theme.css', array('group' => CSS_THEME, 'every_page' => TRUE));
  $sidebar_count = array();

  if (!empty($vars['page']['content']['content']['sidebar_first']) && !empty($vars['page']['content']['content']['sidebar_second'])) {
    $vars['attributes_array']['class'][] = 'page-sidebar-both';
  }
  elseif (!empty($vars['page']['content']['content']['sidebar_first'])) {
    $vars['attributes_array']['class'][] = 'page-sidebar-first';
  }
  elseif (!empty($vars['page']['content']['content']['sidebar_second'])) {
    $vars['attributes_array']['class'][] = 'page-sidebar-second';
  }

  // Add class if page is an unpublished node
  if ($node = menu_get_object()) {
    $status = ($node->status == 1) ? 'published' : 'unpublished';
    $vars['attributes_array']['class'][] = 'node-' . $status;
  }
}

/**
 * Implements hook_preprocess_block().
 */
function cu_omega_preprocess_block(&$vars) {
  // Add bean type to class array so we can maybe do some css stuff
  if ($vars['block']->module == 'bean') {
    // Get the bean elements.
    if (isset($vars['elements']['bean'])) {
      $beans = $vars['elements']['bean'];
      // There is only 1 bean per block.
      $children = element_children($beans);
      $bean = $beans[current(element_children($beans))];
      // Add bean type classes to the parent block.
      $vars['attributes_array']['class'][] = drupal_html_class('block-bean-type-' . $bean['#bundle']);
    }
  }
  if(isset($vars['elements']['content']) && isset($vars['elements']['content']['bean'])) {
    $bean = $vars['elements']['content']['bean'];
    $children = array_intersect_key($bean, array_flip(element_children($bean)));
    $the_bean = current($children);
    $bean_type = $the_bean['#bundle'];
    $vars['attributes_array']['class'][] = 'bean-type-' . $bean_type;
  }

  if (theme_get_setting('after_content_columns')) {
    $after_content_columns = theme_get_setting('after_content_columns');
  }
  else {
    $after_content_columns = 1;
  }

  if (theme_get_setting('lower_columns')) {
    $lower_columns = theme_get_setting('lower_columns');
  }
  else {
    $footer_columns = 1;
  }

  if (theme_get_setting('footer_columns')) {
    $footer_columns = theme_get_setting('footer_columns');
  }
  else {
    $footer_columns = 1;
  }

  $block_counter = &drupal_static(__FUNCTION__, array());
  $vars['block'] = $vars['elements']['#block'];
  // All blocks get an independent counter for each region.
  if (!isset($block_counter[$vars['block']->region])) {
    $block_counter[$vars['block']->region] = -1;
  }
  // Same with zebra striping.
  $vars['block_id'] = $block_counter[$vars['block']->region]++;

  $vars['attributes_array']['class'][] = $vars['block']->region . '-block-' . $block_counter[$vars['block']->region];
  if (empty($vars['grid_size_blocks'])) {
    switch ($vars['block']->region) {

      case 'after_content':
        $vars['attributes_array']['class'][] = 'grid-' . 12/$after_content_columns;
        if (($block_counter[$vars['block']->region]) == 0) {
          $vars['attributes_array']['class'][] = 'new-block-row alpha';
        }
        if (($block_counter[$vars['block']->region]+1) == $after_content_columns) {
          $vars['attributes_array']['class'][] = 'omega';
          $block_counter[$vars['block']->region] = -1;
        }
        break;

      case 'footer':
        $vars['attributes_array']['class'][] = 'grid-' . 12/$footer_columns;
        if (($block_counter[$vars['block']->region]) == 0) {
          $vars['attributes_array']['class'][] = 'new-block-row alpha';
        }
        if (($block_counter[$vars['block']->region]+1) == $footer_columns) {
          $vars['attributes_array']['class'][] = 'omega';
          $block_counter[$vars['block']->region] = -1;
        }
        break;

      case 'lower':
        $vars['attributes_array']['class'][] = 'grid-' . 12/$lower_columns;
        if (($block_counter[$vars['block']->region]) == 0) {
          $vars['attributes_array']['class'][] = 'new-block-row alpha';
        }
        if (($block_counter[$vars['block']->region]+1) == $lower_columns) {
          $vars['attributes_array']['class'][] = 'omega';
          $block_counter[$vars['block']->region] = -1;
        }
        break;

      case 'slider':
        //$vars['classes_array'][] = 'grid-12';
        break;
    }
  }

}

/**
 * Implements theme_image_style().
 */
function cu_omega_image_style(&$vars) {
  // Determine the dimensions of the styled image.
  $dimensions = array(
    'width' => $vars['width'],
    'height' => $vars['height'],
  );

  image_style_transform_dimensions($vars['style_name'], $dimensions);

  $vars['width'] = $dimensions['width'];
  $vars['height'] = $dimensions['height'];

  // Determine the url for the styled image.
  $vars['path'] = image_style_url($vars['style_name'], $vars['path']);
  $vars['attributes']['class'] = array('image-' . $vars['style_name']);
  return theme('image', $vars);
}

/**
 * Implements hook_process_region).
 */
function cu_omega_preprocess_region(&$vars) {
  global $base_url;
  $theme = alpha_get_theme();
  switch ($vars['elements']['#region']) {
    case 'secondary_menu':
      if (!theme_get_setting('use_action_menu')) {
        $vars['secondary_menu'] = $theme->page['secondary_menu'];
      }
      break;

    case 'menu':
      if (array_key_exists('mobile_menu', $theme->page)) {
        $vars['mobile_menu'] = $theme->page['mobile_menu'];
      }
      if (theme_get_setting('use_action_menu')) {
        $color = theme_get_setting('action_menu_color') ? theme_get_setting('action_menu_color') : 'action-blue';
        $vars['attributes_array']['class'][] = $color;
      }
      break;

    case 'site_info':
      if (array_key_exists('footer_menu', $theme->page)) {
        $vars['footer_menu'] = $theme->page['footer_menu'];
      }
      $vars['beboulder']['color'] = 'white';
      break;

    case 'content':
      break;

    case 'branding':
      $vars['print_logo'] = '<img src="' . $base_url . '/' . drupal_get_path('theme','cu_omega') . '/images/print-logo.png" alt="University of Colorado Boulder" />';
      if (variable_get('custom_white_logo') && variable_get('custom_black_logo')) {
        $vars['attributes_array']['class'][] = 'has-custom-logo';
      }
      break;
  }
}

/**
 * Implements hook_process_zone().
 */
function cu_omega_alpha_process_zone(&$vars) {
  $theme = alpha_get_theme();
  if (!theme_get_setting('use_breadcrumbs') && $vars['elements']['#zone'] == 'content') {
    if(!empty($vars['breadcrumb'])) {
      unset($vars['breadcrumb']);
    }
  }

  if (($vars['elements']['#zone'] == 'content') && isset($theme->page['title_image']) && $theme->page['title_image']){
    // Get image url from context and place in tpl variable
    $vars['title_image'] = $theme->page['title_image'];
  }
  switch ($vars['elements']['#zone']) {
    case 'site_information':
      if (array_key_exists('footer_menu', $theme->page)) {
        $vars['footer_menu'] = $theme->page['footer_menu'];
        $vars['footer_menu_color'] = theme_get_setting('footer_menu_color') ? theme_get_setting('footer_menu_color') : 'footer-menu-gray';
      }
      break;
    case 'content':
      $page_title_image_width = theme_get_setting('page_title_image_width') ? theme_get_setting('page_title_image_width') : 'page-title-image-width-content';
      if ($page_title_image_width == 'page-title-image-width-content') {
        $vars['title_image_wrapper_class'] = 'container-content';
        $vars['title_image_title_class'] = '';
      } else {
        $vars['title_image_wrapper_class'] = 'container-full';
        $vars['title_image_title_class'] = 'container-content';
      }
  }
}
/**
 * Implements hook_preprocess_zone().
 */
function cu_omega_alpha_preprocess_zone(&$vars) {
  $theme = alpha_get_theme();

  switch ($vars['elements']['#zone']) {
    // minimize markup of secondary menu if there are no links for it
    case 'secondary_menu':
      if (empty($theme->page['secondary_menu'])) {
        $vars['content'] = '';
        $vars['attributes_array']['id'] = 'zone-secondary-menu-wrapper-empty';
        $key = array_search('zone-secondary-menu-wrapper', $vars['attributes_array']['class']);
        if ($key) {
          $vars['attributes_array']['class'][$key] = 'zone-secondary-menu-wrapper-empty';
        }
      }
      break;
  }
}


/**
 * Implements hook_preprocess_node().
 */
function cu_omega_preprocess_node(&$vars) {
  // Custom display templates will be called node--[type]--[viewmode].tpl.php
  $vars['theme_hook_suggestions'][] = 'node__' . $vars['type'] . '__' . $vars['view_mode'];


  // Making comments appear at the bottom of $content
  $vars['content']['comments']['#weight'] = 1000;

  if (module_exists('context') && $plugin = context_get_plugin('reaction', 'block')) {
    if ($context_content_sidebar_left_blocks = $plugin->block_get_blocks_by_region('content_sidebar_left')) {
      $vars['content_sidebar_left'] = $context_content_sidebar_left_blocks;
      $vars['content_sidebar_left']['#theme_wrappers'] = array('region');
      $vars['content_sidebar_left']['#region'] = 'content_sidebar_left';
    }
    if ($context_content_sidebar_right_blocks = $plugin->block_get_blocks_by_region('content_sidebar_right')) {
      $vars['content_sidebar_right'] = $context_content_sidebar_right_blocks;
      $vars['content_sidebar_right']['#theme_wrappers'] = array('region');
      $vars['content_sidebar_right']['#region'] = 'content_sidebar_right';
    }
    if ($context_content_bottom_blocks = $plugin->block_get_blocks_by_region('content_bottom')) {
      $vars['content_bottom'] = $context_content_bottom_blocks;
      $vars['content_bottom']['#theme_wrappers'] = array('region');
      $vars['content_bottom']['#region'] = 'content_bottom';
      $vars['classes_array'][] = 'content-bottom';
    }
  }

  if ($content_sidebar_left_blocks = block_get_blocks_by_region('content_sidebar_left')) {
    $vars['content_sidebar_left'] = $content_sidebar_left_blocks;
    $vars['content_sidebar_left']['#theme_wrappers'] = array('region');
    $vars['content_sidebar_left']['#region'] = 'content_sidebar_left';
  }

  if ($content_sidebar_right_blocks = block_get_blocks_by_region('content_sidebar_right')) {
    $vars['content_sidebar_right'] = $content_sidebar_right_blocks;
    $vars['content_sidebar_right']['#theme_wrappers'] = array('region');
    $vars['content_sidebar_right']['#region'] = 'content_sidebar_right';
  }

  if ($context_content_bottom_blocks = block_get_blocks_by_region('content_bottom')) {
    $vars['content_bottom'] = $context_content_bottom_blocks;
    $vars['content_bottom']['#theme_wrappers'] = array('region');
    $vars['content_bottom']['#region'] = 'content_bottom';
    $vars['classes_array'][] = 'content-bottom';
  }

  if (!empty($vars['content_sidebar_left']) && !empty($vars['content_sidebar_right'])) {
    $vars['content_sidebar_left']['#region'] = 'content_sidebars';
    $vars['content_sidebar_right']['#region'] = 'content_sidebars';
  }
  switch ($vars['type']) {
    case 'slider':
      unset($vars['content_sidebar_left']);
      unset($vars['content_sidebar_right']);
      break;

    case 'file':
      unset($vars['content_sidebar_left']);
      unset($vars['content_sidebar_right']);
      break;

    case 'video':
      unset($vars['content_sidebar_left']);
      unset($vars['content_sidebar_right']);
      break;

    case 'person':
      unset($vars['content_sidebar_left']);
      unset($vars['content_sidebar_right']);
      break;

    case 'page':
      unset($vars['content']['links']);
      break;
  }
}

function cu_omega_menu_link(array $variables) {
  $element = $variables['element'];
  if (isset($element['#localized_options']['icon'])&& strlen($element['#localized_options']['icon']) > 3 ) {
    $element['#localized_options']['html'] = TRUE;
    $hide = isset($element['#localized_options']['hide_text']) ? $element['#localized_options']['hide_text'] : 0;
    $hide_class = $hide ? 'hide-text' : '';
    $space = $hide ? '' : ' ';
    $element['#title'] = '<i class="fa fa-fw ' . $element['#localized_options']['icon'] . '"></i>' . $space . '<span class="' . $hide_class . '">' . $element['#title']  . '</span>';
  }

  $sub_menu = '';

  if ($element['#below']) {
    $sub_menu = drupal_render($element['#below']);
  }
  $output = l($element['#title'], $element['#href'], $element['#localized_options']);
  return '<li' . drupal_attributes($element['#attributes']) . '>' . $output . $sub_menu . "</li>\n";
}

function cu_omega_links__system_main_menu($vars) {
  $menu_id = $vars['attributes']['id'];
  $html = '  <ul id="' . $menu_id . '">';

  // Add first and last classes to first and last list items
  reset($vars['links']);
  $first = key($vars['links']);
  end($vars['links']);
  $last = key($vars['links']);
  reset($vars['links']);
  $vars['links'][$first]['attributes']['class'][] = 'first';
  $vars['links'][$last]['attributes']['class'][] = 'last';
  foreach ($vars['links'] as $link) {
    $classes = '';
    if (!empty($link['attributes']['class'])) {
      $classes = join(' ', $link['attributes']['class']);
    }
    if (isset($link['icon']) && strlen($link['icon']) > 3) {
      $link['html'] = TRUE;
      $hide = isset($link['hide_text']) ? $link['hide_text'] : 0;
      $hide_class = $hide ? 'hide-text' : '';
      $space = $hide ? '' : ' ';
      $title = '<i class="fa fa-fw '. $link['icon'] . '"></i>' . $space . '<span class="' . $hide_class . '">' . $link['title'] . '</span>';
      $html .= '<li class="' . $classes .'">'.l($title, $link['href'], $link).'</li>';
    }
    else if (isset($link['title']) && isset($link['href'])) {
      $html .= '<li class="' . $classes .'">'.l($link['title'], $link['href'], $link).'</li>';
    }
  }
  $html .= "  </ul>";

  return $html;
}

function cu_omega_links__system_secondary_menu($vars) {

  // Prepare label - set by more_menus.module
  $label = variable_get('secondary_menu_label') ? '<h2 class="inline secondary-menu-label">' . variable_get('secondary_menu_label') . '</h2>': '';
  $menu_id = $vars['attributes']['id'];
  if (theme_get_setting('use_action_menu') && !isset($vars['mobile'])) {
    $html = '  <ul id="action-menu">';
  } else {
    $html = $label . '  <ul id="' . $menu_id . '">';
  }

  // Add first and last classes to first and last list items
  reset($vars['links']);
  $first = key($vars['links']);
  end($vars['links']);
  $last = key($vars['links']);
  reset($vars['links']);

  $vars['links'][$first]['attributes']['class'][] = 'first';
  $vars['links'][$last]['attributes']['class'][] = 'last';

  foreach ($vars['links'] as $link) {
    $classes = '';
    if (!empty($link['attributes']['class'])) {
      $classes = join(' ', $link['attributes']['class']);
    }
    if (isset($link['icon']) && strlen($link['icon']) > 3) {
      $link['html'] = TRUE;
      $hide = isset($link['hide_text']) ? $link['hide_text'] : 0;
      $hide_class = $hide ? 'hide-text' : '';
      $space = $hide ? '' : ' ';
      $title = '<i class="fa fa-fw '. $link['icon'] . '"></i>' . $space . '<span class="' . $hide_class . '">' . $link['title'] . '</span>';
      $html .= '<li class="' . $classes .'">'.l($title, $link['href'], $link).'</li>';
    }
    else if (isset($link['title']) && isset($link['href'])) {
      $html .= '<li class="' . $classes .'">'.l($link['title'], $link['href'], $link).'</li>';
    }
  }
  $html .= "  </ul>";

  return $html;
}

function cu_omega_links__footer_menu($vars) {
  $html = '<ul id="footer-menu-links" class="links inline-menu clearfix">';

  // Add first and last classes to first and last list items
  reset($vars['links']);
  $first = key($vars['links']);
  end($vars['links']);
  $last = key($vars['links']);
  reset($vars['links']);

  $vars['links'][$first]['attributes']['class'][] = 'first';
  $vars['links'][$last]['attributes']['class'][] = 'last';

  foreach ($vars['links'] as $link) {
    $classes = '';
    if (!empty($link['attributes']['class'])) {
      $classes = join(' ', $link['attributes']['class']);
    }
    if (isset($link['icon']) && strlen($link['icon']) > 3) {
      $link['html'] = TRUE;
      $hide = isset($link['hide_text']) ? $link['hide_text'] : 0;
      $hide_class = $hide ? 'hide-text' : '';
      $space = $hide ? '' : ' ';
      $title = '<i class="fa fa-fw '. $link['icon'] . '"></i>' . $space . '<span class="' . $hide_class . '">' . $link['title'] . '</span>';
      $html .= '<li class="' . $classes .'">'.l($title, $link['href'], $link).'</li>';
    }
    else if (isset($link['title']) && isset($link['href'])) {
      $html .= '<li class="' . $classes .'">'.l($link['title'], $link['href'], $link).'</li>';
    }
  }
  $html .= "  </ul>";

  return $html;
}

function cu_omega_breadcrumb($vars) {
  $breadcrumb = $vars['breadcrumb'];
  if (!empty($breadcrumb)) {
    // Replace the Home breadcrumb with a Home icon
    //$breadcrumb[0] = str_replace('Home','<i class="fa fa-home"></i> <span class="home-breadcrumb element-invisible">Home</span>',$breadcrumb[0]);
    // Get current page title and add to breadcrumb array
    $breadcrumb[] = drupal_get_title();
    // Provide a navigational heading to give context for breadcrumb links to
    // screen-reader users. Make the heading invisible with .element-invisible.
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';
    $output .= '<div class="breadcrumb">' . implode(' <i class="fa fa-angle-right"></i> ', $breadcrumb) . '</div>';
    return $output;
  }
}


/**
 * Theme function to output tablinks for classic Quicktabs style tabs.
 *
 * @ingroup themeable
 */
function cu_omega_qt_quicktabs_tabset($vars) {
  $variables = array(
    'attributes' => array(
      'class' => 'clearfix quicktabs-tabs quicktabs-style-' . $vars['tabset']['#options']['style'],
    ),
    'items' => array(),
  );
  foreach (element_children($vars['tabset']['tablinks']) as $key) {
    $item = array();
    if (is_array($vars['tabset']['tablinks'][$key])) {
      $tab = $vars['tabset']['tablinks'][$key];
      if ($key == $vars['tabset']['#options']['active']) {
        $item['class'] = array('active');
      }
      $item['data'] = drupal_render($tab);
      $variables['items'][] = $item;
    }
  }
  return theme('item_list', $variables);
}

/**
 * Overrides theme_menu_local_tasks().
 */
function cu_omega_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    foreach($variables['primary'] as $menu_item_key => $menu_attributes) {
      $variables['primary'][$menu_item_key]['#link']['localized_options']['attributes']['class'][] = strtolower(str_replace(' ','-', $menu_attributes['#link']['title']));
    }
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="primary">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }
  return $output;
}

function cu_omega_preprocess_entity(&$vars) {
  $entity_type = $vars['elements']['#entity_type'];
  if ($entity_type == 'bean') {
    if (isset($vars['attributes_array']['class'])) {
      unset($vars['attributes_array']['class']);
    }
  }
}
