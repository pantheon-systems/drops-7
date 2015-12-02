<?php
/**
 * @file
 * File containing class TranscoderAbstractionFactoryFfmpeg
 */

/**
 * Class that handles FFmpeg transcoding.
 */
class TranscoderAbstractionFactoryFfmpeg extends TranscoderAbstractionFactory implements TranscoderFactoryInterface {
  const INFO_CID = 'video:transcoder:ffmpeg';
  const INFO_CACHE = 'cache';

  /**
   * @var PHPVideoToolkit
   */
  protected $transcoder = NULL;
  protected $realoutputdir = NULL;
  protected $realoutputname = NULL;
  protected $multipass = NULL;
  protected $outputextension = NULL;

  public function __construct() {
    $this->transcoder = new PHPVideoToolkit(variable_get('video_ffmpeg_path', '/usr/bin/ffmpeg'), realpath(file_directory_temp()) . '/');
    PHPVideoToolkit::$ffmpeg_info = $this->getCachedFFmpegInfo();
    parent::__construct();
  }

  public function setInput(array $file) {
    parent::setInput($file);
    $srcuri = $this->settings['input']['uri'];
    $srcpath = drupal_realpath($srcuri);
    if (empty($srcpath)) {
      // If stored on a remote file system, such as S3, download the video to a temporary file.
      $srcpath = video_utility::createTemporaryLocalCopy($srcuri);
      if (empty($srcpath)) {
        watchdog('transcoder', 'Could not download @uri to a temporary file for transcoding.', array('@uri' => $srcuri), WATCHDOG_ERROR);
        return FALSE;
      }
    }

    $result = $this->transcoder->setInputFile($srcpath);
    if ($result !== PHPVideoToolkit::RESULT_OK) {
      watchdog('transcoder', 'Error set options @message', array('@message' => $this->transcoder->getLastError()), WATCHDOG_ERROR);
      $this->errors['input'] = $this->transcoder->getLastError();
      $this->transcoder->reset();
      return FALSE;
    }

    return TRUE;
  }

  public function setOptions(array $options) {
    // Reset the transcoder class keeping the input file info
    $this->transcoder->reset(TRUE);

    $this->multipass = TRUE;
    $this->outputextension = NULL;
    $clipstart = NULL;
    $cliplength = NULL;
    $hasvideo = empty($options['skip_video']);
    $hasaudio = empty($options['skip_audio']);

    $this->setAspectRatioOptions($options);

    foreach ($options as $key => $value) {
      if (empty($value) || $value === 'none') {
        continue;
      }
      if (strncmp($key, 'audio_', 6) === 0 && !$hasaudio) {
        continue;
      }
      if (strncmp($key, 'video_', 6) === 0 && !$hasvideo) {
        continue;
      }

      $result = TRUE;
      switch ($key) {
        case 'max_frame_rate':
          $result = $this->transcoder->setVideoFrameRate($value);
          break;
        case 'video_codec':
          if ($hasvideo) {
            $result = $this->transcoder->setVideoCodec($value);
          }
          break;
        case 'video_preset':
          $result = $this->transcoder->setVideoPreset($value);
          break;
        case 'audio_sample_rate':
          if ($value < 1000) {
            $value *= 1000;
          }
          $value = min($value, 44100);
          $result = $this->transcoder->setAudioSampleFrequency($value);
          break;
        case 'audio_codec':
          $result = $this->transcoder->setAudioCodec($value);
          break;
        case 'audio_channels':
          $result = $this->transcoder->setAudioChannels($value);
          break;
        case 'audio_bitrate':
          if ($value < 1000) {
            $value .= 'k';
          }
          $result = $this->transcoder->setAudioBitRate($value);
          break;
        case 'video_bitrate':
          $result = $this->transcoder->setVideoBitRate($value);
          break;
        case 'pixel_format':
          $result = $this->transcoder->setPixelFormat($value);
          break;
        case 'h264_profile':
          $result = $this->transcoder->addCommand('-vprofile', $value);
          break;
        case 'one_pass':
          if ($value == 1) {
            $this->multipass = FALSE;
          }
          break;
        case 'video_extension':
          $this->outputextension = $value;
          break;
        case 'video_quality':
          $result = $this->transcoder->setConstantQuality($value * 20); // phpVideoToolkit expects 1 to 100 range.
          break;
        case 'clip_start':
          if (preg_match('#^(\d+)$#', $value)) {
            $clipstart = intval($value);
          }
          else {
            $clipstart = $this->transcoder->formatTimecode($value, '%hh:%mm:%ss.%ms', '%st');
          }
          break;
        case 'clip_length':
          if (preg_match('#^(\d+)$#', $value)) {
            $cliplength = intval($value);
          }
          else {
            $cliplength = $this->transcoder->formatTimecode($value, '%hh:%mm:%ss.%ms', '%st');
          }
          break;
        case 'skip_video':
          $result = $this->transcoder->addCommand('-vn');
          $this->multipass = FALSE;
          break;
        case 'skip_audio':
          $result = $this->transcoder->addCommand('-an');
          break;
      }

      if ($cliplength !== NULL && $clipstart !== NULL) {
        $result = $this->transcoder->extractSegment($clipstart, $clipstart + $cliplength, '%st', FALSE, FALSE);
        $cliplength = NULL;
        $clipstart = NULL;
      }

      if ($result !== PHPVideoToolkit::RESULT_OK) {
        watchdog('transcoder', 'Error set options @message', array('@message' => $this->transcoder->getLastError()), WATCHDOG_ERROR);
        $this->errors['options'] = $this->transcoder->getLastError();
        $this->transcoder->reset(true);
        return FALSE;
      }
    }

    return TRUE;
  }

