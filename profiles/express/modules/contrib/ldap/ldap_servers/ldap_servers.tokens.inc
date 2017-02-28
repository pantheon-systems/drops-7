<?php

/**
 * @file
 * collection of functions related to ldap tokens
 */

/**
 * @param string $attr_name such 'field_user_lname', 'name', 'mail', 'dn'
 * @param string $attr_type such as 'field', 'property', etc.  NULL for ldap attributes
 * @param string $attr_ordinal 0, 1, 2, etc.  not used in general
 *
 * @return string such as 'field.field_user_lname', 'samaccountname', etc.
 */
function ldap_servers_make_token($attr_name, $attr_type = NULL, $ordinal = NULL) {
  $inner_token = $attr_name;
  if ($attr_type) {
    $inner_token .= '.' . $attr_type;
  }
  if ($ordinal) {
    $inner_token .= ':' . $ordinal;
  }
  $token = LDAP_SERVERS_TOKEN_PRE . $inner_token . LDAP_SERVERS_TOKEN_POST;
  return $token;
}

/**
 * @param $user_attr_key of form <attr_type>.<attr_name>[:<instance>]
 *   such as field.lname, property.mail, field.aliases:2
 *
 * @return array array($attr_type, $attr_name, $attr_ordinal) such as array('field','field_user_lname', NULL)
 */
function ldap_servers_parse_user_attr_name($user_attr_key) {
  $user_attr_key = trim($user_attr_key, LDAP_SERVERS_TOKEN_PRE . LDAP_SERVERS_TOKEN_POST); // make sure no [] are on attribute
  $parts = explode('.', $user_attr_key);
  $attr_type = $parts[0];
  $attr_name = (isset($parts[1])) ? $parts[1] : FALSE;
  $attr_ordinal = FALSE;

  if ($attr_name) {
    $attr_name_parts = explode(':', $attr_name);
    if (isset($attr_name_parts[1])) {
      $attr_name = $attr_name_parts[0];
      $attr_ordinal = $attr_name_parts[1];
    }
  }
  return array($attr_type, $attr_name, $attr_ordinal);
}

/**
 * @param array $ldap_entry
 * @param string $text such as "[dn]", "[cn]@my.org", "[displayName] [sn]", "Drupal Provisioned"
 * @return string $text with tokens replaced or NULL if replacement not available
 */

function ldap_servers_token_replace($resource, $text, $resource_type = 'ldap_entry') { // user_account

  $desired_tokens = ldap_servers_token_tokens_needed_for_template($text);  // desired tokens are of form "cn","mail", etc.

  if (empty($desired_tokens)) {
    return $text; // if no tokens exist in text, return text itself.  It is literal value
  }

  switch ($resource_type) {
    case 'ldap_entry':
    $tokens = ldap_servers_token_tokenize_entry($resource, $desired_tokens, LDAP_SERVERS_TOKEN_PRE, LDAP_SERVERS_TOKEN_POST);
    break;

    case 'user_account':
    $tokens = ldap_servers_token_tokenize_user_account($resource, $desired_tokens, LDAP_SERVERS_TOKEN_PRE, LDAP_SERVERS_TOKEN_POST);
    break;
  }

  // add lowercase tokens to avoid case sensitivity
  foreach ($tokens as $attribute => $value) {
    $tokens[drupal_strtolower($attribute)] = $value;
  }

  // If $text is not present as an attribute key, insert it and set the key's value to an empty string.
  if (!array_key_exists(drupal_strtolower($text), $tokens)) {
    $tokens[$text] = '';
    $tokens[drupal_strtolower($text)] = '';
  }

  $attributes = array_keys($tokens); //array of attributes (sn, givenname, etc)
  $values = array_values($tokens); //array of attribute values (Lincoln, Abe, etc)
  $result = str_replace($attributes, $values, $text);

  $result = preg_replace('/\[[^\]]*]/', '', $result);  // strip out any unreplaced tokens
  return ($result == '') ? NULL : $result; // return NULL if $result is empty, else $result
}

/**
 * @param array $attributes array of attributes passed by reference
 * @param string $text with tokens in it
 *
 * by reference return add ldap attribute triplet $attribute_maps[<attr_name>] = (<attr_name>, <ordinal>, <data_type>) to $attributes
 */
