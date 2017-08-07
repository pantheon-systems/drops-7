<?php
/**
 * @file
 * Define Linkit file search plugin class.
 */

/**
 * Define that file urls should link directly to the file itself.
 */
define('LINKIT_FILE_URL_TYPE_DIRECT', 'direct');

/**
 * Define that file urls should force download of the file.
 */
define('LINKIT_FILE_URL_TYPE_DOWNLOAD', 'download');

/**
 * Define that file urls should link the file entity view page.
 */
define('LINKIT_FILE_URL_TYPE_ENTITY', 'entity');

/**
 * Reprecents a Linkit file search plugin.
 */
class LinkitSearchPluginFile extends LinkitSearchPluginEntity {

  /**
   * Overrides LinkitSearchPlugin::ui_title().
   */
  function ui_title() {
    return t('Managed files');
  }

  /**
   * Overrides LinkitSearchPlugin::ui_description().
   */
  function ui_description() {
    return t('Extend Linkit with file support (Managed files).');
  }

  /**
   * Overrides LinkitSearchPluginEntity::createDescription().
   *
   * If the file is an image, a small thumbnail can be added to the description.
   * Also, image dimensions can be shown.
   */
  function createDescription($data) {
    $description_array = array();
    // Get image info.
    $imageinfo = image_get_info($data->uri);

    // Add small thumbnail to the description.
    if ($this->conf['image_extra_info']['thumbnail']) {
      $image = $imageinfo ? theme_image_style(array(
          'width' => $imageinfo['width'],
          'height' => $imageinfo['height'],
          'style_name' => 'linkit_thumb',
          'path' => $data->uri,
        )) : '';
    }

    // Add image dimensions to the description.
    if ($this->conf['image_extra_info']['dimensions'] && !empty($imageinfo)) {
      $description_array[] = $imageinfo['width'] . 'x' . $imageinfo['height'] . 'px';
    }

    $description_array[] = parent::createDescription($data);

    // Add tiel files scheme to the description.
    if ($this->conf['show_scheme']) {
      $description_array[] = file_uri_scheme($data->uri) . '://';
    }

    $description = (isset($image) ? $image : '') . implode('<br />' , $description_array);

    return $description;
  }

  /**
   * Overrides LinkitSearchPluginEntity::createGroup().
   */
  function createGroup($entity) {
    // The the standard group name.
    $group = parent::createGroup($entity);

    // Add the scheme.
    if ($this->conf['group_by_scheme']) {
      // Get all stream wrappers.
      $stream_wrapper = file_get_stream_wrappers();
      $group .= ' - ' . $stream_wrapper[file_uri_scheme($entity->uri)]['name'];
    }
    return $group;
  }

  /**
   * Overrides LinkitSearchPluginEntity::createPath().
   *
   * If 'Direct download' is enabled, make the link point to the file entity
   * download endpoint.
   */
  function createPath($entity) {
    $url_type = isset($this->conf['url_type']) ? $this->conf['url_type'] : $this->getDefaultUrlType();

    // We can only support the download type if we have version 2.x of the file_entity module.
    if ($url_type == LINKIT_FILE_URL_TYPE_DOWNLOAD && !(module_exists('file_entity') && function_exists('file_entity_download_uri'))) {
      $url_type =  $this->getDefaultUrlType();
    }

    switch ($url_type) {
      case LINKIT_FILE_URL_TYPE_DIRECT:
        // Check if this is a local file.
        $wrapper = file_stream_wrapper_get_instance_by_uri($entity->uri);
        if ($wrapper instanceof DrupalLocalStreamWrapper) {
          // Create a relative URL to the local file.
          // See https://www.drupal.org/node/837794.
          $path = $wrapper->getDirectoryPath() . '/' . file_uri_target($entity->uri);
        }
        else {
          $path = file_create_url($entity->uri);
        }
        // Process the uri with the insert plugin.
        return linkit_get_insert_plugin_processed_path($this->profile, $path, array('language' => (object) array('language' => FALSE)));

      case LINKIT_FILE_URL_TYPE_DOWNLOAD:
        $uri = file_entity_download_uri($entity);
        // Hack for LINKIT_URL_METHOD_RAW, which won't include the options that
        // we pass to linkit_get_insert_plugin_processed_path().
        if (isset($uri['options']['query']['token']) && $this->profile->data['insert_plugin']['url_method'] == LINKIT_URL_METHOD_RAW) {
          return $uri['path'] . '?token=' . rawurlencode($uri['options']['query']['token']);
        }
        // Process the uri with the insert plugin.
        return linkit_get_insert_plugin_processed_path($this->profile, $uri['path'], $uri['options']);

      case LINKIT_FILE_URL_TYPE_ENTITY:
        // Pass back to the parent if the user wants entity urls.
        return parent::createPath($entity);
    }
  }

