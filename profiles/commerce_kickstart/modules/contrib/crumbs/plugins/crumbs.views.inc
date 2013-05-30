<?php


/**
 * Implements hook_crumbs_plugins().
 */
function views_crumbs_plugins($api) {
  $api->multiPlugin('argTitle');
}


class views_CrumbsMultiPlugin_argTitle implements crumbs_MultiPlugin {

  function describe($api) {
    // We try to get all views pages but avoid views_get_all_view() for performance.
    $q = db_select('menu_router', 'mr');
    $q->condition('page_callback', 'views_page');
    $q->fields('mr', array('path', 'page_arguments'));
    foreach ($q->execute() as $row) {
      $args = unserialize($row->page_arguments);
      if (count($args) > 2) {
        list($view_name, $view_display_id) = $args;
        $api->addRule("$view_name.$view_display_id", $row->path);
      }
    }
  }

  function findTitle($path, $item) {

    if ('views_page' !== $item['page_callback']) {
      return;
    }

    if (count($item['page_arguments']) < 3) {
      return;
    }

    $args = $item['page_arguments'];
    $view_name = array_shift($args);
    $view_display_id = array_shift($args);

    // Build and initialize the view.
    $view = views_get_view($view_name);
    $view->set_display($view_display_id);
    $view->set_arguments($args);

    // Trigger the argument calculation by calling build_title().
    $view->build_title();

    // Check the last argument for a breadcrumb item title.
    $last_arg = end($view->argument);
    if (!is_object($last_arg)) {
      return;
    }
    elseif ($last_arg->is_exception()) {
      return;
      // $last_arg_title = $last_arg->exception_title();
    }
    else {
      $last_arg_title = $last_arg->get_title();
    }

    return array("$view_name.$view_display_id" => $last_arg_title);
  }
}
