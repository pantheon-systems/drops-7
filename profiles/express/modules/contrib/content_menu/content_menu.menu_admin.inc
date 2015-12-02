<?php

/**
 * @file
 * Menu Management Workflow improvements.
 */

/**
 * Callback for altered admin/structure/menu router item.
 *
 * Only show custom menus to regular sitebuilders/editors (no system menus).
 */
function _content_menu_menu_overview_page_extended() {
  $result = db_query("SELECT * FROM {menu_custom} ORDER BY title", array(), array('fetch' => PDO::FETCH_ASSOC));
  $header = array(t('Title'), array('data' => t('Operations'), 'colspan' => '1'));
  $rows = array();
  foreach ($result as $menu) {
    if (content_menu_is_menu_considered($menu['menu_name']) || user_access('administer system menus')) {
      $url = 'admin/structure/menu/manage/' . $menu['menu_name'];
      $row = array(theme('menu_admin_overview', array('title' => $menu['title'], 'name' => $menu['menu_name'], 'description' => $menu['description'])));
      $row[] = array('data' => l(t('Edit'), $url));
      $rows[] = $row;
    }
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}


/**
 * Actual implementation for content_menu_form_menu_overview_form_alter().
 *
 * Overhaul the menu_overview_form to improve menu authoring experience.
 */
function _content_menu_form_menu_overview_form_alter(&$form, &$form_state, $form_id) {
  $form['#theme'] = 'menu_overview_form_extended';

  // Check whether any filtering element should be acitvated.
  $filtering_active = TRUE;
  // Filtering depends on ctools module.
  // @todo Remove ctools dependency as ajax is actually not used.
  $filtering_active = $filtering_active && module_exists('ctools');

  if ($filtering_active) {
    // Add form elements for filters.
    // @todo Add configuration interface for filters.
    require_once 'filters/content_menu_domain_filter.php';
    require_once 'filters/content_menu_language_filter.php';
    $filters = array(new content_menu_domain_filter(), new content_menu_language_filter($form['#menu']['menu_name']));
    // Add filter widgets to form.
    foreach ($filters as $f) {
      $f->addFilterWidget($form, $form_state, $form_id);
    }
  }

  // ToDo: Clean up and untangle operations-changing and target_content setting.
  $hidden = array();
  // Adjust operations for each item.
  foreach (element_children($form) as $mlid) {
    $el = &$form[$mlid];
    if ($filtering_active) {
      foreach ($filters as $f) {
        if ($f->hideElement($el)) {
          unset($form[$mlid]);
          $hidden[$el['#item']['mlid']] = $el['#item']['mlid'];
          continue;
        }
      }
    }
    if (isset($el['#item']['plid']) &&
      isset($hidden[$el['#item']['plid']])) {
      // Check if a parent has been disabled before.
      // If so, the childern will be hidden too.
      unset($form[$mlid]);
      $hidden[$el['#item']['mlid']] = $el['#item']['mlid'];
      continue;
    }

    $el['#inline_edit_title'] = TRUE;
    if (isset($form[$mlid]['operations'])) {
      // Invoke hook to let modules provide target operations for new item.
      drupal_alter('menu_item_element', $el);

      // Make sure that operations are ordered by their weight, then unset it.
      uasort($el['operations'], 'drupal_sort_weight');
      foreach ($el['operations'] as $el_op_key => $el_op) {
        unset($el['operations'][$el_op_key]['weight']);
      }

      // Wrap link-type operations into CTools dropbutton, if available.
      if (module_exists('ctools') && isset($el['operations'])) {
        // Split up operation in dropbutton-ops and other ops.
        $other_operations  = array();
        $dropbutton_operations = array();
        foreach ($form[$mlid]['operations'] as $op_key => $op_item) {
          if (is_array($op_item) && isset($op_item['#type'])) {
            if ($op_item['#type'] == 'link') {
              $dropbutton_operations[$op_key] = array(
                'title' => $op_item['#title'],
                'href'  => $op_item['#href'],
              );
              if (isset($op_item['#options'])) {
                $dropbutton_operations[$op_key] += $op_item['#options'];
              }
            }
            else {
              $other_operations[$op_key] = $op_item;
            }
          }
        }

        // Compose operation from dropbutton and other ops.
        $el['operations'] = array();
        $el['operations']['dropbutton_operations'] = array(
          '#theme' => 'links__ctools_dropbutton',
          '#links' => $dropbutton_operations,
        );
        $el['operations'] += $other_operations;
      }

      // Count operations of this item.
      $el['#operations_count'] = count($form[$mlid]['operations']);
    }

    // Add inline-editing for menu item title.
    if ($el['#inline_edit_title'] && isset($el['title']) && (!isset($el['title']['#type']) || $el['title']['#type'] != 'textfield')) {
      // ToDo: Check title inplace-edit for multilanguage compatibility issues.
      $title = $el['#item']['title'];
      $el['title'] = array(
        '#type' => 'textfield',
        '#default_value' => $title,
        '#size' => 30,
        '#maxlength' => 255,
        '#prefix' => '<span class="menu-overview-title-link">' . $el['title']['#markup'] . '</span>',
        '#suffix' => '<a href="javascript:void();" title="Click to edit title" class="menu-overview-title-edit-trigger">' . t('edit title') . '</a>',
      );
      $form['#attached']['js'] = array(drupal_get_path('module', 'content_menu') . '/content_menu.js');
    }

  }

  // Add "add item" item/row/elements.
  if ($form_id != 'bigmenu_slice_form') {
    content_menu_form_menu_overview_form_alter_additem($form);
  }

  // Add css as via drupal_add_css and put it after ctools button css files.
  drupal_add_css(drupal_get_path('module', 'content_menu') . "/css/content_menu.css", array('weight' => 2));

  // Change submit handler function to our own implementation.
  $form['#submit'] = array('content_menu_menu_overview_form_submit_extended');
  // Prevent 'destination' from hijacking our submit.
  unset($_GET['destination']);

  if (!isset($form['submit'])) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save Configuration'),
    );
  }
}


