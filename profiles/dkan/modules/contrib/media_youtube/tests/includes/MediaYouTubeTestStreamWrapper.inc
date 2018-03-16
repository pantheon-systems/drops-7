<?php

/**
 * @file
 * Extends the MediaYouTubeStreamWrapper class to make it suitable for local testing.
 */

/**
 *  Create an instance like this:
 *  $youtube = new MediaYouTubeTestStreamWrapper('youtube://v/[video-code]');
 */
class MediaYouTubeTestStreamWrapper extends MediaYouTubeStreamWrapper {
  function getOriginalThumbnailPath() {
    $parts = $this->get_parameters();
    $uri = file_stream_wrapper_uri_normalize('youtube://v/' . check_plain($parts['v']));
    $external_url = file_create_url($uri);
    $oembed_url = url('media-youtube-test/oembed', array('query' => array('url' => $external_url, 'format' => 'json'), 'absolute' => TRUE));
    $response = drupal_http_request($oembed_url);

    if (!isset($response->error)) {
      $data = drupal_json_decode($response->data);
      return $data['thumbnail_url'];
    }
    else {
      throw new Exception(t('Error Processing Request. (Error: %code, %error)', array('%code' => $response->code, '%error' => $response->error)));
    }
  }
}
