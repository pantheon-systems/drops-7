<?php
/**
 * @file
 * Class file used to store video presets on the video.
 */

class Preset {
  public static function getEnabledPresets($field_name = NULL) {
    if ($field_name != NULL) {
      $field = field_info_field($field_name);
      $names = $field['settings']['presets'];
      if (empty($names)) {
        return self::getEnabledPresets();
      }
    }
    else {
      $names = variable_get('video_preset', array());
    }
    $names = array_filter($names);

    $presets = array();
    $all = self::getAllPresets();
    $error = array();

    foreach ($names as $name) {
      if (isset($all[$name])) {
        $presets[$name] = $all[$name];
      }
      else {
        $error[] = $name;
      }
    }

    // Display a warning when presets are missing.
    if (!empty($error)) {
      $params = array('%preset-names' => implode(', ', $error));
      $message = t('The following selected presets are no longer available: %preset-names.', $params);

      // Log to the watchdog when none of the presets are available.
      if (empty($presets)) {
        watchdog('video', 'The following selected presets are no longer available: %preset-names.', $params);
      }

      if (!empty($field)) {
        $message .= ' ' . t('Please update the @field-name field settings.', array('@field-name' => $field_name));
      }
      else {
        $message .= ' ' . t('Please update the Video module settings.');
      }
      drupal_set_message($message, 'warning', FALSE);
    }

    // When no presets are available and a field was supplied, try without a field.
    if (empty($presets) && !empty($field)) {
      return self::getEnabledPresets();
    }

    return $presets;
  }

  /**
   * Returns the default presets.
   */
  public static function getDefaultPresets() {
    $presets = array();

    foreach (module_implements('video_default_presets') as $module) {
      $function = $module . '_video_default_presets';
      if (function_exists($function)) {
        $result = $function();
        if (isset($result) && is_array($result)) {
          foreach ($result as $preset) {
            $preset['module'] = $module;
            $presets[$preset['name']] = $preset;
          }
        }
      }
    }

    drupal_alter('video_default_presets', $presets);

    return $presets;
  }

  /**
   * Gets a list of all presets.
   */
  public static function getAllPresets() {
    $presets = array();

    // Get all the presets from the database.
    $result = db_select('video_preset', 'p')->fields('p')->execute();

    // Iterate through all the presets and structure them in an array.
    foreach ($result as $preset) {
      $preset = (array) $preset;
      $preset['module'] = NULL;
      $preset['overridden'] = NULL;
      $preset['settings'] = !empty($preset['settings']) ? unserialize($preset['settings']) : array();
      $presets[$preset['name']] = $preset;
    }

    // Now allow other modules to add their default presets.
    foreach (self::getDefaultPresets() as $preset) {
      if (empty($preset['name'])) {
        continue;
      }

      // Check if this default preset has been overridden.
      if (isset($presets[$preset['name']])) {
        $presets[$preset['name']]['overridden'] = TRUE;
        $presets[$preset['name']]['module'] = $preset['module'];
      }
      else {
        $preset['overridden'] = FALSE;
        $presets[$preset['name']] = $preset;
      }
    }

    uksort($presets, 'strnatcasecmp');

    return $presets;
  }

  /**
   * Retrieves a single preset.
   */
  public static function getPreset($preset_name) {
    $preset = FALSE;

    // Check if it is a default preset.
    $default_presets = self::getDefaultPresets();
    if (isset($default_presets[$preset_name])) {
      $preset = $default_presets[$preset_name];
      $preset['overridden'] = FALSE;
    }

    // Get the preset from the database.
    $dbpreset = db_select('video_preset', 'p')
      ->fields('p')
      ->condition('p.name', $preset_name)
      ->execute()
      ->fetchAssoc();
    if ($dbpreset) {
      $dbpreset['settings'] = !empty($dbpreset['settings']) ? unserialize($dbpreset['settings']) : array();
      if ($preset == NULL) {
        $dbpreset['overridden'] = NULL;
        $dbpreset['module'] = NULL;
      }
      else {
        $dbpreset['overridden'] = TRUE;
        $dbpreset['module'] = $preset['module'];
      }
      $preset = $dbpreset;
    }

    // Return the preset.
    return $preset;
  }

  /**
   * Deletes a preset from the database.
   */
  public static function deletePreset($preset_name) {
    $preset = self::getPreset($preset_name);
    if (empty($preset) || $preset['overridden'] === FALSE) {
      return;
    }

    // Find out whether there is a default watermark.
    $watermark_override = isset($preset['settings']['video_watermark_fid']) ? intval($preset['settings']['video_watermark_fid']) : 0;
    $watermark_default = 0;
    if ($preset['overridden']) {
      $default_presets = Preset::getDefaultPresets();
      $default_preset = $default_presets[$preset_name];
      $watermark_default = isset($default_preset['settings']['video_watermark_fid']) ? intval($default_preset['settings']['video_watermark_fid']) : 0;
    }

    // Delete the watermark when it is not the default one.
    if ($watermark_override > 0 && $watermark_override != $watermark_default) {
      $file = file_load($preset['settings']['video_watermark_fid']);
      if (!empty($file)) {
        file_usage_delete($file, 'video');
        $file->status = 0;
        file_save($file);
      }
    }

    // Delete the preset.
    db_delete('video_preset')
      ->condition('name', $preset_name)
      ->execute();

    // Rebuild Theme Registry
    drupal_theme_rebuild();
  }
}
