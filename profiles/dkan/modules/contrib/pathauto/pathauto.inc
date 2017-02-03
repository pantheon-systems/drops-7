<?php

/**
 * @file
 * Miscellaneous functions for Pathauto.
 *
 * This also contains some constants giving human readable names to some numeric
 * settings; they're included here as they're only rarely used outside this file
 * anyway. Use module_load_include('inc', 'pathauto') if the constants need to
 * be available.
 *
 * @ingroup pathauto
 */

/**
 * Case should be left as is in the generated path.
 */
define('PATHAUTO_CASE_LEAVE_ASIS', 0);

/**
 * Case should be lowercased in the generated path.
 */
define('PATHAUTO_CASE_LOWER', 1);

/**
 * "Do nothing. Leave the old alias intact."
 */
define('PATHAUTO_UPDATE_ACTION_NO_NEW', 0);

/**
 * "Create a new alias. Leave the existing alias functioning."
 */
define('PATHAUTO_UPDATE_ACTION_LEAVE', 1);

/**
 * "Create a new alias. Delete the old alias."
 */
define('PATHAUTO_UPDATE_ACTION_DELETE', 2);

/**
 * Remove the punctuation from the alias.
 */
define('PATHAUTO_PUNCTUATION_REMOVE', 0);

/**
 * Replace the punctuation with the separator in the alias.
 */
define('PATHAUTO_PUNCTUATION_REPLACE', 1);

/**
 * Leave the punctuation as it is in the alias.
 */
define('PATHAUTO_PUNCTUATION_DO_NOTHING', 2);

/**
 * Check to see if there is already an alias pointing to a different item.
 *
 * @param $alias
 *   A string alias.
 * @param $source
 *   A string that is the internal path.
 * @param $language
 *   A string indicating the path's language.
 * @return
 *   TRUE if an alias exists, FALSE if not.
 */
function _pathauto_alias_exists($alias, $source, $language = LANGUAGE_NONE) {
  $pid = db_query_range("SELECT pid FROM {url_alias} WHERE source <> :source AND alias = :alias AND language IN (:language, :language_none) ORDER BY language DESC, pid DESC", 0, 1, array(
    ':source' => $source,
    ':alias' => $alias,
    ':language' => $language,
    ':language_none' => LANGUAGE_NONE,
  ))->fetchField();

  return !empty($pid);
}

/**
 * Fetches an existing URL alias given a path and optional language.
 *
 * @param $source
 *   An internal Drupal path.
 * @param $language
 *   An optional language code to look up the path in.
 * @return
 *   FALSE if no alias was found or an associative array containing the
 *   following keys:
 *   - pid: Unique path alias identifier.
 *   - alias: The URL alias.
 */
function _pathauto_existing_alias_data($source, $language = LANGUAGE_NONE) {
  $pid = db_query_range("SELECT pid FROM {url_alias} WHERE source = :source AND language IN (:language, :language_none) ORDER BY language DESC, pid DESC", 0, 1, array(':source' => $source, ':language' => $language, ':language_none' => LANGUAGE_NONE))->fetchField();
  return path_load(array('pid' => $pid));
}

/**
 * Clean up a string segment to be used in an URL alias.
 *
 * Performs the following possible alterations:
 * - Remove all HTML tags.
 * - Process the string through the transliteration module.
 * - Replace or remove punctuation with the separator character.
 * - Remove back-slashes.
 * - Replace non-ascii and non-numeric characters with the separator.
 * - Remove common words.
 * - Replace whitespace with the separator character.
 * - Trim duplicate, leading, and trailing separators.
 * - Convert to lower-case.
 * - Shorten to a desired length and logical position based on word boundaries.
 *
 * This function should *not* be called on URL alias or path strings because it
 * is assumed that they are already clean.
 *
 * @param $string
 *   A string to clean.
 * @return
 *   The cleaned string.
 */
