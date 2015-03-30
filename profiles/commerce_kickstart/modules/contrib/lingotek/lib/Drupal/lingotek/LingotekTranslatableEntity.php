<?php

/**
 * @file
 * Defines LingotekTranslatableEntity.
 */

/**
 * An interface for entities that are eligible for translation via the Lingotek platform.
 */
interface LingotekTranslatableEntity {

  /**
   * Return the Drupal Entity type
   *
   * @return string
   *   The entity type associated with this object
   */
  public function getEntityType();

  /**
   * Return the ID
   *
   * @return int
   *   The ID associated with this object
   */
  public function getId();

  /**
   * Gets the contents of this item formatted as XML that can be sent to Lingotek.
   *
   * @return string
   *   The XML document representing the entity's translatable content.
   */
  public function documentLingotekXML();

  /**
   * Gets a Lingotek metadata value for this item.
   *
   * @param string $key
   *   The key whose value should be returned.
   *
   * @return string
   *   The value for the specified key, if it exists.
   */
  public function getMetadataValue($key);

  /**
   * Sets a Lingotek metadata value for this item.
   *
   * @param string $key
   *   The key for a name/value pair.
   * @param string $value
   *   The value for a name/value pair.
   */
  public function setMetadataValue($key, $value);

  /**
   * Updates the local content of $target_code with data from a Lingotek Document
   *
   * @param string $lingotek_locale
   *   The code for the language that needs to be updated.
   * @return bool
   *   TRUE if the content updates succeeded, FALSE otherwise.
   */
  public function downloadTriggered($lingotek_locale);

  /**
   * Gets the Lingotek document ID for this entity.
   *
   * @return mixed
   *   The integer document ID if the entity is associated with a 
   *   Lingotek document. FALSE otherwise.
   */
  public function lingotekDocumentId();

  public function getWorkflowId();

  public function getProjectId();

  public function getVaultId();

  public function getTitle();

  public function getDescription();
  
  public function getDocumentName();
  
  public function getUrl();
  
  public function getNote();
  
  public function preDownload($lingotek_locale, $completed);
  
  public function postDownload($lingotek_locale, $completed);
  
  public function setTitle($title);

  public function setStatus($status);

  public function setTargetsStatus($status);
  /*
   * Returns the source locale for the translatable entity
   */
  public function getSourceLocale();
}
