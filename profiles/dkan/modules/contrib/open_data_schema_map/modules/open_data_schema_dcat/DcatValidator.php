<?php
use JsonSchema\Uri;

/**
 * Class DcatValidator
 * @package DcatValidator
 */
class DcatValidator extends OdsmValidator {
  /**
   * {@inheritdoc}
   */
  protected function getDatasetIdProperty() {
    return 'dct:identifier';
  }

  /**
   * {@inheritdoc}
   */
  protected function getSchemaInfo() {
    if (empty($this->schemaInfo)) {
      $retriever = new JsonSchema\Uri\UriRetriever();
      $schema_folder = DRUPAL_ROOT . '/' . drupal_get_path('module', 'open_data_schema_dcat') . '/data';
      $schema = $retriever->retrieve('file://' . $schema_folder . '/distribution.json');

      $this->schemaInfo = new \stdClass();
      $this->schemaInfo->schema = $schema;
      $this->schemaInfo->schema_folder = $schema_folder;
      $this->schemaInfo->api_endpoint = 'catalog.json';
    }
    return $this->schemaInfo;
  }

  /**
   * {@inheritdoc}
   */
  protected function getDatasetsFromData($data) {
    return $data;
  }
}
