<?php
/**
 * @file
 * File containing class TranscoderAbstractionFactoryZencoder
 */

/**
 * Class that handles Zencoder transcoding.
 */
class TranscoderAbstractionFactoryZencoder extends TranscoderAbstractionFactory implements TranscoderFactoryInterface {
  protected $options = array();
  private $postbackurl;
  private $outputdestination;

  public function __construct() {
    parent::__construct();
    $this->options['api_key'] = variable_get('video_zencoder_api_key');
    $this->postbackurl = variable_get('video_zencoder_postback', url('postback/jobs', array('absolute' => TRUE)));
    $this->outputdestination = variable_get('video_zencoder_output_destination');
  }

  public function setInput(array $file) {
    parent::setInput($file);
    $this->options['input'] = file_create_url($this->settings['input']['uri']);

    if (variable_get('video_zencoder_testing_mode', FALSE)) {
      $this->options['input'] = variable_get('video_zencoder_test_file_path', 'http://example.com/video.mp4');
    }
  }

  public function setOptions(array $options) {
    foreach ($options as $key => $value) {
      if (empty($value) || $value === 'none') {
        continue;
      }

      switch ($key) {
        case 'pixel_format':
        case 'video_preset':
        case 'default':
          break;
        case 'video_extension':
          $this->options['output']['format'] = $value;
          break;
        case 'wxh':
          $this->options['output']['size'] = $value;
          break;
        case 'video_quality':
          $this->options['output']['quality'] = intval($value);
          break;
        case 'video_speed':
          $this->options['output']['speed'] = intval($value);
          break;
        case 'video_upscale':
          $this->options['output']['upscale'] = $value;
          break;
        case 'one_pass':
          $this->options['output']['one_pass'] = $value == 1;
          break;
        case 'video_aspectmode':
          $this->options['output']['aspect_mode'] = $value;
          break;
        case 'bitrate_cap':
          $this->options['output']['decoder_bitrate_cap'] = intval($value);
          break;
        case 'buffer_size':
          $this->options['output']['decoder_buffer_size'] = intval($value);
          break;
        default:
          if (strncmp('video_watermark_', $key, 16) === 0) {
            break;
          }
          $this->options['output'][$key] = $value;
          break;
      }
    }

    // set notifications
    $this->options['output']['notifications']['format'] = 'json';
    $this->options['output']['notifications']['url'] = $this->postbackurl;

    // thumbnails
    if ($this->options['output']['thumbnails']['number'] > 0) {
      $this->options['output']['thumbnails'] = array(
        'format' => $this->options['output']['thumbnails']['format'],
        'number' => $this->options['output']['thumbnails']['number'],
        'size' => variable_get('video_thumbnail_size', '320x240'),
        'prefix' => 'thumbnail-' . $this->settings['input']['fid'],
      );
    }
    else {
      unset($this->options['output']['thumbnails']);
    }

    // watermark
    if (!empty($options['video_watermark_enabled']) && !empty($options['video_watermark_fid'])) {
      $file = file_load($options['video_watermark_fid']);
      $audioonly = !empty($options['video_watermark_onlyforaudio']);
      $isaudio = strncmp($this->settings['input']['filemime'], 'audio/', 6) === 0;

      if (!empty($file) && (!$audioonly || $isaudio)) {
        $wm = array('url' => file_create_url($file->uri));
        if (isset($options['video_watermark_y']) && $options['video_watermark_y'] !== '') {
          $wm['y'] = $options['video_watermark_y'];
        }
        if (isset($options['video_watermark_x']) && $options['video_watermark_x'] !== '') {
          $wm['x'] = $options['video_watermark_x'];
        }
        if (isset($options['video_watermark_height']) && $options['video_watermark_height'] !== '') {
          $wm['height'] = $options['video_watermark_height'];
        }
        if (isset($options['video_watermark_width']) && $options['video_watermark_width'] !== '') {
          $wm['width'] = $options['video_watermark_width'];
        }
        $this->options['output']['watermarks'] = array($wm);
      }
    }

    return TRUE;
  }

