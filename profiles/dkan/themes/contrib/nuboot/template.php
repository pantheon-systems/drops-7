<?php
/**
 * @file
 * Theme specific functions.
 */

/**
 * Remove dkan button styles so we can use our own.
 */
function nuboot_css_alter(&$css) {
  unset($css[drupal_get_path('module', 'dkan_dataset') . '/dkan_dataset_btn.css']);
}

/**
 * Implements template_preprocess_html.
 */
function nuboot_preprocess_html(&$variables) {
  drupal_add_css('//fonts.googleapis.com/css?family=Droid+Sans:400,700|Open+Sans:300italic,400italic,600italic,700italic,800italic,400,300,600,700,800', array('type' => 'external'));
  drupal_add_css('//netdna.bootstrapcdn.com/font-awesome/4.0.3/css/font-awesome.css', array('type' => 'external'));
}

/**
 * Implements template_preprocess_page.
 */
function nuboot_preprocess_page(&$vars) {
  if (drupal_is_front_page()) {
    $vars['title'] = '';
    unset($vars['page']['content']['system_main']['default_message']);
  }
  // Remove title on dataset edit and creation pages.
  if (!empty($vars['node']) && in_array($vars['node']->type, array('dataset', 'resource')) || arg(1) == 'add') {
    $vars['title'] = '';
  }
}

/**
 * Theme function for iframe link.
 */
function nuboot_link_iframe_formatter_original($variables) {
  $link_options = $variables['element'];
  $link = l($link_options['title'], $link_options['url'], $link_options);
  return '<i class="fa fa-external-link"></i>  ' . $link;
}

/**
 * Implements theme_breadcrumb().
 */
function nuboot_breadcrumb($variables) {
  if (drupal_is_front_page()) {
    return;
  }
  $breadcrumb = $variables['breadcrumb'];
  $contexts = array();

  if (!empty($breadcrumb)) {
    $output = '<h2 class="element-invisible">' . t('You are here') . '</h2>';

    $crumbs = '<ul class="breadcrumb">';
    if (!drupal_is_front_page()) {
      $crumbs .= '<li class="home-link"><a href="' . url('<front>') . '"><i class="fa fa fa-home"></i><span> Home</span></a></li>';
    }

    // Remove null values.
    $breadcrumb = array_filter($breadcrumb);
    $i = 1;
    foreach ($breadcrumb as $value) {
      if ($i == count($breadcrumb)) {
        $crumbs .= '<li class="active-trail">' . $value . '</li>';
      }
      else {
        $crumbs .= '<li>' . $value . '</li>';
      }
      $i++;
    }
    $crumbs .= '</ul>';
    return $crumbs;
  }
}

/**
 * Overrides theme_menu_local_tasks().
 */