/**
 * Menu item element alterations provided by default by this module.
 *
 * This helper function is called by this module's hook_menu_item_element_alter
 * implementation content_menu_menu_item_element_alter().
 *
 * @param $el Array The form element of the menu item to alter.
 */
function _content_menu_menu_item_element_alter(&$el) {
  $el_item = &$el['#item'];
  $el_ops = &$el['operations'];
  $target_content = '';

  $link_path = ($el_item['link_path'] == '<front>') ? variable_get('site_frontpage', 'node') : $el_item['link_path'];
  $link_path = (array) explode('/', $link_path);
  $link_path = array_pad($link_path, 2, '');

  // Change "edit" operation.
  if (isset($el_ops['edit']) && ($el_ops['edit']['#type'] == 'link')) {

    $el_ops['edit']['weight'] = 1;

    // Add "select content" operation.
    $selectcontenturl = content_menu_variable_get_add_existing_content_url();
    if ($selectcontenturl) {
      $el_ops['selectcontent'] = array(
        '#type' => 'link',
        'weight' => 2,
        '#title' => t('select content'),
        '#href' => $selectcontenturl,
        '#options' => array(
          'query' => content_menu_assemble_query_string($el_item),
        ),
      );
    }

    // Add "new <node-type>" operation to Dummies.
    if ($link_path[0] == 'menu-dummy') {
      $node_type_count = 0;
      foreach (node_type_get_types() as $node_type) {
        $type = $node_type->type;
        if (user_access('create ' . $type . ' content')) {
          $node_menu_options = variable_get('menu_options_' . $type, array('main-menu' => 'main-menu'));
          if (in_array($el_item['menu_name'], $node_menu_options)) {
            $el_ops['new_' . $type] = array(
              '#type' => 'link',
              'weight' => 10 + $node_type_count,
              '#title' => t('New @type', array('@type' => t($node_type->name))),
              '#href' => 'node/add/' . str_replace("_", "-", $type),
              '#options' => array(
                'query' => content_menu_assemble_query_string($el_item),
              ),
            );
          }
        }
      }
    }

    // Add "configure" operation by cloning "edit" operation.
    $el_ops['configure'] = $el_ops['edit'];
    $el_ops['configure']['#title'] = t('configure item');
    $el_ops['configure']['weight'] = 3;

    // Change "edit" operation to ...
    // ... do nothing for menu item dummies.
    if ($link_path[0] == 'menu-dummy') {
      $target_content = t('Placeholder');
    }
    // ... lead to node edit form for node-url-items.
    elseif (($link_path[0] == 'node') && (is_numeric($link_path[1]))) {
      $node = node_load($link_path[1]);
      if (is_object($node)) {
        if (node_access('update', $node)) {
          $el_ops['edit']['#href'] = 'node/' . $link_path[1] . '/edit';
        }
        $node_type = node_type_get_type($node);
        $target_content = t($node_type->name);
        // Mark unpublished nodes.
        if (!$node->status) {
          $target_content = '<span class="menu-item-content-target-unpublished">' . $target_content . '</span>';
        }
      }
    }
    // ... lead to panel page edit form for panel paged.
    elseif (($el_item['page_callback'] == 'page_manager_page_execute')) {
      if (user_access('use page manager')) {
        $page_arguments = unserialize($el_item['page_arguments']);
        $el_ops['edit']['#href'] = 'admin/structure/pages/edit/page-' . implode('/', $page_arguments);
      }
      $target_content = t('Panel');
    }
    // ... lead to view edit form for views.
    elseif ($el_item['page_callback'] == 'views_page') {
      if (user_access('administer views')) {
        $page_arguments = unserialize($el_item['page_arguments']);
        $el_ops['edit']['#href'] = 'admin/structure/views/view/' . $page_arguments[0] . '/edit/' . $page_arguments[1];
      }
      $el_ops['selectcontent']['#href'] = CONTENT_MENU_ADD_EXISTING_VIEW_URL;
      $el_ops['selectcontent']['#title'] = t('select view');
      $target_content = t('View');
    }
    // ... menu position rule.
    elseif ($link_path[0] == 'menu-position') {
      if (!user_access('administer menu positions')) {
        $el_ops['configure'] = $el_ops['edit'] = array();
      }
      $target_content = t('Menu rule');
      $el['#inline_edit_title'] = FALSE;
    }
    else {
      // If no other content or target is identifier, assume generic URL.
      if (url_is_external($el_item['link_path'])) {
        // External.
        $target_content = t('URL');
      }
      else {
        // Internal.
        $target_content = t('Path');
      }
    }

    // Delete the "edit" operation altogether if its url didn't change.
    if (isset($el_ops['edit']['#href'])) {
      if ($el_ops['edit']['#href'] == $el_ops['configure']['#href']) {
        $el_ops['edit'] = array();
        $el_ops['selectcontent']['weight'] = 4;
      }
      else {
        $el_ops['edit']['#title'] = t('edit content');
        $el_ops['edit']['#options']['query']['destination'] = current_path();
      }
    }
  }
  else {
    $el_ops['edit'] = array();
  }

  // Add "delete" operation for menu_position items.
  if (!isset($el_ops['delete']) && ($link_path[0] == 'menu-position') && (is_numeric($link_path[1]) && user_access('administer menu positions'))) {
    $el_ops['delete'] = array(
      '#type' => 'link',
      '#title' => t('delete'),
      '#href' => 'admin/structure/menu-position/delete/' . intval($link_path[1]),
      '#options' => array('query' => array('destination' => current_path())),
    );
  }

  // Push "delete" operation to end of ops list.
  if (isset($el_ops['delete'])) {
    $el_ops['delete']['#title'] = t('delete item');
    $el_ops['delete']['weight'] = 100;
  }

  $el['target_content'] = array('#type' => 'markup', '#markup' => $target_content);

}


