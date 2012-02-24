<?php
/**
 * @file
 * Handles pulling and processing of app data from the server
 *
 */

define ("ENABLED", 1);
define ("DISABLED", 0);
define ("INCOMPATABLE", -1);
define ("INSTALLABLE", 2);


function apps_server_local($server) {
  $apps = array();
  $infos = system_rebuild_module_data();
  foreach($infos as $module => $info) {
    if(isset($info->info['apps']['server'])) {
      if($info->info['apps']['server'] = $server['name']) {
        foreach($info->info['apps']['manifests'] as $mfile) {
          $mfile = drupal_get_path("module", $module) . "/" .$mfile;
          $app = drupal_parse_info_file($mfile);
          foreach($app['screenshots'] as $id=>$image) {
            $app['screenshots'][$id] = url(dirname($mfile) . "/" . $image);
          }
            $app['logo'] = url(dirname($mfile) . "/" . $app['logo']);
          $apps[] = $app;
        }
      }
    }
  }
  return $apps;
}
/*
 * returns all servers/server defined by the current profile
 * PARAM server_name: the  name of a server
 * RETURN : an array of server arrays one server array if $server_name has a value
 *
 * TODO: Turn Store Objects in to Object instead of arrays OPIC-372
 */
function apps_servers($server_name = FALSE) {
  $servers_cache = cache_get('apps_servers');
  $servers = $servers_cache ? $servers_cache->data : FALSE;
  if (!$servers) {
    $profile = drupal_get_profile();
    // Get apps servers from profile
    $servers = ($s = module_invoke($profile, 'apps_servers_info')) ? $s : array();
    // Allow modules to add apps servers as well.
    $servers += module_invoke_all('apps_servers_info');

    drupal_alter('apps_servers', $servers);

    foreach($servers as $name =>$server) {
      $servers[$name]['name'] = $name;
    }
  }
  // if we have the dev console turned on add it as a server
  if (variable_get('apps_enable_developement_console', FALSE)) {
    $servers['development'] = array(
      'name' => 'development',
      'description' => 'Local Apps in Developement',
      'title' => 'Development',
      'manifest' => FALSE,
      'featured app' => FALSE,
    );
  }
  cache_set('apps_servers', $servers);
  if($server_name) {
    if(isset($servers[$server_name])) {
      return $servers[$server_name];
    }
  }
  else {
    return $servers;
  }
  return FALSE;
}
/**
 * retrieve apps from the manifest 
 * PARAM $server: a server object return from apps_servers
 * PARAM $condition: and array of key and values to match in the app arrays
 * PARAM $add_theme: if True apps_apps_add_theme will be called when done getting apps
 * RETURN : an array of app info arrays
 */
function apps_apps($server, $condition = array(), $add_theme=FALSE) { 
  if (!is_array($server)) {
    $server = apps_servers($server);
  }
  $manifest = apps_manifest($server);
  if (isset($manifest['error'])) {
    $e= new Exception($manifest['error']);
    $e->request = $manifest['request'];
    throw $e;
  }
  $apps = $manifest['apps'];
  foreach($apps as $id => $app) {
    $intersect = array_intersect_assoc($app, $condition);
    if ($intersect !== $condition) {
      unset($apps[$id]);
    }
  }
  if (empty($apps)) {
    $apps = array('#markup' => '<div class="messages">' . t('No applications currently available.') . '</div>');
  }
  elseif ($add_theme) {
    apps_apps_add_theme($apps);
  }
  return $apps;
}

/**
 * takes an array of apps and ands #theme properties so that
 * it can be drupal rendered
 * PARAM $apps: an array of apps from apps_apps
 * RETURN : the apps array with add #theme settings for a list 
 * view of the apps
 */
function apps_apps_add_theme(&$apps) {
  foreach($apps as $id=> $app) {
    // add teaser theme
    $apps[$id]['#theme'] = 'apps_app_teaser';
    // add style name for image_style theming
    if($apps[$id]['logo']) {
      $apps[$id]['logo']['style_name']= 'apps_logo';
    }
  }
  $apps['#theme'] = 'apps_list';
}


function apps_client_id() {
  $id = variable_get('apps_client_id', FALSE);
  if (!$id) {
    $id = uniqid(rand(0, 10000), TRUE);
    variable_set('apps_client_id', $id);
  }
  return $id;
  
}

