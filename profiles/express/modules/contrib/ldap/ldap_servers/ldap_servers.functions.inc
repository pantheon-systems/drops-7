<?php
// $Id: ldap_servers.functions.inc,v 1.3.2.1 2011/02/08 06:01:00 johnbarclay Exp $

/**
 * @file
 * collection of functions that don't belong in server object
 */

function ldap_server_module_load_include($type, $module, $name = NULL) {
  $result = module_load_include($type, $module, $name);
  if ($result === FALSE) {
    print "Failed to load file $name.$type in module $module";
    drupal_exit();
  }
}


/**
 * returns new user account if created, otherwise integer error message such
 * as LDAP_CREATE_ACCOUNT_ALREADY_EXISTS, LDAP_CREATE_ERROR
 *
 */
//  $account = ldap_create_drupal_account($authname, $accountname, $ldap_user['mail'], $ldap_user['dn'], $sid, array(), $status);

function ldap_create_drupal_account($authname, $accountname, $mail, $dn, $sid, $status, $edit) {

  $edit['name'] = $accountname;
  $edit['pass'] = user_password(20);
  $edit['mail'] = $mail;
  $edit['init'] = $mail;
  $edit['status'] = $status;
  if (!isset($edit['signature'])) {
    $edit['signature'] = '';
  }

  // save 'init' data to know the origin of the ldap authentication provisioned account
  $edit['data']['ldap_authentication']['init'] = array(
    'sid'  => $sid,
    'dn'   => $dn,
    'mail' => $mail,
  );

  if (!$account = user_save( NULL, $edit)) {
    drupal_set_message(t('User account creation failed because of system problems.'), 'error');
    return FALSE;
  }
  else {
    user_set_authmaps($account, array('authname_ldap_authentication' => $authname));
  }
  return $account;
}


/**
  * Modify an LDAP Entry
  */
function ldap_user_modify($userdn, $attributes, $ldap_server) {
  $status = ldap_modify($ldap_server->connection, $userdn, $attributes);
  if (!$status) {
    watchdog(
      'ldap_servers',
      'Error: user_modify() failed to modify ldap entry w/ base DN "!dn" with values: !values',
      array('!dn' => $userdn, '!value' => var_export($attributes, TRUE)),
      WATCHDOG_ERROR
    );
  }

  return $status;
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
 *
 *  this attempts to find bad dns, but should only be used as warningswe
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
 * @param array $ldap_entry
 * @param string $text
 * @return string text with tokens replaced
 */

function ldap_server_token_replace($ldap_entry, $text) {
  $desired_tokens = ldap_server_tokens_needed_for_template($text);
  $tokens = ldap_server_tokenize_entry($ldap_entry, $desired_tokens, LDAP_SERVERS_TOKEN_PRE, LDAP_SERVERS_TOKEN_POST);
  $result = str_replace(array_keys($tokens), array_values($tokens), $text);
  return $result;
}


/**
 * Turn an ldap entry into a token array suitable for the t() function
 * @param ldap entry array $ldap_entry
 * @param string prefix token prefix such as !,%,[
 * @param string suffix token suffix such as ]
 * @param $token_keys either an array of key names such as array('cn', 'dn') or string 'all' to return all tokens.
 * @return token array suitable for t() functions of with lowercase keys as exemplified below


an ldap entry such as:

   'dn' => 'cn=jdoe,ou=campus accounts,ou=toledo campus,dc=ad,dc=myuniversity,dc=edu',
    'mail' => array( 0 => 'jdoe@myuniversity.edu', 'count' => 1),
    'sAMAccountName' => array( 0 => 'jdoe', 'count' => 1),

should return tokens such as:

    -- from dn attribute
    [cn] = jdoe
    [cn:0] = jdoe
    [cn:last] => jdoe
    [ou] = campus accounts
    [ou:0] = campus accounts
    [ou:1] = toledo campus
    [ou:last] = toledo campus
    [dc] = ad
    [dc:0] = ad
    [dc:1] = myuniversity
    [dc:2] = edu
    [dc:last] = edu

    -- from other attributes
    [mail] = jdoe@myuniversity.edu
    [mail:0] = jdoe@myuniversity.edu
    [mail:last] = jdoe@myuniversity.edu
    [samaccountname] = jdoe
    [samaccountname:0] = jdoe
    [samaccountname:last] = jdoe

 */
function ldap_server_tokenize_entry($ldap_entry, $token_keys = 'all', $pre = LDAP_SERVERS_TOKEN_PRE, $post = LDAP_SERVERS_TOKEN_POST) {

  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  $tokens = array();

  // 1. tokenize dn
  $dn_parts = ldap_explode_dn($ldap_entry['dn'], 0); // escapes attribute values, need to be unescaped later.
  unset($dn_parts['count']);
  $parts_count = array();
  $parts_last_value = array();
  foreach ($dn_parts as $pair) {
    list($attr_name, $attr_value) = explode('=', $pair);
    $attr_value = ldap_pear_unescape_dn_value($attr_value);
    if (!($attr_value = ldap_server_check_plain($attr_value, $attr_name))) {
      continue;
    }

    if (!isset($parts_count[$attr_name])) {
      $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . $post] = $attr_value;
      $parts_count[$attr_name] = 0;
    }
    $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . (int)$parts_count[$attr_name] . $post] = $attr_value;

    $parts_last_value[$attr_name] = $attr_value;
    $parts_count[$attr_name]++;
  }
  foreach ($parts_count as $attr_name => $count) {
    $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . 'last' . $post] = $parts_last_value[$attr_name];
  }

  // tokenize other attributes
  if ($token_keys == 'all') {
    $token_keys = array_keys($ldap_entry);
    $token_keys = array_filter($token_keys, "is_string");
    foreach ($token_keys as $attr_name) {
      $attr_value = $ldap_entry[$attr_name];
      if (is_array($attr_value) && is_scalar($attr_value[0]) && $attr_value['count'] == 1) {
        if ($value = ldap_server_check_plain($attr_value[0], $attr_name)) {
          $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . $post] = $value;
          $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . '0' . $post] = $value;
          $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . 'last' . $post] = $value;
        }       
      }
      elseif (is_array($attr_value) && $attr_value['count'] > 1) {
        if ($value = ldap_server_check_plain($attr_value[$attr_value['count']-1], $attr_name)) {
          $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . 'last' . $post] = $value;
        }
        for ($i=0; $i<$attr_value['count']; $i++) {
          if ($value = ldap_server_check_plain($attr_value[$i], $attr_name)) {
            $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . $i . $post] = $value;
          }
        }
      }
      elseif (is_scalar($attr_value)) {
        if ($value = ldap_server_check_plain($attr_value, $attr_name)) {
          $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . $post] = $value;
        }
      }
    }
  }
  else {
    foreach ($token_keys as $token_key) {
      $parts = explode(LDAP_SERVERS_TOKEN_DEL, $token_key);
      $last_key = $parts[count($parts) -1];
      if ($last_key == 'last') {
        $attr_name = join(LDAP_SERVERS_TOKEN_DEL, array_pop($parts));
        $count =  $ldap_entry[$attr_name]['count'];
        $value = $ldap_entry[$attr_name][$count-1];
      }
      elseif (is_numeric($last_key) || $last_key == '0') {
        $discard = array_pop($parts);
        $attr_name = join(LDAP_SERVERS_TOKEN_DEL, $parts);
        $value = $ldap_entry[$attr_name][(int)$last_key];
      }
      elseif (empty($ldap_entry[$token_key])) {
          continue;
      }
      else {
        $value = $ldap_entry[$token_key][0];
      }
      $value = ldap_server_check_plain($value, $token_key);
      if ($value === FALSE) {
         continue; // don't tokenize data that can't pass check_plain
      }
      $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . $post] = $value;
    }
  }

  // include the dn.  it will not be handled correctly by previous loops
  $tokens[$pre . 'dn' . $post] = ldap_server_check_plain($ldap_entry['dn']);

  return $tokens;
}


