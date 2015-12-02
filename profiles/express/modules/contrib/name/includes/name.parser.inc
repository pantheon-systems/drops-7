<?php

/**
 * @file
 * Provides the functionality and information about the
 * Name module name parsing engine.
 */

/**
 * Provides help to the characters that are recognized in
 * the name_format() format parameter string.
 */
function theme_name_format_parameter_help() {
  $output = '<div>' . t('The following characters are recognized in the format parameter string:') . '</div>';
  $output .= '<dl>';
  foreach (name_replacement_tokens() as $token => $title) {
    $output .= "<dt>{$token}</dt><dd>{$title}</dd>";
  }
  $output .= '</dl>';

  return $output;
}
/**
 * Provides the tokens that the name parse can handle.
 *
 * Todo:: make the labels generic
 */
function name_replacement_tokens() {
  $tokens = array(
    't' => t('Title'),
    'g' => t('Given name'),
    'm' => t('Middle name(s)'),
    'f' => t('Family name'),
    'c' => t('Credentials'),
    's' => t('Generational suffix'),
    'x' => t('First letter given'),
    'y' => t('First letter middle'),
    'z' => t('First letter family'),
    'e' => t('Conditional: Either the given or family name. Given name is given preference.'),
    'E' => t('Conditional: Either the given or family name. Family name is given preference.'),
    'i' => t('Separator 1'),
    'j' => t('Separator 2'),
    'k' => t('Separator 3'),
    '\\' => t('You can prevent a character in the format string from being expanded by escaping it with a preceding backslash.'),
    'L' => t('Modifier: Converts the next token to all lowercase.'),
    'U' => t('Modifier: Converts the next token to all uppercase.'),
    'F' => t('Modifier: Converts the first letter to uppercase.'),
    'G' => t('Modifier: Converts the first letter of ALL words to uppercase.'),
    'T' => t('Modifier: Trims whitespace around the next token.'),
    'S' => t('Modifier: Ensures that the next token is safe for the display.'),
    '+' => t('Conditional: Insert the token if both the surrounding tokens are not empty.'),
    '-' => t('Conditional: Insert the token if the previous token is not empty'),
    '~' => t('Conditional: Insert the token if the previous token is empty'),
    '=' => t('Conditional: Insert the token if the next token is not empty.'),
    '^' => t('Conditional: Insert the token if the next token is empty.'),
    '|' => t('Conditional: Uses the previous token unless empty, otherwise it uses this token.'),
    '(' => t('Group: Start of token grouping.'),
    ')' => t('Group: End of token grouping.'),
    /*

 // Placeholders for token support insertion on the [object / key | entity / bundle].
 '1' => t('Token placeholder 1'),
 '2' => t('Token placeholder 2'),
 '3' => t('Token placeholder 3'),
 '4' => t('Token placeholder 4'),
 '5' => t('Token placeholder 5'),
 '6' => t('Token placeholder 6'),
 '7' => t('Token placeholder 7'),
 '8' => t('Token placeholder 8'),
 '9' => t('Token placeholder 9'),

 */
  );

  return $tokens;
}

function _name_generate_tokens($name_components) {
  $name_components = (array) $name_components;
  $name_components += array(
    'title' => '',
    'given' => '',
    'middle' => '',
    'family' => '',
    'credentials' => '',
    'generational' => '',
  );
  $settings = name_settings();
  $tokens = array(
    't' => isset($name_components['title']) ? $name_components['title'] : NULL,
    'g' => isset($name_components['given']) ? $name_components['given'] : NULL,
    'm' => isset($name_components['middle']) ? $name_components['middle'] : NULL,
    'f' => isset($name_components['family']) ? $name_components['family'] : NULL,
    'c' => isset($name_components['credentials']) ? $name_components['credentials'] : NULL,
    's' => isset($name_components['generational']) ? $name_components['generational'] : NULL,
    'x' => isset($name_components['given']) && strlen($name_components['given']) ? $name_components['given']{0} : NULL,
    'y' => isset($name_components['middle']) && strlen($name_components['middle']) ? $name_components['middle']{0} : NULL,
    'z' => isset($name_components['family']) && strlen($name_components['family']) ? $name_components['family']{0} : NULL,
    'i' => $settings['sep1'],
    'j' => $settings['sep2'],
    'k' => $settings['sep3'],
  );
  $given = trim((string) $name_components['given']);
  $family = trim((string) $name_components['family']);
  if ($given || $family) {
    $tokens += array(
      'e' => $given ? $given : $family,
      'E' => $family ? $family : $given,
    );
  }
  else {
    $tokens += array(
      'e' => NULL,
      'E' => NULL,
    );
  }
  return $tokens;
}