  public function setOutput($output_directory, $output_name, $overwrite_mode = FILE_EXISTS_REPLACE) {
    $this->realoutputdir = $output_directory;
    $this->realoutputname = $output_name;

    $tmpoutput = video_utility::createTempFile(video_utility::getExtension($output_name));
    $tmpoutputdir = dirname($tmpoutput);
    $tmpoutputname = basename($tmpoutput);

    parent::setOutput($tmpoutputdir, $tmpoutputname, $overwrite_mode);

    // Overwrite is necessary to have two-pass encoding for WebM and Ogg Theora
    $result = $this->transcoder->setOutput($tmpoutputdir . '/', $tmpoutputname, PHPVideoToolkit::OVERWRITE_EXISTING);
    if ($result !== PHPVideoToolkit::RESULT_OK) {
      watchdog('transcoder', 'Error set options @message', array('@message' => $this->transcoder->getLastError()), WATCHDOG_ERROR);
      $this->errors['output'] = $this->transcoder->getLastError();
      $this->transcoder->reset(true);
      return FALSE;
    }

    return TRUE;
  }

  public function extractFrames($destinationScheme, $format) {
    // When $job is null, we are viewing the thumbnails before the form has been submitted.
    $fid = intval($this->settings['input']['fid']);
    $job = video_jobs::load($fid);

    // Get the file system directory.
    $dsturibase = $destinationScheme . '://' . variable_get('video_thumbnail_path', 'videos/thumbnails') . '/' . $fid . '/';
    file_prepare_directory($dsturibase, FILE_CREATE_DIRECTORY);
    $dstwrapper = file_stream_wrapper_get_instance_by_scheme($destinationScheme);

    // get the video file informations
    $file_info = $this->getFileInfo();
    $duration = floor($file_info['duration']['seconds']);
    $no_of_thumbnails = variable_get('video_thumbnail_count', 5);

    // Precaution for very short videos
    if ((2 * $no_of_thumbnails) > $duration) {
      $no_of_thumbnails = floor($duration / 2);
    }

    $thumbs = array();
    for ($i = 1; $i <= $no_of_thumbnails; $i++) {
      $seek = ceil($duration / ($no_of_thumbnails + 1) * $i);
      $filename = file_munge_filename('thumbnail-' . $fid . '_' . sprintf('%04d', $i) . '.' . $format, '', FALSE);
      $dsturi = $dsturibase . $filename;

      if (!file_exists($dsturi)) {
        // Create a temporary file that will be moved to the final URI later
        $dstpath = video_utility::createTempFile($format);

        $error = NULL;
        if (class_exists('ffmpeg_movie')) {
          $movie = new ffmpeg_movie($this->transcoder->getInputFile());
          $frames = $movie->getFrameCount();
          $fps = $movie->getFrameRate();
          $frame = $movie->getFrame(min($frames, (int) $seek * $fps));
          $thumb = $frame->toGDImage();
          $result = video_utility::imageSave($thumb, $dstpath, $format);
          if (!$result) {
            $error = t('Unknown FFmpeg-php error');
          }
        }
        else {
          $this->transcoder->extractFrame($seek, FALSE, '%st');
          $result = $this->transcoder->setOutput(dirname($dstpath) . '/', basename($dstpath), PHPVideoToolkit::OVERWRITE_EXISTING);
          if ($result === PHPVideoToolkit::RESULT_OK) {
            $result = $this->transcoder->execute() === PHPVideoToolkit::RESULT_OK;
          }

          if (!$result) {
            $error = $this->transcoder->getLastError();
            $this->transcoder->reset(true);
          }
        }

        // Check if the extraction was successfull
        if ($error === NULL) {
          if (!file_exists($dstpath)) {
            $error = t('generated file %file does not exist', array('%file' => $dstpath));
          }
          elseif (filesize($dstpath) == 0) {
            $error = t('generated file %file is empty', array('%file' => $dstpath));
            drupal_unlink($dstpath);
          }
        }
        if ($error !== NULL) {
          form_set_error(NULL, t('Error generating thumbnail for video %videofilename: !error.', array('%videofilename' => $this->settings['input']['filename'], '!error' => $error)));
          watchdog('transcoder', 'Error generating thumbnail for video %videofilename: !error.', array('%videofilename' => $this->settings['input']['filename'], '!error' => $error), WATCHDOG_ERROR);
          continue;
        }

        // Move the file to the final URI
        copy($dstpath, $dsturi);
      }

      // Begin building the file object.
      $thumb = new stdClass();
      $thumb->status = 0;
      $thumb->filename = basename($dsturi);
      $thumb->uri = $dsturi;
      $thumb->filemime = $dstwrapper->getMimeType($dsturi);
      $thumbs[] = $thumb;
    }

    return !empty($thumbs) ? $thumbs : FALSE;
  }

