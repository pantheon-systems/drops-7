<?php
/**
 * @file
 * TermFixture Class Definition
 */

class TermFixtureImport {
  public $term;
  protected $vocabulary;
  public $map;
  public $import_path;
  protected $parents = array();

  public function __construct($export, $vocabulary, $map, $import_path) {
    $this->vocabulary = $vocabulary;
    $this->map = $map;
    $this->import_path = $import_path;

    // Try to preload term from uuid.
    $query = new EntityFieldQuery();
    $term = $query->entityCondition('entity_type', 'taxonomy_term')
                  ->propertyCondition('uuid', $export['uuid'], '=')
                  ->execute();
    if (count($term)) {
      $term = array_keys($term['taxonomy_term']);
      $term = taxonomy_term_load($term[0]);
    }
    else {
      $term = new stdClass();
      $term->vid = $vocabulary->vid;
    }
    $this->term = $term;
    $this->mapTerm($export);
  }

  protected function mapTerm($export) {
    foreach ($this->map as $key => $value) {
      if (isset($export[$value])) {
        if (is_array($export[$value])) {
          foreach ($export[$value] as $k => $i) {
            if (isset($i['file'])) {
              $export[$value][$k] = $this->copyFile($i['file']);
            }
          }
          $this->term->{$key}[LANGUAGE_NONE] = $export[$value];
        }
        else {
          $this->term->{$key} = $export[$value];
        }  
      }
    }
    if (isset($export['parents'])) {
      $this->parents = $export['parents'];
    }
  }

  protected function copyFile($file) {
    $filepath = drupal_realpath($this->import_path . '/' . $file);
    // Create managed File object and associate with Image field.
    $file = (object) array(
      'uid' => 1,
      'uri' => $filepath,
      'filemime' => file_get_mimetype($filepath),
      'status' => 1,
    );
    // We save the file to the root of the files directory.
    $file = file_copy($file, 'public://');
    return (array) $file;
  }

  public function safeToSave() {
    // Try to preload term from uuid.
    foreach ($this->parents as $parent) {
      $query = new EntityFieldQuery();
      $term = $query->entityCondition('entity_type', 'taxonomy_term')
                  ->propertyCondition('uuid', $parent, '=')
                  ->execute();
      if (!isset($term['taxonomy_term'])) {
        return FALSE;
      }
    }
    return TRUE;
  }

  public function save() {
    $parents = array();
    if (count($this->parents)) {
      foreach ($this->parents as $parent) {
        $query = new EntityFieldQuery();
        $term = $query->entityCondition('entity_type', 'taxonomy_term')
                    ->propertyCondition('uuid', $parent, '=')
                    ->execute();
        $parents = array_merge($parents, array_keys($term['taxonomy_term']));
      }
    }
    if (!count($parents)) {
      $parents = array(0);
    }
    $this->term->parent = $parents;
    taxonomy_term_save($this->term);
  }
}
