<?php
class LibraryUpdater extends Updater implements DrupalUpdaterInterface {
  public function getInstallDirectory() {
     return DRUPAL_ROOT . "/sites/all/libraries";
  }

  public function isInstalled() {
    return is_dir(DRUPAL_ROOT . "/sites/all/libraries/{$this->name}");
  }

  public static function canUpdateDirectory($directory) {
    if (!file_scan_directory($directory, '/.*\.info$/')) {
      return TRUE;
    }
    return FALSE;
  }

  public static function canUpdate($project_name) {
    return is_dir(DRUPAL_ROOT . "/sites/all/libraries/$project_name");
  }
  public function __construct($source) {
    $this->source = $source;
    $this->name = self::getProjectName($source);
    $this->title = self::getProjectName($source);
  }

}

