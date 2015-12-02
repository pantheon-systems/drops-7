<?php
/**
 * @file
 * File containing class video_handler_field_video_duration
 */

/**
 * Views field handler to show video duration.
 */
class video_handler_field_video_duration extends views_handler_field {
  function render($values) {
    if (!empty($values->video_queue_duration)) {
      return gmdate("H:i:s", $values->video_queue_duration);
    }

    return NULL;
  }
}
