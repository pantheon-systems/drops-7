<?php

/**
 * @file
 * Imagecrop class to handle the javascript imagecrop.
 *
 * @date
 * Dec 19, 2010
 */

class ImageCrop {

  private $allowedExtensions = array('image/jpeg', 'image/gif', 'image/png', 'image/pjpeg');
  private $inCroppingMode = FALSE;
  public $skipPreview = FALSE;
  public $extraControls = FALSE;

  private $file;
  private $imageStyle;
  private $entityType = 'none';
  private $bundle = 'none';
  private $fieldName = 'none';
  private $styleDestination;
  private $cropDestination;
  private $imageWidth;
  private $originalImageWidth;
  private $originalImageHeight;

  private $isResizable = FALSE;
  private $downscalingAllowed = TRUE;
  private $resizeAspectRatio = FALSE;
  private $width = 0;
  private $startWidth = 0;
  private $height = 0;
  private $startHeight = 0;
  private $xoffset = 0;
  private $yoffset = 0;
  private $scale = 'original';
  private $disableIfNoData = FALSE;
  private $hasSettings = FALSE;

  /**
   * Construct imagecrop.
   */
  public function __construct() {
    $this->skipPreview = variable_get('imagecrop_skip_preview', FALSE);
    $this->extraControls = variable_get('imagecrop_ui_controls', FALSE);
  }

  /**
   * Load the imagecrop settings for the given fid or filesource.
   * @param $lookup fid of filesource
   * @param $use_fid the first param is a fid
   */
  public function loadFile($filesource, $use_fid = TRUE) {

    if ($use_fid) {
      $this->file = file_load($filesource);
    }
    else {
      $query = db_select('file_managed', 'f');
      $query->fields('f', array('fid', 'uid', 'uri', 'filemime'));
      $query->condition('uri', $filesource);
      $this->file = $query->execute()->fetchObject();
    }

    if (!$this->file) {
      throw new Exception('The image to crop was not found.');
    }

    if (!in_array($this->file->filemime, $this->allowedExtensions)) {
      throw new Exception('The file to crop was not an image.');
    }

    $this->file->filepath = file_create_url($this->file->uri);

  }

  /**
   * Get the current file.
   */
  public function getFile() {
    return $this->file;
  }

  /**
   * Set the current file
   */
  public function setFile($file) {
    $this->file = $file;
  }

  /**
   * Set the entity type from the current imagecrop.
   */
  public function setEntityType($entity_type) {
    $this->entityType = $entity_type;
  }

 /**
   * Get the entity type from the current imagecrop.
   */
  public function getEntityType() {
    return $this->entityType;
  }

  /**
   * Set the bundle from the current imagecrop.
   */
  public function setBundle($bundle) {
    $this->bundle = $bundle;
  }

  /**
   * Get the bundle from the current imagecrop.
   */
  public function getBundle() {
    return $this->bundle;
  }

  /**
   * Set the field name from the current imagecrop.
   */
  public function setFieldName($field_name) {
    $this->fieldName = $field_name;
  }

  /**
   * Get the field name from the current imagecrop.
   */
  public function getFieldName() {
    return $this->fieldName;
  }

  /**
   * Is the crop resizable or not.
   */
  public function isResizable() {
    return $this->isResizable;
  }

  /**
   * Get the X offset from the current imagecrop object.
   */
  public function getXOffset() {

    return $this->xoffset;

  }

  /**
   * Get the X offset from the current imagecrop object.
   */
  public function getYOffset() {

    return $this->yoffset;

  }

  /**
   * Get the width from the current crop area.
   */
  public function getWidth() {
    return $this->width;
  }

  /**
   * Get the height from the current crop area.
   */
  public function getHeight() {
    return $this->height;
  }

  /**
   * Get the width from the image to crop.
   */
  public function getImageWidth() {
    return $this->imageWidth;
  }

  /**
   * Get the original width from the image to crop.
   */
  public function getOriginalImageWidth() {
    return $this->originalImageWidth;
  }

  /**
   * Set the scaling width from the image to crop.
   */
  public function setScale($scale) {
    $this->scale = $scale;
  }

  /**
   * Get the scaling from the image to crop.
   */
  public function getScale() {
    return $this->scale;
  }

  /**
   * Get the height from the image to crop.
   */
  public function getImageHeight() {
    return $this->imageHeight;
  }

  /**
   * Get the original height from the image to crop.
   */
  public function getOriginalImageHeight() {
    return $this->originalImageHeight;
  }

