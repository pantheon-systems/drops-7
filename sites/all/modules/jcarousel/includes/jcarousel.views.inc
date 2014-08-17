<?php

/**
 * @file
 * Views integration for jCarousel module.
 */

/**
 * Implements hook_views_plugin().
 */
function jcarousel_views_plugins() {
  $plugins['style']['jcarousel'] = array(
    'title' => t('jCarousel'),
    'help' => t('Display rows in a carousel via jCarousel.'),
    'handler' => 'jcarousel_style_plugin',
    'path' => drupal_get_path('module', 'jcarousel') . '/includes',
    'theme' => 'jcarousel_view',
    'theme path' => drupal_get_path('module', 'jcarousel') . '/includes',
    'uses row plugin' => TRUE,
    'uses options' => TRUE,
    'uses grouping' => FALSE,
    'type' => 'normal',
  );
  return $plugins;
}

/**
 * Menu callback; Handle AJAX Views requests for carousels.
 */
function jcarousel_views_ajax() {
  if (isset($_REQUEST['view_name']) && isset($_REQUEST['view_display_id'])) {
    $name = $_REQUEST['view_name'];
    $display_id = $_REQUEST['view_display_id'];
    $args = isset($_REQUEST['view_args']) && $_REQUEST['view_args'] !== '' ? explode('/', $_REQUEST['view_args']) : array();
    $path = isset($_REQUEST['view_path']) ? $_REQUEST['view_path'] : NULL;
    $dom_id = isset($_REQUEST['jcarousel_dom_id']) ? intval($_REQUEST['jcarousel_dom_id']) : NULL;
    $first = isset($_REQUEST['first']) ? intval($_REQUEST['first']) : NULL;
    $last = isset($_REQUEST['last']) ? intval($_REQUEST['last']) : NULL;
    views_include('ajax');
    $object = new stdClass();

    $object->status = FALSE;
    $object->display = '';

    $arg = explode('/', $_REQUEST['view_path']);

    // Load the view.
    if ($view = views_get_view($name)) {
      $view->set_display($display_id);

      if ($view->access($display_id)) {

        // Fix 'q' for paging.
        if (!empty($path)) {
          $_GET['q'] = $path;
        }

        // Disable the pager, render between the start and end values.
        // Views 2:
        if (isset($view->pager)) {
          $view->pager['use_pager'] = FALSE;
          $view->pager['offset'] = $first;
          $view->pager['items_per_page'] = $last - $first;

          $view->display[$display_id]->handler->set_option('use_pager', 0);
          $view->display[$display_id]->handler->set_option('offset', $first);
          $view->display[$display_id]->handler->set_option('items_per_page', $last - $first);
        }
        // Views 3:
        else {
          $view->set_items_per_page($last - $first);
          $view->set_offset($first);

          // Redundant but apparently needed.
          $view->items_per_page = $last - $first;
          $view->offset = $first;
        }

        // Reuse the same DOM id so it matches that in Drupal.settings.
        $view->jcarousel_dom_id = $dom_id;

        $errors = $view->validate();
        if ($errors === TRUE) {
          $object->status = TRUE;
          $object->title = $view->get_title();
          $object->display .= $view->preview($display_id, $args);
        }
        else {
          foreach ($errors as $error) {
            drupal_set_message($error, 'error');
          }
        }
      }
    }
    $messages = theme('status_messages');
    $object->messages = $messages ? '<div class="views-messages">' . $messages . '</div>' : '';

    drupal_json_output($object);
  }
}

/**
 * Adds necessary CSS and JS for Views-based carousels.
 */