/**
 * Add item/row/elements to menu overview form for "add item" functionality.
 */
function content_menu_form_menu_overview_form_alter_additem(&$form) {
  // Assemble and add the "Add link" item.
  $form['additem'] = array(
    '#attributes' => array(
      'class' => array('menu-additem'),
    ),
    'hidden' => array(
      '#type' => 'checkbox',
      '#title' => t('Enable the new menu link'),
      '#title_display' => 'invisible',
      '#default_value' => TRUE,
    ),
    'weight' => array(
      '#type' => 'weight',
      // '#delta' has to be in sync with:
      // a) CONTENT_MENU_ADD_ITEM_WEIGHT.
      // b) Delta set in menu_form_node_form_alter().
      '#delta' => 50,
      // Real weight is set in content_menu_menu_overview_form_submit_extended.
      '#default_value' => CONTENT_MENU_ADD_ITEM_WEIGHT,
      '#title_display' => 'invisible',
      '#title' => 'Weight',
    ),
    'mlid' => array(
      '#type' => 'hidden',
      '#value' => '0',
    ),
    'plid' => array(
      '#type' => 'hidden',
      '#default_value' => '0',
    ),
    'operations' => array(),
    '#operations_count' => 0,
    'title' => array(
      '#type' => 'textfield',
      '#title' => t('New menu item'),
      '#default_value' => '',
      '#size' => 30,
      '#maxlength' => 255,
      '#required' => FALSE,
    ),
  );

  $target_types = content_menu_get_target_types($form['#menu']['menu_name']);

  // Retrieve possible add-"type"-operations for "Add link" item.
  foreach ($target_types as $op_key => $op_info) {
    $target_types[$op_key] = $op_info['label'];
  }

  // Build "type" select for "Add link" item.
  $form['additem']['target_content'] = array(
    '#type'      => 'select',
    '#title'     => '&nbsp;',
    '#options'   => $target_types,
    '#default_value' => variable_get('content_menu_default_add_operation', 'url'),
  );
}


