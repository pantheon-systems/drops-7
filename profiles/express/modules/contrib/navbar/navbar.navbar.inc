<?php

/**
 * @file
 * Navbar hooks implemented by the Navbar module.
 */

/**
 * Implements hook_navbar().
 */
function navbar_navbar() {
  global $user;

  $items = array();

  // Provides a simple link to escape the administration area.
  $items['home'] = array(
    '#type' => 'navbar_item',
    'tab' => array(
      '#type' => 'link',
      '#title' => t('Back to site'),
      '#href' => '<front>',
      '#options' => array(
        'attributes' => array(
          'title' => t('Return to site content'),
          'class' => array(
            'navbar-icon',
            'navbar-icon-escape-admin',
            'element-hidden'
          ),
          'data-navbar-escape-admin' => NULL,
        ),
      ),
    ),
    '#weight' => -20,
  );

  // Retrieve the administration menu from the database.
  $tree = navbar_get_menu_tree();

  // If no menu links exist because the user does not have access to any, then
  // don't render a menu tab.
  if (!empty($tree)) {

    // Add attributes to the links before rendering.
    navbar_menu_navigation_links($tree);

    $menu = array(
      '#heading' => t('Administration menu'),
      'navbar_administration' => array(
        '#type' => 'container',
        '#attributes' => array(
          'class' => array('navbar-menu-administration'),
        ),
        'administration_menu' => menu_tree_output($tree),
      ),
    );

    // This flow is commented out until we figure out how to return a JSONP
    // response in Drupal 7.
    //
    // To conserve bandwidth, we only include the top-level links in the HTML.
    // The subtrees are included in a JSONP script, cached by the browser. Here we
    // add that JSONP script. We add it as an external script, because it's a
    // Drupal path, not a file available via a stream wrapper.
    // @see navbar_subtrees_jsonp()
    // $menu['navbar_administration']['#attached']['js'][url('navbar/subtrees/' . _navbar_get_subtree_hash())] = array('type' => 'external');
    // The administration element has a link that is themed to correspond to
    // a navbar tray. The tray contains the full administrative menu of the site.
    $items['administration'] = array(
      '#type' => 'navbar_item',
      'tab' => array(
        '#type' => 'link',
        '#title' => t('Menu'),
        '#href' => 'admin',
        '#options' => array(
          'attributes' => array(
            'title' => t('Admin menu'),
            'class' => array('navbar-icon', 'navbar-icon-menu',),
          ),
        ),
      ),
      'tray' => $menu,
      '#weight' => -15,
    );
  }

  // Add logout & user account links or login link.
  if (module_exists('user')) {
    if ($user->uid) {
      $links = array(
        'account' => array(
          'title' => t('View profile'),
          'href' => 'user',
          'html' => TRUE,
          'attributes' => array(
            'title' => t('User account'),
          ),
        ),
        'logout' => array(
          'title' => t('Log out'),
          'href' => 'user/logout',
        ),
      );
    }
    else {
      $links = array(
        'login' => array(
          'title' => t('Log in'),
          'href' => 'user',
        ),
      );
    }

    $items['user'] = array(
      '#type' => 'navbar_item',
      'tab' => array(
        '#type' => 'link',
        '#title' => format_username($user),
        '#href' => 'user',
        '#options' => array(
          'attributes' => array(
            'title' => t('My account'),
            'class' => array('navbar-icon', 'navbar-icon-user',),
          ),
        ),
      ),
      'tray' => array(
        '#heading' => t('User account actions'),
        'user_links' => array(
          '#theme' => 'links__navbar_user',
          '#links' => $links,
          '#attributes' => array(
            'class' => array('navbar-menu', 'navbar-menu-user'),
          ),
        ),
      ),
      '#weight' => 100,
    );
  }

  // Add a shortcuts tab.
  if (module_exists('shortcut')) {
    $links = shortcut_renderable_links();
    $shortcut_set = shortcut_current_displayed_set();
    $configure_link = NULL;
    if (shortcut_set_edit_access($shortcut_set)) {
      $configure_link = array(
        '#type' => 'link',
        '#title' => t('Edit shortcuts'),
        '#href' => 'admin/config/user-interface/shortcut/' . $shortcut_set->set_name,
        '#options' => array('attributes' => array('class' => array('edit-shortcuts'))),
      );
    }
    if (!empty($links) || !empty($configure_link)) {
      $items['shortcuts'] = array(
        '#type' => 'navbar_item',
        'tab' => array(
          '#type' => 'link',
          '#title' => t('Shortcuts'),
          '#href' => 'admin/config/user-interface/shortcut',
          '#options' => array(
            'attributes' => array(
              'title' => t('Shortcuts'),
              'class' => array('navbar-icon', 'navbar-icon-shortcut',),
            ),
          ),
        ),
        'tray' => array(
          '#heading' => t('User-defined shortcuts'),
          '#wrapper_attributes' => array(
            'class' => array('navbar-tray-shortcuts', 'navbar-menu-shortcuts'),
          ),
          'shortcuts' => $links,
          'configure' => $configure_link,
        ),
        '#weight' => -10,
      );
    }
  }

  return $items;
}