  public function execute() {
    // Execute the command in a temporary directory
    $drupaldir = getcwd();
    $tmpdir = video_utility::createTempDir();
    chmod($tmpdir, 0777);
    chdir($tmpdir);

    // Execute the command
    $result = $this->transcoder->execute($this->multipass);

    // Restore the directory
    chdir($drupaldir);

    // Log an error when trancoding fails
    $tmpoutputpath = $this->settings['base_url'] . '/' . $this->settings['filename'];
    if ($result !== PHPVideoToolkit::RESULT_OK || !file_exists($tmpoutputpath) || ($filesize = filesize($tmpoutputpath)) == 0) {
      $errorlist = $this->transcoder->getErrors();
      $_commandoutput = $this->transcoder->getCommandOutput();
      $commandoutput = array();
      foreach ($_commandoutput as $cmd) {
        $commandoutput[] = '<pre>' . check_plain($cmd['command']) . '</pre><pre>' . check_plain($cmd['output']) . '</p>';
      }

      watchdog('transcoder', 'FFmpeg failed to transcode %video. !errorlist !commandlist', array(
        '%video' => $this->settings['input']['filename'],
        '!errorlist' => theme('item_list', array('type' => 'ol', 'items' => $errorlist, 'title' => t('Reported errors'))),
        '!commandlist' => theme('item_list', array('type' => 'ol', 'items' => $commandoutput, 'title' => t('Executed commands and output')))
      ), WATCHDOG_ERROR);
      $this->errors['execute'] = $errorlist;
      $this->transcoder->reset(true);
      return FALSE;
    }

    // Post-process the file with qt-faststart
    $cmd = variable_get('video_ffmpeg_qtfaststart_path');

    if ($cmd != NULL && is_file($cmd) && $this->outputextension == 'mp4') {
      $qttmpfile = $tmpoutputpath . '-qt';
      $output = array();
      $retval = 0;
      exec($cmd . ' ' . escapeshellarg($tmpoutputpath) . ' ' . escapeshellarg($qttmpfile) . ' 2>&1', $output, $retval);

      // qt-faststart does not return an error code when it doesn't generate an output file,
      // so also check if the output file has been generated.
      if ($retval == 0 && file_exists($qttmpfile)) {
        drupal_unlink($tmpoutputpath);
        rename($qttmpfile, $tmpoutputpath);
      }
      else {
        watchdog('transcoder', 'Error while executing @cmdname on video @filename: @output', array('@cmdname' => 'qt-faststart', '@filename' => $this->realoutputname, '@output' => implode("\n", $output)), WATCHDOG_ERROR);
        if (file_exists($qttmpfile)) {
          drupal_unlink($qttmpfile);
        }
      }
    }

    // Post-process the file with flvtool2
    $cmd = variable_get('video_ffmpeg_flvtool2_path');
    if ($cmd != NULL && is_file($cmd) && $this->outputextension == 'flv') {
      $output = array();
      $retval = 0;
      exec($cmd . ' -U ' . escapeshellarg($tmpoutputpath) . ' 2>&1', $output, $retval);

      if ($retval != 0) {
        watchdog('transcoder', 'Error while executing @cmdname on video @filename: @output', array('@cmdname' => 'flvtool2', '@filename' => $this->realoutputname, '@output' => implode("\n", $output)), WATCHDOG_ERROR);
      }
    }

    $file_info = $this->getFileInfo();
    $realoutputuri = $this->realoutputdir . '/' . $this->realoutputname;
    copy($tmpoutputpath, $realoutputuri);
    drupal_unlink($tmpoutputpath);

    $output = new stdClass();
    $output->filename = $this->realoutputname;
    $output->uri = $realoutputuri;
    $output->filesize = $filesize;
    $output->timestamp = REQUEST_TIME;
    $output->jobid = NULL;
    $output->duration = floor($file_info['duration']['seconds']);

    return $output;
  }