/**
 * Returns HTML for the menu overview form into a table.
 *
 * Code taken mainly from original theme_menu_overview_form().
 *
 * @param $variables
 *   An associative array containing:
 *   - form: A render element representing the form.
 *
 * @ingroup themeable
 */
function theme_menu_overview_form_extended($variables) {
  if (module_exists('bigmenu')) {
    drupal_add_js(drupal_get_path('module', 'bigmenu') . '/bigmenu.js');
    drupal_add_css(drupal_get_path('module', 'bigmenu') . '/bigmenu.css');
  }
  $form = $variables['form'];

  drupal_add_tabledrag('menu-overview', 'match', 'parent', 'menu-plid', 'menu-plid', 'menu-mlid', TRUE, MENU_MAX_DEPTH - 1);
  drupal_add_tabledrag('menu-overview', 'order', 'sibling', 'menu-weight');

  // Count max number of operations per item.
  $operations_count_max = 1;
  foreach (element_children($form) as $mlid) {
    if (isset($form[$mlid]['#operations_count'])) {
      $operations_count_max = max($operations_count_max, $form[$mlid]['#operations_count']);
    }
  }

  $header = array(
    t('Menu link'),
    array('data' => t('Enabled'), 'class' => array('checkbox')),
    t('Weight'),
    t('Target'),
    array('data' => t('Operations'), 'colspan' => $operations_count_max),
  );

  // Build item tables.
  $rows = array();
  foreach (element_children($form) as $mlid) {
    if (isset($form[$mlid]['hidden'])) {
      $element = &$form[$mlid];
      // Build a list of operations.
      $operations = array();
      foreach (element_children($element['operations']) as $op) {
        $operations[] = array('data' => drupal_render($element['operations'][$op]), 'class' => array('menu-operations'));
      }
      while (count($operations) < $operations_count_max) {
        $operations[] = '';
      }

      // Add special classes to be used for tabledrag.js.
      $element['plid']['#attributes']['class'] = array('menu-plid');
      $element['mlid']['#attributes']['class'] = array('menu-mlid');
      $element['weight']['#attributes']['class'] = array('menu-weight');

      // Change the parent field to a hidden.
      // This allows any value but hides the field.
      $element['plid']['#type'] = 'hidden';
      $depth = isset($element['#item']) ? $element['#item']['depth'] : 1;

      $row = array();
      $row[] = theme('indentation', array('size' => $depth - 1)) . drupal_render($element['title']);
      $row[] = array('data' => drupal_render($element['hidden']), 'class' => array('checkbox', 'menu-enabled'));
      $row[] = drupal_render($element['weight']) . drupal_render($element['plid']) . drupal_render($element['mlid']);
      $row[] = array('data' => drupal_render($element['target_content']), 'class' => array('menu-target-content'));
      $row = array_merge($row, $operations);

      $row = array_merge(array('data' => $row), $element['#attributes']);
      $row['class'][] = 'draggable';
      // Highlight row if menu item has been created within the last minute.
      if (isset($element['#item']['mlid']) && isset($_SESSION['content_menu_inserted_links'][$element['#item']['mlid']])) {
        if ((time() - $_SESSION['content_menu_inserted_links'][$element['#item']['mlid']]['created']) <= 60) {
          $row['class'][] = 'ok';
        }
        unset($_SESSION['content_menu_inserted_links'][$element['#item']['mlid']]);
      }
      $rows[] = $row;
    }
  }
  $output = '';
  if (empty($rows)) {
    $rows[] = array(array('data' => $form['#empty_text'], 'colspan' => '7'));
  }
  // Render selection filter widgets on top.
  if (isset($form['#content_menu_filter_widget'])) {
    foreach ($form['#content_menu_filter_widget'] as $widgetname) {
      $output .= drupal_render($form[$widgetname]);
    }
  }
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'menu-overview')));
  $output .= drupal_render_children($form);
  return $output;
}