function pathauto_cleanstring($string) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['cache'] = &drupal_static(__FUNCTION__);
  }
  $cache = &$drupal_static_fast['cache'];

  // Generate and cache variables used in this function so that on the second
  // call to pathauto_cleanstring() we focus on processing.
  if (!isset($cache)) {
    $cache = array(
      'separator' => variable_get('pathauto_separator', '-'),
      'strings' => array(),
      'transliterate' => variable_get('pathauto_transliterate', FALSE) && module_exists('transliteration'),
      'punctuation' => array(),
      'reduce_ascii' => (bool) variable_get('pathauto_reduce_ascii', FALSE),
      'ignore_words_regex' => FALSE,
      'lowercase' => (bool) variable_get('pathauto_case', PATHAUTO_CASE_LOWER),
      'maxlength' => min(variable_get('pathauto_max_component_length', 100), _pathauto_get_schema_alias_maxlength()),
    );

    // Generate and cache the punctuation replacements for strtr().
    $punctuation = pathauto_punctuation_chars();
    foreach ($punctuation as $name => $details) {
      $action = variable_get('pathauto_punctuation_' . $name, PATHAUTO_PUNCTUATION_REMOVE);
      switch ($action) {
        case PATHAUTO_PUNCTUATION_REMOVE:
          $cache['punctuation'][$details['value']] = '';
          break;
        case PATHAUTO_PUNCTUATION_REPLACE:
          $cache['punctuation'][$details['value']] = $cache['separator'];
          break;
        case PATHAUTO_PUNCTUATION_DO_NOTHING:
          // Literally do nothing.
          break;
      }
    }

    // Generate and cache the ignored words regular expression.
    $ignore_words = variable_get('pathauto_ignore_words', PATHAUTO_IGNORE_WORDS);
    $ignore_words_regex = preg_replace(array('/^[,\s]+|[,\s]+$/', '/[,\s]+/'), array('', '\b|\b'), $ignore_words);
    if ($ignore_words_regex) {
      $cache['ignore_words_regex'] = '\b' . $ignore_words_regex . '\b';
      if (function_exists('mb_eregi_replace')) {
        $cache['ignore_words_callback'] = 'mb_eregi_replace';
      }
      else {
        $cache['ignore_words_callback'] = 'preg_replace';
        $cache['ignore_words_regex'] = '/' . $cache['ignore_words_regex'] . '/i';
      }
    }
  }

  // Empty strings do not need any proccessing.
  if ($string === '' || $string === NULL) {
    return '';
  }

  // Check if the string has already been processed, and if so return the
  // cached result.
  if (isset($cache['strings'][$string])) {
    return $cache['strings'][$string];
  }

  // Remove all HTML tags from the string.
  $output = strip_tags(decode_entities($string));

  // Optionally transliterate (by running through the Transliteration module)
  if ($cache['transliterate']) {
    $output = transliteration_get($output);
  }

  // Replace or drop punctuation based on user settings
  $output = strtr($output, $cache['punctuation']);

  // Reduce strings to letters and numbers
  if ($cache['reduce_ascii']) {
    $output = preg_replace('/[^a-zA-Z0-9\/]+/', $cache['separator'], $output);
  }

  // Get rid of words that are on the ignore list
  if ($cache['ignore_words_regex']) {
    $words_removed = $cache['ignore_words_callback']($cache['ignore_words_regex'], '', $output);
    if (drupal_strlen(trim($words_removed)) > 0) {
      $output = $words_removed;
    }
  }

  // Always replace whitespace with the separator.
  $output = preg_replace('/\s+/', $cache['separator'], $output);

  // Trim duplicates and remove trailing and leading separators.
  $output = _pathauto_clean_separators($output, $cache['separator']);

  // Optionally convert to lower case.
  if ($cache['lowercase']) {
    $output = drupal_strtolower($output);
  }

  // Shorten to a logical place based on word boundaries.
  $output = truncate_utf8($output, $cache['maxlength'], TRUE);

  // Cache this result in the static array.
  $cache['strings'][$string] = $output;

  return $output;
}

/**
 * Trims duplicate, leading, and trailing separators from a string.
 *
 * @param $string
 *   The string to clean path separators from.
 * @param $separator
 *   The path separator to use when cleaning.
 * @return
 *   The cleaned version of the string.
 *
 * @see pathauto_cleanstring()
 * @see pathauto_clean_alias()
 */