  /**
   * Set the status of cropping mode (TRUE = busy cropping).
   */
  public function setInCroppingMode($inCroppingMode) {
    $this->inCroppingMode = $inCroppingMode;
  }

  /**
   * Get the current value for cropping mode.
   */
  public function getInCroppingMode() {
    return $this->inCroppingMode;
  }

  /**
   * Set the current cropped image style.
   */
  public function setImageStyle($style_name) {

    $this->imageStyle = image_style_load($style_name);

    if (!$this->imageStyle) {
      throw new Exception('The image style to crop was not found.');
    }

    // add default settings
    foreach ($this->imageStyle['effects'] as $effect) {
      if ($effect['name'] == 'imagecrop_javascript') {
        $this->width = $effect['data']['width'];
        $this->startWidth = $effect['data']['width'];
        $this->height = $effect['data']['height'];
        $this->startHeight = $effect['data']['height'];

        if ($effect['data']['xoffset']) {
          $this->xoffset = $effect['data']['xoffset'];
        }

        if ($effect['data']['yoffset']) {
          $this->yoffset = $effect['data']['yoffset'];
        }

        $this->isResizable = $effect['data']['resizable'];
        $this->disableIfNoData = $effect['data']['disable_if_no_data'];
        $this->resizeAspectRatio = !empty($effect['data']['aspect_ratio']) ? $effect['data']['aspect_ratio'] : FALSE;
        $this->downscalingAllowed = !$effect['data']['downscaling'];
        break;
      }
    }

  }

  /**
   * Get the current cropped image style.
   */
  public function getImageStyle() {
    return $this->imageStyle;
  }

  /**
   * Set the crop destinations.
   */
  public function setCropDestinations() {

    $this->styleDestination = image_style_path($this->imageStyle['name'], $this->file->uri);
    if (file_uri_scheme($this->file->uri) != 'public') {
      $this->cropDestination = $this->styleDestination;
    }
    else {
      $this->cropDestination = image_style_path('_imagecrop_temp', $this->file->uri);
    }

  }

  /**
   * Get the destination from the image for current style.
   */
  public function getStyleDestination() {
    return $this->styleDestination;
  }

  /**
   * Get the destination from the image for cropping the current style.
   * @param $filepath Boolean, TRUE to return the filepath, FALSE to return the uri.
   */
  public function getCropDestination($filepath = TRUE) {
    $image_url = ($filepath) ?  file_create_url($this->cropDestination) : $this->cropDestination;
    $image_url .= '?time=' . $_SERVER['REQUEST_TIME'];
    return $image_url;
  }

  /**
   * Check if a user has permissions to the current file.
   */
  public function hasUserAccess($account) {

    // access to all images
    if (user_access('crop any image')) {
      return TRUE;
    }

    // If not administer nodes, check if the files uid is the same like users uid.
    if ($this->file && !user_access('administer nodes') && $account->uid != $this->file->uid) {
      return FALSE;
    }

    return TRUE;

  }