/**
 * Request manifest and media assets from the app server
 * PARAM $server: a server object return from apps_servers
 * RETURN : an array represntation of the json manifest, with images
 * relpaced by file objects
 *
 * TODO: Handle lack of server presense OPIC-374
 */
function apps_request_manifest($server) { 
  //$manifest_cache = cache_get("apps_manifest_{$server['name']}");
  $manifest_cache = false;
  $manifest = $manifest_cache ? $manifest_cache->data : FALSE;

  if(!$manifest) {
    if(!$server['manifest']) {
       $apps = apps_server_local($server);
       $manifest = $server;
       $manifest['apps'] = $apps;
       apps_request_manifest_image_process($manifest);
       return $manifest;
    }
    $info =  drupal_parse_info_file(dirname(__file__) . '/apps.info');
    $rest_data = array(
      'client_id' => apps_client_id(),
      'apps_version' => $info['version'],
      'format' => 'json',
    ) + $server;
    //I would like to do a post request but it is not happening
    //$request = drupal_http_request($server['manifest'], array('method' => 'POST', 'data' => drupal_http_build_query($rest_data)));
    $request = drupal_http_request($server['manifest'] . "?" . drupal_http_build_query($rest_data));

    if (isset($request->error)) {
      $manifest = array(
        'error' =>"Manifest: {$server['manifest']} {$request->error}",
        'request' => $request,
        'apps' => array(),
      );
    }
    else if ($manifest = json_decode($request->data, TRUE)) {
      // add any local apps
      //$manifest['apps'] = $manifest['apps'] + $apps;
      apps_request_manifest_image_process($manifest);
      cache_set("apps_manifest_{$server['name']}", $manifest, 'cache', time() + 60 +30);
    }
    else {
      $manifest = array(
        'error' =>"Manifest: {$server['manifest']} json not parsable",
        'request' => $request,
        'apps' => array(),
      );
    }
  }

  return $manifest;
}

/*
 * Downloads images so that they can be sized with image styles
 */
function apps_request_manifest_image_process(&$manifest) {
  foreach($manifest['apps'] as $id => $app) {
    
    //Get Logo
    $logo_uri = $app['logo'];
    $logo = (array) apps_retrieve_app_image($app['logo'], "{$app['name']} Logo");

    $manifest['apps'][$id]['logo'] = !empty($logo) ? $logo : FALSE;

    //Get screenshots
    foreach ($app['screenshots'] as $ss_id => $ss) {
      $screenshot =  (array) apps_retrieve_app_image($ss, "{$app['name']} Screenshot $id");
      
      if(!empty($screenshot)) {
        $manifest['apps'][$id]['screenshots'][$ss_id] = $screenshot; 
      }
      else {
        unset($manifest['apps'][$id]['screenshots'][$ss_id]); 
      }
    }
  }

}

/**
 * Download a image from the manifest 
 *
 * PARAM url: the url of the image
 * PARAM title: the title of the image
 * PARAM alt: the alt text of the image
 * RETURN : a file object for the download file or FALSE if no image was downloaded
 *
 */
function apps_retrieve_app_image($url, $title = FALSE, $alt = FALSE) {
  //allow for local files from the development server type
  $check = parse_url($url);
  if(!$check || !isset($check['host']) || !$check['host']){
    $url = url($url, array('absolute'=>TRUE));
  }
  
  //to prevent strict warning need to store this in a variable
  $url_parts = explode("/", parse_url($url, PHP_URL_PATH));
  
  $file_name = array_pop($url_parts);
  
  $uri = file_build_uri("apps/$file_name");
  $current = FALSE;
  
  $fids = db_select('file_managed', 'f')
    ->condition('uri', $uri)
    ->fields('f', array('fid'))
    ->execute()
    ->fetchCol();
  
  if (!empty($fids) && isset($fids[0]) && is_numeric($fids[0])) {
    $current = file_load($fids[0]);
  }
   //check to see if the remote file is newer than the one that we have
  if($current){
    $remote = get_headers($url, 1); //get the remote headers
    $remote_timestamp = strtotime($remote['Last-Modified']);
    
    if($remote_timestamp <= $current->timestamp && $remote['Content-Length'] == $current->filesize){
      //TODO make this a file 
      $current->path = $current->uri;
      $current->title = $title ? $title : '';
      $current->alt = $alt ? $alt : $title;
      return $current;
    }
  }
   
  $request = drupal_http_request($url, array(), 'GET');

  if (isset($request->data)) {
    if (!file_exists(drupal_realpath(file_build_uri('apps')))) {
      drupal_mkdir(file_build_uri('apps'));
    }
    $file = file_save_data($request->data, $uri, FILE_EXISTS_REPLACE);
    if ($current) {
      $file->fid = $current->fid;
    }
    $file->title = $title ? $title : '';
    $file->alt = $alt ? $alt : $file->title;
    file_save($file);
    $file->path = $file->uri;
    return $file;
  }
}