function _pathauto_clean_separators($string, $separator = NULL) {
  static $default_separator;

  if (!isset($separator)) {
    if (!isset($default_separator)) {
      $default_separator = variable_get('pathauto_separator', '-');
    }
    $separator = $default_separator;
  }

  $output = $string;

  // Clean duplicate or trailing separators.
  if (strlen($separator)) {
    // Escape the separator.
    $seppattern = preg_quote($separator, '/');

    // Trim any leading or trailing separators.
    $output = preg_replace("/^$seppattern+|$seppattern+$/", '', $output);

    // Replace trailing separators around slashes.
    if ($separator !== '/') {
      $output = preg_replace("/$seppattern+\/|\/$seppattern+/", "/", $output);
    }

    // Replace multiple separators with a single one.
    $output = preg_replace("/$seppattern+/", $separator, $output);
  }

  return $output;
}

/**
 * Clean up an URL alias.
 *
 * Performs the following alterations:
 * - Trim duplicate, leading, and trailing back-slashes.
 * - Trim duplicate, leading, and trailing separators.
 * - Shorten to a desired length and logical position based on word boundaries.
 *
 * @param $alias
 *   A string with the URL alias to clean up.
 * @return
 *   The cleaned URL alias.
 */
function pathauto_clean_alias($alias) {
  $cache = &drupal_static(__FUNCTION__);

  if (!isset($cache)) {
    $cache = array(
      'maxlength' => min(variable_get('pathauto_max_length', 100), _pathauto_get_schema_alias_maxlength()),
    );
  }

  $output = $alias;

  // Trim duplicate, leading, and trailing back-slashes.
  $output = _pathauto_clean_separators($output, '/');

  // Trim duplicate, leading, and trailing separators.
  $output = _pathauto_clean_separators($output);

  // Shorten to a logical place based on word boundaries.
  $output = truncate_utf8($output, $cache['maxlength'], TRUE);

  return $output;
}

/**
 * Apply patterns to create an alias.
 *
 * @param $module
 *   The name of your module (e.g., 'node').
 * @param $op
 *   Operation being performed on the content being aliased
 *   ('insert', 'update', 'return', or 'bulkupdate').
 * @param $source
 *   An internal Drupal path to be aliased.
 * @param $data
 *   An array of keyed objects to pass to token_replace(). For simple
 *   replacement scenarios 'node', 'user', and others are common keys, with an
 *   accompanying node or user object being the value. Some token types, like
 *   'site', do not require any explicit information from $data and can be
 *   replaced even if it is empty.
 * @param $type
 *   For modules which provided pattern items in hook_pathauto(),
 *   the relevant identifier for the specific item to be aliased
 *   (e.g., $node->type).
 * @param $language
 *   A string specify the path's language.
 * @return
 *   The alias that was created.
 *
 * @see _pathauto_set_alias()
 * @see token_replace()
 */