  public function setOutput($output_directory, $output_name, $overwrite_mode = FILE_EXISTS_REPLACE) {
    parent::setOutput($output_directory, $output_name, $overwrite_mode);
    $this->options['output']['label'] = 'video-' . $this->settings['input']['fid'];
    $this->options['output']['filename'] = $this->settings['filename'];
    $this->options['output']['public'] = !variable_get('video_zencoder_private', FALSE);
    $baseurl = NULL;

    if ($this->outputdestination == 's3') {
      $bucket = variable_get('amazons3_bucket');
      // For now, silently ignore the "Use Amazon S3 module" setting when the bucket is not found
      if ($bucket !== NULL) {
        $baseurl = 's3://' . $bucket . '/';
      }
    }
    elseif ($this->outputdestination == 'rcf') {
      $username = variable_get('rackspace_cloud_username');
      $key = variable_get('rackspace_cloud_api_key');
      $container = variable_get('rackspace_cloud_container');
      $authurl = variable_get('rackspace_cloud_auth_url');
      // For now, silently ignore the "Use Rackspace Cloud Files module" setting when the cloudfiles module isn't setup
      if ($username !== NULL && $key !== NULL && $container !== NULL && $authurl !== NULL) {
        $scheme = $authurl == 'https://lon.auth.api.rackspacecloud.com' ? 'cf+uk' : 'cf';
        $baseurl = $scheme . '://' . rawurlencode($username) . ':' . rawurlencode($key) . '@' . $container . '/';
      }
    }

    if ($baseurl != NULL) {
      $this->options['output']['base_url'] = $baseurl . file_uri_target($output_directory) . '/';

      if (isset($this->options['output']['thumbnails'])) {
        $this->options['output']['thumbnails']['base_url'] = $baseurl . variable_get('video_thumbnail_path', 'videos/thumbnails') . '/' . $this->settings['input']['fid'] . '/';
      }
    }
  }

  /**
   * For new videos, this function is never called, because all thumbnails are
   * extracted and saved to the databases during the post back handler in
   * TranscoderAbstractionFactoryZencoder::processPostback().
   */
  public function extractFrames($destinationScheme, $format) {
    // Check if the job has been completed.
    // If the job has not been completed, don't bother checking for
    // thumbnails
    $fid = $this->settings['input']['fid'];
    $job = video_jobs::load($fid);
    if (empty($job)) {
      return array();
    }
    // No thumbnails available yet
    if ($job->video_status != VIDEO_RENDERING_COMPLETE) {
      return array();
    }

    $path = variable_get('video_thumbnail_path', 'videos/thumbnails') . '/' . $fid;

    // Get the file system directory.
    $dsturibase = $destinationScheme . '://' . $path . '/';
    file_prepare_directory($dsturibase, FILE_CREATE_DIRECTORY);
    $dstwrapper = file_stream_wrapper_get_instance_by_scheme($destinationScheme);

    // Find the old base url setting. If it is not present, don't check for legacy thumbnails
    $base_url = variable_get('video_zencoder_base_url');
    if (empty($base_url)) {
      return array();
    }

    // Where to copy the thumbnails from.
    $final_path = variable_get('video_zencoder_use_full_path', FALSE) ? drupal_realpath(file_uri_scheme($this->settings['input']['uri']) . '://' . $path) : '/' . $path;
    $srcuribase = variable_get('video_zencoder_base_url') . $final_path . '/';

    $thumbs = array();
    // Total thumbs to generate
    $no_of_thumbnails = variable_get('video_thumbnail_count', 5);
    for ($i = 0; $i < $no_of_thumbnails; $i++) {
      $filename = file_munge_filename('thumbnail-' . $fid . '_' . sprintf('%04d', $i) . '.png', '', TRUE);
      $dsturi = $dsturibase . $filename;

      // Download file from S3, if available
      if (!file_exists($dsturi)) {
        $srcuri = $srcuribase . $filename;
        if (!file_exists($srcuri)) {
          watchdog('zencoder',
            'Error downloading thumbnail for video %filename: %thumbpath does not exist.',
            array('%filename' => $this->settings['input']['filename'], '%thumbpath' => $srcuri),
            WATCHDOG_ERROR);
          break;
        }

        $this->moveFile($srcuri, $dsturi);

        // Delete the source, it is no longer needed
        drupal_unlink($srcuri);
      }

      $thumb = new stdClass();
      $thumb->status = 0;
      $thumb->filename = $filename;
      $thumb->uri = $dsturi;
      $thumb->filemime = $dstwrapper->getMimeType($dsturi);
      $thumbs[] = $thumb;
    }

    return !empty($thumbs) ? $thumbs : FALSE;
  }

