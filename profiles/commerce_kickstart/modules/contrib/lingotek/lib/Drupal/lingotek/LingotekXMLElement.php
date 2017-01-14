<?php

/**
 * @file
 * Defines LingotekXMLElement
 */

/**
 * An extension of SimpleXMLElement to add CDATA
 */
class LingotekXMLElement extends SimpleXMLElement {
  public function addCData($text) {
    $xml = dom_import_simplexml($this);
    $doc = $xml->ownerDocument;
    $xml->appendChild($doc->createCDATASection($text));
  }

  /**
   * Add a SimpleXMLElement object as a child
   * @param SimpleXMLElement $child
   * @param type $element_name
   */
  public function addChildXML(SimpleXMLElement $child, $element_name = NULL) {
    if (!$element_name) {
      $element_name = $child->getName();
    }
    $this_child = $this->addChild($element_name);
    $to_dom = dom_import_simplexml($this_child);
    $from_dom = dom_import_simplexml($child);
    foreach ($from_dom->childNodes as $child_element) {
      $to_dom->appendChild($to_dom->ownerDocument->importNode($child_element, TRUE));
    }
  }

  /**
   * Add a SimpleXMLElement object to this object
   * @param SimpleXMLElement $xml
   */
  public function addXML(SimpleXMLElement $xml) {
    $to_dom = dom_import_simplexml($this);
    $from_dom = dom_import_simplexml($xml);
    foreach ($from_dom->childNodes as $child_element) {
      $to_dom->appendChild($to_dom->ownerDocument->importNode($child_element, TRUE));
    }
  }
}