function pathauto_create_alias($module, $op, $source, $data, $type = NULL, $language = LANGUAGE_NONE) {
  // Retrieve and apply the pattern for this content type.
  $pattern = pathauto_pattern_load_by_entity($module, $type, $language);
  if (empty($pattern)) {
    // No pattern? Do nothing (otherwise we may blow away existing aliases...)
    return '';
  }

  // Special handling when updating an item which is already aliased.
  $existing_alias = NULL;
  if ($op == 'update' || $op == 'bulkupdate') {
    if ($existing_alias = _pathauto_existing_alias_data($source, $language)) {
      switch (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE)) {
        case PATHAUTO_UPDATE_ACTION_NO_NEW:
          // If an alias already exists, and the update action is set to do nothing,
          // then gosh-darn it, do nothing.
          return '';
      }
    }
  }

  // Replace any tokens in the pattern. Uses callback option to clean replacements. No sanitization.
  $alias = token_replace($pattern, $data, array(
    'sanitize' => FALSE,
    'clear' => TRUE,
    'callback' => 'pathauto_clean_token_values',
    'language' => (object) array('language' => $language),
    'pathauto' => TRUE,
  ));

  // Check if the token replacement has not actually replaced any values. If
  // that is the case, then stop because we should not generate an alias.
  // @see token_scan()
  $pattern_tokens_removed = preg_replace('/\[[^\s\]:]*:[^\s\]]*\]/', '', $pattern);
  if ($alias === $pattern_tokens_removed) {
    return '';
  }

  $alias = pathauto_clean_alias($alias);

  // Allow other modules to alter the alias.
  $context = array(
    'module' => $module,
    'op' => $op,
    'source' => &$source,
    'data' => $data,
    'type' => $type,
    'language' => &$language,
    'pattern' => $pattern,
  );
  drupal_alter('pathauto_alias', $alias, $context);

  // If we have arrived at an empty string, discontinue.
  if (!drupal_strlen($alias)) {
    return '';
  }

  // If the alias already exists, generate a new, hopefully unique, variant.
  $original_alias = $alias;
  pathauto_alias_uniquify($alias, $source, $language);
  if ($original_alias != $alias) {
    // Alert the user why this happened.
    _pathauto_verbose(t('The automatically generated alias %original_alias conflicted with an existing alias. Alias changed to %alias.', array(
      '%original_alias' => $original_alias,
      '%alias' => $alias,
    )), $op);
  }

  // Return the generated alias if requested.
  if ($op == 'return') {
    return $alias;
  }

  // Build the new path alias array and send it off to be created.
  $path = array(
    'source' => $source,
    'alias' => $alias,
    'language' => $language,
  );
  $path = _pathauto_set_alias($path, $existing_alias, $op);
  return $path;
}

/**
 * Check to ensure a path alias is unique and add suffix variants if necessary.
 *
 * Given an alias 'content/test' if a path alias with the exact alias already
 * exists, the function will change the alias to 'content/test-0' and will
 * increase the number suffix until it finds a unique alias.
 *
 * @param $alias
 *   A string with the alias. Can be altered by reference.
 * @param $source
 *   A string with the path source.
 * @param $langcode
 *   A string with a language code.
 */
function pathauto_alias_uniquify(&$alias, $source, $langcode) {
  if (!_pathauto_alias_exists($alias, $source, $langcode)) {
    return;
  }

  // If the alias already exists, generate a new, hopefully unique, variant
  $maxlength = min(variable_get('pathauto_max_length', 100), _pathauto_get_schema_alias_maxlength());
  $separator = variable_get('pathauto_separator', '-');
  $original_alias = $alias;

  $i = 0;
  do {
    // Append an incrementing numeric suffix until we find a unique alias.
    $unique_suffix = $separator . $i;
    $alias = truncate_utf8($original_alias, $maxlength - drupal_strlen($unique_suffix, TRUE)) . $unique_suffix;
    $i++;
  } while (_pathauto_alias_exists($alias, $source, $langcode));
}

/**
 * Verify if the given path is a valid menu callback.
 *
 * Taken from menu_execute_active_handler().
 *
 * @param $path
 *   A string containing a relative path.
 * @return
 *   TRUE if the path already exists.
 */
function _pathauto_path_is_callback($path) {
  // We need to use a try/catch here because of a core bug which will throw an
  // exception if $path is something like 'node/foo/bar'.
  // @todo Remove when http://drupal.org/node/1302158 is fixed in core.
  try {
    $menu = menu_get_item($path);
  }
  catch (Exception $e) {
    return FALSE;
  }

  if (isset($menu['path']) && $menu['path'] == $path) {
    return TRUE;
  }
  elseif (is_file(DRUPAL_ROOT . '/' . $path) || is_dir(DRUPAL_ROOT . '/' . $path)) {
    // Do not allow existing files or directories to get assigned an automatic
    // alias. Note that we do not need to use is_link() to check for symbolic
    // links since this returns TRUE for either is_file() or is_dir() already.
    return TRUE;
  }
  return FALSE;
}

/**
 * Private function for Pathauto to create an alias.
 *
 * @param $path
 *   An associative array containing the following keys:
 *   - source: The internal system path.
 *   - alias: The URL alias.
 *   - pid: (optional) Unique path alias identifier.
 *   - language: (optional) The language of the alias.
 * @param $existing_alias
 *   (optional) An associative array of the existing path alias.
 * @param $op
 *   An optional string with the operation being performed.
 *
 * @return
 *   The saved path from path_save() or NULL if the path was not saved.
 *
 * @see path_save()
 */