  /**
   * Load the crop settings that are available.
   */
  public function loadCropSettings() {

    $size = getimagesize(drupal_realpath($this->file->uri));
    $this->imageWidth = $this->originalImageWidth = $size[0];
    $this->imageHeight = $this->originalImageHeight = $size[1];

    $settings = db_select('image_crop_settings')
      ->fields('image_crop_settings')
      ->condition('fid', $this->file->fid)
      ->condition('style_name', $this->imageStyle['name'])
      ->execute()->fetchObject();

    // Load settings
    if ($settings) {

      $this->xoffset = $settings->xoffset;
      $this->yoffset = $settings->yoffset;

      if (!$this->inCroppingMode || $this->isResizable) {
        $this->width = $settings->width;
        $this->height = $settings->height;
      }

      $this->scale = $settings->scale;
      $this->hasSettings = TRUE;

    }
    // Check for default scale.
    else {

      if (variable_get('imagecrop_scale_default', FALSE)) {

        $step = variable_get('imagecrop_scale_step', 50);
        $popup_width = variable_get('imagecrop_popup_width', 700);
        $popup_height = variable_get('imagecrop_popup_height', 600) - 50;

        if ($this->extraControls) {
          $popup_width -= 215;
        }

        $scale_width = $this->originalImageWidth;
        $aspect = $this->originalImageWidth / $this->originalImageHeight;
        if ($step > 0) {
          $scale_width -= $step;
          while ($scale_width > $this->width && ($scale_width / $aspect) > $this->height) {
            $scaled_height = intval($scale_width / $aspect);
            if ($scaled_height < $popup_height && $scale_width < $popup_width) {
              $this->scale = $scale_width;
              break;
            }
            $scale_width -= $step;
          }
        }

      }

    }

    if ($this->scale != 'original') {
      $aspect = $this->originalImageWidth / $this->originalImageHeight;
      $this->imageWidth = $this->scale;
      $this->imageHeight = intval($this->imageWidth / $aspect);
    }

    if ($this->resizeAspectRatio == 'KEEP') {
      $this->resizeAspectRatio = $this->imageWidth / $this->imageHeight;
    }
    elseif ($this->resizeAspectRatio == 'CROP') {
      $this->resizeAspectRatio = $this->width / $this->height;
    }

    if (!is_numeric($this->xoffset)) {

      switch ($this->xoffset) {

        case 'right':
          $this->xoffset = $this->imageWidth - $this->width;
        break;

        case 'center':
          $this->xoffset = round(($this->imageWidth / 2) - ($this->width / 2));
        break;

        case 'left':
        default:
          $this->xoffset = 0;
        break;

      }

    }

    if (!is_numeric($this->yoffset)) {

      switch ($this->yoffset) {

        case 'bottom':
          $this->yoffset = $this->imageHeight - $this->height;
        break;

        case 'center':
          $this->yoffset = round(($this->imageHeight / 2) - ($this->height / 2));
        break;

        case 'top':
        default:
          $this->yoffset = 0;
        break;

      }

    }

  }

  /**
   * Apply all the crop settings to the given image source.
   * @param $image image object from image module
   */
  public function applyCrop(&$image) {

    if (!$this->hasSettings && $this->disableIfNoData) {
      return;
    }

    // Scale first if requested
    if ($this->scale != 'original') {
      $data = array(
        'width' => $this->scale,
        'height' => PHP_INT_MAX,
      );
      image_scale_effect($image, $data);
    }

    $data = array(
      'anchor' => $this->xoffset . '-' . $this->yoffset,
      'width' => $this->width,
      'height' => $this->height,
    );
    image_crop_effect($image, $data);

  }

  /**
   * Write the file to crop, and apply all effects, untill the imagecrop effectso cropping can be done.
   */
  public function writeCropreadyImage() {

    $unset = FALSE;
    $style = $this->imageStyle;

    foreach ($this->imageStyle['effects'] as $key => $effect) {
      if ($effect['name'] == 'imagecrop_javascript') {
        $unset = TRUE;
      }
      unset($style['effects'][$key]);
    }

    if ($this->scale !== 'original') {
      $style['effects'][] = array(
        'effect callback' => 'image_scale_effect',
        'data' => array(
          'width' => $this->scale,
        ),
      );
    }

    image_style_create_derivative($style, $this->file->uri, $this->cropDestination);

  }

  /**
   * Add all the files for the cropping UI.
   */
  public function addImagecropUi($in_cropping_mode) {

    drupal_add_js(drupal_get_path('module', 'imagecrop') . '/js/imagecrop.js');
    drupal_add_css(drupal_get_path('module', 'imagecrop') . '/imagecrop.css');

    if (variable_get('imagecrop_show_cancel_button', FALSE)) {
      $popup_js = drupal_get_path('module', 'imagecrop') . '/js/popups/' . variable_get('imagecrop_popup', 'basic') . '.js';
      if (file_exists($popup_js)) {
        drupal_add_js($popup_js);
      }
    }

    // Add crop ui if in cropping mode.
    if ($in_cropping_mode) {

      drupal_add_js(drupal_get_path('module', 'imagecrop') . '/js/imagecrop.ui.crop.js');

      drupal_add_library('system', 'ui.draggable');

      $settings = array(
        'manipulationUrl' => url('imagecrop/generate_image'),
        'cropped' => isset($_GET['cropping']),
      );

      if ($this->isResizable) {
        drupal_add_library('system', 'ui.resizable');
        drupal_add_library('system', 'effects.scale');
        $settings['resizeAspectRatio'] = $this->resizeAspectRatio;
        $settings['minWidth'] = ($this->downscalingAllowed ? 0 : $this->startWidth);
        $settings['minHeight'] = ($this->downscalingAllowed ? 0 : $this->startHeight);
        $settings['startHeight'] = $this->startHeight;
        $settings['startWidth'] = $this->startWidth;
      }

      drupal_add_js(array('imagecrop' => $settings), 'setting');

    }

  }

}