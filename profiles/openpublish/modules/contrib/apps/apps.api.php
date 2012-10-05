<?php

/*
 * provide configureation information to the apps about an enabled app
 *
 * One can register demo content as a sepeate module or as a set of callbacks
 * hook_apps_app_info is a hook that will only be called on
 * app modules
 *
 * RETURN: and associtive array
 */
function hook_apps_app_info() {
  return array(
    //Demo Content
    'demo content description' => 'This tells what add demo content will do it is placed on the configure form',
    //The perfered way for an app to provide demo content is to have a module 
    //that when enabled will add demo content, and when disabled will removed 
    //demo content
    //this module should be a submodule or part of the manifest dependent modules
    'demo content module' => 'appname_demo_content',

    //If the demo content is provide in a differnt way one should provide the
    //following callbacks
    'demo content enabled' => 'appname_demo_content_enabled', //should return True if demo content is on
    'demo content enable' => 'appname_demo_content_enable', //should turn on demo content and return true
    'demo content disable' => 'appname_demo_content_disable', //should turn off demo content and return true

    'configure form' => 'appname_app_configure_form', // This form will be render on the app config page
    'post install callback' => 'appname_app_post_install', // This will be called after the app is enabled intialy or of the app has been uninstalled
    'status callback' => 'appname_app_status'
    /*
    This call back is used to render a status table on the config page.  it should be an array with two keys (and on optional third) 
    array(
      'title' =>'Status'  //title of the table,
      'items' => array(  //rows in the table with any keys
        array(
          'severity' =>    REQUIREMENT_WARNING, //REQUIREMENT_OK REQUIREMENT_INFO, REQUIREMENT_ERROR
          'title' => 'Example',
          'description' => t("Instrunctions for Example"),
          'action' => array(l("Link to do something!", "")),
        ),
      ),
      // headers are optional but these are the default
      'headers' => array('severity', 'title', 'description', 'action')
    );
    serverity and 
    */
}


/**
 * provides server information to the apps
 *
 * this hook is only called on the current profile
 * RETURN: an array of assoctive server arrays
 */
function hook_apps_servers_info() {
  return array(
    'servername' => array(
      'title' => t('Store Title'), //the title to be use for the server
      'description' => t('A description of the server and what apps it might have'),
      'manifest' => 'http://apps.com/app/query', // the location of the  json manifest
    ),
  );
}

/**
 * Add an apps install step to an installation profile.
 * 
 * Use this in your hook_install_tasks to add all the needed tasks for installing
 * apps. Set the App Server key and any default selected apps.
 */
function hook_install_tasks($install_state) {
  $tasks = array();
  require_once(drupal_get_path('module', 'apps') . '/apps.profile.inc');
  $server = array(
    'title' => 'App Server Name'
    'machine name' => 'apps_server_machine_name',
    'default apps' => array(
      'app_machine_name_1',
      'app_machine_name_2',
    ),
    'required apps' => array(

    ),
    'default content callback' => 'distro_default_content',
  );
  $tasks = $tasks + apps_profile_install_tasks($install_state, $server);
  return $tasks;
}

/**
 * This is the struture of the json manifest
 */

$js = <<<JS
{
  "distro": {
    "distros" : "openpublic",
    "core" : "7"
  },
  "featured app": "ideation",
  "manifest version": 1.0,
  "apps": [
    // This starts a single app manifest
    {
      // The Title of the app
      "name": "Ideation",
      // Description of what the app will do
      "description": "You think it, we log it",
      // The current version of the app
      "version" : "1.0-alpha",
      // Who created the app
      "author" : "Phase2 Technology",
      // Url of the creates site
     "author_url" : "http://www.phase2technology.com",
      // An array of screen shots for display on the detailed page
      "screenshots" : ["http://appserver.openpublicapp.com/sites/default/files/ideation-screenshot1.jpg"],
      // The logo image for the app
      "logo" : "http://drupal.org/files/images/ideation.jpg",
      // The machine_name of the main module
      "machine_name" : "ideation",
      // The key from the downloadables array for where to get the main module
      "downloadable" : "ideation 7.x",
      // A hash of dependant modules. The key is the machine name of the module. The value is the module
      // and a version specification. This uses the .info format for module dependencies.  The values in
      // this hash are the keys used in the downloadables hash later in the manifest.
      "dependencies": {
        "views": "views 7.x-1.0",
        "votingapi": "votingapi 7.x-2.4",
        "fivestar": "fivestar 6.x-2.x-dev"
      },
      // Libraries will be installed to sites/all/libraries/{key}. The values in this hash are the
      // keys used in the downloadables hash later in the manifest.
      "libraries": {
        "jquery_ideation": "jquery_ideation 1.0"
      }
      // A hash of resources to be downloaded. The key is used else where in the manifest. The value
      // should be a url to a publicly downloadable archive (tar gz zip)
      "downloadables": {
        "ideation 7.x" : "http://appserver.openpublicapp.com/sites/default/files/fserver/ideation-7.x.tgz",
        "jquery_ideation 1.0" : "http://appserver.openpublicapp.com/sites/default/files/fserver/jquery_ideation-1.0.tgz",
        "views 7.x.3.0-alpha1" : "http://ftp.drupal.org/files/projects/views-7.x-3.0-alpha1.tar.gz",
        "fivestar 6.x-2.x-dev" : "http://ftp.drupal.org/files/projects/fivestar-7.x-2.x-dev.tar.gz",
        "votingapi 7.x-2.4" : "http://ftp.drupal.org/files/projects/votingapi-7.x-2.4.tar.gz"
      }
    }
    // This ends a single app manifest
  ]
}


JS;