function _pathauto_set_alias(array $path, $existing_alias = NULL, $op = NULL) {
  $verbose = _pathauto_verbose(NULL, $op);

  // Alert users that an existing callback cannot be overridden automatically
  if (_pathauto_path_is_callback($path['alias'])) {
    if ($verbose) {
      _pathauto_verbose(t('Ignoring alias %alias due to existing path conflict.', array('%alias' => $path['alias'])));
    }
    return;
  }
  // Alert users if they are trying to create an alias that is the same as the internal path
  if ($path['source'] == $path['alias']) {
    if ($verbose) {
      _pathauto_verbose(t('Ignoring alias %alias because it is the same as the internal path.', array('%alias' => $path['alias'])));
    }
    return;
  }

  // Skip replacing the current alias with an identical alias
  if (empty($existing_alias) || $existing_alias['alias'] != $path['alias']) {
    $path += array('pathauto' => TRUE, 'original' => $existing_alias);

    // If there is already an alias, respect some update actions.
    if (!empty($existing_alias)) {
      switch (variable_get('pathauto_update_action', PATHAUTO_UPDATE_ACTION_DELETE)) {
        case PATHAUTO_UPDATE_ACTION_NO_NEW:
          // Do not create the alias.
          return;
        case PATHAUTO_UPDATE_ACTION_LEAVE:
          // Create a new alias instead of overwriting the existing by leaving
          // $path['pid'] empty.
          break;
        case PATHAUTO_UPDATE_ACTION_DELETE:
          // The delete actions should overwrite the existing alias.
          $path['pid'] = $existing_alias['pid'];
          break;
      }
    }

    // Save the path array.
    path_save($path);

    if ($verbose) {
      if (!empty($existing_alias['pid'])) {
        _pathauto_verbose(t('Created new alias %alias for %source, replacing %old_alias.', array('%alias' => $path['alias'], '%source' => $path['source'], '%old_alias' => $existing_alias['alias'])));
      }
      else {
        _pathauto_verbose(t('Created new alias %alias for %source.', array('%alias' => $path['alias'], '%source' => $path['source'])));
      }
    }

    return $path;
  }
}

/**
 * Output a helpful message if verbose output is enabled.
 *
 * Verbose output is only enabled when:
 * - The 'pathauto_verbose' setting is enabled.
 * - The current user has the 'notify of path changes' permission.
 * - The $op parameter is anything but 'bulkupdate' or 'return'.
 *
 * @param $message
 *   An optional string of the verbose message to display. This string should
 *   already be run through t().
 * @param $op
 *   An optional string with the operation being performed.
 * @return
 *   TRUE if verbose output is enabled, or FALSE otherwise.
 */
function _pathauto_verbose($message = NULL, $op = NULL) {
  static $verbose;

  if (!isset($verbose)) {
    $verbose = variable_get('pathauto_verbose', FALSE) && user_access('notify of path changes');
  }

  if (!$verbose || (isset($op) && in_array($op, array('bulkupdate', 'return')))) {
    return FALSE;
  }

  if ($message) {
    drupal_set_message($message);
  }

  return $verbose;
}

/**
 * Clean tokens so they are URL friendly.
 *
 * @param $replacements
 *   An array of token replacements that need to be "cleaned" for use in the URL.
 * @param $data
 *   An array of objects used to generate the replacements.
 * @param $options
 *   An array of options used to generate the replacements.
 */
function pathauto_clean_token_values(&$replacements, $data = array(), $options = array()) {
  foreach ($replacements as $token => $value) {
    // Only clean non-path tokens.
    if (!preg_match('/(path|alias|url|url-brief)\]$/', $token)) {
      $replacements[$token] = pathauto_cleanstring($value);
    }
  }
}

/**
 * Return an array of arrays for punctuation values.
 *
 * Returns an array of arrays for punctuation values keyed by a name, including
 * the value and a textual description.
 * Can and should be expanded to include "all" non text punctuation values.
 *
 * @return
 *   An array of arrays for punctuation values keyed by a name, including the
 *   value and a textual description.
 */