function _name_format($name_components, $format = '', $settings = array(), $tokens = NULL) {
  if (empty($format)) {
    return '';
  }

  if (!isset($tokens)) {
    $tokens = _name_generate_tokens($name_components);
  }
  // Nuetralise any escapped backslashes.
  $format = str_replace('\\\\', "\t", $format);

  $pieces = array();
  $len = strlen($format);
  $modifiers = '';
  $conditions = '';
  $depth = 0;
  for ($i = 0; $i < strlen($format); $i++) {
    $char = $format{$i};
    $last_char = ($i > 0) ? $format{$i -1} : FALSE;
    $next_char = ($i < $len - 2) ? $format{$i + 1} : FALSE;

    // Handle escaped letters.
    if ($char == '\\') {
      continue;
    }
    if ($last_char == '\\') {
      $pieces[] = _name_format_add_component($char, $modifiers, $conditions);
      continue;
    }

    switch ($char) {
      case 'L':
      case 'U':
      case 'F':
      case 'T':
      case 'S':
      case 'G':
        $modifiers .= $char;
        break;

      case '=':
      case '^':
      case '|':
      case '+':
      case '-':
      case '~':
        $conditions .= $char;
        break;

      case '(':
      case ')':
        $remaining_string = substr($format, $i);
        if ($char == '(' && $closing_bracket = _name_format_closing_bracket_position($remaining_string)) {
          $sub_string = _name_format($tokens, substr($format, $i + 1, $closing_bracket - 1), $settings, $tokens);

          // Increment the counter past the closing bracket.
          $i += $closing_bracket;
          $pieces[] = _name_format_add_component($sub_string, $modifiers, $conditions);
        }
        else {
          // Unmatched, add it.
          $pieces[] = _name_format_add_component($char, $modifiers, $conditions);
        }
        break;

      default:
        if (array_key_exists($char, $tokens)) {
          $char = $tokens[$char];
        }
        $pieces[] = _name_format_add_component($char, $modifiers, $conditions);
        break;
    }
  }

  $parsed_pieces = array();
  for ($i = 0; $i < count($pieces); $i++) {
    $component = $pieces[$i]['value'];
    $conditions = $pieces[$i]['conditions'];

    $last_component = ($i > 0) ? $pieces[$i - 1]['value'] : FALSE;
    $next_component = ($i < count($pieces) - 1) ? $pieces[$i + 1]['value'] : FALSE;

    if (empty($conditions)) {
      $parsed_pieces[$i] = $component;
    }
    else {
      // Modifier: Conditional insertion. Insert if both the surrounding tokens are not empty.
      if (strpos($conditions, '+') !== FALSE && !empty($last_component) && !empty($next_component)) {
        $parsed_pieces[$i] = $component;
      }

      // Modifier: Conditional insertion. Insert if the previous token is not empty.
      if (strpos($conditions, '-') !== FALSE && !empty($last_component)) {
        $parsed_pieces[$i] = $component;
      }

      // Modifier: Conditional insertion. Insert if the previous token is empty.
      if (strpos($conditions, '~') !== FALSE && empty($last_component)) {
        $parsed_pieces[$i] = $component;
      }

      // Modifier: Insert the token if the next token is empty.
      if (strpos($conditions, '^') !== FALSE && empty($next_component)) {
        $parsed_pieces[$i] = $component;
      }

      // Modifier: Insert the token if the next token is not empty.
      // This overrides the above two settings.
      if (strpos($conditions, '=') !== FALSE && !empty($next_component)) {
        $parsed_pieces[$i] = $component;
      }

      // Modifier: Conditional insertion. Uses the previous token unless empty, otherwise insert this token.
      if (strpos($conditions, '|') !== FALSE) {
        if (empty($last_component)) {
          $parsed_pieces[$i] = $component;
        }
        else {
          unset($parsed_pieces[$i]);
        }
      }

    }
  }
  return str_replace('\\\\', "\t", implode('', $parsed_pieces));
}

function _name_format_add_component($string, &$modifiers = '', &$conditions = '') {
  $string = _name_format_apply_modifiers($string, $modifiers);
  $piece = array(
    'value' => $string,
    'conditions' => $conditions,
  );
  $conditions = '';
  $modifiers = '';
  return $piece;
}

function _name_format_apply_modifiers($string, $modifiers) {
  if (!is_null($string) || strlen($string)) {
    if ($modifiers) {
      $_pre = $string;
      for ($j = 0; $j < strlen($modifiers); $j++) {
        switch ($modifiers{$j}) {
          case 'L':
            $string = drupal_strtolower($string);
            break;
          case 'U':
            $string = drupal_strtoupper($string);
            break;
          case 'F':
            $string = drupal_ucfirst($string);
            break;
          case 'G':
            if (!empty($string)) {
              $parts = explode(' ', $string);
              $string = array();
              foreach ($parts as $part) {
                $string[] = drupal_ucfirst($part);
              }
              $string = implode(' ', $string);
            }
            break;
          case 'T':
            $string = trim($string);
            break;
          case 'S':
            $string = check_plain($string);
            break;
        }
      }
    }
  }
  return $string;
}

/**
 * Helper function to put out the first matched bracket position.
 *
 * Accepts strings in the format, ^ marks the matched bracket.
 *   '(xxx^)xxx(xxxx)xxxx' or '(xxx(xxx(xxxx))xxx^)'
 */
function _name_format_closing_bracket_position($string) {
  // Simplify the string by removing escaped brackets.
  $depth = 0;
  $string = str_replace(array('\(', '\)'), array('__', '__'), $string);
  for ($i = 0; $i < strlen($string); $i++) {
    $char = $string{$i};
    if ($char == '(') {
      $depth++;
    }
    elseif ($char == ')') {
      $depth--;
      if ($depth == 0) {
        return $i;
      }
    }
  }
  return FALSE;
}