  /**
   * Overrides LinkitSearchPluginEntity::getQueryInstance().
   */
  function getQueryInstance() {
    // Call the parent getQueryInstance method.
    parent::getQueryInstance();
    // Only search for permanent files.
    $this->query->propertyCondition('status', FILE_STATUS_PERMANENT);
  }

  /**
   * Overrides LinkitSearchPluginEntity::buildSettingsForm().
   */
  function buildSettingsForm() {
    $form = parent::buildSettingsForm();

    $form['entity:file']['show_scheme'] = array(
      '#title' => t('Show file scheme'),
      '#type' => 'checkbox',
      '#default_value' => isset($this->conf['show_scheme']) ? $this->conf['show_scheme'] : '',
    );

    $form['entity:file']['group_by_scheme'] = array(
      '#title' => t('Group files by scheme'),
      '#type' => 'checkbox',
      '#default_value' => isset($this->conf['group_by_scheme']) ? $this->conf['group_by_scheme'] : '',
    );

    $form['entity:file']['url_type'] = array(
      '#title' => t('URL type'),
      '#type' => 'radios',
      '#options' => array(
        LINKIT_FILE_URL_TYPE_DIRECT => t('Direct file link'),
        LINKIT_FILE_URL_TYPE_DOWNLOAD => t('Download file link'),
        LINKIT_FILE_URL_TYPE_ENTITY => t('Entity view page'),
      ),
      '#default_value' => isset($this->conf['url_type']) ? $this->conf['url_type'] : $this->getDefaultUrlType(),
    );
    // We can only support the download type if we have version 2.x of the file_entity module.
    if (!(module_exists('file_entity') && function_exists('file_entity_download_uri'))) {
      unset($form['entity:file']['url_type']['#options'][LINKIT_FILE_URL_TYPE_DOWNLOAD]);
    }

    $image_extra_info_options = array(
      'thumbnail' => t('Show thumbnails <em>(using the image style !linkit_thumb_link)</em>', array('!linkit_thumb_link' => l(t('linkit_thumb'), 'admin/config/media/image-styles/edit/linkit_thumb'))),
      'dimensions' => t('Show pixel dimensions'),
    );

    $form['entity:file']['image_extra_info'] = array(
      '#title' => t('Images'),
      '#type' => 'checkboxes',
      '#options' => $image_extra_info_options,
      '#default_value' => isset($this->conf['image_extra_info']) ? $this->conf['image_extra_info'] : array('thumbnail', 'dimensions'),
    );

    return $form;
  }

  /**
   * Gets the default URL type to use if no URL type has been explicitly set.
   *
   * @return string The URL type
   */
  protected function getDefaultUrlType() {
    $info = entity_get_info('file');
    $callback = $info['uri callback'];
    if ($callback == 'entity_metadata_uri_file') {
      // The Drupal core file URI callback would be used if we were to use the
      // "Entity view page" URL mode, which generates absolute URLs to files.
      // The "Direct link" URL mode is preferable in this case as it generates
      // relative links.
      return LINKIT_FILE_URL_TYPE_DIRECT;
    }
    else {
      // We use a custom URI callback such as in Media 1.x or File Entity 2.x.
      // The "Entity view page" URL mode is preferable in this case.
      return LINKIT_FILE_URL_TYPE_ENTITY;
    }
  }
}