function pathauto_punctuation_chars() {
  $punctuation = &drupal_static(__FUNCTION__);

  if (!isset($punctuation)) {
    $cid = 'pathauto:punctuation:' . $GLOBALS['language']->language;
    if ($cache = cache_get($cid)) {
      $punctuation = $cache->data;
    }
    else {
      $punctuation = array();
      $punctuation['double_quotes']      = array('value' => '"', 'name' => t('Double quotation marks'));
      $punctuation['quotes']             = array('value' => '\'', 'name' => t("Single quotation marks (apostrophe)"));
      $punctuation['backtick']           = array('value' => '`', 'name' => t('Back tick'));
      $punctuation['comma']              = array('value' => ',', 'name' => t('Comma'));
      $punctuation['period']             = array('value' => '.', 'name' => t('Period'));
      $punctuation['hyphen']             = array('value' => '-', 'name' => t('Hyphen'));
      $punctuation['underscore']         = array('value' => '_', 'name' => t('Underscore'));
      $punctuation['colon']              = array('value' => ':', 'name' => t('Colon'));
      $punctuation['semicolon']          = array('value' => ';', 'name' => t('Semicolon'));
      $punctuation['pipe']               = array('value' => '|', 'name' => t('Vertical bar (pipe)'));
      $punctuation['left_curly']         = array('value' => '{', 'name' => t('Left curly bracket'));
      $punctuation['left_square']        = array('value' => '[', 'name' => t('Left square bracket'));
      $punctuation['right_curly']        = array('value' => '}', 'name' => t('Right curly bracket'));
      $punctuation['right_square']       = array('value' => ']', 'name' => t('Right square bracket'));
      $punctuation['plus']               = array('value' => '+', 'name' => t('Plus sign'));
      $punctuation['equal']              = array('value' => '=', 'name' => t('Equal sign'));
      $punctuation['asterisk']           = array('value' => '*', 'name' => t('Asterisk'));
      $punctuation['ampersand']          = array('value' => '&', 'name' => t('Ampersand'));
      $punctuation['percent']            = array('value' => '%', 'name' => t('Percent sign'));
      $punctuation['caret']              = array('value' => '^', 'name' => t('Caret'));
      $punctuation['dollar']             = array('value' => '$', 'name' => t('Dollar sign'));
      $punctuation['hash']               = array('value' => '#', 'name' => t('Number sign (pound sign, hash)'));
      $punctuation['at']                 = array('value' => '@', 'name' => t('At sign'));
      $punctuation['exclamation']        = array('value' => '!', 'name' => t('Exclamation mark'));
      $punctuation['tilde']              = array('value' => '~', 'name' => t('Tilde'));
      $punctuation['left_parenthesis']   = array('value' => '(', 'name' => t('Left parenthesis'));
      $punctuation['right_parenthesis']  = array('value' => ')', 'name' => t('Right parenthesis'));
      $punctuation['question_mark']      = array('value' => '?', 'name' => t('Question mark'));
      $punctuation['less_than']          = array('value' => '<', 'name' => t('Less-than sign'));
      $punctuation['greater_than']       = array('value' => '>', 'name' => t('Greater-than sign'));
      $punctuation['slash']              = array('value' => '/', 'name' => t('Slash'));
      $punctuation['back_slash']         = array('value' => '\\', 'name' => t('Backslash'));

      // Allow modules to alter the punctuation list and cache the result.
      drupal_alter('pathauto_punctuation_chars', $punctuation);
      cache_set($cid, $punctuation);
    }
  }

  return $punctuation;
}

/**
 * Fetch the maximum length of the {url_alias}.alias field from the schema.
 *
 * @return
 *   An integer of the maximum URL alias length allowed by the database.
 */
function _pathauto_get_schema_alias_maxlength() {
  $maxlength = &drupal_static(__FUNCTION__);
  if (!isset($maxlength)) {
    $schema = drupal_get_schema('url_alias');
    $maxlength = $schema['fields']['alias']['length'];
  }
  return $maxlength;
}