/**
 * Produce the Process Manifest
 *
 * Starting with the json manifest and adding data around the current status of the app
 * in this install
 *
 * PARAM $server: a server object return from apps_servers
 * RETURN : an array of the process json manifest
 *
 *TODO: Cache status data and clear on changes to any module status
 */
function apps_manifest($server) {
  $manifests = &drupal_static(__function__);
  require_once DRUPAL_ROOT . '/includes/install.inc';
  if (!$manifests[$server['name']] && !isset($manifests[$server['name']]['error'])) {
    // Get manifest from server
    $manifest = apps_request_manifest($server);
    //if there is an error with the manifest jump out now
    if (isset($manifest['error'])) {
      return $manifest;
    }
    $modules = system_rebuild_module_data();
    $apps = array();
    foreach($manifest['apps'] as $app) {
      $current_app_module = isset($modules[$app['machine_name']]) ? $modules[$app['machine_name']] : FALSE;
      $app['enabled'] = $current_app_module && $current_app_module->status;
      $app['incompatable'] = FALSE;
      $app['installed'] = (bool) $current_app_module;
      $app['dep_installed'] = TRUE;
      $app['disabled'] = $current_app_module && empty($current_app_module->status) && $current_app_module->schema_version > SCHEMA_UNINSTALLED;
      $app['featured'] = ($app['machine_name'] == $manifest['featured app']);
      $app['server'] = $server;
      $app['dependencies'] = isset($app['dependencies']) ? $app['dependencies'] : array();
      $app['libraries'] = isset($app['libraries']) ? $app['libraries'] : array();
      // Add info to dependencies
      foreach($app['dependencies'] as $name_version => $downloadable) {
        //Parse dep versions
        $version = drupal_parse_dependency($name_version);
        $name = $version['name'];

        //check status of modules
        $current = isset($modules[$name]) ? $modules[$name] : FALSE;
        $incompatable = $current ? (bool) drupal_check_incompatibility($version, $current->info['version']) : FALSE;
        $installed = (bool) $current;
        $enabled = $current && $current->status;
        $status = $incompatable ? INCOMPATABLE : (!$installed ? INSTALLABLE : ($enabled ? ENABLED : DISABLED));

        if($status == INCOMPATABLE) {
          //if any one module is incompatable then the app is incompatable
          $app['incompatable'] = TRUE;
        }
        if($status == INSTALLABLE) {
          //if any one module is installable then we are not installed yet
          $app['dep_installed'] = FALSE;
        }
        //rebuild dep with new data
        $info = array(
          'downloadable' => $downloadable,
          'version' => $version,
          'status' => $status,
          'incompatable' => $incompatable,
          'enabled' => $enabled,
          'installed' => $installed,
        );
        unset($app['dependencies'][$name_version]);
        $app['dependencies'][$version['name']] = $info;
      }
      if (isset($app['libraries'])) {
        foreach($app['libraries'] as $name_version => $downloadable) {
          $info = array(
            'downloadable' => $downloadable,
            'version' => array('name'=>$name_version),
            'status' => INSTALLABLE,
            'incompatable' => 0,
            'enabled' => 0,
            'installed' =>  is_dir(DRUPAL_ROOT . "/sites/all/libraries/$name_version"),
          );
          $app['libraries'][$name_version] = $info;
        }
      }
      $app['status'] = $app['incompatable'] ? INCOMPATABLE : (!$app['installed'] || !$app['dep_installed'] ? INSTALLABLE : ($app['enabled'] ? ENABLED : DISABLED));
      $apps[$app['machine_name']] = $app;
      
    }
    //overide json apps with our inhanced apps
    apps_add_app_info($apps);
    $manifest['apps'] = $apps;
    $manifests[$server['name']] = $manifest;
  }
  return $manifests[$server['name']];
}