  public function execute() {
    libraries_load('zencoder');
    $zencoder = new Services_Zencoder();

    try {
      $encoding_job = $zencoder->jobs->create($this->options);

      $output = new stdClass();
      $output->filename = $this->settings['filename'];
      $output->uri = $this->settings['base_url'] . '/' . $this->settings['filename'];
      $output->filesize = 0;
      $output->timestamp = time();
      $output->jobid = intval($encoding_job->id);
      $output->duration = 0;

      return $output;
    }
    catch (Services_Zencoder_Exception $e) {
      $errors = $e->getErrors();
      $this->errors['execute'] = $errors;
      watchdog('zencoder', 'Zencoder reports errors while converting %file:<br/>!errorlist', array('%file' => $this->settings['filename'], '!errorlist' => theme('item_list', array('items' => $errors))), WATCHDOG_ERROR);
      return FALSE;
    }
  }

  public function getName() {
    return 'Zencoder';
  }

  public function getValue() {
    return 'TranscoderAbstractionFactoryZencoder';
  }

  public function isAvailable(&$errormsg) {
    registry_rebuild();

    if (!module_exists('zencoderapi')) {
      $errormsg = t('You must install and enable the Zencoder API module to use Zencoder to transcode videos.');
      return FALSE;
    }
    elseif (!class_exists('Services_Zencoder')) {
      $errormsg = t('The Zencoder API module has not been setup properly.');
      return FALSE;
    }

    return TRUE;
  }

  public function getVersion() {
    return '1.2';
  }

