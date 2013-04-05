<?php

/**
 * @file
 * Defines LingotekPhase.
 */
 
/**
 * A class representing a Lingotek Workflow Phase.
 */
class LingotekPhase {
  /**
   * The phase data
   *
   * @var int
   */
  protected $phase;
  
  /**
   * Constructor.
   *
   * @param object $phase
   *   Phase data as returned by a getPhase API call.
   */
  public function __construct($phase) {
    $this->phase = $phase;
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
   * Factory method for getting a loaded LingotekPhase object.
   *
   * @param int $phase_id
   *   A phase ID.
   *
   * @return LingotekPhase
   *   A loaded LingotekPhase object.
   */
  public static function load($phase_id) {
    $api = LingotekApi::instance();
    $api_phase = $api->get_phase($phase_id);
    $phase = new LingotekPhase($api_phase);
    $phase->setApi($api);
    
    return $phase;
  }
  
  /**
   * Factory method for getting a loaded LingotekPhase object.
   *
   * @param object $api_phase
   *   Phase data as returned by a getPhase Lingotek API call.
   *
   * @return LingotekPhase
   *   A loaded LingotekPhase object.
   */
  public static function loadWithData($api_phase) {
    $api = LingotekApi::instance();
    $phase = new LingotekPhase($api_phase);
    $phase->setApi($api);
    
    return $phase;
  }
  
  
  /**
   * Determines whether or not the current phase is eligible to be marked as complete.
   *
   * @return bool
   *   TRUE if the phase can be marked as complete. FALSE otherwise.
   */
  public function canBeMarkedComplete() {
    $result = FALSE;
    
    // These phase types need to be at 100% complete in order to 
    // be eligible for mark as complete.
    $needs_100_complete_phase_types = array(
      'TRANSLATION',
      'REVIEW',
    );
    
    if (in_array($this->phase->type, $needs_100_complete_phase_types)) {
      if ($this->phase->percentComplete == 100 && !$this->phase->isMarkedComplete) {
        $result = TRUE;
      }
    }
    elseif (!$this->phase->isMarkedComplete) {
      // All other phase types should be able to be marked as complete regardless
      // of completion percentage.
      $result = TRUE;
    }
    
    return $result;
  }
  
  /**
   * Magic get for phase property access.
   */
  public function __get($property) {
    $value = NULL;
    
    if (!empty($this->phase->$property)) {
      $value = $this->phase->$property;
    }
    
    return $value;
  }
}
