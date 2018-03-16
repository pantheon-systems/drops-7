<?php
use JsonSchema\Uri;

/**
 * Class PodValidator
 * @package podValidator
 */
class PodValidator extends OdsmValidator {
  /**
   * {@inheritdoc}
   */
  protected function getDatasetIdProperty() {
    return 'identifier';
  }

  /**
   * {@inheritdoc}
   */
  protected function getSchemaInfo() {
    $retriever = new JsonSchema\Uri\UriRetriever();
    $schema_folder = DRUPAL_ROOT . '/' . drupal_get_path('module', 'open_data_schema_pod') . '/data/v1.1';
    if (module_exists('open_data_federal_extras')) {
      $schema_filename = 'dataset.json';
    }
    else {
      $schema_filename = 'dataset-non-federal.json';
    }
    $schema = $retriever->retrieve('file://' . $schema_folder . '/' . $schema_filename);

    $schema_obj = new \stdClass();
    $schema_obj->schema = $schema;
    $schema_obj->schema_folder = $schema_folder;
    $schema_obj->api_endpoint = 'data.json';
    return $schema_obj;
  }
  /**
   * {@inheritdoc}
   */
  protected function getDatasetsFromData($data) {
    return $data->dataset;
  }
}
