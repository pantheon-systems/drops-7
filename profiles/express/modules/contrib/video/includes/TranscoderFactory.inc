<?php
/**
 * @file
 * Transcoder Abstract Factory classes
 */

/**
 * Interface for transcoder classes
 */
interface TranscoderFactoryInterface {
  /**
   * Extract frames from the current video.
   *
   * The thumbnails are be saved to the given scheme. The files are not
   * be saved to the database, this will be done by the caller.
   * The uid, filesize and timestamp properties are not set by this method.
   *
   * @param $destinationScheme
   *   A valid stream wrapper scheme
   * @param $format
   *   png or jpg
   *
   * @return
   *   array of file objects, FALSE on error
   */
  public function extractFrames($destinationScheme, $format);

  public function execute();

  public function getName();

  public function getValue();

  public function isAvailable(&$errormsg);

  public function getDimensions();

  public function setInput(array $file);

  /**
   * Reset internal variables to their initial state.
   */
  public function reset($keepinput = FALSE);

  /**
   * Process postback jobs
   */
  public function processPostback();

  /**
   * Get enabled and supporting codecs by the transcoder.
   */
  public function getCodecs();

  /**
   * Get available output file formats from the transcoder.
   */
  public function getAvailableFormats($type = FALSE);

  /**
   * Returns a list of all supported pixel formats.
   *
   * Returns NULL when pixel formats are not configurable for this transcoder.
   */
  public function getPixelFormats();

  /**
   * Admin settings form for the transcoder
   */
  public function adminSettings();

  /**
   * Validate admin settings. This will call when Drupal admin settings validate.
   */
  public function adminSettingsValidate($form, &$form_state);

  /**
   * Whether the transcoder works by sending jobs to an external system.
   *
   * True for transcoders like Zencoder, false for transcoders like FFmpeg.
   */
  public function isOffSite();
}

/**
 * Abstract class for the transcoder classes to keep common methods
 */
abstract class TranscoderAbstractionFactory {
  protected $settings = array();
  protected $errors = array();

  protected function __construct() {
  }

  public function isAvailable(&$errormsg) {
    return TRUE;
  }

  /**
   * Set Input file to add input file in to settings variable
   */
  public function setInput(array $file) {
    // @TODO : do some validation to check the file exists;
    if (!empty($file)) {
      if (empty($file['uri']))
        $file += (array) file_load($file['fid']);
      $this->settings['input'] = $file;
    }
    else
      drupal_set_message(t('Video file not found.'), 'error');
  }

  /**
   * Set options is to set transcoding settings before send to the transcoder.
   */
  public function setOptions(array $options) {
    foreach ($options as $key => $value) {
      $this->settings[$key] = $value;
    }
  }

  /**
   * Set output file for transcoding, this would be the result file.
   */
  public function setOutput($output_directory, $output_name, $overwrite_mode = FILE_EXISTS_REPLACE) {
    // @TODO : do some validation to check the file exists
    if (count($output_directory) == 1)
      $this->settings['base_url'] = $output_directory;
    if (count($output_name) == 1)
      $this->settings['filename'] = $output_name;
    else
      $this->errors['output'] = 'Output file not found.';
  }

  /**
   * Get the installed transcoder version.
   */
  public function getVersion() {
    return '1.0';
  }

  /**
   * Get file informations
   * @return
   *   Associative array with file informations like duration, dimensions
   */
  public function getFileInfo() {
    return NULL;
  }

  public function getDimensions() {
    $i = $this->getFileInfo();

    if (empty($i) || empty($i['video']['dimensions']['width']) || empty($i['video']['dimensions']['height'])) {
      return NULL;
    }

    return array(
      'width' => intval($i['video']['dimensions']['width']),
      'height' => intval($i['video']['dimensions']['height']),
    );
  }

  /**
   * Get errors
   */
  public function getErrors() {
    return $this->errors;
  }

  /**
   * Check for errors if any
   */
  public function checkErrors() {
    return !empty($this->errors);
  }

  /**
   * Process postback jobs
   */
  public function processPostback() {
    return MENU_NOT_FOUND;
  }

  /**
   * Reset internal variables to their initial state.
   */
  public function reset($keepinput = FALSE) {
    if (!$keepinput) {
      unset($this->settings['input']);
    }

    unset($this->settings['options']);
    unset($this->settings['output']);
    $this->errors = array();
  }
}
