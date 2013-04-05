<?php

/*
 * Display the kiala tracking content in an iframe, or link that opens up an
 * iframe in a modal window.
 */
class commerce_kiala_track_views_handler_tracking_iframe_field extends commerce_kiala_track_views_handler_tracking_link_field {
  function render($values) {
    if ($tracking_number = $this->get_value($values)) {
      $settings = commerce_kiala_settings();
      $embed_type = $settings['tracktrace_embed_type'];
      
      return commerce_kiala_track_render_embedded_trackback_content($tracking_number, $embed_type, $this->options['text']);
    }
  }
}