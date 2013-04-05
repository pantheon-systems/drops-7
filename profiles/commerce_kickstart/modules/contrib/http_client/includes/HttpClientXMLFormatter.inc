<?php

/**
 * Class for handling xml-responses.
 * Returns a SimpleXML object
 *
 * @author Simon Ljungberg <simon.ljungberg@goodold.se>
 */

class HttpClientXMLFormatter implements HttpClientFormatter {
  private $adaptive_root;
  private $default_root;

  /**
   * Creates a HttpClientXMLFormatter.
   *
   * @param string $default_root
   *  Optional. Defaults to 'result'. The default name that should be used for root elements,
   *  if $adaptive_root is set to FALSE the default name will always be used.
   * @param bool $adaptive_root
   *  Optional. Defaults to FALSE. If $adaptive_root is set to TRUE and the source data has a
   *  single root attribute the serializer will use that attribute as root. The object {"foo":"bar"}
   *  would be serialized to <foo>bar</foo> instead of <result><foo>bar</foo></result>.
   */
  public function __construct($default_root = 'result', $adaptive_root = FALSE) {
    $this->default_root = $default_root;
    $this->adaptive_root = $adaptive_root;
  }

  /**
   * Serializes arbitrary data to the implemented format.
   * Directly stolen from http_server by Hugo Wetterberg
   *
   * @param mixed $data
   *  The data that should be serialized.
   * @return string
   *  The serialized data as a string.
   */
  public function serialize($data) {
    $doc = new DOMDocument('1.0', 'utf-8');
    $root_tag = $this->default_root;

    // Normalize any objects into an array.
    if (is_object($data)) {
      $data = get_object_vars($data);
    }
    // Check if we should adapt the name of the root element.
    if ($this->adaptive_root && is_array($data) && (count($data) == 1) && !is_numeric(key($data))) {
      $root_tag = $this->sanitizeNodeName(key($data));
      $data = current($data);
    }

    $root = $doc->createElement($root_tag);
    $doc->appendChild($root);

    $this->xml_recurse($doc, $root, $data);

    return $doc->saveXML();
  }

  /**
   * Sanitizes a string so that it's suitable for use as a element
   * or attribute name.
   *
   * @param string $name
   * @return string
   *  The sanitized name.
   */
  private function sanitizeNodeName($name) {
    $name = preg_replace('/[^A-Za-z0-9_]/', '_', $name);
    return preg_replace('/^([0-9]+)/', '_$1', $name);
  }

  /**
   * Return the mime type that the formatter can parse.
   */
  public function accepts(){
    return $this->mimeType();
  }

  /**
   * Return the content type form the data the formatter generates.
   */
  public function contentType(){
    return $this->mimeType();
  }

  /**
   * Unserializes data in the implemented format.
   *
   * @param string $data
   *  The data that should be unserialized.
   * @return mixed
   *  The unserialized data.
   */
  public function unserialize($data) {
    $xml = simplexml_load_string($data);

    if ($xml instanceof SimpleXMLElement) {
      // Only return data if we got well formed xml
      return $xml;
    }
    else {
      // Data was messed up
      throw new InvalidArgumentException('XML response was malformed.');
    }
  }

  public function mimeType() {
    return 'application/xml';
  }

  /**
   * Directly stolen from http_server by Hugo Wetterberg
   */
  protected function xml_recurse(&$doc, &$parent, $data) {
    if (is_object($data)) {
      $data = get_object_vars($data);
    }

    if (is_array($data)) {
      $assoc = FALSE || empty($data);
      foreach ($data as $key => $value) {
        if (is_numeric($key)) {
          $key = 'item';
        }
        else {
          $assoc = TRUE;
          $key = $this->sanitizeNodeName($key);
        }
        $element = $doc->createElement($key);
        $parent->appendChild($element);
        $this->xml_recurse($doc, $element, $value);
      }

      if (!$assoc) {
        $parent->setAttribute('is_array', 'true');
      }
    }
    else if ($data !== NULL) {
      $parent->appendChild($doc->createTextNode($data));
    }
  }
}
