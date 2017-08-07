<?php

/**
 * @file
 * collection of functions that don't belong in server object
 */

/**
  * Modify an LDAP Entry
  *
  * this has been depracated in favor or $ldap_server->modifyLdapEntry($dn, $attributes)
  * which handles empty attributes better.
  *
  */
function ldap_user_modify($dn, $attributes, $ldap_server) {
  $result = ldap_modify($ldap_server->connection, $dn, $attributes);
  if (!$result) {
    $error = "LDAP Server ldap_modify(%dn) in ldap_user_modify() Error Server ID = %sid, LDAP Err No: %ldap_errno LDAP Err Message: %ldap_err2str ";
    $tokens = array('%dn' => $dn, '%sid' => $this->sid, '%ldap_errno' => ldap_errno($this->connection), '%ldap_err2str' => ldap_err2str(ldap_errno($this->connection)));
    watchdog('ldap_servers', $error, $tokens, WATCHDOG_ERROR);
  }
  return $result;
}

/**
 * Modify a password
 */
function ldap_password_modify($userdn, $new_password, $ldap_server) {

  $new_password = "\"" . $new_password . "\"";
  $len = drupal_strlen($new_password);
  $new_pass = NULL;
  for ($i = 0; $i < $len; $i++) {
    $new_pass .= "{$new_password{$i}}\000";
  }

  $status = ldap_mod_replace($ldap_server->connection, $userdn, array('unicodePwd' => $new_pass));
  if (!$status) {
    watchdog(
      'ldap_servers',
      'Error: password_modify() failed to modify ldap password w/ base DN "!dn"',
      array('!dn' => $userdn),
      WATCHDOG_ERROR
    );
  }

  return $status;
}

/**
 * Converts a password to the format that Active Directory supports (for the 
 * purpose of changing or setting).  Note that AD needs the field to be called
 * unicodePwd (as opposed to userPassword)
 * 
 * @param string $password
 *    The password that is being formatted for Active Directory unicodePwd field
 * @return string
 *    $password surrounded with quotes and in UTF-16LE encoding
 */
function ldap_servers_convert_password_for_active_directory_unicodePwd($password) {
  // This function can be called with $attributes['unicodePwd'] as an array.
  if (!is_array($password)) {
    return mb_convert_encoding("\"{$password}\"", "UTF-16LE");
  }
  else {
    // Presumably there is no use case for there being more than one password in
    // the $attributes array, hence it will be at index 0 and we return in kind.
    return array(mb_convert_encoding("\"{$password[0]}\"", "UTF-16LE"));
  }
}

/**
 *
 *  this attempts to find bad dns, but should only be used as warnings
 *  as the ldap spec allows for any old character to be escaped and ldap
 *  implementations may not follow the spec.
 *
 *  http://www.ietf.org/rfc/rfc2253.txt
 *
 */
function ldap_baddn($dn, $dn_name) {
  $result = array();
  $valid_attr_name = '[_a-zA-Z\d\s]';
  $valid_attr_values = '[_\-a-zA-Z\d\s]';
  $regex = '/^(' . $valid_attr_name . '*\=' . $valid_attr_values . '*[,]{1})*(' . $valid_attr_name . '*\=' . $valid_attr_values . '*){1}$/';
  $match = (preg_match($regex, $dn)) ? TRUE : FALSE;
  $result['boolean'] = $match;
  if (!$match) {
    $tokens = array('%dn' => htmlspecialchars($dn), '%dn_name' => $dn_name);
    $result['text'] = t('Possible invalid format for:', $tokens)
    . '<em>' . $tokens['%dn'] . '</em>.<br/>  ' .
    t('The format may be correct for your ldap, but please double check.', $tokens);
  }
  return $result;
}

/**
 *
 *  this attempts to find bad dns, but should only be used as warningswe
 *  as the ldap spec allows for any old character to be escaped and ldap
 *  implementations may not follow the spec.
 *
 *  http://www.ietf.org/rfc/rfc2253.txt
 *
 */

function ldap_badattr($attr, $attr_name) {
  $result = array();
  $valid_attr_name = '[_a-zA-Z\d\s]';
  $regex = '/^(' . $valid_attr_name . '){1,}$/';
  $match = (preg_match($regex, $attr)) ? TRUE : FALSE;
  $result['boolean'] = $match;
  if (!$match) {
    $tokens = array('%attr' => htmlspecialchars($attr), '%attr_name' => $attr_name);
    $result['text'] = t('Possible invalid format for %attr_name:', $tokens) . ' <code><em>' . $tokens['%attr']
      . '</em></code><br/>' . t('The format may be correct for your ldap, but please double check.', $tokens);
  }
  return $result;
}

/**
 * get array of required attributes for an ldap query

 * @param string $sid server id or ldap server object being used
 * @param string $direction LDAP_USER_PROV_DIRECTION_* constant
 * @param string $ldap_context
 * @param boolean $reset cache
 *
 */
