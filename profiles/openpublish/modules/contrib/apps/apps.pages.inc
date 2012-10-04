<?php

/**
 * callback list off all apps
 */
function apps_install_page($server_name) {
  apps_include('manifest'); 
  $element = array(
    '#theme'=> 'apps_install_page',
  );
  //find all apps
  try {
    $apps = apps_apps($server_name, array(), TRUE);
    $element['apps'] = $apps;
    $featured_apps = apps_apps($server_name , array('featured' => TRUE)) ;
    if(!empty($featured_apps)) {
      $element['featured_app'] = array_pop($featured_apps);
    }
  }
  catch (Exception $e) {
    drupal_set_message("There was the following error - " . $e->getMessage(), 'warning');
  }
  return $element; 
}

/**
 * callback for listing of installed apps
 */
function apps_manage_page($server_name) {
  apps_include('manifest'); 
  $element = array(
    '#theme'=> 'apps_manage_page',
  );
  // find all installed apps
  try {
    $apps = apps_apps($server_name, array("installed" =>TRUE), TRUE);
    $element['apps'] = $apps;
  }
  catch (Exception $e) {
    drupal_set_message("There was the following error - " . $e->getMessage(), 'warning');
  }
  return $element;
}

/**
 * callback for listing of installed apps
 */
function apps_update_page($server_name) { 
  apps_include('manifest'); 
  // find all updateable apps (NOTE: currently this never retruns anything
  //$apps = apps_apps($server_name, array("updateable" =>TRUE), TRUE);
  return array(
    '#theme'=> 'apps_update_page',
  );
}

/**
 * callback for app config page
 */
function apps_app_config_page($server_name, $app_name) {
  apps_include('manifest');
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $app = $apps[$app_name];
  $element = array();
  if(apps_app_callback($app, "demo content enable callback")) {
    $element[] = drupal_get_form('apps_demo_content_form', $app);
  }

  // check for a status table
  if($data_callback = apps_app_callback($app, "status callback")) {
    $data = $data_callback();
    $header = isset($data['header']) ? $data['header'] : NULL;
    $title = isset($data['title']) ? $data['title'] : 'Status';
    $items = isset($data['items']) ? $data['items'] : array();
    $element[] = array(
      '#type' => 'fieldset',
      '#title' => 'status',
      'table' => apps_app_status_report($items, $header),
    );
  }
  if($form = apps_app_callback($app, "configure form")) {
    $element[] =  drupal_get_form($form);
  }
  return $element;
  return "$app_name has no configuration";
}
function apps_app_config_action_redirect($server_name, $app_name) {
  apps_include('manifest');
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $app = $apps[$app_name];
  drupal_goto(apps_app_page_path($app, 'configure'));
}
/**
 * callback for the app detail page
 */
function apps_app_details_page($server_name, $app_name) {
  apps_include('manifest');
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $apps[$app_name]['#theme'] = 'apps_app_page';
  return $apps[$app_name];
}

/**
 *  Title for the app detail page
 */
function apps_app_title($server_name, $app_name) {
  apps_include('manifest');
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  return t("{$apps[$app_name]['name']} Details");
}

/**
 * callback for the market page
 *
 * This is only used if there is more then one server
 */
function apps_market_page() {
   module_load_include("inc", "system", "system.admin");
   return system_admin_menu_block_page();
}

/**
 *
 */
function _apps_app_access($perm, $server_name, $action, $app_name) {
  apps_include('manifest');
  try {
    $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  }
  catch(Exception $e) {
    drupal_set_message("There was the following error - " . $e->getMessage(), 'warning');
    return FALSE;
  }
  if(user_access($perm) && isset($apps[$app_name]) ) {
    if(!$action) {
      return TRUE;
    }
    $app = $apps[$app_name];
    switch($action) {
      case FALSE:
        return TRUE;
      case 'enable':
        return ($app['status'] === APPS_DISABLED);
      case 'disable':
        return ($app['status'] === APPS_ENABLED);
      case 'install':
        return ($app['status'] === APPS_INSTALLABLE);
      case 'configure-action':
      case 'configure': 
        return (bool) apps_app_callback($app, "configure form") || apps_app_callback($app, "demo content enabled callback");
        //not doing this now for config as it brakes going to config after install
        // we only want this to show if we are on the configure page
        return ($_GET['q'] == apps_app_page_path($app, 'configure')) &&
        (apps_app_callback($app, "configure form") ||apps_app_callback($app, "demo content enabled callback")) ;
      case 'uninstall': 
        return ($app['disabled']);
    }
  }
}

