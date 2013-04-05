<?php

/**
 * @file
 * Defines LingotekNode.
 */
 
/**
 * A class wrapper for Lingotek-specific behavior on nodes.
 */
class LingotekNode implements LingotekTranslatableEntity {  
  /**
   * A Drupal node.
   *
   * @var object
   */
  protected $node;
  
  
  /**
   * Lingotek Lingonode properties.
   *
   * @var object
   */
  protected $lingonode;
  
  /**
   * A reference to the Lingotek API.
   *
   * @var LingotekApi
   */
  protected $api = NULL;
  
  /**
   * Constructor.
   *
   * This is private since we want consumers to instantiate via the factory methods.
   *
   * @param object $node
   *   A Drupal node.
   */
  private function __construct($node) {
    $this->node = $node;
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
   * Factory method for getting a loaded LingotekNode object.
   *
   * @param object $node
   *   A Drupal node.
   *
   * @return LingotekNode
   *   A loaded LingotekNode object.
   */
  public static function load($node) {
    $node = new LingotekNode($node);
    $node->setApi(LingotekApi::instance());
    return $node;
  }
  
  
  /**
   * Method for loading the values for the lingonode
   * 
   * @param none
   * 
   * @return boolean
   * 
   */
  private function loadLingonode(){
    if($this->nid){
    // add in all values from the lingonode table (when missing set use global defaults)
      $lingonode = (object)lingotek_lingonode($this->nid);
      if($lingonode){
        $lingonode->auto_download = $lingonode->sync_method === FALSE ? variable_get('lingotek_sync') : $lingonode->sync_method;
        $this->lingonode = $lingonode;
        return TRUE;
      }
    }
    return FALSE;
  }
  
  /**
   * Factory method for getting a loaded LingotekNode object.
   *
   * @param int $node_id
   *   A Drupal node ID.
   *
   * @return mixed
   *   A loaded LingotekNode object on success, FALSE on failure.
   */
  public static function loadById($node_id) {
    $node = FALSE;
    if ($drupal_node = node_load($node_id)) {
      $node = self::load($drupal_node);
    }
    return $node;
  }
  
  
  /**
   * Loads a LingotekNode by Lingotek Document ID.
   *
   * @param string $lingotek_document_id
   *   The Document ID whose corresponding node should be loaded.
   *
   * @return mixed
   *   A LingotekNode object on success, FALSE on failure.
   */
  public static function loadByLingotekDocumentId($lingotek_document_id) {
    $node = FALSE;
    $key = 'document_id';
    $query = db_select('lingotek', 'l')->fields('l');
    $query->condition('lingokey', $key.'%', 'LIKE');
    $query->condition('lingovalue', $lingotek_document_id);
    $result = $query->execute();

    if ($record = $result->fetchAssoc()) {
      $node = self::loadById($record['nid']);
    }

    return $node;
  }
  
  
  /**
   * Gets the Lingotek document ID for this entity.
   *
   * @return mixed
   *   The integer document ID if the entity is associated with a 
   *   Lingotek document. FALSE otherwise.
   */
  public function lingotekDocumentId() {
    return lingotek_lingonode($this->node->nid, 'document_id');
  }
  
  /**
   * Gets the contents of this item formatted as XML that can be sent to Lingotek.
   *
   * @return string
   *   The XML document representing the entity's translatable content.
   */
  public function documentLingotekXML() {
    return lingotek_xml_node_body($this->node);
  }  
  
  /**
   * Magic get for access to node and node properties.
   */  
  public function __get($property_name) {
    $property = NULL;
    
    if ($property === 'node') {
      $property = $this->node;
    }
    elseif (isset($this->node->$property_name)) {
      $property = $this->node->$property_name;
    } else { // attempt to lookup the value in the lingonode table
      $val = lingotek_lingonode($this->node->nid,$property_name); 
      $property = ($val !== FALSE) ? $val : $property;
    } 
    
    return $property;
  }
  
  /**
   * Gets a Lingotek metadata value for this item.
   *
   * @param string $key
   *   The key whose value should be returned.
   *
   * @return string
   *   The value for the specified key, if it exists.
   */
  public function getMetadataValue($key) {
    // Necessary to fully implement the interface, but we don't do anything
    // on LingotekNode objects, explicitly.
  }
  
  /**
   * Sets a Lingotek metadata value for this item.
   *
   * @param string $key
   *   The key for a name/value pair.
   * @param string $value
   *   The value for a name/value pair.
   */
  public function setMetadataValue($key, $value) {
    // Necessary to fully implement the interface, but we don't do anything
    // on LingotekNode objects, explicitly.    
  }
  
  /**
   * Updates the local content with data from a Lingotek Document.
   *
   * @return bool
   *   TRUE if the content updates succeeded, FALSE otherwise.
   */
  public function updateLocalContent() {
    // Necessary to fully implement the interface, but we don't do anything
    // on LingotekNode objects, explicitly.    
  }
  
}