function apps_add_app_info(&$apps) {
  $info_cache = cache_get("apps_installed_app_info");
  $info = array();
  if ($info_cache) {
    $info = $info_cache->data();
  }
  $set_cache = FALSE;
  foreach($apps as $id => $app) {
    if(!isset($info[$app['machine_name']])) {
       $info[$app['machine_name']] = ($i = module_invoke($app['machine_name'], 'apps_app_info')) ? $i : array();
       $set_cache = TRUE;
    }
    $apps[$id] += $info[$app['machine_name']];
    //add defaults
    $info_defaults = array(
      'configure form' => $app['machine_name'] . "_apps_app_configure_form",
      'demo content description' => "Enabling the demo content will add content to the site that can help to understand how the app functions.  When it is disabled all of the demo content will be removed.",
    );
    $apps[$id] += $info_defaults;
    
    // set demo content callbacks if demo content module is set
    if (isset($apps[$id]['demo content module'])) {
      $apps[$id] += array(
        'demo content enabled callback' => 'apps_demo_content_enabled',
        'demo content enable callback' => 'apps_demo_content_enable',
        'demo content disable callback' => 'apps_demo_content_disable',
      );
    }
  }
  if ($set_cache) {
    cache_set("apps_install_app_info", $info);
  }
}

function apps_app_callback($app, $key) {
  module_load_include("inc", $app['machine_name'], $app['machine_name'] . ".app");
  if(isset($app['file'])) {
    require_once $app['file'];
  }
  if (isset($app[$key]) && module_exists($app['machine_name']) && function_exists($app[$key])){
    return $app[$key];
  }
}


/**
 * default demo content enabled callback
 * 
 * check to see if the apps demo content module is enabled
 */
function apps_demo_content_enabled($app) {
  return (module_exists($app['demo content module']));
}

/**
 * default demo content enable callback
 * 
 * enables the app's demo content module
 * PARAM $app: an app array
 * RETURN : bool if the module was enabled
 */
function apps_demo_content_enable($app) {
  $success = module_enable(array($app['demo content module']));
  drupal_flush_all_caches();
  return $success;
}

/**
 * default demo content disable callback
 * 
 * disables the app's demo content module
 * PARAM $app: an app array
 * RETURN : bool if the module was disabled
 */
function apps_demo_content_disable($app) {
  $success = module_disable(array($app['demo content module']));
  drupal_flush_all_caches();
  return !apps_demo_content_enabled($app);
}

/**
 * formcall back for demo content
 */
function apps_demo_content_form($form, &$form_state, $app) {
  $form = array();
  $cb = apps_app_callback($app, 'demo content enabled callback');
  $enabled = $cb($app);
  $form['demo_content_group'] = array(
    '#type' => 'fieldset',
    '#title' => "Demo Content for {$app['name']}",
  );
  $form['demo_content_group']['demo_content_current'] = array(
    '#markup' => $app['demo content description']. " <i>Demo content for {$app['name']} is currently " . ($enabled ? 'enabled' : 'disabled') . ".</i><br /><br />",
  );
  $form['demo_content_action'] = array(
    '#type' => 'value',
    '#value' => !$enabled,
  );
  $form['app'] = array(
    '#type' => 'value',
    '#value' => $app
  );
  //$form['actions'] = array('#type' => 'actions');
  //$form['#theme'] ='system_settings_form';
  //$form['actions']['demo_content_button'] = array(
  $form['demo_content_group']['demo_content_button'] = array(
    '#type' => 'button',
    '#value' => !$enabled ? t('Enable Demo Content') : t('Disable Demo Content'),
    '#submit' => array('apps_demo_content_form_submit'),
    '#executes_submit_callback' => TRUE,
  );
  return $form;
}

/**
 * callback for demp content form
 */
function apps_demo_content_form_submit($form, &$form_state) {
  $app = $form_state['values']['app'];
  $enable = $form_state['values']['demo_content_action'];
  if($enable) {
     $cb = apps_app_callback($app, 'demo content enable callback'); 
     $success = $cb($app);
     if($success) {
       drupal_set_message("Enabled demo content for {$app['name']}");
     }
  }
  else {
     $cb = apps_app_callback($app, 'demo content disable callback'); 
     $success = $cb($app);
     if($success) {
       drupal_set_message("Disabled demo content for {$app['name']}");
     }
  }
}


