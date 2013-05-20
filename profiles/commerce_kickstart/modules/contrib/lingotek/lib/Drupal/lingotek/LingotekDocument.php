<?php

/**
 * @file
 * Defines LingotekDocument.
 */
 
/**
 * A class representing a Lingotek Document
 */
class LingotekDocument {
  /**
   * A Lingotek Document ID.
   *
   * @var int
   */
  public $document_id;
  
  /**
   * A reference to the Lingotek API.
   *
   * @var LingotekApi
   */
  protected $api = NULL;
  
  /**
   * Static store for Documents already loaded in this request.
   */
  public static $documents = array();
  
  /**
   * Constructor.
   *
   * @param $document_id
   *   A Lingotek Document ID.
   */
  public function __construct($document_id) {
    $this->document_id = intval($document_id);
  }
  
  /**
   * Gets the translation targets associated with this document.
   *
   * @return array
   *   An array of Translation Target, as returned by a getDocument
   *   Lingotek API call
   */
  public function translationTargets() {
    $targets = array();
    
    if ($document = LingotekApi::instance()->getDocument($this->document_id)) {
      if (!empty($document->translationTargets)) {
        foreach ($document->translationTargets as $target) {
          $targets[Lingotek::convertLingotek2Drupal($target->language)] = $target;
        }        
      }
    }
    
    return $targets;
  }
  
  /**
   * Gets the current workflow phase for the document.
   *
   * @param int $translation_target_id
   *   The ID of the translation target whose current phase should be returned.
   *
   * @return mixed
   *   A LingotekPhase object if the current phase could be found, or FALSE on failure.
   */
  public function currentPhase($translation_target_id) {
    $phase = FALSE;
    
    if ($progress = $this->translationProgress()) {
      foreach ($progress->translationTargets as $target) {
        if ($target->id == $translation_target_id && !empty($target->phases)) {
          $current_phase = FALSE;
          foreach ($target->phases as $phase) {

            if (!$phase->isMarkedComplete) {
              $current_phase = $phase;
              break;
            }
          }

          // Return either the first uncompleted phase, or the last phase if all phases are complete.
          $current_phase = ($current_phase) ? $current_phase : end($target->phases);
          $phase = LingotekPhase::loadWithData($current_phase);
          break;
        }
      }
    }
    
    return $phase;
  }
  
  /**
   * Determines whether or not the document has Translation Targets in a complete-eligible phase.
   *
   * @return bool
   *   TRUE if complete-eligible phases are present. FALSE otherwise.
   */
  public function hasPhasesToComplete() {
    $result = FALSE;
    
    if (class_exists('LingotekPhase')) {
      $progress = $this->translationProgress();
      if( is_object( $progress ) ) {
        foreach ($progress->translationTargets as $target) {
          $current_phase = $this->currentPhase($target->id);
          if (is_object($current_phase) && $current_phase->canBeMarkedComplete()) {
            $result = TRUE;
            break;
          }
        }
      }
    }
    
    return $result;
  }
  
  /**
   * Gets the translation progress data for the Document.
   *
   * @return mixed
   *   The data object returned by a call to getDocumentProgress on success, FALSE on failure.
   */
  public function translationProgress() {
    $progress = &drupal_static(__FUNCTION__ . '-' . $this->document_id);
    
    if (!$progress) {
      $progress = $this->api->getDocumentProgress($this->document_id);
    }
    
    return $progress;
  }
  
  /**
   * Injects reference to an API object.
   *
   * @param LingotekApi $api
   *   An instantiated Lingotek API object.
   */
  public function setApi(LingotekApi $api) {
    $this->api = $api;
  }
  
  /**
   * Factory method for getting a loaded LingotekDocument object.
   *
   * @param int $document_id
   *   A Lingotek Document ID.
   *
   * @return LingotekDocument
   *   A loaded LingotekDocument object.
   */
  public static function load($document_id) {
    $document_id = intval($document_id);
    if (empty($documents[$document_id])) {
      $document = new LingotekDocument($document_id);
      $document->setApi(LingotekApi::instance());
      $documents[$document_id] = $document;
    }
    
    return $documents[$document_id];
  }
}
