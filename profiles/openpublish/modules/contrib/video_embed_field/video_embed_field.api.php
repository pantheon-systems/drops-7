<?php

/**
 * API Info for video_embed_field module
 */

/**
 * Creates a hook that other modules can implement to get handlers - 
 * hook_video_embed_handler_info
 * Can be used to add more handlers if needed - from other modules and such
 * Handler should be an array of the form
 * array(
 *   'title' => 'Title', //The title of the handler - to be used as the field group header - will be wrapped with t()
 *   'function' => 'function_name_to_call', //should be of the signature function_name($url, $settings) and should return the embed code
 *   'form' => 'function_name_for_form', //function to create settings form (optional)
 *   'domains' => array('youtube.com'), //the domains that this handler will create embed code for
 *   'defaults' => array(), //The default settings for the module, used for both the form and the callback function
 * );
 */
function hook_video_embed_handler_info() {

}