function ldap_server_check_plain($text, $attr_name = 'attribute', $detailed_watchdog_log = FALSE) {
  
  try {
    $text = @check_plain($text);
  }
  catch (Exception $e) {
    if ($detailed_watchdog_log) {
      $watchdog_tokens = array('%attr_name' => $attr_name);
      watchdog('ldap_servers', 'skipped tokenization of attribute %attr_name because the value would not pass check_plain function.', $watchdog_tokens, WATCHDOG_DEBUG);
    }
    $text = FALSE;
  }
  return $text;
}
/**
 * @param string $template in form [cn]@myuniversity.edu
 * @return array of all tokens in the template such as array('cn')
 */
function ldap_server_tokens_needed_for_template($template, $pre = LDAP_SERVERS_TOKEN_PRE, $post = LDAP_SERVERS_TOKEN_POST) {
  preg_match_all('/
    \[             # [ - pattern start
    ([^\[\]]*)  # match $type not containing whitespace : [ or ]
    \]             # ] - pattern end
    /x', $template, $matches);

  return @$matches[1];

}

function ldap_servers_show_sample_user_tokens($sid) {

  $ldap_server = ldap_servers_get_servers($sid, 'all', TRUE);
  $test_username = $ldap_server->testingDrupalUsername;
  if (!$test_username || ! (
     $ldap_server->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT ||
     $ldap_server->bind_method == LDAP_SERVERS_BIND_METHOD_ANON
    )
   ) {
    return FALSE;
  }

  if ($ldap_user = $ldap_server->user_lookup($test_username)) {
    $table = theme('ldap_server_ldap_entry_table', array(
      'entry' => $ldap_user['attr'],
      'username' => $test_username,
      ));
  }
  else {
    $table = '<p>' . t('No sample user data found') . '</p>';
  }

  return $table;
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
 * ldap_server_massage_text($value, 'attr_value', LDAP_SERVER_MASSAGE_QUERY_ARRAY)
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
        else {
          foreach ($value as $i => $val) {
            $value[$i] = drupal_strtolower($val);
          }
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
        if (!is_array($values)) {
            $values = array($values);
        }

        foreach ($values as $key => $val) {
            // Escaping of filter meta characters
            $val = str_replace('\\', '\5c', $val);
            $val = str_replace('*',  '\2a', $val);
            $val = str_replace('(',  '\28', $val);
            $val = str_replace(')',  '\29', $val);

            // ASCII < 32 escaping
            $val = ldap_pear_asc2hex32($val);

            if (null === $val) $val = '\0';  // apply escaped "null" if string is empty

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
                    $val = '\20'.$val;
                }
                for ($i = 0; $i < strlen($matches[3]); $i++) {
                    $val = $val.'\20';
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
        $string = preg_replace("/\\\([0-9A-Fa-f]{2})/e", "''.chr(hexdec('\\1')).''", $string);
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
                if (strlen($hex) == 1) $hex = '0'.$hex;
                $string = str_replace($char, '\\'.$hex, $string);
            }
        }
        return $string;
    }
