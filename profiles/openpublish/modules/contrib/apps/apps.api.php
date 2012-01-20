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
    'demo content enabled' => 'appname_demo_content_enabled' //should return True if demo content is on
    'demo content enable' => 'appname_demo_content_enable' //should turn on demo content and return true
    'demo content disable' => 'appname_demo_content_disable' //should turn off demo content and return true

    'configure form' => 'appname_app_configure_form' // This form will be render on the app config page
    'post install callback' => 'appname_app_post_install' // This will be called after the app is enabled intialy or of the app has been uninstalled
  );
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
    {
      "name": "Ideation", // The Title of the app
      "description": "You think it, we log it", // Description of what the app will do
      "version" : "1.0-alpha", //current version 
      "author" : "Phase2 Technology", //who create the app
      "author_url" : "http://www.phase2technology.com", //url of the creates site
      // an array of screen shots for display on the detailed page
      "screenshots" : ["http://appserver.openpublicapp.com/sites/default/files/ideation-screenshot1.jpg"],
      // the logo image for the app
      "logo" : "http://drupal.org/files/images/ideation.jpg",
      //the machine_name of the main module
      "machine_name" : "ideation",
      //the key from the downloadables array for where to get the main module
      "downloadable" : "ideation 7.x",
      //a hash of depend modules where the key is the name of the module (one can include version req) and the value
      //is the key of the downloadable for that module
      //Note the whole dep tree must be listed including deps of deps
      "dependencies": {
        "views": "views 7.x-1.0",
        "votingapi": "votingapi 7.x-2.4",
        "fivestar": "fivestar 6.x-2.x-dev"
      },
      //Libraries will be installed to sites/all/libraries/KEY
      "libraries": {
        "jquery_ideation": "jquery_ideation 1.0"
      }
      //a hash of downloadables the key is use else where in the manifest and the value should be a url
      //to a compress file (tar gz zip)
      "downloadables": {
        "ideation 7.x" : "http://appserver.openpublicapp.com/sites/default/files/fserver/ideation-7.x.tgz",
        "jquery_ideation 1.0" : "http://appserver.openpublicapp.com/sites/default/files/fserver/jquery_ideation-1.0.tgz",
        "views 7.x.3.0-alpha1" : "http://ftp.drupal.org/files/projects/views-7.x-3.0-alpha1.tar.gz",
        "fivestar 6.x-2.x-dev" : "http://ftp.drupal.org/files/projects/fivestar-7.x-2.x-dev.tar.gz",
        "votingapi 7.x-2.4" : "http://ftp.drupal.org/files/projects/votingapi-7.x-2.4.tar.gz"
      }
    }
  ]
}


JS;
