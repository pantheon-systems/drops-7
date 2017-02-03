<?php
/**
 * @file
 * Class file used to wrap the transcoder helper functions.
 */

class Transcoder {
  /**
   * @var TranscoderFactoryInterface
   */
  private $transcoder;

  public function __construct() {
    $transcoder = variable_get('video_convertor', 'TranscoderAbstractionFactoryFfmpeg');
    $this->transcoder = self::createTranscoder($transcoder);
  }

  public function hasTranscoder() {
    return $this->transcoder != NULL;
  }

  /**
   * Returns the current transcoder implementation.
   *
   * @return
   *   TranscoderFactoryInterface
   */
  public function getTranscoder() {
    return $this->transcoder;
  }

  /**
   * Extract frames from the video file. This helper function will interact with
   * only the database and it will save all the thumbnail file reference in to
   * the database.
   *
   * @return
   *   array of file objects, or false on failure
   */
  public function extractFrames(array $video, array $field) {
    global $user;

    $thumbnails = db_query('SELECT f.* FROM {file_managed} f INNER JOIN {video_thumbnails} tn ON tn.thumbnailfid = f.fid WHERE tn.videofid = :fid ORDER BY f.fid', array(':fid' => $video['fid']))->fetchAllAssoc('fid');
    if (!empty($thumbnails)) {
      return $thumbnails;
    }

    if ($this->transcoder == NULL) {
      return array();
    }
    $scheme = !empty($field['settings']['uri_scheme_thumbnails']) ? $field['settings']['uri_scheme_thumbnails'] : 'public';
    $format = !empty($field['settings']['thumbnail_format']) ? $field['settings']['thumbnail_format'] : 'png';
    $this->transcoder->setInput($video);
    $thumbnails = $this->transcoder->extractFrames($scheme, $format);
    $this->transcoder->reset();

    if ($thumbnails === FALSE) {
      return FALSE;
    }

    foreach ($thumbnails as $thumb) {
      // Determine whether there is an existing thumbnail
      $thumb->fid = (int)db_query('SELECT fid FROM {file_managed} WHERE uri = :uri', array(':uri' => $thumb->uri))->fetchField(0);
      $thumb->uid = (int)$user->uid;
      // For the media module
      $thumb->type = 'image';
      file_save($thumb);

      db_merge('video_thumbnails')->key(array('videofid' => $video['fid'], 'thumbnailfid' => $thumb->fid))->execute();
    }

    return $thumbnails;
  }

  /**
   * Processes up to 'video_ffmpeg_instances' jobs in the current thread.
   *
   * @see video_jobs::loadQueue()
   */
  public function runQueue() {
    if ($this->hasTranscoder() && $videos = video_jobs::loadQueue()) {
      foreach ($videos as $video) {
        $this->executeConversion($video);
      }
    }
  }

