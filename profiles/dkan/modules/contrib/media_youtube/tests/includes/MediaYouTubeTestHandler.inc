<?php

/**
 * @file
 * Extends the MediaInternetYouTubeHandler class to make it suitable for local testing.
 */

/**
 * A test handler for YouTube videos.
 *
 * @see MediaInternetYouTubeHandler().
 */
class MediaYouTubeTestHandler extends MediaInternetYouTubeHandler {
  public function getOEmbed() {
    $uri = $this->parse($this->embedCode);
    $external_url = file_create_url($uri);
    $oembed_url = url('media-youtube-test/oembed', array('query' => array('url' => $external_url, 'format' => 'json'), 'absolute' => TRUE));
    $response = drupal_http_request($oembed_url);

    if (!isset($response->error)) {
      return drupal_json_decode($response->data);
    }
    else {
      throw new Exception("Error Processing Request. (Error: {$response->code}, {$response->error})");
      return;
    }
  }

  /**
   * Check if a YouTube video ID is valid.
   *
   * @return boolean
   *   TRUE if the video ID is valid, or throws a
   *   MediaInternetValidationException otherwise.
   */
  public function validId($id, $type = 'v') {
    $uri = file_stream_wrapper_uri_normalize('youtube://' . $type . '/' . check_plain($id));
    $external_url = file_create_url($uri);
    $oembed_url = url('media-youtube-test/oembed', array('query' => array('url' => $external_url, 'format' => 'json'), 'absolute' => TRUE));
    $response = drupal_http_request($oembed_url, array('method' => 'HEAD'));

    if ($response->code == 401) {
      throw new MediaInternetValidationException('Embedding has been disabled for this YouTube video.');
    }
    elseif ($response->code != 200) {
      throw new MediaInternetValidationException('The YouTube video ID is invalid or the video was deleted.');
    }

    return TRUE;
  }
}