function ldap_servers_token_extract_attributes(&$attribute_maps,  $text) {
  $tokens = ldap_servers_token_tokens_needed_for_template($text);
  foreach ($tokens as $token) {
    $token = str_replace(array(LDAP_SERVERS_TOKEN_PRE, LDAP_SERVERS_TOKEN_POST), array('', ''), $token);
    $parts = explode(LDAP_SERVERS_TOKEN_DEL, $token);
    $ordinal = (isset($parts[1]) && $parts[1]) ? $parts[1] : 0;
    $attr_name = $parts[0];
    $source_data_type = NULL;

    $parts2 = explode(LDAP_SERVERS_TOKEN_MODIFIER_DEL, $attr_name);
    if (count($parts2) > 1) {
      $attr_name = $parts2[0];
      $conversion = $parts2[1];
    }
    else {
      $conversion = NULL;
    }
    $attribute_maps[$attr_name] = ldap_servers_set_attribute_map(@$attribute_maps[$attr_name], $conversion, array($ordinal => NULL));
  }
}

/**
 * @param string $token or token expression with singular token in it, eg. [dn], [dn;binary], [titles:0;binary] [cn]@mycompany.com
 *
 *
 *
 * @return array(<attr_name>, <ordinal>, <conversion>)
 */
function ldap_servers_token_extract_parts($token) {
  $attributes = array();
  ldap_servers_token_extract_attributes($attributes, $token);
  if (is_array($attributes)) {
    $keys = array_keys($attributes);
    $attr_name = $keys[0];
    $attr_data = $attributes[$attr_name];
    $ordinals = array_keys($attr_data['values']);
    $ordinal = $ordinals[0];
    return array($attr_name, $ordinal, $attr_data['conversion']);
  }
  else {
    return array(NULL, NULL, NULL);
  }

}



/**
 * Turn an ldap entry into a token array suitable for the t() function
 * @param ldap entry array $ldap_entry
 * @param string prefix token prefix such as !,%,[
 * @param string suffix token suffix such as ]
 * @param $token_keys either an array of key names such as array('cn', 'dn') or string 'all' to return all tokens.
 * @return token array suitable for t() functions of with lowercase keys as exemplified below


$ldap_entry should be in form of single entry returned from ldap_search() function:

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

    [guid:0;base64_encode] = apply base64_encode() function to value
    [guid:0;bin2hex] = apply bin2hex() function to value
    [guid:0;msguid] = apply ldap_servers_msguid() function to value
    [guid:0;binary] = apply ldap_servers_binary() function to value. this is the most generic binary function

 */