  public function getFileInfo() {
    $file = $this->settings['input']['uri'];
    $cid = 'video:file:' . md5($file);
    $cache = cache_get($cid);
    if (!empty($cache->data)) {
      return $cache->data;
    }

    $data = $this->transcoder->getFileInfo();
    cache_set($cid, $data, self::INFO_CACHE, time() + 7 * 24 * 3600);
    return $data;
  }

  public function getCodecs() {
    $info = $this->getCachedFFmpegInfo();
    $codecs = array(
      'decode' => array('audio' => array(), 'video' => array()),
      'encode' => array('audio' => array(), 'video' => array()),
    );

    if (!empty($info['codecs'])) {
      foreach ($info['codecs'] as $key => $value) {
        $codecs['encode'][$key] = array();
        $codecs['decode'][$key] = array();
        foreach ($value as $codec_key => $codec) {
          if ($codec['encode']) {
            $codecs['encode'][$key][$codec_key] = $codec['fullname'];
          }
          if ($codec['decode']) {
            $codecs['decode'][$key][$codec_key] = $codec['fullname'];
          }
        }
        uasort($codecs['encode'][$key], 'strnatcasecmp');
        uasort($codecs['encode'][$key], 'strnatcasecmp');
      }
    }

    return $codecs;
  }

  public function getAvailableFormats($type = FALSE) {
    $info = $this->getCachedFFmpegInfo();
    if (empty($info['formats'])) {
      return array();
    }

    $formats = array();
    switch ($type) {
      case FALSE:
        return array_keys($info['formats']);
      case 'both' :
        foreach ($info['formats'] as $id => $data) {
          if ($data['mux'] === TRUE && $data['demux'] === TRUE) {
            $formats[$id] = $data['fullname'];
          }
        }
        break;
      case 'muxing' :
        foreach ($info['formats'] as $id => $data) {
          if ($data['mux'] === TRUE) {
            $formats[$id] = $data['fullname'];
          }
        }
        break;
      case 'demuxing' :
        foreach ($info['formats'] as $id => $data) {
          if ($data['demux'] === TRUE) {
            $formats[$id] = $data['fullname'];
          }
        }
        break;
    }

    if (isset($formats['ogg']) && !isset($formats['ogv'])) {
      $formats['ogv'] = $formats['ogg'];
      unset($formats['ogg']);
      asort($formats);
    }

    return $formats;
  }

