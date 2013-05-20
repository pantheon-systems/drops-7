<?php

/**
 * Processor for making searches insensitive to accents and other non-ASCII characters.
 */
class SearchApiTransliteration extends SearchApiAbstractProcessor {

  protected function process(&$value) {
    $value = transliteration_get($value, '', language_default('language'));
  }

}