function ldap_servers_attributes_needed($sid, $ldap_context = 'all', $reset = TRUE) {

  static $attributes;
  $sid = is_object($sid) ? $sid->sid : $sid;
  $static_cache_id = ($sid) ? $ldap_context . '__' . $sid : $ldap_context;
  if (!is_array($attributes)) {
    $attributes = array();
  }
  if (!isset($attributes[$static_cache_id]) || $reset) {
    $params = array(
      'sid' => $sid,
      'ldap_context' => $ldap_context,
    );
    drupal_alter('ldap_attributes_needed', $attributes[$static_cache_id], $params);
  }
  $return = $attributes[$static_cache_id];

  return $return;
}


/**
 * function to massage (change case, escape, unescape) ldap attribute names
 * and values.  The primary purpose of this is to articulate and ensure consistency
 * across ldap modules.
 *
 * @param mixed $value to be massaged
 * @param enum $value_type = 'attr_name' or 'attr_value;
 * @param enum $context...see LDAP_SERVER_MASSAGE_* constants
 *
 * .e.g. ldap_server_massage_text($value, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_LDAP)
 */
function ldap_server_massage_text($value, $value_type, $context) {

  $scalar = is_scalar($value);

  if ($value_type == 'attr_value') {

    if ($context == LDAP_SERVER_MASSAGE_QUERY_LDAP) {
      $value = ldap_pear_escape_filter_value($value);
    }
    elseif ($context == LDAP_SERVER_MASSAGE_STORE_LDAP) {
      $value = ldap_pear_escape_dn_value($value);
    }

    switch ($context) {

      case LDAP_SERVER_MASSAGE_DISPLAY:
      case LDAP_SERVER_MASSAGE_TOKEN_REPLACE:

      case LDAP_SERVER_MASSAGE_QUERY_LDAP:
      case LDAP_SERVER_MASSAGE_QUERY_DB:
      case LDAP_SERVER_MASSAGE_QUERY_ARRAY:
      case LDAP_SERVER_MASSAGE_QUERY_PROPERTY:

      case LDAP_SERVER_MASSAGE_STORE_LDAP:
      case LDAP_SERVER_MASSAGE_STORE_DB:
      case LDAP_SERVER_MASSAGE_STORE_ARRAY:
      case LDAP_SERVER_MASSAGE_STORE_PROPERTY:

      break;

    }
  }
  elseif ($value_type == 'attr_name') { // attr_name
    switch ($context) {

      case LDAP_SERVER_MASSAGE_DISPLAY:
      break;

      case LDAP_SERVER_MASSAGE_TOKEN_REPLACE:

      case LDAP_SERVER_MASSAGE_QUERY_LDAP:
      case LDAP_SERVER_MASSAGE_QUERY_DB:
      case LDAP_SERVER_MASSAGE_QUERY_ARRAY:
      case LDAP_SERVER_MASSAGE_QUERY_PROPERTY:

      case LDAP_SERVER_MASSAGE_STORE_LDAP:
      case LDAP_SERVER_MASSAGE_STORE_DB:
      case LDAP_SERVER_MASSAGE_STORE_ARRAY:
      case LDAP_SERVER_MASSAGE_STORE_PROPERTY:
        if ($scalar) {
          $value = drupal_strtolower($value);
        }
        elseif (is_array($value)) {
          foreach ($value as $i => $val) {
            $value[$i] = drupal_strtolower($val);
          }
        }
        else {
         // neither scalar nor array $value
        }
      break;

    }
  }

  return $value;

}

 /**
    * from pear net_ldap2-2.0.11
    *
    * Escapes the given VALUES according to RFC 2254 so that they can be safely used in LDAP filters.
    *
    * Any control characters with an ACII code < 32 as well as the characters with special meaning in
    * LDAP filters "*", "(", ")", and "\" (the backslash) are converted into the representation of a
    * backslash followed by two hex digits representing the hexadecimal value of the character.
    *
    * @param array $values Array of values to escape
    *
    * @static
    * @return array Array $values, but escaped
    */
    function ldap_pear_escape_filter_value($values = array()) {
        // Parameter validation
        $is_scalar = is_scalar($values);
        if ($is_scalar) {
            $values = array($values);
        }
        if ($values === NULL) {
          return NULL;
        }

       foreach ($values as $key => $val) {
            // Escaping of filter meta characters
            $val = str_replace('\\', '\5c', $val);
            $val = str_replace('*',  '\2a', $val);
            $val = str_replace('(',  '\28', $val);
            $val = str_replace(')',  '\29', $val);

            // ASCII < 32 escaping
            $val = ldap_pear_asc2hex32($val);

            if (null === $val) {
              $val = '\0';
            }  // apply escaped "null" if string is empty

            $values[$key] = $val;
       }

        return ($is_scalar) ? $values[0] : $values;
    }

    /**
    * Undoes the conversion done by {@link escape_filter_value()}.
    *
    * Converts any sequences of a backslash followed by two hex digits into the corresponding character.
    *
    * @param array $values Array of values to escape
    *
    * @static
    * @return array Array $values, but unescaped
    */
    function ldap_pear_unescape_filter_value($values = array()) {
        // Parameter validation
        $is_scalar = is_scalar($values);
        if (!is_array($values)) {
            $values = array($values);
        }

        foreach ($values as $key => $value) {
            // Translate hex code into ascii
            $values[$key] = ldap_pear_hex2asc($value);
        }

        return ($is_scalar) ? $values[0] : $values;
    }


 /**
    * Escapes a DN value according to RFC 2253
    *
    * Escapes the given VALUES according to RFC 2253 so that they can be safely used in LDAP DNs.
    * The characters ",", "+", """, "\", "<", ">", ";", "#", "=" with a special meaning in RFC 2252
    * are preceeded by ba backslash. Control characters with an ASCII code < 32 are represented as \hexpair.
    * Finally all leading and trailing spaces are converted to sequences of \20.
    *
    * @param array $values An array containing the DN values that should be escaped
    *
    * @static
    * @return array The array $values, but escaped
    */
    function ldap_pear_escape_dn_value($values = array()) {
        // Parameter validation
        $is_scalar = is_scalar($values);

        if (!is_array($values)) {
            $values = array($values);
        }

        foreach ($values as $key => $val) {
            // Escaping of filter meta characters
            $val = str_replace('\\', '\\\\', $val);
            $val = str_replace(',',    '\,', $val);
            $val = str_replace('+',    '\+', $val);
            $val = str_replace('"',    '\"', $val);
            $val = str_replace('<',    '\<', $val);
            $val = str_replace('>',    '\>', $val);
            $val = str_replace(';',    '\;', $val);
            $val = str_replace('#',    '\#', $val);
            $val = str_replace('=',    '\=', $val);

            // ASCII < 32 escaping
            $val = ldap_pear_asc2hex32($val);

            // Convert all leading and trailing spaces to sequences of \20.
            if (preg_match('/^(\s*)(.+?)(\s*)$/', $val, $matches)) {
                $val = $matches[2];
                for ($i = 0; $i < strlen($matches[1]); $i++) {
                    $val = '\20' . $val;
                }
                for ($i = 0; $i < strlen($matches[3]); $i++) {
                    $val = $val . '\20';
                }
            }

            if (null === $val) $val = '\0';  // apply escaped "null" if string is empty

            $values[$key] = $val;
        }

        return ($is_scalar) ? $values[0] : $values;
    }

    /**
    * Undoes the conversion done by escape_dn_value().
    *
    * Any escape sequence starting with a baskslash - hexpair or special character -
    * will be transformed back to the corresponding character.
    *
    * @param array $values Array of DN Values
    *
    * @return array Same as $values, but unescaped
    * @static
    */
    function ldap_pear_unescape_dn_value($values = array()) {
      $is_scalar = is_scalar($values);

        // Parameter validation
        if (!is_array($values)) {
            $values = array($values);
        }

        foreach ($values as $key => $val) {
            // strip slashes from special chars
            $val = str_replace('\\\\', '\\', $val);
            $val = str_replace('\,',    ',', $val);
            $val = str_replace('\+',    '+', $val);
            $val = str_replace('\"',    '"', $val);
            $val = str_replace('\<',    '<', $val);
            $val = str_replace('\>',    '>', $val);
            $val = str_replace('\;',    ';', $val);
            $val = str_replace('\#',    '#', $val);
            $val = str_replace('\=',    '=', $val);

            // Translate hex code into ascii
            $values[$key] = ldap_pear_hex2asc($val);
        }

      return ($is_scalar) ? $values[0] : $values;
    }



    /**
    * Converts all Hex expressions ("\HEX") to their original ASCII characters
    *
    * @param string $string String to convert
    *
    * @static
    * @author beni@php.net, heavily based on work from DavidSmith@byu.net
    * @return string
    */

    function ldap_pear_hex2asc($string) {

      if (version_compare(PHP_VERSION, '5.3.0') >= 0) {
        $string = preg_replace_callback(
          "/\\\([0-9A-Fa-f]{2})/",
          function ($m){
            return chr(hexdec($m[1]));
          },
          $string);
      }
      else {
        $string = preg_replace("/\\\([0-9A-Fa-f]{2})/e", "''.chr(hexdec('\\1')).''", $string);
      }

      return $string;
    }

    /**
    * Converts all ASCII chars < 32 to "\HEX"
    *
    * @param string $string String to convert
    *
    * @static
    * @return string
    */
    function ldap_pear_asc2hex32($string) {
        for ($i = 0; $i < strlen($string); $i++) {
            $char = substr($string, $i, 1);
            if (ord($char) < 32) {
                $hex = dechex(ord($char));
                if (strlen($hex) == 1) $hex = '0' . $hex;
                $string = str_replace($char, '\\' . $hex, $string);
            }
        }
        return $string;
    }