  public function getPixelFormats() {
    $info = $this->getCachedFFmpegInfo();
    if (empty($info['pixelformats'])) {
      return array();
    }
    return array_keys($info['pixelformats']);
  }

  public function getVersion() {
    $info = $this->getCachedFFmpegInfo();
    if ($info['ffmpeg-found']) {
      $ffmpegoravconv = NULL;
      return self::getVersionFromOutput($info['raw'], $ffmpegoravconv);
    }

    return FALSE;
  }

  public function getName() {
    return 'FFmpeg / avconv';
  }

  public function getValue() {
    return 'TranscoderAbstractionFactoryFfmpeg';
  }

  public function adminSettings() {
    $form = array();

    $form['ffmpeg'] = array(
      '#type' => 'fieldset',
      '#title' => $this->getName(),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#states' => array(
        'visible' => array(
          ':input[name=video_convertor]' => array('value' => 'TranscoderAbstractionFactoryFfmpeg'),
        ),
      ),
    );

    $form['ffmpeg']['video_ffmpeg_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to FFmpeg or avconv executable'),
      '#description' => t('Absolute path to the FFmpeg or avconv executable.') . ' ' . t('When you install a new FFmpeg version, please <a href="@performance-page">clear the caches</a> to let Drupal detect the updated codec support.', array('@performance-page' => url('admin/config/development/performance'))),
      '#default_value' => variable_get('video_ffmpeg_path', '/usr/bin/ffmpeg'),
    );

    // Video thumbnails
    $form['ffmpeg']['video_thumbnail_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to save thumbnails'),
      '#description' => t('Path to save video thumbnails extracted from videos.'),
      '#default_value' => variable_get('video_thumbnail_path', 'videos/thumbnails'),
    );
    $form['ffmpeg']['video_thumbnail_count'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of thumbnails'),
      '#description' => t('Number of thumbnails to extract from video.'),
      '#default_value' => variable_get('video_thumbnail_count', 5),
      '#size' => 5,
    );

    // Video conversion settings.
    $form['ffmpeg']['helpers'] = array(
      '#type' => 'fieldset',
      '#title' => t('Helper programs'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['ffmpeg']['helpers']['video_ffmpeg_qtfaststart_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to qt-faststart'),
      '#default_value' => variable_get('video_ffmpeg_qtfaststart_path'),
      '#description' => t('When you enter the path to @toolname here, it will be run after any @filetype files are transcoded. It improves the loading time of @filetype videos. On Linux, the default for this field is %default_value.', array('@toolname' => 'qt-faststart', '@filetype' => 'MP4', '%default_value' => '/usr/bin/qt-faststart')),
    );
    $form['ffmpeg']['helpers']['video_ffmpeg_flvtool2_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to flvtool2'),
      '#default_value' => variable_get('video_ffmpeg_flvtool2_path'),
      '#description' => t('When you enter the path to @toolname here, it will be run after any @filetype files are transcoded. It improves the loading time of @filetype videos. On Linux, the default for this field is %default_value.', array('@toolname' => 'flvtool2', '@filetype' => 'FLV', '%default_value' => '/usr/bin/flvtool2')),
    );

    return $form;
  }

  public function adminSettingsValidate($form, &$form_state) {
    $v =& $form_state['values'];

    if (!empty($v['video_ffmpeg_path'])) {
      $errorhelp = '';
      if (module_exists('video_ui')) {
        $errorhelp = '<br/>' . t('Visit the <a href="@ffmpeg-debug-page">FFmpeg debug page</a> for information thay may help you find the cause of this problem.', array('@ffmpeg-debug-page' => url('admin/config/media/video/ffmpeg-info', array('query' => array('ffmpegpath' => $v['video_ffmpeg_path'])))));
      }

      $toolkit = new PHPVideoToolkit($v['video_ffmpeg_path']);
      $ffmpeg = $toolkit->getFFmpegInfo(FALSE);
      $ffmpegoravconv = NULL;
      if (!$ffmpeg['ffmpeg-found'] || ($version = self::getVersionFromOutput($ffmpeg['raw'], $ffmpegoravconv)) == NULL) {
        form_error($form['ffmpeg']['video_ffmpeg_path'], t('FFmpeg not found at %path. To convert videos and create thumbnails you have to install FFmpeg on your server. For more information please see the !documentation.' . $errorhelp, array('%path' => $v['video_ffmpeg_path'], '!documentation' => l(t('documentation'), 'http://video.heidisoft.com/documentation/ffmpeg-installtion-scripts'))));
      }
      elseif (empty($ffmpeg['codecs']['video']) || empty($ffmpeg['codecs']['audio'])) {
        form_error($form['ffmpeg']['video_ffmpeg_path'], t('FFmpeg version %version was found, but no supported codecs could be found. Please check whether FFmpeg and all support libraries have been installed correctly.' . $errorhelp, array('%version' => $version)));
      }
      else {
        drupal_set_message(t('%ffmpegoravconv version %version found on your system.', array('%version' => $version, '%ffmpegoravconv' => $ffmpegoravconv)), 'status');
      }

      // Clear FFmpeg information when the path has changed.
      cache_clear_all(self::INFO_CID, self::INFO_CACHE);
    }

    if (!empty($v['video_ffmpeg_qtfaststart_path'])) {
      if (!is_file($v['video_ffmpeg_qtfaststart_path'])) {
        form_error($form['ffmpeg']['helpers']['video_ffmpeg_qtfaststart_path'], t('The file %file does not exist.', array('%file' => $v['video_ffmpeg_qtfaststart_path'])));
      }
    }

    if (!empty($v['video_ffmpeg_flvtool2_path'])) {
      if (!is_file($v['video_ffmpeg_flvtool2_path'])) {
        form_error($form['ffmpeg']['helpers']['video_ffmpeg_flvtool2_path'], t('The file %file does not exist.', array('%file' => $v['video_ffmpeg_flvtool2_path'])));
      }
    }
  }

  /**
   * Returns a cached copy of PHPVideoToolkit::getFFmpegInfo()
   *
   * @return
   *   array of FFmpeg installation information.
   */
  private function getCachedFFmpegInfo() {
    $cache = cache_get(self::INFO_CID, self::INFO_CACHE);
    if (!empty($cache->data)) {
      return $cache->data;
    }

    $info = $this->transcoder->getFFmpegInfo(FALSE);
    cache_set(self::INFO_CID, $info, self::INFO_CACHE);
    return $info;
  }

  /**
   * Returns the FFmpeg version string from an output string.
   *
   * FFmpeg returns its version in the output of almost any call.
   *
   * Used instead of PHPVideoToolkit::getVersion(), because PHPVideoToolkit
   * has not been updated and does not support git versions.
   *
   * @param
   *   string containing output of ffmpeg -version
   * @return
   *   string containing version of FFmpeg
   */
  public static function getVersionFromOutput($output, &$ffmpegoravconv) {
    $version = NULL;
    $ffmpegoravconv = NULL;

    $matches = array();

    // Git check outs
    if (preg_match('/(ffmpeg|avconv) version N-\d+-g([a-f0-9]+)/', $output, $matches)) {
      $ffmpegoravconv = $matches[1];
      $version = 'git: ' . $matches[2];
    }

    // Git check outs
    elseif (preg_match('/(ffmpeg|avconv) version git-(\d{4}-\d{2}-\d{2}-[a-f0-9]+)/', $output, $matches)) {
      $ffmpegoravconv = $matches[1];
      $version = 'git: ' . $matches[2];
    }

    // Release
    elseif (preg_match('/(ffmpeg|avconv) version ([0-9.]+)/i', $output, $matches)) {
      $ffmpegoravconv = $matches[1];
      $version = $matches[2];
    }

    // Fallback to unrecognized string
    elseif (preg_match('/(ffmpeg|avconv) version ([^\n]+)/i', $output, $matches)) {
      $ffmpegoravconv = $matches[1];
      $version = $matches[2];
      if (($pos = strpos($version, ' Copyright')) !== FALSE) {
        $version = drupal_substr($version, 0, $pos);
      }
      $version = t('unrecognized: !version', array('!version' => $version));
    }

    if ($ffmpegoravconv == 'ffmpeg') {
      $ffmpegoravconv = 'FFmpeg';
    }

    return $version;
  }

  /**
   * Reset internal variables to their initial state.
   */
  public function reset($keepinput = FALSE) {
    parent::reset($keepinput);
    $this->transcoder->reset($keepinput);
    $this->multipass = NULL;
  }

  /**
   * Whether the transcoder works by sending jobs to an external system.
   *
   * True for transcoders like Zencoder, false for transcoders like FFmpeg.
   */
  public function isOffSite() {
    return FALSE;
  }

  /**
   * Set aspect ratio and size related transcoder options.
   */
  private function setAspectRatioOptions(array $options) {
    $targetdimensions = explode('x', $options['wxh'], 2);
    $file_info = $this->getFileInfo();
    $sourcedimensions = array(
      intval($file_info['video']['dimensions']['width']),
      intval($file_info['video']['dimensions']['height'])
    );

    // If no change is necessary, just return.
    if ($targetdimensions == $sourcedimensions) {
      return;
    }

    $targetaspect = round($targetdimensions[0] / $targetdimensions[1], 4);
    $sourceaspect = round($sourcedimensions[0] / $sourcedimensions[1], 4);

    $aspectmode = !empty($options['video_aspectmode']) ? $options['video_aspectmode'] : 'preserve';
    switch ($aspectmode) {
      default:
      case 'preserve':
        if ($sourceaspect >= $targetaspect) {
          // Source video is wider than destination.
          // Decrease the height.
          $factor = $targetdimensions[0] < $sourcedimensions[0] ? $targetdimensions[0] / $sourcedimensions[0] : $sourcedimensions[0] / $targetdimensions[0];
          $width = $targetdimensions[0];
          $height = round($sourcedimensions[1] * $factor);
        }
        else {
          $factor = $targetdimensions[1] < $sourcedimensions[1] ? $targetdimensions[1] / $sourcedimensions[1] : $sourcedimensions[1] / $targetdimensions[1];
          $width = round($sourcedimensions[0] * $factor);
          $height = $targetdimensions[1];
        }

        if ($width != $sourcedimensions[0] || $height != $sourcedimensions[1]) {
          $this->transcoder->addCommand('-vf', 'scale='.$width.':'.$height);
        }
        break;

      case 'crop':
        if ($sourceaspect >= $targetaspect) {
          // Source video is wider than destination.
          // Cut off left and right.
          $factor = $targetdimensions[1] < $sourcedimensions[1] ? $targetdimensions[1] / $sourcedimensions[1] : $sourcedimensions[1] / $targetdimensions[1];
          $width = round($factor * $sourcedimensions[0]);
          $height = $sourcedimensions[1];
        }
        else {
          $factor = $targetdimensions[0] < $sourcedimensions[0] ? $targetdimensions[0] / $sourcedimensions[0] : $sourcedimensions[0] / $targetdimensions[0];
          $width = $sourcedimensions[0];
          $height = round($factor * $sourcedimensions[1]);
        }
        $this->transcoder->addCommand('-vf', 'crop='.$width.':'.$height.',scale='.$targetdimensions[0].':'.$targetdimensions[1]);
        break;

      case 'pad':
        if ($sourceaspect >= $targetaspect) {
          // Source video is wider than destination.
          // Add padding to top and bottom.
          $factor = $targetdimensions[1] > $sourcedimensions[1] ? $targetdimensions[1] / $sourcedimensions[1] : $sourcedimensions[1] / $targetdimensions[1];
          $width = $sourcedimensions[0];
          $height = round($factor * $sourcedimensions[1]);
          $x = 0;
          $y = ($height - $sourcedimensions[1]) / 2;
        }
        else {
          $factor = $targetdimensions[0] > $sourcedimensions[0] ? $targetdimensions[0] / $sourcedimensions[0] : $sourcedimensions[0] / $targetdimensions[0];
          $width = round($factor * $sourcedimensions[0]);
          $height = $sourcedimensions[1];
          $x = ($width - $sourcedimensions[0]) / 2;
          $y = 0;
        }
        $this->transcoder->addCommand('-vf', 'pad='.$width.':'.$height.':'.$x.':'.$y.',scale='.$targetdimensions[0].':'.$targetdimensions[1]);
        break;

      case 'stretch':
        $result = $this->transcoder->setVideoDimensions($targetdimensions[0], $targetdimensions[1]);
        break;
    }
  }
}