  /**
   * This helper function will help to execute video conversion job by loading
   * job from the database and once it completed saving its data in to the
   * database.
   *
   * @param $video
   * @return
   *   TRUE on success, FALSE on failure. Also check $video->video_status.
   */
  public function executeConversion(stdClass $video) {
    global $user;

    // Check the video state
    if ($video->video_status != VIDEO_RENDERING_INQUEUE && $video->video_status != VIDEO_RENDERING_PENDING) {
      $status = array(
        VIDEO_RENDERING_ACTIVE => 'activated previously',
        VIDEO_RENDERING_COMPLETE => 'completed',
        VIDEO_RENDERING_FAILED => 'failed'
      );
      watchdog('transcoder', 'Video conversion has been @status. You should add video to the queue. Please check the re-queue to enable the video conversion.', array('@status' => $status[$video->video_status]), WATCHDOG_WARNING);
      return FALSE;
    }

    if ($this->transcoder == NULL) {
      return FALSE;
    }

    // update the video conversion start time and status
    $video->statusupdated = time();
    $video->started = $video->statusupdated;
    $video->video_status = VIDEO_RENDERING_ACTIVE;
    video_jobs::update($video);

    $fieldname = !empty($video->data['field_name']) ? $video->data['field_name'] : NULL;
    $presets = Preset::getEnabledPresets($fieldname);
    $converted_scheme = file_uri_scheme($video->uri);
    $thumbnail_format = 'png';
    $thumbnail_number = intval(variable_get('video_thumbnail_count', 5));

    // Apply field-specific settings
    if ($fieldname != NULL) {
      $field = field_info_field($video->data['field_name']);
      // Find the scheme and thumbnail format for the converted videos
      if (!empty($field['settings']['uri_scheme_converted'])) {
        $converted_scheme = $field['settings']['uri_scheme_converted'];
      }
      if (!empty($field['settings']['thumbnail_format'])) {
        $thumbnail_format = $field['settings']['thumbnail_format'];
      }
      // If no automatic thumbnail generation, set thumbnail number to 0
      if ($field['settings']['autothumbnail'] != 'auto') {
        $thumbnail_number = 0;
      }
    }

    $this->transcoder->setInput((array) $video);
    $transcodingsuccess = TRUE;
    $output = array();
    $output_directory = str_replace('original', 'converted', drupal_dirname($video->uri)) . '/' . $video->fid;
    $output_directory = $converted_scheme . '://' . file_uri_target($output_directory);

    if (!file_prepare_directory($output_directory, FILE_CREATE_DIRECTORY)) {
      watchdog('transcoder', 'Video conversion failed. Could not create the directory: %dir', array('%dir' => $output_directory), WATCHDOG_ERROR);
      $transcodingsuccess = FALSE;
    }
    // if no presets enabled then write an error log
    elseif (empty($presets)) {
      watchdog('transcoder', 'No preset enabled. Please !presets_message.', array('!presets_message' => l(t('enable or create a preset'), 'admin/config/media/video/presets')), WATCHDOG_ERROR, 'admin/config/media/video/presets');
      $transcodingsuccess = FALSE;
    }
    else {
      foreach ($presets as $name => $preset) {
        // override the widthXheight if enabled
        $preset['settings']['wxh'] = (variable_get('video_use_preset_wxh', FALSE)) ? $preset['settings']['wxh'] : $video->dimensions;
        $preset['settings']['thumbnails']['format'] = $thumbnail_format;
        $preset['settings']['thumbnails']['number'] = $thumbnail_number;

        // Only create thumbnails for the first preset
        $thumbnail_number = 0;

        // set transcoder options
        if (!$this->transcoder->setOptions($preset['settings'])) {
          // setOptions should write to the watchdog log.
          $transcodingsuccess = FALSE;
          break;
        }

        $output_name = file_munge_filename(str_replace(' ', '_', pathinfo($video->filename, PATHINFO_FILENAME) . ' ' . strtolower($name)) . '_' . time() . '.' . $preset['settings']['video_extension'], '');
        $this->transcoder->setOutput($output_directory, $output_name);
        if ($output_file = $this->transcoder->execute()) {
          $output[] = $output_file;
        }
        else {
          $transcodingsuccess = FALSE;
          break;
        }

        $this->transcoder->reset(TRUE);
      }
    }

    if (!$transcodingsuccess) {
      video_jobs::setFailed($video);
    }
    else {
      // add files to file_managed table and add reference to the file_usage table.
      $offsite = $this->transcoder->isOffSite();
      $this->cleanConverted($video->fid);
      foreach ($output as $file) {
        $file->filemime = video_utility::getMimeType($file->uri);
        $file->status = FILE_STATUS_PERMANENT;
        $file->uid = $video->uid;
        $file->type = 'video'; // For the media module
        // Empty file to prevent 'Warning: filesize(): stat failed' error.
        if ($offsite && $file->filesize == 0) {
          file_put_contents($file->uri, '');
        }
        file_save($file);
        file_usage_add($file, 'file', $video->entity_type, $video->entity_id);
        $output_vid = array(
          'vid' => $video->vid,
          'original_fid' => $video->fid,
          'output_fid' => $file->fid,
          'job_id' => !empty($file->jobid) ? $file->jobid : NULL,
        );
        drupal_write_record('video_output', $output_vid);
      }

      // add duration to the video_queue table
      $video->duration = round($file->duration);

      // Change the status if the file exists for onsite transcoders.
      if (!$offsite && file_exists($file->uri)) {
        video_jobs::setCompleted($video);
      }
      else {
        // Else: just update the video.
        video_jobs::update($video);
      }
    }

    $this->transcoder->reset();

    return $transcodingsuccess;
  }

  /**
   * This helper function clean the database records if exist for current job.
   */
  protected function cleanConverted($fid) {
    $output_fids = db_select('video_output', 'vo')
      ->fields('vo', array('output_fid'))
      ->condition('original_fid', $fid)
      ->execute()->fetchCol();

    if (empty($output_fids)) {
      return;
    }

    $output_files = file_load_multiple($output_fids);

    // Delete for all output files file_usage and file if not used anymore.
    foreach ($output_files as $output_file) {
      file_usage_delete($output_file, 'file');
      file_delete($output_file);
    }

    // Delete original_fid and all output_fid's from video_output table.
    db_delete('video_output')
      ->condition('original_fid', $fid)
      ->execute();
  }

  /**
   * Retuns all transcoders implemented to work with the video module.
   */
  public function getAllTranscoders() {
    // Lets find our transcoder classes and build our radio options
    // We do this by scanning our transcoders folder
    $form = array(
      'radios' => array(
        '' => t('No transcoder'),
      ),
      'help' => array(),
      'admin_settings' => array(),
    );
    // check inside sub modules
    $modules = module_list();
    $files = array();
    foreach ($modules as $module) {
      $module_files = array();
      $module_path = drupal_get_path('module', $module) . '/transcoders';
      foreach (file_scan_directory($module_path, '/.*\.inc/') as $filekey => $file) {
        $file->module = $module;
        $module_files[] = $file;
      }
      $files = array_merge($files, $module_files);
    }
    foreach ($files as $file) {
      module_load_include('inc', $file->module, '/transcoders/' . $file->name);
      $focus = new $file->name;

      $errorMessage = '';
      if (!$focus->isAvailable($errorMessage)) {
        $form['help'][] = t('@name is unavailable: !errormessage', array('@name' => $focus->getName(), '!errormessage' => $errorMessage));
      }
      else {
        $form['radios'][$file->name] = check_plain($focus->getName());
        $form['admin_settings'] = $form['admin_settings'] + $focus->adminSettings();
      }
    }
    return $form;
  }

  /**
   * Create a new instance of the transcoder implementation.
   *
   * @return
   *   TranscoderFactoryInterface
   */
  public static function createTranscoder($transcoder) {
    if ($transcoder == '') {
      return NULL;
    }

    if (!class_exists($transcoder, TRUE)) {
      drupal_set_message(t('The transcoder %transcoder is not configured properly.', array('%transcoder' => $transcoder)), 'error');
    }

    return new $transcoder;
  }
}