function jcarousel_views_add($view, $display_id = NULL) {
  static $dom_counter = 0;

  if (!isset($display_id)) {
    $display_id = empty($view->current_display) ? 'default' : $view->current_display;
  }

  // Save the settings for the carousel, these will be used by the JavaScript.
  $options = array();

  // Keep track of each settings addition and give a unique ID. This can be
  // useful when displaying the same view multiple times with different
  // arguments (i.e. such as in a panel).
  $options['view_options'] = array(
    'view_args' => check_plain(implode('/', $view->args)),
    'view_path' => check_plain($_GET['q']),
    'view_base_path' => $view->get_path(),
    'view_display_id' => $display_id,
    'view_name' => $view->name,
    'jcarousel_dom_id' => isset($view->jcarousel_dom_id) ? $view->jcarousel_dom_id : ++$dom_counter,
  );

  foreach ($view->style_plugin->options as $key => $option) {
    if ($option) {
      $options[$key] = is_numeric($option) ? (int) $option : $option;
    }
  }

  // By default limit the scrolling to the same number of items as are visible
  // to avoid display glitches.
  if (empty($options['scroll']) && !empty($options['visible'])) {
    $options['scroll'] = $options['visible'];
  }

  // By default limit the scrolling to the same number of items as are visible
  // to avoid display glitches.
  if (empty($options['scroll']) && !empty($options['visible'])) {
    $options['scroll'] = $options['visible'];
  }

  // Get the total number of items in this view.
  $count_query = $view->build_info['count_query']->countQuery();
  $count = $count_query->execute()->fetchField();

  // Views may already populate total_rows later, but since we've already
  // generated this value, we might as well make it available.
  $view->total_rows = $count;

  // If there is just one item disable the auto-scroll and rotation.
  if ($count == 1) {
    $options['wrap'] = NULL;
    $options['auto'] = 0;
  }

  // Determine AJAX functionality in a backwards-compatible way. Versions prior
  // to jCarousel 2.6 used the view-level "Use AJAX" option instead of a style
  // setting. We check $view->style_options here intentionally instead of
  // $view->style_plugin->options.
  $use_ajax = isset($view->style_options['ajax']) ? $view->style_options['ajax'] : $view->use_ajax;

  // If using AJAX, adjust the view's positioning based on the current page.
  if ($use_ajax) {
    $options['ajax'] = TRUE;
    $options['size'] = $count;

    // Views 2:
    if (isset($view->pager)) {
      // Enable and adjust the pager to get the correct page.
      $use_pager = $view->pager['use_pager'];
      $view->pager['use_pager'] = TRUE;
      $view->build($display_id);
      $view->pager['use_pager'] = $use_pager;

      // Create generic variable names.
      $pager_current_page = $view->pager['current_page'];
      $pager_items_per_page = $view->pager['items_per_page'];
      $pager_offset = $view->pager['offset'];
    }
    // Views 3:
    else {
      // Adjusting the query is not necessary.
      $view->build($display_id);

      // Create generic variable names.
      $pager_current_page = $view->current_page;
      $pager_items_per_page = $view->items_per_page;
      $pager_offset = $view->offset;
    }

    // If starting in the middle of a view, initialize the carousel at that
    // position. Strangely the carousel must pre-load empty LI items all the way
    // up until the active item, making this inefficient for large lists.
    if ($pager_current_page) {
      // TODO: Pagers and carousels do not work well together. jCarousel should
      // give items the class "jcarousel-item-[offset]", but instead it always
      // starts with "1", making it impossible to define a prepopulated list
      // as the middle of an AJAX view.
      $options['start'] = ($pager_current_page * $pager_items_per_page) + ($pager_offset + 1);
      $options['offset'] = ($pager_current_page * $pager_items_per_page) + ($pager_offset + 1);
    }
    elseif ($pager_offset) {
      $options['start'] = $pager_offset + 1;
      $options['offset'] = $pager_offset + 1;
    }
  }

  $identifier = drupal_clean_css_identifier('jcarousel_dom_' . $options['view_options']['jcarousel_dom_id']);
  return jcarousel_add($identifier, $options);
}

/**
 * Preprocess function for jcarousel-view.tpl.php.
 */
function template_preprocess_jcarousel_view(&$variables) {
  $view = $variables['view'];
  $display_id = empty($view->current_display) ? 'default' : $view->current_display;

  // Add necessary JavaScript and CSS.
  $attachments = jcarousel_views_add($view, $display_id);

  // Find the JavaScript settings in the returned array.
  foreach ($attachments['js'] as $data) {
    if (isset($data['type']) && $data['type'] == 'setting') {
      $settings = reset($data['data']['jcarousel']['carousels']);
    }
  }

  // Build the list of classes for the carousel.
  $options = $view->style_plugin->options;
  $variables['jcarousel_classes_array'] = array(
    'jcarousel',
    drupal_clean_css_identifier('jcarousel-view--' . $view->name . '--' . $display_id),
    drupal_clean_css_identifier('jcarousel-dom-' . $settings['view_options']['jcarousel_dom_id']),
  );
  if (!empty($options['skin'])) {
    $variables['jcarousel_classes_array'][] = 'jcarousel-skin-' . $options['skin'];
  }
  $variables['jcarousel_classes'] = implode(' ', $variables['jcarousel_classes_array']);

  // Views 2/3 compatibility.
  $pager_offset = isset($view->pager['offset']) ? $view->pager['offset'] : $view->offset;

  // Give each item a class to identify where in the carousel it belongs.
  foreach ($variables['rows'] as $id => $row) {
    $number = $id + 1 + $pager_offset;
    $zebra = ($number % 2 == 0) ? 'even' : 'odd';
    $variables['row_classes'][$id] = 'jcarousel-item-' . $number . ' ' . $zebra;
  }
}