/**
 * Submit handler for menu_overview_form extended with item adding handling.
 */
function content_menu_menu_overview_form_submit_extended($form, &$form_state) {
  // Edit item handling.
  $max_weight = 0;
  // Step though items.
  foreach (element_children($form) as $mlid) {
    $el = &$form[$mlid];
    // Keep track of maximum weight.
    if (isset($el['#item']['weight']) && ($el['#item']['weight'] > $max_weight)) {
      $max_weight = $el['#item']['weight'];
    }
    // If title changed, ...
    if (isset($el['#item']) && isset($el['title']['#value'])) {
      if ($el['title']['#value'] != $el['title']['#default_value']) {
        // Inject item title.
        $title = $form_state['input'][$mlid]['title'];
        $el['#item']['title'] = $el['#item']['link_title'] = $title;
        // Change default value of weight field to trigger
        // menu_overview_form_submit() marking the item as updated.
        $el['weight']['#default_value'] += 1;
      }
    }
  }

  // Do standard menu.module's item processing.
  menu_overview_form_submit($form, $form_state);

  // Add item handling.
  $item = $form_state['input']['additem'];

  // If title has been entered for new menu item, prepare add operation.
  if (trim($item['title'] != '')) {
    // Complete data for new menu item.
    $item['link_title'] = $item['title'];
    $item['name'] = $form['#menu']['menu_name'];
    $item['hidden'] = $item['hidden'] ? 0 : 1;
    $item['customized'] = 1;
    if ($item['weight'] == CONTENT_MENU_ADD_ITEM_WEIGHT) {
      $item['weight'] = $max_weight + 1;
    }

    // Determine operation details.
    $operations = content_menu_get_target_types($item['name']);
    $op = $operations[$item['target_content']];

    // Prepare arguments for handler callback function.
    $args = array('form' => $form, 'form_state' => $form_state, 'item' => $item);
    if (isset($op['arguments']) && is_array($op['arguments'])) {
      $args += $op['arguments'];
    }

    // Call handler callback function.
    if (function_exists($op['handler'])) {
      // Get rid of the 'Your configuration has been saved.' message.
      if ($op['clean_menu_save_message']) {
        foreach ($_SESSION['messages']['status'] as $msg_key => $msg_text) {
          if ($_SESSION['messages']['status'][$msg_key] == t('Your configuration has been saved.')) {
            unset($_SESSION['messages']['status'][$msg_key]);
          }
        }
      }

      $result = call_user_func_array($op['handler'], $args);
    }
  }
}