function ldap_servers_token_tokenize_entry($ldap_entry, $token_keys = 'all', $pre = LDAP_SERVERS_TOKEN_PRE, $post = LDAP_SERVERS_TOKEN_POST) {

  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  $tokens = array();
  $watchdog_tokens = array();
  if (function_exists('debug_backtrace') && $backtrace = debug_backtrace()) {
    $watchdog_tokens['%calling_function'] = $backtrace[1]['function'];
  }
  if (!is_array($ldap_entry)) {
    if ($detailed_watchdog_log) {
      watchdog('ldap_servers', 'skipped tokenization of ldap entry because no ldap entry provided when called from %calling_function.', $watchdog_tokens, WATCHDOG_DEBUG);
    }
    return $tokens; // empty array
  }
      
  // add lowercase keyed entries to ldap array
  foreach ($ldap_entry as $key => $values) {
    $ldap_entry[drupal_strtolower($key)] = $values;
  }

  // 1. tokenize dn
  $dn_parts = ldap_explode_dn($ldap_entry['dn'], 0); // escapes attribute values, need to be unescaped later.
  unset($dn_parts['count']);
  $parts_count = array();
  $parts_last_value = array();
  foreach ($dn_parts as $pair) {
    list($attr_name, $attr_value) = explode('=', $pair);
    $attr_value = ldap_pear_unescape_dn_value($attr_value);
    try {
      $attr_value = check_plain($attr_value);
    }
    catch (Exception $e) {
      if ($detailed_watchdog_log) {
        $watchdog_tokens['%attr_name'] = $attr_name;
        watchdog('ldap_servers', 'skipped tokenization of attribute %attr_name because the value would not pass check_plain function.', $watchdog_tokens, WATCHDOG_DEBUG);
      }
      continue; // don't tokenize data that can't pass check_plain
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
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . $post] = check_plain($attr_value[0]);
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . '0' . $post] = check_plain($attr_value[0]);
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . 'last' . $post] = check_plain($attr_value[0]);
      }
      elseif (is_array($attr_value) && $attr_value['count'] > 1) {
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . 'last' . $post] = check_plain($attr_value[$attr_value['count']-1]);
        for ($i=0; $i<$attr_value['count']; $i++) {
          $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . $i . $post] = check_plain($attr_value[$i]);
        }
      }
      elseif (is_scalar($attr_value)) {
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . $post] = check_plain($attr_value);
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . '0' . $post] = check_plain($attr_value);
        $tokens[$pre . ldap_server_massage_text($attr_name, 'attr_name', LDAP_SERVER_MASSAGE_TOKEN_REPLACE) . LDAP_SERVERS_TOKEN_DEL . 'last' . $post] = check_plain($attr_value);
      }
    }
  }
  else {
    foreach ($token_keys as $full_token_key) {
      // $token_key = 'dn', 'mail', 'mail:0', 'mail:last', 'dept:1', 'guid:0;tobase64etc.
      $value = NULL;

      $conversion = FALSE;
      $parts = explode(';', $full_token_key);
      if (count($parts) == 2) {
        $conversion = $parts[1];
        $token_key = $parts[0];
      }
      else {
        $token_key = $full_token_key;
      }

      $parts = explode(LDAP_SERVERS_TOKEN_DEL, $token_key);
      $attr_name = drupal_strtolower($parts[0]);
      $ordinal_key = isset($parts[1]) ? $parts[1] : 0;
      $i = NULL;

      if ($attr_name == 'dn' || !isset($ldap_entry[$attr_name])) { // don't use empty() since a 0, "", etc value may be a desired value
        continue;
      }
      else {
        $count = $ldap_entry[$attr_name]['count'];
        if ($ordinal_key === 'last') {
          $i = ($count > 0) ? $count - 1 : 0;
          $value = $ldap_entry[$attr_name][$i];
        }
        elseif (is_numeric($ordinal_key) || $ordinal_key == '0') {
          $value = $ldap_entry[$attr_name][$ordinal_key];
        }
        else {
          continue;  // don't add token if case not covered
        }
      }

      if ($conversion) {
        switch ($conversion) {

          case 'base64_encode':
            $value = base64_encode($value);
            break;

          case 'bin2hex':
            $value = bin2hex($value);
            break;

          case 'msguid':
            $value = ldap_servers_msguid($value);
            break;

          case 'binary':
            $value = ldap_servers_binary($value);
            break;
        }
      }


      $tokens[$pre . $full_token_key . $post] = $value;
      if ($full_token_key != drupal_strtolower($full_token_key)) {
        $tokens[$pre . drupal_strtolower($full_token_key) . $post] = $value;
      }
    }
  }

  // include the dn.  it will not be handled correctly by previous loops
  $tokens[$pre . 'dn' . $post] = check_plain($ldap_entry['dn']);
  return $tokens;
}

/**
 *
 * @param drupal user object $user_account
 * @param array or 'all' $token_keys 'all' signifies return
 *   all token/value pairs available; otherwise array lists
 *   token keys (e.g. property.name ...NOT [property.name])
 * @param string $pre prefix of token
 * @param string $post suffix of token
 *
 *
 * @return should return token/value pairs in array such as
 *   'status' => 1
 *   'uid' => 17
 */

