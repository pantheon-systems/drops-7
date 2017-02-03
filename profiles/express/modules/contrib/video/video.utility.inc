<?php
/**
 * @file
 * This file will be used to keep all utility functions data structures.
 */

/**
 * Helper functions for the Video module.
 *
 * These functions are put in a utility class so they will only be
 * auto-loaded when needed.
 */
class video_utility {
  /**
   * Retrieve a single entity
   */
  public static function loadEntity($type, $id) {
    $entities = entity_load($type, array($id));
    if (empty($entities)) {
      return NULL;
    }

    return reset($entities);
  }

  /**
   * Clears all caches associated with a certain entity.
   *
   * @param $type
   *   Entity type
   * @param $id
   *   Entity ID
   */
  public static function clearEntityCache($type, $id) {
    // Clear the entity controller cache
    entity_get_controller($type)->resetCache(array($id));

    // Clear field cache
    cache_clear_all('field:' . $type . ':' . $id, 'cache_field');

    // Clear page cache and block cache
    cache_clear_all(NULL, NULL);

    if (module_exists('entitycache')) {
      // Clear entitycache cache
      cache_clear_all($id, 'cache_entity_' . $type);
    }
  }

  /**
   * Finds a stream wrapper using the output URL.
   *
   * Examples of output URLs:
   * - http://bucket.s3.amazonaws.com/dir/file.ext
   *   output: s3://file.ext
   * - http://yoursite/sites/yoursite/files/dir/file.ext
   *   output: public://dir/file.ext
   *
   * If no suitable stream wrapper is found, returns NULL.
   *
   * @todo optimize: find the wrapperbase -> wrapper mapping just once.
   */
  public static function urlToUri($url) {
    $uri = NULL;

    $mapping = self::getStreamBaseMapping();
    foreach ($mapping as $wrapperscheme => $wrapperbase) {
      $wrapperbaselength = drupal_strlen($wrapperbase);

      if (strncmp($wrapperbase, $url, $wrapperbaselength) === 0) {
        $uri = $wrapperscheme . '://' . drupal_substr($url, $wrapperbaselength);
        // Remove the query part
        if (($pos = strpos($uri, '?')) !== FALSE) {
          $uri = substr($uri, 0, $pos);
        }
        break;
      }
    }

    return $uri;
  }

  private static $streamBaseMapping = NULL;

  /**
   * Maintains a mapping from scheme to external URL prefix.
   *
   * @return
   *   Array from string to string.
   */
  private static function getStreamBaseMapping() {
    if (self::$streamBaseMapping != NULL) {
      return self::$streamBaseMapping;
    }

    self::$streamBaseMapping = array();

    foreach (file_get_stream_wrappers() as $wrapperscheme => $wrapperinfo) {
      $wrapper = file_stream_wrapper_get_instance_by_scheme($wrapperscheme);
      $externalUrl = $wrapper->getExternalUrl();
      // Some stream wrappers, like the ones that derive from
      // MediaReadOnlyStreamWrapper, return no default external URL. Ignore
      // those, they're not useful for the Video module anyway.
      if (!empty($externalUrl)) {
        self::$streamBaseMapping[$wrapperscheme] = $externalUrl;
      }
    }

    return self::$streamBaseMapping;
  }

  public static function getExtension($filename) {
    return drupal_strtolower(pathinfo($filename, PATHINFO_EXTENSION));
  }

  public static function imageSave($resource, $path, $type) {
    switch ($type) {
      case 'png':
        return imagepng($resource, $path);

      case 'jpg':
        return imagejpeg($resource, $path);
    }

    throw new Exception('Unknown type "' . $type . '"');
  }

  private static $tmpfilesdirs;
  public static function createTempFile($extension = '') {
    if (empty(self::$tmpfilesdirs)) {
      self::$tmpfilesdirs = array();
      drupal_register_shutdown_function(array('video_utility', 'cleanTemporaryFilesDirs'));
    }

    $tmpnam = tempnam(file_directory_temp(), 'drupal-video-');

    if (!empty($extension)) {
      drupal_unlink($tmpnam);
      $tmpnam = $tmpnam . '.' . $extension;
    }

    self::$tmpfilesdirs[] = $tmpnam;
    return $tmpnam;
  }

  public static function createTempDir() {
    $dirname = self::createTempFile();
    drupal_unlink($dirname);
    drupal_mkdir($dirname);
    return $dirname;
  }

  public static function cleanTemporaryFilesDirs() {
    if (!empty(self::$tmpfilesdirs)) {
      foreach (self::$tmpfilesdirs as $tmpfiledir) {
        self::unlink($tmpfiledir);
      }
    }
  }