/**
 * callback for app install
 * TODO: check to see the app is install able and then install
 * TODO: should goto config page but pass on the current destination
 * NOTE: it is expected that this page would be called with a drupal desination set
 */
function apps_app_install($server_name, $app_name) {
  apps_include('installer');
  require_once DRUPAL_ROOT . '/includes/authorize.inc';
  $action = arg(APPSERVER_BASEPATH_DEPTH + 5);
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $app = $apps[$app_name];
  switch ($action) {
    case FALSE:
      $next = apps_app_page_path($app, 'install/install');
      $_SESSION['apps_install_next'] = $next;
      apps_download_apps($app);
    case 'install':
      $next = apps_app_page_path($app, 'enable');
      $_SESSION['apps_install_next'] = $next;
      $form = apps_install_downloads(); 
      if ($form) {
        return $form;
      }
  }
  return "";
}


function apps_app_page_path($app, $type = '') {
  if (isset($app['machine_name'])) {
    return APPSERVER_BASEPATH . "/{$app['server']['name']}/app/{$app['machine_name']}/$type";
  }
}

/**
 * callback for the enable action
 *
 * Enables the app and got to config page if it exists
 */
function apps_app_enable($server_name, $app_name) {
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $app = $apps[$app_name];
  $next = apps_app_page_path($app, 'configure');
  $success = module_enable(array($app['machine_name']), TRUE);
  if ($success) {
    drupal_flush_all_caches();
    drupal_set_message("Enabled {$app['name']} app");
    drupal_set_message(t("Please go to !homelink to see the new app in action. ", 
			array('!homelink' => l("the home page", "<front>")))
		      );
    if (!$app['disabled'] && ($cb = apps_app_callback($app, "post install callback"))) {
      $cb($app);
    }
    if(!_apps_app_access('administer apps', $server_name, 'configure', $app_name)) {
      $next = apps_app_page_path($app, '');
    }
  }
  else {
    drupal_set_message("{$app['name']} App Not Enabled");
    $next = apps_app_page_path($app, '');
  }
  drupal_goto($next);
}

/**
 * callback for app disable
 */
function apps_app_disable($server_name, $app_name) {
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $app = $apps[$app_name];
  //force the user to disable demo content before disabling the app
  if (($is_cb = apps_app_callback($app, "demo content enabled callback")) &&
    $is_cb($app)) {
    $next = apps_app_page_path($app, 'configure');
    drupal_set_message("Please disable demo content before disabling the app");
    drupal_goto($next);
  }

  $success = module_disable(array($app['machine_name']));
  drupal_flush_all_caches();
  drupal_set_message("Disabled {$app['name']} app");
  $next = apps_app_page_path($app, '');
  drupal_goto($next);
  return '';
}

/**
 * callback for app uninstall
 */
function apps_app_uninstall($server_name, $app_name) {
  $apps = apps_apps($server_name , array('machine_name' => $app_name)) ;
  $app = $apps[$app_name];
  $next = apps_app_page_path($app, 'configure');
  require_once DRUPAL_ROOT . '/includes/install.inc';
  $uninstall[] = $app['machine_name'];
  if (isset($app['demo content module']) && $app['demo content module']) {
    array_unshift($uninstall, $app['demo content module']);
  }
  $success = drupal_uninstall_modules($uninstall);
  if ($success) {
    drupal_flush_all_caches();
    drupal_set_message("Uninstalled {$app['name']} app");
  }
  else {
    drupal_set_message("Uninstalling {$app['name']} app failed. Please ensure all modules that depend on this module are also uninstalled", FALSE);
  }
  $next = apps_app_page_path($app, '');
  drupal_goto($next);
}