function ldap_servers_token_tokenize_user_account($user_account, $token_keys = 'all', $pre = LDAP_SERVERS_TOKEN_PRE, $post = LDAP_SERVERS_TOKEN_POST) {

  $detailed_watchdog_log = variable_get('ldap_help_watchdog_detail', 0);
  $tokens = array();

  $user_entered_password_available = (boolean)ldap_user_ldap_provision_pwd('get');
  // ldapUserPwd((property_exists($user_account, 'ldapUserPwd') && $user_account->ldapUserPwd));

  if ($token_keys == 'all') {
    // add lowercase keyed entries to ldap array
    foreach ((array)$user_account as $property_name => $value) {
      if (is_scalar($value) && $property_name != 'password') {
        $token_keys[] = 'property.' . $property_name;
        if (drupal_strtolower($property_name) != $property_name) {
          $token_keys[] = 'property.' . drupal_strtolower($property_name);
        }
      }
      elseif (isset($user_account->{$attr_name}[LANGUAGE_NONE][0]['value']) && is_scalar($user_account->{$attr_name}[LANGUAGE_NONE][0]['value'])) {
        $token_keys[] = 'field.' . $property_name;
        if (drupal_strtolower($property_name) != $property_name) {
          $token_keys[] = 'field.' . drupal_strtolower($property_name);
        }
      }
      else {
        // field or property with no value, so no token can be generated
      }
    }
    $ldap_user_conf_admin = new LdapUserConfAdmin();
    if ($ldap_user_conf->setsLdapPassword) {
      $token_keys[] = 'password.random';
      $token_keys[] = 'password.user-random';
    }
  }

  foreach ($token_keys as $token_key) {
    $parts = explode('.', $token_key);
    $attr_type = $parts[0];
    $attr_name = $parts[1];
    $attr_conversion = (isset($parts[2])) ? $parts[2] : 'none';
    $value = FALSE;
    $skip = FALSE;

    switch ($attr_type) {
      case 'field':
        $value = @is_scalar($user_account->{$attr_name}[LANGUAGE_NONE][0]['value']) ? $user_account->{$attr_name}[LANGUAGE_NONE][0]['value'] : '';
      break;

      case 'property':
        $value = @is_scalar($user_account->{$attr_name}) ? $user_account->{$attr_name} : '';
      break;

      case 'password':

        switch ($attr_name) {

          case 'user':
          case 'user-only':
            $pwd = ldap_user_ldap_provision_pwd('get');
            $value = ($pwd) ? $pwd : NULL;
            break;

          case 'user-random':
            $pwd = ldap_user_ldap_provision_pwd('get');
            $value = ($pwd) ? $pwd : user_password();
            break;

          case 'random':
            $value = user_password();
            break;

        }
        if (empty($value)) {
          $skip = TRUE;
        }
      break;
    }

    if (!$skip) {

      switch ($attr_conversion) {

        case 'none':
          break;

        case 'to-md5':
          $value = "{MD5}" . base64_encode( pack( "H*", md5( $value ) ) );
          break;

        case 'to-lowercase':
          $value = drupal_strtolower($value);
          break;
      }

      $tokens[$pre . $token_key . $post] = ($attr_type == 'password') ? $value : check_plain($value);
      if ($token_key != drupal_strtolower($token_key)) {
        $tokens[$pre . drupal_strtolower($token_key) . $post] = ($attr_type == 'password') ? $value : check_plain($value);
      }
    }
  }
  return $tokens;
}

/**
 * @param string $template in form [cn]@myuniversity.edu
 * @return array of all tokens in the template such as array('cn')
 */
function ldap_servers_token_tokens_needed_for_template($template, $pre = LDAP_SERVERS_TOKEN_PRE, $post = LDAP_SERVERS_TOKEN_POST) {
  preg_match_all('/
    \[             # [ - pattern start
    ([^\[\]]*)  # match $type not containing whitespace : [ or ]
    \]             # ] - pattern end
    /x', $template, $matches);

  return @$matches[1];

}

function ldap_servers_token_show_sample_user_tokens($sid) {

  $ldap_server = ldap_servers_get_servers($sid, 'all', TRUE);
  $test_username = $ldap_server->testingDrupalUsername;
  if (!$test_username || ! (
    $ldap_server->bind_method == LDAP_SERVERS_BIND_METHOD_SERVICE_ACCT ||
    $ldap_server->bind_method == LDAP_SERVERS_BIND_METHOD_ANON
      )
    ) {
    return FALSE;
  }

  if ($ldap_user = $ldap_server->userUserNameToExistingLdapEntry($test_username)) {
    $table = theme('ldap_server_ldap_entry_table', array(
      'entry' => $ldap_user['attr'],
      'username' => $test_username,
      'dn' => $ldap_user['dn'],
      ));
  }
  else {
    $table = '<p>' . t('No sample user data found') . '</p>';
  }

  return $table;
}
