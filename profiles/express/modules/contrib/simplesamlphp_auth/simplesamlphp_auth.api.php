<?php
/**
 * @file
 * Describes hooks provided by the simplesamlphp_auth module.
 */

/**
 * Allow modules to change the url passed to simplesamlphp during logout.
 *
 * Called when a user logs out of Drupal.
 *
 * @param string $gotourl
 *   The url to be passed to simplesamlphp.
 * @param object $account
 *   The user being logged out.
 */
function hook_simplesamlphp_auth_logout_gotourl_alter(&$gotourl, $account) {
  // Example of reacting to a value on the account.
  if ($account->field_example == 'example_value') {
    $gotourl = url(
      'thanks-for-stopping-by',
      array(
        'absolute' => TRUE,
      )
    );
  }

  // Example of adding the destination value to the redirect URL.
  if (isset($_GET['destination'])) {
    // Ensure our redirect URL is absolute.
    $redirect_url = url(
      $_GET['destination'],
      array(
        'absolute' => TRUE,
      )
    );

    // Add our redirect URL as a querystring to the full URL.
    $gotourl = $gotourl . '?redirect=' . drupal_encode_path($redirect_url);
  }
}