/**
 * Menu add item operation handler for a new dummy menu item.
 */
function content_menu_menu_form_handler_dummy($form, $form_state, $item) {
  drupal_set_message(t('A new menu item has been created.'));
  $item['link_path'] = 'menu-dummy';
  content_menu_link_save($item);
}


/**
 * Menu add item operation handler for a new node menu item.
 */
function content_menu_menu_form_handler_node($form, $form_state, $item, $type) {
  $types = node_type_get_types();
  drupal_set_message(t("You're about to create a new menu item with new %type content.", array('%type' => $types[$type]->name)));
  drupal_set_message(t('Complete the process by filling out and saving the form below.'));
  $type = str_replace("_", "-", $type);
  drupal_goto('node/add/' . $type, array('query' => content_menu_assemble_query_string($item)));
}


/**
 * Menu add item operation handler for a new generic menu item.
 */
function content_menu_menu_form_handler_url($form, $form_state, $item) {
  drupal_set_message(t("You're about to create a new generic menu item linking to a URL path."));
  drupal_set_message(t('Complete the process by filling out and saving the form below.'));
  drupal_goto('admin/structure/menu/manage/' . $item['name'] . '/add', array('query' => content_menu_assemble_query_string($item)));
}


/**
 * Menu add item operation handler for a new generic menu item.
 */
function content_menu_menu_form_handler_menu_position_rule($form, $form_state, $item) {
  drupal_set_message(t("You're about to create a new generic menu position rule."));
  drupal_set_message(t('Complete the process by filling out and saving the form below.'));
  drupal_goto('admin/structure/menu-position/add', array('query' => content_menu_assemble_query_string($item)));
}


/**
 * Menu add item operation handler for a new menu item to existing content.
 */
function content_menu_menu_form_handler_menu_position_existing($form, $form_state, $item) {
  $url = content_menu_variable_get_add_existing_content_url();
  // If using default view, ensure to only goto view page if views is enabled.
  if (empty($url)) {
    content_menu_menu_form_handler_url($form, $form_state, $item);
  }
  else {
    drupal_set_message(t("You're about to create a new menu item for an existing content."));
    drupal_set_message(t('Complete the process by finding and selecting an existing content to add to the menu.'));
    drupal_goto($url, array('query' => content_menu_assemble_query_string($item)));
  }
}

/**
 * Menu add item operation handler for a new menu item to an existing view.
 */
function content_menu_menu_form_handler_existing_view($form, $form_state, $item) {
  drupal_set_message(t("You're about to create a new menu item for an existing view."));
  drupal_set_message(t('Complete the process by finding and selecting the view to add to the menu.'));
  drupal_goto(CONTENT_MENU_ADD_EXISTING_VIEW_URL, array('query' => content_menu_assemble_query_string($item)));
}

/**
 * List all views with a page display.
 */
function content_menu_add_exiting_view($form, $form_state) {
  if (!module_exists('views')) {
    return;
  }

  $form = array();

  $item = content_menu_get_menu_item_from_querystring();
  $form['#menu_item'] = $item;

  $all_views = views_get_all_views();
  foreach ($all_views as $view) {
    foreach ($view->display as $display) {
      if ($display->display_plugin == 'page') {
        if ((!property_exists($view, 'disabled') || (property_exists($view, 'disabled') && !$view->disabled))) {
          $view_path = $display->display_options['path'];
          $view_title = check_plain($view->name . ' - ' . $display->display_title . ' (' . $display->display_options['path'] . ')');
          if (drupal_valid_path($view_path)) {
            $options[$view_path] = $view_title;
          }
        }
      }
    }
  }

  if (!empty($options)) {
    asort($options);
    $form['existing_view'] = array(
      '#type' => 'radios',
      '#title' => t('Link to this view'),
      '#default_value' => isset($item['link_path']) ? $item['link_path'] : NULL,
      '#required' => TRUE,
      '#options' => $options,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('content_menu_add_exiting_view_submit'),
    );
  }
  else {
    $no_views_notice = t('There are no available views.');
    if (module_exists('views_ui') && user_access('administer views')) {
      $no_views_notice .= ' ' . t('You can create views in the <a href="@url">Views UI</a>', array('@url' => url('admin/structure/views')));
    }
    $form['view_info'] = array(
      '#title' => t('Use these views'),
      '#type' => 'item',
      '#markup' => $no_views_notice,
    );
  }
  return $form;
}

