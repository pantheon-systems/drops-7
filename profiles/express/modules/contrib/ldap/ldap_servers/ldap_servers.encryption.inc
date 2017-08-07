<?php

/**
 * @file
 * Provides functions for encryption/decryption.
 * http://stackoverflow.com/questions/2448256/php-mcrypt-encrypting-decrypting-file
 */

define('LDAP_SERVERS_MODE', 'CTR');
/**
 * Return a random salt of a given length for crypt-style passwords
 *
 * @param int length
 *   The requested length.
 *
 * @return string
 *   A (fairly) random salt of the requested length.
 *
 */
function ldap_servers_random_salt( $length ) {
  $possible = '0123456789' . 'abcdefghijklmnopqrstuvwxyz' . 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' . './';
  $salt = "";

  mt_srand((double)microtime() * 1000000);
  while ( strlen( $salt ) < $length ) {
    $salt .= substr( $possible, ( rand() % strlen( $possible ) ), 1 );
  }

  return $salt;
}


function _ldap_servers_encrypt_types($type = 'all') {

  $hashes = array();
  $encrypts = array();
  if (extension_loaded('mcrypt')) {  // only support with extension

    /**
      LDAP_SERVERS_ENC_TYPE_MD5C => 'MD5 Crypt',
      LDAP_SERVERS_ENC_TYPE_SALTED_MD5 => 'Salted MD5',
      LDAP_SERVERS_ENC_TYPE_SHA => 'SHA',
      LDAP_SERVERS_ENC_TYPE_SALTED_SHA => 'SHA Salted',
    );
    */

   /** $encrypts = array(
      LDAP_SERVERS_ENC_TYPE_EXTENDED_DES => 'Extended DES',
      LDAP_SERVERS_ENC_TYPE_BLOWFISH => 'Blowfish',
      LDAP_SERVERS_ENC_TYPE_SALTED_CRYPT => 'Salted Crypt',
    ); */

    $encrypts = array(
      LDAP_SERVERS_ENC_TYPE_CLEARTEXT => 'No Encryption'
    );
    if (function_exists('mcrypt_module_open')) {
      $encrypts[LDAP_SERVERS_ENC_TYPE_BLOWFISH] = 'Blowfish';
    }

  }

 // $hashes[LDAP_SERVERS_ENC_TYPE_MD5] = 'MD5';
//  $encrypts[LDAP_SERVERS_ENC_TYPE_CRYPT] = 'Crypt';

  if ($type == 'encrypt') {
    return $encrypts;
  }

  if ($type == 'hash') {
    return $hashes;
  }

  return array_merge($hashes, $encrypts);


}
/**
 * Encrypt Password Method
 *
 * @param string clear_txt
 *   Plaintext password.
 *
 * @return string
 *   Encrypted text, formatted for use as an LDAP password.
 *
 * @link http://php.net/manual/en/function.mcrypt-generic-init.php
 */
function _ldap_servers_encrypt_has_mcrypt_and_warn() {
  if (!function_exists('mcrypt_module_open')) {
    watchdog('ldap_servers', 'Encryption is set to blowfish, but mcrypt module in not installed', array(), WATCHDOG_ERROR);
    return FALSE;
  }
  else {
    return TRUE;
  }
}

function _ldap_servers_encrypt($clear_txt, $enc_type = NULL) {

  if (!$enc_type) {
    $enc_type = variable_get('ldap_servers_encryption' , LDAP_SERVERS_ENC_TYPE_CLEARTEXT);
  }

  if ($enc_type == LDAP_SERVERS_ENC_TYPE_CLEARTEXT) {
    return $clear_txt;
  }

  $key = variable_get('ldap_servers_encrypt_key', drupal_get_hash_salt());

  switch ($enc_type) {

    case LDAP_SERVERS_ENC_TYPE_BLOWFISH: // Blowfish
        // Open mcrypt module.
      if (_ldap_servers_encrypt_has_mcrypt_and_warn()) {
        $td = mcrypt_module_open('blowfish', '', LDAP_SERVERS_CYPHER_MODE, '');
        // Determine maximum mycrypt key length.
        $key_length = mcrypt_enc_get_key_size($td);
        // Shorten key to allowed length.
        $key = substr($key, 0, $key_length);
        // Create the initialization vector.
        $iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_RAND);
        // Encrypt the text.
        mcrypt_generic_init($td, $key, $iv);
        $crypttext = mcrypt_generic($td, $clear_txt);
        mcrypt_generic_deinit($td);
        // Build the encrypted string.
        $cipher_txt = $iv . $crypttext;
        // Close the module.
        mcrypt_module_close($td);
      }
      break;

    default: // Cleartext
      $cipher_txt = $clear_txt;
  }

  return base64_encode($cipher_txt);
}

/**
 * Encrypt Decrypt Method
 *
 * @param string $cipher_txt
 *   ciphered text.
 *
 * @return string
 *   clear text
 *
 * http://stackoverflow.com/questions/2448256/php-mcrypt-encrypting-decrypting-file
 */

function _ldap_servers_decrypt($cipher_txt, $enc_type = NULL) {

  $key = variable_get('ldap_servers_encrypt_key', drupal_get_hash_salt());
  if (!$enc_type) {
    $enc_type = variable_get('ldap_servers_encryption' , LDAP_SERVERS_ENC_TYPE_CLEARTEXT);
  }
  if ($enc_type == LDAP_SERVERS_ENC_TYPE_CLEARTEXT) {
    return $cipher_txt;
  }

  $cipher_txt = base64_decode($cipher_txt);
  switch ($enc_type) {

    case LDAP_SERVERS_ENC_TYPE_BLOWFISH: // Blowfish
      if (_ldap_servers_encrypt_has_mcrypt_and_warn()) {
        $clear_txt = "";
        // Open mcrypt module.
        $td = mcrypt_module_open('blowfish', '', LDAP_SERVERS_CYPHER_MODE, '');
        // Determine maximum mycrypt key length.
        $key_length = mcrypt_enc_get_key_size($td);
        // Shorten key to allowed length.
        $key = substr($key, 0, $key_length);
        // Determine the algorithm IV.
        $ivsize = mcrypt_enc_get_iv_size($td);
        // Process if the decoded cipher text is sufficient.
        if (strlen($cipher_txt) > $ivsize) {
          // Split apart IV and text.
          $iv = substr($cipher_txt, 0, $ivsize);
          $cipher_txt = substr($cipher_txt, $ivsize);
          // If the IV exists, decrypt the text.
          if ($iv) {
            mcrypt_generic_init($td, $key, $iv);
            $clear_txt = mdecrypt_generic($td, $cipher_txt);
            mcrypt_generic_deinit($td);
          }
        }
        // Close the module.
        mcrypt_module_close($td);
      }
      break;

    default: // Cleartext
      $clear_txt = $cipher_txt;
  }
  return $clear_txt;
}