  private static $temporaryCopies;
  public static function createTemporaryLocalCopy($uri) {
    if (empty(self::$temporaryCopies)) {
      self::$temporaryCopies = array();
    }
    elseif (isset(self::$temporaryCopies[$uri])) {
      return self::$temporaryCopies[$uri];
    }

    // Try to fetch the file using http. This is much faster in the case of S3 via the Amazon S3 module
    $url = file_create_url($uri);
    if ($url != $uri && ($copy = self::createTemporaryLocalCopy($url))) {
      self::$temporaryCopies[$uri] = $copy;
      return $copy;
    }

    $copy = self::createTempFile(self::getExtension($uri));
    if (copy($uri, $copy)) {
      self::$temporaryCopies[$uri] = $copy;
      return $copy;
    }

    return FALSE;
  }

  public static function unlink($uri) {
    if (is_file($uri)) {
      drupal_unlink($uri);
    }
    elseif (is_dir($uri)) {
      foreach (scandir($uri) as $obj) {
        if ($obj != '.' && $obj != '..') {
          self::unlink($uri . '/' . $obj);
        }
      }
      drupal_rmdir($uri);
    }
  }

  public static function objectToArray($data) {
    if (is_array($data) || is_object($data)) {
      $result = array();
      foreach ($data as $key => $value) {
        $result[$key] = self::objectToArray($value);
      }
      return $result;
    }
    return $data;
  }

  public static function validateDimensionList($str) {
    return preg_match('#^([1-9]\d{0,3}x[1-9]\d{0,3})(\n[1-9]\d{0,3}x[1-9]\d{0,3})*$#', $str) > 0;
  }

  public static function getDimensions() {
    $dims = explode("\n", str_replace("\r", '', variable_get('video_dimensions', self::getDefaultDimensions())));
    return array_combine($dims, $dims);
  }

  /**
   * Provide a default list of dimensions until a user saves a custom list in admin/config/media/video.
   *
   * @see http://drupal.org/node/1761800#comment-6502420
   */
  public static function getDefaultDimensions() {
    return "320x180\n320x240\n384x216\n384x288\n400x225\n400x300\n432x243\n432x324\n480x200\n480x270\n480x360\n512x288\n560x315\n560x420\n640x360\n640x480\n648x486\n720x405\n720x480\n720x486\n720x540\n720x576\n768x576\n852x480\n864x486\n960x400\n960x540\n960x720\n1024x576\n1280x720\n1280x960\n1280x1080\n1440x1080\n1920x800\n1920x1080";
  }

  /**
   * Return our list of video extensions and their associated player.
   */
  public static function getVideoExtensions() {
    return array(
      'divx' => 'video_play_divx',
      'mkv' => 'video_play_divx',
      'mov' => 'video_play_html5',
      '3gp' => 'video_play_quicktime',
      '3g2' => 'video_play_quicktime',
      'mp4' => 'video_play_html5',
      'm4v' => 'video_play_html5',
      'rm' => 'video_play_realmedia',
      'f4v' => 'video_play_flv',
      'flv' => 'video_play_flv',
      'swf' => 'video_play_flash',
      'dir' => 'video_play_dcr',
      'dcr' => 'video_play_dcr',
      'asf' => 'video_play_windowsmedia',
      'wmv' => 'video_play_windowsmedia',
      'avi' => 'video_play_windowsmedia',
      'mpg' => 'video_play_windowsmedia',
      'mpeg' => 'video_play_windowsmedia',
      'ogg' => 'video_play_html5',
      'ogv' => 'video_play_html5',
      'webm' => 'video_play_html5',
      'mp3' => 'video_play_html5',
    );
  }

  /**
   * Return our supported video players.
   */
  public static function getVideoPlayers() {
    return array(
      'video_play_html5' => t('HTML5 Player'),
      'video_play_divx' => t('Divx Player'),
      'video_play_quicktime' => t('Quicktime'),
      'video_play_realmedia' => t('Real Media Player'),
      'video_play_flv' => t('FLV Flash Players'),
      'video_play_flash' => t('SWF Flash Player'),
      'video_play_dcr' => t('Director/Shockwave'),
      'video_play_windowsmedia' => t('Windows Media Player'),
      'video_play_theora' => t('Theora Player'),
    );
  }

  /**
   * Video-specific replacement for file_get_mimetype()
   *
   * The Drupal file_get_mimetype() function lacks support for some
   * video formats.
   */
  public static function getMimeType($uri) {
    $extension = self::getExtension($uri);

    switch ($extension) {
      // Work-around for missing WebM support in file_get_mimetype().
      // See http://drupal.org/node/1347624
      case 'webm':
        return 'video/webm';

      case 'm3u8':
        // Could also be application/x-mpegurl
        return 'application/vnd.apple.mpegurl';

      case 'ts':
        // RFC3555
        return 'video/MP2T';

      default:
        return file_get_mimetype($uri);
    }
  }
}