  public function adminSettings() {
    $t = get_t();

    $form = array();
    $zencoder_api = variable_get('video_zencoder_api_key', NULL);
    if (empty($zencoder_api)) {
      $form['zencoder_user'] = array(
        '#type' => 'fieldset',
        '#title' => $t('Zencoder setup'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#description' => $t('Add your email address, password and <em>save configurations</em> to create your Zencoder account. It will help you to transcode and manage your videos using Zencode website. Once you save your configurations then this will automatically create an account on the Zencoder.com and password and all ther other relevent details will be emailed to you.', array('!link' => l($t('Zencoder.com'), 'http://zencoder.com'))),
        '#states' => array(
          'visible' => array(
            ':input[name=video_convertor]' => array('value' => 'TranscoderAbstractionFactoryZencoder'),
          ),
        ),
      );
      $form['zencoder_user']['zencoder_username'] = array(
        '#type' => 'textfield',
        '#title' => $t('Your email address'),
        '#default_value' => variable_get('site_mail', 'me@localhost'),
        '#size' => 50,
        '#description' => $t('Make sure the email is accurate, since we will send all the password details to manage transcoding online and API key details to this.')
      );
      $form['zencoder_user']['agree_terms_zencoder'] = array(
        '#type' => 'checkbox',
        '#title' => $t('Agree Zencoder !link.', array('!link' => l($t('Terms and Conditions'), 'http://zencoder.com/terms', array('attributes' => array('target' => '_blank'))))),
        '#default_value' => variable_get('agree_terms_zencoder', TRUE),
      );
    }
    else {
      // Zencoder API is exists
      $form['zencoder_info'] = array(
        '#type' => 'fieldset',
        '#title' => t('Zencoder'),
        '#collapsible' => FALSE,
        '#collapsed' => FALSE,
        '#states' => array(
          'visible' => array(
            ':input[name=video_convertor]' => array('value' => 'TranscoderAbstractionFactoryZencoder'),
          ),
        ),
      );
      $form['zencoder_info']['api_status'] = array(
        '#type' => 'item',
        '#title' => t('Zencoder API status'),
        '#markup' => $this->getCurrentStatus(),
      );
      $form['zencoder_info']['video_zencoder_api_key'] = array(
        '#type' => 'textfield',
        '#title' => t('Zencoder API key'),
        '#default_value' => variable_get('video_zencoder_api_key', NULL),
        '#description' => t('Leave empty and submit the form to start creating a new Zencoder account.')
      );
      $form['zencoder_info']['video_thumbnail_count_zc'] = array(
        '#type' => 'textfield',
        '#title' => t('Number of thumbnails'),
        '#description' => t('Number of thumbnails to display from video.'),
        '#default_value' => variable_get('video_thumbnail_count', 5),
        '#size' => 5,
      );
      $form['zencoder_info']['video_thumbnail_size'] = array(
        '#type' => 'select',
        '#title' => t('Dimension of thumbnails'),
        '#default_value' => variable_get('video_thumbnail_size', '320x240'),
        '#options' => video_utility::getDimensions(),
      );
      $form['zencoder_info']['video_zencoder_postback'] = array(
        '#type' => 'textfield',
        '#title' => t('Postback URL for Zencoder'),
        '#description' =>
          t('Important: Don\'t change this if you don\'t know what you\'re doing. The Postback URL is used by Zencoder to send transcoding status notifications to Drupal.') . '<br/>' .
          t('Default: %value', array('%value' => url('postback/jobs', array('absolute' => TRUE)))),
        '#default_value' => $this->postbackurl,
      );
      $form['zencoder_info']['video_zencoder_postback_donotvalidate'] = array(
        '#type' => 'checkbox',
        '#title' => t('Do not validate the Postback URL'),
        '#description' => t('The Postback URL is validated by retrieving the URL from the local server. In some cases this fails while it works fine for the Zencoder notification sender. Use this checkbox to disable Postback URL validation.'),
        '#default_value' => variable_get('video_zencoder_postback_donotvalidate', FALSE),
      );

      // testing
      $form['zencoder_info']['testing'] = array(
        '#type' => 'fieldset',
        '#title' => t('Testing mode'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['zencoder_info']['testing']['video_zencoder_testing_mode'] = array(
        '#type' => 'checkbox',
        '#title' => t('Test mode'),
        '#default_value' => variable_get('video_zencoder_testing_mode', FALSE),
        '#description' => t('Enable test mode to test upload/playback locally (if you have no public IP to test)')
      );
      $form['zencoder_info']['testing']['video_zencoder_test_file_path'] = array(
        '#type' => 'textfield',
        '#title' => t('Path to test video file'),
        '#description' => t('Add the path to a video file for Zencoder to transcode.
          You must use this file for testing when using a local machine with no public IP
          address from which Zencoder can download video.'),
        '#default_value' => variable_get('video_zencoder_test_file_path', 'http://example.com/video.mp4'),
      );

      // advanced
      $form['zencoder_info']['advanced'] = array(
        '#type' => 'fieldset',
        '#title' => t('Advanced'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );

      $tempdestinations = array(
        '' => t('Zencoder temporary storage') . ' (' . t('default') . ')',
      );
      if (module_exists('amazons3') && variable_get('amazons3_bucket', FALSE)) {
        $tempdestinations['s3'] = t('Amazon S3 bucket %bucket', array('%bucket' => variable_get('amazons3_bucket')));
      }
      if (module_exists('cloud_files') && variable_get('rackspace_cloud_container', FALSE)) {
        $tempdestinations['rcf'] = t('Rackspace Cloud Files container %container', array('%container' => variable_get('rackspace_cloud_container')));
      }

      if (count($tempdestinations) > 1) {
        $form['zencoder_info']['advanced']['video_zencoder_output_destination'] = array(
          '#type' => 'radios',
          '#title' => t('Location for Zencoder output'),
          '#default_value' => $this->outputdestination === NULL ? '' : $this->outputdestination,
          '#options' => $tempdestinations,
          '#description' => t('Normally, Zencoder uploads its transcoded files to its own Amazon S3 bucket from which the Video module will copy the file to the final destination. Use this setting to use a different location. If the selected location is identical to the final destination, this saves resource intensive copy operations during handling of the postback. The final destination is set per video field and defaults to the public files folder.'),
        );

        if (isset($tempdestinations['s3'])) {
          $form['zencoder_info']['advanced']['video_zencoder_output_destination']['#description'] .= '<br/>' .
            t('To enable Zencoder to upload directly to your Amazon S3 bucket, read the <a href="@zencoder-s3-url">Zencoder manual</a>.', array('@zencoder-s3-url' => url('https://app.zencoder.com/docs/guides/getting-started/working-with-s3')));
        }
      }

      if (module_exists('amazons3')) {
        $form['zencoder_info']['advanced']['video_zencoder_private'] = array(
          '#type' => 'checkbox',
          '#title' => t('Store files on Amazon S3 privately'),
          '#default_value' => variable_get('video_zencoder_private', FALSE),
          '#description' => t('Files stored privately are only accessible by visitors when <a href="@amazons3-settings">Presigned URLs</a> are enabled. These URLs expire, allow you to control access to the video files. For this setting to work, you must set %destination-setting-name to Amazon S3.', array('@amazons3-settings' => url('admin/config/media/amazons3'), '%destination-setting-name' => t('Location for Zencoder output'))),
        );
      }
    }
    return $form;
  }

  public function adminSettingsValidate($form, &$form_state) {
    $v = $form_state['values'];

    if (variable_get('video_zencoder_api_key', FALSE)) {
      // Workaround for the use of the same variable in FFmpeg
      $form_state['values']['video_thumbnail_count'] = $form_state['values']['video_thumbnail_count_zc'];
      unset($form_state['values']['video_thumbnail_count_zc']);

      // Check the postback URL if validation hasn't been disabled
      if (empty($v['video_zencoder_postback_donotvalidate'])) {
        $testurl = $v['video_zencoder_postback'];
        $testcode = md5(mt_rand(0, REQUEST_TIME));
        if (strpos($testurl, '?') === FALSE) {
          $testurl .= '?test=1';
        }
        else {
          $testurl .= '&test=1';
        }
        variable_set('video_postback_test', $testcode);
        $result = drupal_http_request($testurl);
        variable_del('video_postback_test');

        $error = NULL;
        if ($result->code != 200) {
          $error = t('The postback URL cannot be retrieved: @error (@code).', array('@code' => $result->code, '@error' => empty($result->error) ? t('unknown error') : $result->error));
        }
        elseif (empty($result->data) || trim($result->data) != $testcode) {
          $error = t('The postback URL is not valid: returned data contains unexpected value &quot;@value&quot;.', array('@value' => $result->data));
        }

        if ($error != NULL) {
          form_error($form['zencoder_info']['video_zencoder_postback'], $error);
        }
      }
    }
    else {
      // check terms and condition
      if ($form_state['values']['agree_terms_zencoder'] == 0) {
        form_set_error('agree_terms_zencoder', t('You must agree to the !link.', array('!link' => l(t('terms and conditions'), 'http://zencoder.com/terms'))));
      }
      // check for email exists
      // Validate the e-mail address:
      if ($error = user_validate_mail($form_state['values']['zencoder_username'])) {
        form_set_error('zencoder_username', $error);
      }

      // get the API key from zencoder and save it to variable
      if (!form_get_errors()) {
        $mail = $form_state['values']['zencoder_username'];
        $result = $this->createUser($mail);
        if ($result !== TRUE) {
          form_set_error('zencoder_username', $result);
        }
        else {
          // Unset the form values because they do not need to be saved.
          unset($form_state['values']['zencoder_username']);
          unset($form_state['values']['agree_terms_zencoder']);
        }
      }
    }
  }

  /**
   * Create Zencoder user account
   */
  protected function createUser($mail) {
    libraries_load('zencoder');
    $zencoder = new Services_Zencoder();

    try {
      // $result is Services_Zencoder_Account
      $result = $zencoder->accounts->create(array(
        'terms_of_service' => '1',
        'email' => $mail,
        'affiliate_code' => 'drupal-video',
      ));

      variable_set('video_zencoder_api_key', $result->api_key);
      drupal_set_message(t('Your Zencoder details are as below.<br/><b>API Key</b> : @api_key<br/> <b>Password</b> : @password<br/> You can now login to the <a href="@zencoder-url">Zencoder website</a> and track your transcoding jobs online. Make sure you <b>save user/pass combination somewhere</b> before you proceed.', array('@api_key' => $result->api_key, '@password' => $result->password, '@zencoder-url' => url('http://zencoder.com'))), 'status');

      return TRUE;
    }
    catch (Services_Zencoder_Exception $e) {
      if ($e->getErrors() == NULL) {
        return $e->getMessage();
      }

      $errors = '';
      foreach ($e->getErrors() as $error) {
        if ($error == 'Email has already been taken') {
          drupal_set_message(t('Your account already exists on Zencoder. So <a href="@login-url">login</a> to here and enter a full access API key key below.', array('@login-url' => 'https://app.zencoder.com/api')));
          variable_set('video_zencoder_api_key', t('Please enter your API key'));
          return TRUE;
        }
        $errors .= $error;
      }

      return $errors;
    }
  }

  public function processPostback() {
    if (strcasecmp($_SERVER['REQUEST_METHOD'], 'POST') !== 0) {
      echo 'This is the Zencoder notification handler. It seems to work fine.';
      return;
    }

    ignore_user_abort(TRUE);
    libraries_load('zencoder');
    $zencoder = new Services_Zencoder();

    try {
      $notification = $zencoder->notifications->parseIncoming();
    } catch (Services_Zencoder_Exception $e) {
      watchdog('transcoder', 'Postback received from Zencoder could not be decoded: @errormsg', array('@errormsg' => $e->getMessage()));
      echo 'Bad request';
      return;
    }

    if (!isset($notification->job->id)) {
      watchdog('transcoder', 'Postback received from Zencoder is missing the job-id parameter');
      echo 'Invalid data';
      return;
    }

    // Check output/job state
    $jobid = intval($notification->job->id);
    $video_output = db_query('SELECT vid, original_fid, output_fid FROM {video_output} WHERE job_id = :job_id', array(':job_id' => $jobid))->fetch();
    if (empty($video_output)) {
      echo 'Not found';
      return;
    }

    $fid = intval($video_output->original_fid);
    watchdog('transcoder', 'Postback received from Zencoder for fid: @fid, Zencoder job id: @jobid.', array('@fid' => $fid, '@jobid' => $jobid));

    // Find the transcoding job.
    $video = video_jobs::load($fid);
    if (empty($video)) {
      echo 'Transcoding job not found in database';
      return;
    }

    // Zencoder API 2.1.0 and above use $notification->job->outputs.
    // For now, only one output is supported.
    $output = isset($notification->output) ? $notification->output :  current($notification->job->outputs);

    // Find all error situations
    if ($output->state === 'cancelled') {
      video_jobs::setFailed($video);
      echo 'Cancelled';
      return;
    }

    if ($output->state === 'failed') {
      $errorlink = t('no specific information given');
      if (!empty($output->error_message)) {
        if (!empty($output->error_link)) {
          $errordetail = l(t($output->error_message), $output->error_link);
        }
        else {
          $errordetail = t($output->error_message);
        }
      }

      video_jobs::setFailed($video);
      watchdog('transcoder', 'Zencoder reports errors in postback for fid @fid, job id @jobid: !errordetail', array('@fid' => $fid, '@jobid' => $jobid, '!errordetail' => $errordetail), WATCHDOG_ERROR);
      echo 'Failure';
    }

    if ($notification->job->state !== 'finished') {
      return;
    }

    // Move the converted video to its final destination
    $outputfile = file_load($video_output->output_fid);
    if (empty($outputfile)) {
      echo 'Output file not found in database';
      return;
    }

    // Sometimes the long duration of the copy() call causes Zencoder
    // to timeout and retry the notification postback later.
    // So we only copy the file when it doesn't exist or has a different file size.
    if (!file_exists($outputfile->uri) || filesize($outputfile->uri) != $output->file_size_in_bytes) {
      if (!$this->moveFile($output->url, $outputfile->uri)) {
        video_jobs::setFailed($video);
        watchdog('transcoder', 'While processing Zencoder postback, failed to copy @source-uri to @target-uri.', array('@source-uri' => $output->url, '@target-uri' => $outputfile->uri), WATCHDOG_ERROR);
        return;
      }
    }

    $outputfile->filesize = $output->file_size_in_bytes;
    file_save($outputfile);

    // Actual processing of the response
    $video->duration = round($output->duration_in_ms / 1000);
    video_jobs::setCompleted($video);

    // Clear the field cache. Normally, node_save() does this, but that function is not invoked in all cases
    video_utility::clearEntityCache($video->entity_type, $video->entity_id);

    // If there are no thumbnails, quit now.
    if (empty($output->thumbnails)) {
      return;
    }

    // Retrieve the thumbnails from the notification structure
    // Pre-2.1.0, each thumbnail list was an array, now it is an object
    $thumbnails = is_array($output->thumbnails[0]) ? $output->thumbnails[0]['images'] : $output->thumbnails[0]->images;
    if (empty($thumbnails)) {
      return;
    }

    // Find the entity to which the file belongs
    $entity = video_utility::loadEntity($video->entity_type, $video->entity_id);
    if (empty($entity)) {
      watchdog('transcoder', 'The entity to which the transcoded video belongs can\'t be found anymore. Entity type: @entity-type, entity id: @entity-id.', array('@entity-type' => $video->entity_type, '@entity-id' => $video->entity_id), WATCHDOG_ERROR);
      return;
    }

    // The following information was saved in video_jobs::create()
    $fieldname = $video->data['field_name'];
    $field = field_info_field($fieldname);
    $langcode = $video->data['langcode'];
    $delta = $video->data['delta'];

    // Insanity checks
    if (empty($entity->{$fieldname}[$langcode][$delta])) {
      // The field can't be found anymore. This may be a problem.
      watchdog('transcoder', 'The field to which video @filename was uploaded doesn\'t seem to exist anymore. Entity type: @entity-type, entity id: @entity-id, field name: @fieldname, field language: @langcode, delta: @delta.', array('@filename' => $video->filename, '@entity-type' => $video->entity_type, '@entity-id' => $video->entity_id, '@fieldname' => $fieldname, '@langcode' => $langcode, '@delta' => $delta), WATCHDOG_WARNING);
      return;
    }
    if ($entity->{$fieldname}[$langcode][$delta]['fid'] != $video->fid) {
      // The field does not contain the file we uploaded.
      watchdog('transcoder', 'The field to which video @filename was uploaded doesn\'t seem to contain this video anymore. Entity type: @entity-type, entity id: @entity-id, field name: @fieldname, field language: @langcode, delta: @delta.', array('@filename' => $video->filename, '@entity-type' => $video->entity_type, '@entity-id' => $video->entity_id, '@fieldname' => $fieldname, '@langcode' => $langcode, '@delta' => $delta), WATCHDOG_WARNING);
      return;
    }

    // Destination of thumbnails
    $thumbscheme = !empty($field['settings']['uri_scheme_thumbnails']) ? $field['settings']['uri_scheme_thumbnails'] : 'public';
    $thumburibase =  $thumbscheme . '://' . variable_get('video_thumbnail_path', 'videos/thumbnails') . '/' . $video->fid . '/';
    file_prepare_directory($thumburibase, FILE_CREATE_DIRECTORY);
    $thumbwrapper = file_stream_wrapper_get_instance_by_scheme($thumbscheme);

    // Turn the thumbnails into managed files.
    // Because two jobs for the same video may finish simultaneously, lock here so
    // there are no errors when inserting the files.
    if (!lock_acquire('video_zencoder_thumbnails:' . $video->fid, count($thumbnails) * 30)) {
      if (lock_wait('video_zencoder_thumbnails:' . $video->fid, count($thumbnails) * 30)) {
        watchdog('transcoder', 'Failed to acquire lock to download thumbnails for @video-filename.', array('@video-filename' => $video->filename), WATCHDOG_ERROR);
        return;
      }
    }

    $existingthumbs = db_query('SELECT f.uri, f.fid, f.filesize FROM {file_managed} f INNER JOIN {video_thumbnails} t ON (f.fid = t.thumbnailfid) WHERE t.videofid = :fid', array(':fid' => $video->fid))->fetchAllAssoc('uri');
    $thumbs = array();
    $tnid = 0;
    foreach ($thumbnails as $thumbnail) {
      // Pre-2.1.0, each thumbnail was an array
      $thumbnail = (object)$thumbnail;
      $urlpath = parse_url($thumbnail->url, PHP_URL_PATH);
      $ext = video_utility::getExtension($urlpath);
      $thumb = new stdClass();
      $thumb->uid = $outputfile->uid; // $entity may not have a uid property, so take it from the output file.
      $thumb->status = FILE_STATUS_PERMANENT;
      $thumb->filename = 'thumbnail-' . $video->fid . '_' . sprintf('%04d', $tnid++) . '.' . $ext;
      $thumb->uri = $thumburibase . $thumb->filename;
      $thumb->filemime = $thumbwrapper->getMimeType($thumb->uri);
      $thumb->type = 'image'; // For the media module
      $thumb->filesize = $thumbnail->file_size_bytes;
      $thumb->timestamp = REQUEST_TIME;

      $shouldcopy = TRUE;
      if (isset($existingthumbs[$thumb->uri])) {
        // If the thumbnail has the same size in the database compared to the notification data, don't copy
        if (file_exists($thumb->uri) && $existingthumbs[$thumb->uri]->filesize == $thumb->filesize) {
          $shouldcopy = FALSE;
        }
        $thumb->fid = intval($existingthumbs[$thumb->uri]->fid);
      }

      if ($shouldcopy && !$this->moveFile($thumbnail->url, $thumb->uri)) {
        watchdog('transcoder', 'Could not copy @thumbsrc to @thumbdest.', array('@thumbsrc' => $thumbnail->url, '@thumbdest' => $thumb->uri), WATCHDOG_ERROR);
        continue;
      }

      file_save($thumb);

      // Saving to video_thumbnails and file_usage is only necessary when this is a new thumbnail
      if (!isset($existingthumbs[$thumb->uri])) {
        db_insert('video_thumbnails')->fields(array('videofid' => $video->fid, 'thumbnailfid' => $thumb->fid))->execute();
        file_usage_add($thumb, 'file', $video->entity_type, $video->entity_id);
      }

      $thumbs[$thumb->fid] = $thumb;
    }
    lock_release('video_zencoder_thumbnails:' . $video->fid);

    // Clear the field cache. Normally, node_save() does this, but that function is not invoked in all cases
    video_utility::clearEntityCache($video->entity_type, $video->entity_id);

    // Skip setting the thumbnail if there are no thumbnails or when the current value is already valid
    $currentthumb = isset($entity->{$fieldname}[$langcode][$delta]['thumbnail']) ? intval($entity->{$fieldname}[$langcode][$delta]['thumbnail']) : 0;
    if (empty($thumbs) || isset($thumbs[$currentthumb])) {
      return;
    }

    // Set a random thumbnail fid on the entity and save the entity
    $entity->{$fieldname}[$langcode][$delta]['thumbnail'] = array_rand($thumbs);

    switch ($video->entity_type) {
      case 'node':
        node_save($entity);
        break;
      case 'comment':
        comment_save($entity);
        break;
      default:
        // entity_save() is supplied by the entity module
        if (function_exists('entity_save')) {
          entity_save($video->entity_type, $entity);
        }
        break;
    }
  }

  private function moveFile($srcurl, $dsturi) {
    $unlinksrc = FALSE;

    // If the Amazon S3 or Cloud Files module is used, we know that the file is on s3:// or rcf://.
    // We must move the file, because the original must be deleted.
    if ($this->outputdestination == 's3' || $this->outputdestination == 'rcf') {
      $srcuri = $this->outputdestination . ':/' . parse_url($srcurl, PHP_URL_PATH);
      // Check if the file is already at the right place
      if ($srcuri === $dsturi) {
        return TRUE;
      }
      // Move the file if the target is also s3:// or rcf://
      if (file_uri_scheme($dsturi) == $this->outputdestination) {
        return rename($srcuri, $dsturi);
      }

      // The src file needs to be removed after copying.
      $unlinksrc = TRUE;
    }

    // Check if $srcurl is actually a $uri
    $srcuri = $srcurl;
    if (strncmp('http', $srcuri, 4) === 0) {
      $srcuri = video_utility::urlToUri($srcurl);
      if ($srcuri === NULL) {
        $srcuri = $srcurl;
      }
    }

    // Check if the file is already at the right place
    if ($srcuri === $dsturi) {
      return TRUE;
    }

    $result = copy($srcuri, $dsturi);
    if ($result && $unlinksrc) {
      unlink($srcuri);
    }

    return $result;
  }

  /**
   * Get enabled and supporting codecs by Zencoder.
   */
  public function getCodecs() {
    $auto = t('Default for this extension');

    return array(
      'encode' => array(
        'video' => array(
          '' => $auto,
          'h264' => 'H.264',
          'vp8' => 'VP8',
          'theora' => 'Theora',
          'vp6' => 'VP6',
          'mpeg4' => 'MPEG-4',
          'wmv' => 'WMV',
        ),
        'audio' => array(
          '' => $auto,
          'aac' => 'AAC',
          'mp3' => 'MP3',
          'vorbis' => 'Vorbis',
          'wma' => 'WMA',
        )
      ),
      'decode' => array(),
    );
  }

  public function getAvailableFormats($type = FALSE) {
    return array(
      '3g2' => '3G2',
      '3gp' => '3GP',
      '3gp2' => '3GP2',
      '3gpp' => '3GPP',
      '3gpp2' => '3GPP2',
      'aac' => 'AAC',
      'f4a' => 'F4A',
      'f4b' => 'F4B',
      'f4v' => 'F4V',
      'flv' => 'FLV',
      'm4a' => 'M4A',
      'm4b' => 'M4B',
      'm4r' => 'M4R',
      'm4v' => 'M4V',
      'mov' => 'MOV',
      'mp3' => 'MP3',
      'mp4' => 'MP4',
      'oga' => 'OGA',
      'ogg' => 'OGG',
      'ogv' => 'OGV',
      'ogx' => 'OGX',
      'ts' => 'TS',
      'webm' => 'WebM',
      'wma' => 'WMA',
      'wmv' => 'WMV',
    );
  }

  public function getPixelFormats() {
    // Zencoder doesn't support this
    return array();
  }

  /**
   * Reset internal variables to their initial state.
   */
  public function reset($keepinput = FALSE) {
    parent::reset($keepinput);

    if (!$keepinput) {
      unset($this->options['input']);
    }
    unset($this->options['output']);
  }

  /**
   * Check the current Zencoder API status.
   */
  public function getCurrentStatus() {
    $json = drupal_http_request('http://status.zencoder.com/api/events.json');
    $message = t('All systems go');

    if ($json->code != 200) {
      $message = t('The Zencoder Status API URL cannot be retrieved: @error (@code).', array('@code' => $json->code, '@error' => empty($json->error) ? t('unknown error') : $json->error));
    }
    else {

      $status_messages = drupal_json_decode($json->data);

      if (!empty($status_messages)) {
        $last_message = reset($status_messages);
        $last_event = $last_message['event'];
        $message = nl2br(t('@status', array('@status' =>  $last_event['description'])));
      }
    }

    return $message;
  }

  /**
   * Whether the transcoder works by sending jobs to an external system.
   *
   * True for transcoders like Zencoder, false for transcoders like FFmpeg.
   */
  public function isOffSite() {
    return TRUE;
  }
}
