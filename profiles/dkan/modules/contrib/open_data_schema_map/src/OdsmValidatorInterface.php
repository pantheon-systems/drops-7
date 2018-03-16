<?php

/**
 * Interface OdsmValidatorInterface
 * @package openDataSchemaMap
 */
interface OdsmValidatorInterface {

  /**
   * Process all datasets for validation.
   */
  public function validate();

  /**
   * Get number of datasets.
   * @return int
   *   Number of datasets.
   */
  public function datasetCount();

  /**
   * Get validation errors.
   *
   * @return array
   *   Array of validation errors array, with id/property/error keys.
   */
  public function getErrors();
}
