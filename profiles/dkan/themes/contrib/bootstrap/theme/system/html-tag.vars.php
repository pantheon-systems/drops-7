<?php
/**
 * @file
 * html-tag.vars.php
 */

/**
 * Implements hook_process_html_tag().
 */
function bootstrap_process_html_tag(&$variables) {
  // Reference the element and tag name for easier coding below.
  $element = &$variables['element'];
  $tag = $element['#tag'];
  if ($tag === 'style' || $tag === 'script') {
    // Remove default "type" attribute. Leave others unaffected as it may be
    // needed and used for other purposes.
    // @see http://stackoverflow.com/a/5265361/1226717
    // @see https://drupal.org/node/2201779
    $types = array(
      // @see http://www.w3.org/TR/html5/document-metadata.html#attr-style-type
      'style' => 'text/css',
      // @see http://www.w3.org/TR/html5/scripting-1.html#attr-script-type
      'script' => 'text/javascript',
    );
    if (!empty($element['#attributes']['type']) && $element['#attributes']['type'] === $types[$tag]) {
      unset($element['#attributes']['type']);
    }

    // Remove CDATA comments. CDATA is only required for DOCTYPES that are XML
    // based, HTML5 is not.
    $cdata_prefix = array(
      'style' => "\n<!--/*--><![CDATA[/*><!--*/\n",
      'script' => "\n<!--//--><![CDATA[//><!--\n",
    );
    $cdata_suffix = array(
      'style' => "\n/*]]>*/-->\n",
      'script' => "\n//--><!]]>\n",
    );
    if (
      !empty($element['#value_prefix']) && $element['#value_prefix'] === $cdata_prefix[$tag] &&
      !empty($element['#value_suffix']) && $element['#value_suffix'] === $cdata_suffix[$tag]
    ) {
      unset($element['#value_prefix'], $element['#value_suffix']);
    }

    // Remove the "media=all" attribute, leave others unaffected.
    if (isset($element['#attributes']['media']) && $element['#attributes']['media'] === 'all') {
      unset($element['#attributes']['media']);
    }
  }
}