/**
 * Admin settings form for Apps.
 */
function apps_config_form() {
  $form = array();
  $form['apps_enable_dev_console'] = array(
    '#title' => t('Enable Development Console'),
    '#description' => t('Allows local Apps to be displayed in a development app server.'),
    '#type' => 'checkbox',
    '#default_value'=> variable_get('apps_enable_dev_console', FALSE),
  );
  $form['apps_allow_voting'] = array(
    '#title' => t('Allow Voting'),
    '#description' => t('Allow users who can install apps to also rate them.  Requires that the appserver is running the appserver module.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('apps_allow_voting', TRUE),
  );
  $form = system_settings_form($form);
  $form['#submit'][] = 'apps_config_form_submit';
  return $form;
}

/**
 * Once the settings are saved, flush all caches. This will trigger the
 * Apps dev server to appear in the Apps console.
 */
function apps_config_form_submit() {
  drupal_flush_all_caches();
}


/**
 * apps_app_status_report()
 *
 * @PARAM items: an array of items to display the keys of which should be the 
 * keys of the $header param or use the default.
 * The default are severity, title, description and action.  Severity is expected to be
 * an int in (REQUIREMENT_INFO, REQUIREMENT_OK, REQUIREMENT_WARNING, REQUIREMENT_ERROR)
 * @PARAM header: an array of header titles, the keys of which should match the keys in $items
 * there is a default for this value
 *
 * @RETURN: a render array for the status report table
 */
function apps_app_status_report($items, $header = NULL) {
  
  if(!isset($header)) {
    $header = array(
      'severity' => 'Status',
      'title' => 'Title',
      'description' => 'Description',
      'action' => 'Action',
    );
  }
  $rows = array();
    $ok_image = array('#theme'=>"image", "#path" => "misc/message-24-ok.png", "#alt"=>"OK");
    $info_image = array('#theme'=>"image", "#path" => "misc/message-24-info.png", "#alt"=>"OK");
    $error_image = array('#theme'=>"image", "#path" => "misc/message-24-error.png", "#alt"=>"OK");
    $warning_image = array('#theme'=>"image", "#path" => "misc/message-24-warning.png", "#alt"=>"OK");
    //$ok_image = array('#theme'=>"image", "#path" => "misc/watchdog-ok.png", "#alt"=>"OK");
    //$info_image = array('#theme'=>"image", "#path" => "misc/watchdog-info.png", "#alt"=>"OK");
    //$error_image = array('#theme'=>"image", "#path" => "misc/watchdog-error.png", "#alt"=>"OK");
    //$warning_image = array('#theme'=>"image", "#path" => "misc/watchdog-warning.png", "#alt"=>"OK");
  $severities = array(
    REQUIREMENT_INFO => array(
      'title' => t('Info'),
      'class' => 'info',
      'image' => render($info_image),
    ),
    REQUIREMENT_OK => array(
      'title' => t('OK'),
      'class' => 'ok',
      'image' => render($ok_image),
    ),
    REQUIREMENT_WARNING => array(
      'title' => t('Warning'),
      'class' => 'warning',
      'image' => render($warning_image),
    ),
    REQUIREMENT_ERROR => array(
      'title' => t('Error'),
      'class' => 'error',
      'image' => render($error_image),
    ),
  );
  foreach($items as $item) {
    $row = array();
    foreach(array_keys($header) as $key) {
      if(isset($item[$key])) {
        if($key =='severity') {
          $row['data'][] = $severities[$item[$key]]['image'];
          $row['class'][] = $severities[$item[$key]]['class'];
        }
        elseif($key == 'action') {
          $row['data'][] = theme('item_list', array("items" => $item[$key]));
        
        }
        else {
          $row['data'][] = $item[$key];
        }
      }
      else {
          $row['data'][] = '';
      }
    }
    $rows[] = $row;
  }
  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );


}
