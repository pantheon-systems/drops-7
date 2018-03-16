<?php
/**
 * @file
 * TermFixture Class Definition
 */

class TermFixtureExport {
  public $term;
  public $map;
  public $export_path;
  public $export;
  public function __construct($tid, $map, $export_path) {
    $this->term = taxonomy_term_load($tid);
    $this->map = $map;
    $this->export_path = $export_path;
    $this->export = array();
    $this->mapTerm();
  }

  public function mapTerm() {
    foreach ($this->map as $key => $property) {
      $values = array();
      if (is_array($this->term->{$key}) && count($this->term->{$key}) > 0) {        
        $iter = $this->term->{$key}[LANGUAGE_NONE];
        foreach ($iter as $value) {
          if (isset($value['filename'])) {
            $values[] = array('file' => $this->copyFile($value['uri']));
          }
          else {
            $values[] = $value;
          }
        }
      }
      else {
        $values = $this->term->{$key};
      }
      $this->export[$property] = $values;
    }
    $this->populateParents();
  }

  private function copyFile($uri) {
    $origin = drupal_realpath($uri);
    $file_info = pathinfo($origin);
    $destination = $this->export_path . '/' . $file_info['basename'];
    $destination = DRUPAL_ROOT . '/' . $destination;
    file_unmanaged_copy($origin, $destination);
    return $file_info['basename'];
  }

  private function populateParents() {
    $parents = taxonomy_get_parents($this->term->tid);
    if (count($parents)) {
      $uuids = array();
      foreach ($parents as $parent) {
        $uuids[] = $parent->uuid;
      }
      $this->export['parents'] = $uuids;
    }
  }
}