function nuboot_menu_local_tasks(&$variables) {
  $output = '';

  if (!empty($variables['primary'])) {
    $variables['primary']['#prefix'] = '<h2 class="element-invisible">' . t('Primary tabs') . '</h2>';
    $variables['primary']['#prefix'] .= '<ul class="tabs--primary nav nav-pills">';
    $variables['primary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['primary']);
  }

  if (!empty($variables['secondary'])) {
    $variables['secondary']['#prefix'] = '<h2 class="element-invisible">' . t('Secondary tabs') . '</h2>';
    $variables['secondary']['#prefix'] .= '<ul class="tabs--secondary pagination pagination-sm">';
    $variables['secondary']['#suffix'] = '</ul>';
    $output .= drupal_render($variables['secondary']);
  }

  return $output;
}

/**
 * Overrides theme_menu_local_task().
 */
function nuboot_menu_local_task($variables) {
  $link = $variables['element']['#link'];
  $link_text = $link['title'];
  $icon_type = '';
  if (isset($link['path'])) {
    switch ($link['path']) {
      case 'node/%/edit':
        $icon_type = 'edit';
        break;

      case 'node/%/view':
        $icon_type = 'eye';
        break;

      case 'node/%/resource':
        $icon_type = 'plus';
        break;

      case 'node/%/datastore':
        $icon_type = 'cogs';
        break;

      case 'node/%/datastore/import':
        $icon_type = 'refresh';
        break;

      case 'node/%/datastore/drop':
        $icon_type = 'trash-o';
        break;

      case 'node/%/datastore/unlock':
        $icon_type = 'unlock';
        break;

      case 'node/%/download':
        $icon_type = 'download';
        break;

      case 'node/%/dataset':
        $icon_type = 'caret-left';
        break;

      case 'node/%/api':
        $icon_type = 'flask';
        break;

      case 'node/%/group':
        $icon_type = 'users';
        break;

      case 'node/%/members':
        $icon_type = 'user';
        break;

      case 'node/%/revisions':
        $icon_type = 'folder-open-o';
        break;

      case 'node/%/datastore/delete-items':
        $icon_type = 'eraser';
        break;

      default:
        $icon_type = '';
        break;
    }
  }
  $icon = '<i class="fa fa-lg fa-' . $icon_type . '"></i> ';
  $link_text = $icon . $link_text;
  $link['localized_options']['html'] = TRUE;

  if (!empty($variables['element']['#active'])) {
    // Add text to indicate active tab for non-visual users.
    $active = '<span class="element-invisible">' . t('(active tab)') . '</span>';

    // If the link does not contain HTML already, check_plain() it now.
    // After we set 'html'=TRUE the link will not be sanitized by l().
    if (empty($link['localized_options']['html'])) {
      $link['title'] = check_plain($link['title']);
    }
    $link['localized_options']['html'] = TRUE;
    $link_text = t('!local-task-title!active', array('!local-task-title' => $link['title'], '!active' => $active));

    $icon = '<i class="fa fa-lg fa-' . $icon_type . '"></i> ';
    $link_text = $icon . $link_text;
    // Ensure the HTML in $link_text is not escaped.
    $link['localized_options']['html'] = TRUE;
  }
  return '<li' . (!empty($variables['element']['#active']) ? ' class="active"' : '') . '>' . l($link_text, $link['href'], $link['localized_options']) . "</li>\n";
}

/**
 * Returns HTML for an inactive facet item.
 *
 *   An associative array containing the keys 'text', 'path', 'options', and
 *   'count'. See the l() and theme_facetapi_count() functions for information
 *   about these variables.
 *
 * @ingroup themeable
 */
function nuboot_facetapi_link_inactive($variables) {
  // Builds accessible markup.
  // @see http://drupal.org/node/1316580
  $accessible_vars = array(
    'text' => $variables['text'],
    'active' => FALSE,
  );

  $accessible_markup = theme('facetapi_accessible_markup', $accessible_vars);

  // Sanitizes the link text if necessary.
  $sanitize = empty($variables['options']['html']);
  $variables['text'] = ($sanitize) ? check_plain($variables['text']) : $variables['text'];

  // Adds count to link if one was passed.
  if (isset($variables['count'])) {
    $variables['text'] .= ' ' . theme('facetapi_count', $variables);
  }

  // Resets link text, sets to options to HTML since we already sanitized the
  // link text and are providing additional markup for accessibility.
  $variables['text'] .= $accessible_markup;
  $variables['options']['html'] = TRUE;
  return theme_link($variables);
}

/**
 * Returns HTML for an inactive facet item.
 *
 *   An associative array containing the keys 'text', 'path', and 'options'. See
 *   the l() function for information about these variables.
 *
 * @see l()
 *
 * @ingroup themeable
 */
function nuboot_facetapi_link_active($variables) {
  // Sanitizes the link text if necessary.
  $sanitize = empty($variables['options']['html']);
  $link_text = ($sanitize) ? check_plain($variables['text']) : $variables['text'];

  // Theme function variables fro accessible markup.
  // @see http://drupal.org/node/1316580
  $accessible_vars = array(
    'text' => $variables['text'],
    'active' => TRUE,
  );

  $accessible_markup = theme('facetapi_accessible_markup', $accessible_vars);
  $variables['text'] .= $accessible_markup;
  $variables['options']['html'] = TRUE;
  return theme_link($variables);
}

/**
 * Theme social icons.
 */
function nuboot_sitewide_social_block() {
  $path = isset($_GET['q']) ? $_GET['q'] : '<front>';
  $link = url($path, array('absolute' => TRUE));

  $output = array(
    '#theme' => 'item_list',
    '#items' => array(
      'googleplus' => array(
        'data' => l('<i class="fa fa-lg fa-google-plus-square"></i> ' . t('Google+'),
        'https://plus.google.com/share', array(
          'query' => array(
            'url' => $link,
          ),
          'attributes' => array(
            'target' => '_blank',
          ),
          'html' => TRUE,
        )),
        'class' => array('nav-item'),
      ),
      'twitter' => array(
        'data' => l('<i class="fa fa-lg fa-twitter-square"></i> ' . t('Twitter'),
        'https://twitter.com/share', array(
          'query' => array(
            'url' => $link,
          ),
          'attributes' => array(
            'target' => '_blank',
          ),
          'html' => TRUE,
        )),
        'class' => array('nav-item'),
      ),
      'facebook' => array(
        'data' => l('<i class="fa fa-lg fa-facebook-square"></i> ' . t('Facebook'),
        'https://www.facebook.com/sharer.php', array(
          'query' => array(
            'u' => $link,
          ),
          'attributes' => array(
            'target' => '_blank',
          ),
          'html' => TRUE,
        )),
        'class' => array('nav-item'),
      ),
    ),
    '#attributes' => array(
      'class' => array('nav', 'nav-simple', 'social-links'),
    ),
  );

  return $output;
}

/**
 * Implements hook_form_alter().
 */
function nuboot_form_alter(&$form, &$form_state, $form_id) {
  switch ($form_id) {
    case 'colorizer_admin_settings':
      $form['colorizer_global']['colorizer_cssfile']['#default_value'] = 'colorizer/colorizer.css';
      $form['colorizer_global']['colorizer_incfile']['#default_value'] = 'colorizer/colorizer.inc';
      break;

  }
}

/**
 * Overrides theme_file_widget().
 *
 * https://drupal.org/files/issues/bootstrap-undefined-index-2177089-1.patch
 */
function nuboot_file_widget($variables) {
  $element = $variables['element'];
  $output = '';

  $hidden_elements = array();
  foreach (element_children($element) as $child) {
    if (isset($element[$child]['#type']) && $element[$child]['#type'] === 'hidden') {
      $hidden_elements[$child] = $element[$child];
      unset($element[$child]);
    }
  }

  $element['upload_button']['#prefix'] = '<span class="input-group-btn">';
  $element['upload_button']['#suffix'] = '</span>';

  // The "form-managed-file" class is required for proper Ajax functionality.
  $output .= '<div class="file-widget form-managed-file clearfix input-group">';
  if (!empty($element['fid']['#value'])) {
    // Add the file size after the file name.
    $element['filename']['#markup'] .= ' <span class="file-size">(' . format_size($element['#file']->filesize) . ')</span> ';
  }
  $output .= drupal_render_children($element);
  $output .= '</div>';
  $output .= render($hidden_elements);
  return $output;
}
