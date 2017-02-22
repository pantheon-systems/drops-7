<?php

namespace podValidator;

include __DIR__ . '/../../autoload.php';

use JsonSchema\Uri\UriRetriever;
use JsonSchema\RefResolver;
use JsonSchema\Validator;

class validate {

  function __construct($url) {
    $this->url = $url;
    $this->errors = array();
  }

  public function getDataJSON()
  {
    if (!isset($this->dataset)) {
      $this->dataset = array();
      $arrContextOptions = array(
        "ssl"=>array(
          "verify_peer"=>false,
          "verify_peer_name"=>false,
        ),
      );
      $resp = drupal_http_request($this->url, array('context' => stream_context_create($arrContextOptions)));
      if ($resp->code == 200) {
        $this->dataJSON = json_decode($resp->data);
        foreach($this->dataJSON->dataset as $dataset) {
          $this->dataset[$dataset->identifier] = $dataset;
        }
        $this->dataJSON->dataset = $this->dataset;
      }
      else {
        throw new Exception(t("POD validator could not access %url", array("%url" => $this->url)));
      }
    }
  }

  public function getDataset($id)
  {
    return $this->dataset[$id];
  }

  public function getIdentifiers()
  {
    $this->identifers = array();
    $data = $this->dataJSON;
    foreach ($data->dataset as $dataset) {
      $this->identifiers[] = $dataset->identifier;
    }
  }

  public function process($id) {
    $retriever = new UriRetriever;
    $schemaFolder = DRUPAL_ROOT . '/' . drupal_get_path('module', 'open_data_schema_pod') . '/data/v1.1';
    if (module_exists('open_data_federal_extras')) {
      $schema = $retriever->retrieve('file://' . $schemaFolder . '/dataset.json');
    } else {
      $schema = $retriever->retrieve('file://' . $schemaFolder . '/dataset-non-federal.json');
    }
    $data = $this->getDataset($id);

    RefResolver::$maxDepth = 10;
    $refResolver = new RefResolver($retriever);
    $refResolver->resolve($schema, 'file://' . $schemaFolder . '/');
    $validator = new Validator();
    $validator->check($data, $schema);
    return $validator;
  }

  public function datasetCount()
  {
    $this->getDataJSON();
    return count($this->dataset);
  }

  public function processAll() {
    $this->getDataJSON();
    $this->getIdentifiers();
    $this->validated = array();
    foreach ($this->identifiers as $id) {
      $validator = $this->process($id);

      if ($validator->isValid()) {
      }
      else {
        foreach ($validator->getErrors() as $error) {
          $this->errors[] = array('id' => $id, 'property' => $error['property'], 'error' => $error['message']);
        }
      }
    }
  }

  public function getErrors() {
    return $this->errors;
  }
}
