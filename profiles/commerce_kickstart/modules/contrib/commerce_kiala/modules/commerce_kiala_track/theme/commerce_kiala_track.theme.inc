<?php

/*
 * Add iframe variables
 */
function template_preprocess_commerce_kiala_trackandtrace(&$vars) {
  $settings = commerce_kiala_settings();
  
  $vars['iframe_width'] = $settings['tracktrace_iframe_width'];
  $vars['iframe_height'] = $settings['tracktrace_iframe_height'];
  
  global $user;
  
  $language = !empty($user->language) ? $user->language : language_default()->language;
  $handler = commerce_kiala_track_get_tracking_embeddable_handler($language, $vars['tracking_number']);
  $vars['iframe_url'] = $handler->getUrl();
}