/**
 * Submit handler for the form above.
 */
function content_menu_add_exiting_view_submit($form, $form_state) {
  $menu_item = $form['#menu_item'];
  $menu_item['link_path'] = $form_state['values']['existing_view'];
  content_menu_link_save($menu_item);
}

/**
 * Assembles a query string with menu item data.
 */
function content_menu_assemble_query_string($item) {
  if (empty($item['name']) && isset($item['menu_name'])) {
    $item['name'] = $item['menu_name'];
  }
  $query = array(
    'menu_mlid'   => isset($item['mlid']) ? $item['mlid'] : NULL,
    'menu_title'  => $item['title'],
    'menu_plid'   => $item['plid'],
    'menu_name'   => $item['name'],
    'menu_hidden' => $item['hidden'],
    'menu_weight' => $item['weight'],
    'menu_link_path' => isset($item['link_path']) ? $item['link_path'] : NULL ,
    'destination' => isset($item['destination']) ? $item['destination'] : ('admin/structure/menu/manage/' . $item['name']),
    'msg_buffer' => isset($item['msg_buffer']) ? $item['msg_buffer'] : NULL,
  );
  return $query;
}


/**
 * Retrieve all supported menu add item operations.
 */
function content_menu_get_target_types($menu_name) {
  // Invoke hook to let modules provide target operations for new item.
  $target_types = array();
  $context = array('menu_name' => $menu_name);
  drupal_alter('menu_item_target_types', $target_types, $context);
  return $target_types;
}


/**
 * Actual implementation for content_menu_menu_item_target_types_alter().
 *
 * Extend the target types for a new menu item provided by default.
 * See content_menu.api.php for further documentation.
 */
function _content_menu_menu_item_target_types_alter(&$target_types, &$context) {
  $target_types['url'] = array(
    'label' => t('URL'),
    'handler' => 'content_menu_menu_form_handler_url',
    'clean_menu_save_message' => TRUE,
  );

  $target_types['dummy'] = array(
    'label' => t('Placeholder'),
    'handler' => 'content_menu_menu_form_handler_dummy',
    'clean_menu_save_message' => FALSE,
  );

  foreach (node_type_get_types() as $node_type) {
    if (user_access('create ' . $node_type->type . ' content') || user_access('administer nodes')) {
      $node_menu_options = variable_get('menu_options_' . $node_type->type, array('main-menu' => 'main-menu'));
      if (in_array($context['menu_name'], $node_menu_options)) {

        $target_types['node_' . $node_type->type] = array(
          'label' => t('New @type', array('@type' => t($node_type->name))),
          'handler' => 'content_menu_menu_form_handler_node',
          'arguments' => array('type' => $node_type->type),
          'clean_menu_save_message' => TRUE,
        );

      }
    }
  }

  $target_types['existing'] = array(
    'label' => t('Existing content'),
    'handler' => 'content_menu_menu_form_handler_menu_position_existing',
    'clean_menu_save_message' => TRUE,
  );

  if (module_exists('views')) {
    $target_types['existing_view'] = array(
      'label' => t('Existing view'),
      'handler' => 'content_menu_menu_form_handler_existing_view',
      'clean_menu_save_message' => TRUE,
    );
  }

  if (module_exists('menu_position') && user_access('administer menu positions')) {
    $target_types['menu_position_rule'] = array(
      'label' => t('Menu position rule'),
      'handler' => 'content_menu_menu_form_handler_menu_position_rule',
      'clean_menu_save_message' => TRUE,
    );
  }
}
