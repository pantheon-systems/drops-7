<?php
/**
 * Menu callback for when something has been authorized for oauth2
 *
 * @param $csid Should contain the id of the consumer when used in the client flow
 */
function oauth2_common_page_authorized($csid) {
  // If we have an oauth_token we're acting as a consumer and just got authorized

  if (!empty($_GET['code'])) {

    $consumer = $csid ? DrupalOAuthConsumer::loadById($csid, FALSE) : FALSE;
    if ($consumer) {
      $request_token = DrupalOAuthToken::loadByKey($_SESSION['oauthconnector_request_key'], $consumer, OAUTH_COMMON_TOKEN_TYPE_REQUEST);
    }



    if (!empty($request_token)) {
      $client = new DrupalOAuth2Client($consumer, $request_token);
      $callback_url = url('oauth/authorized2/' . $consumer->csid, array('absolute' => TRUE));
      $options = array(
        'get' => FALSE,
        'params' => array(
          'grant_type' => 'authorization_code',
          'code' => $_GET['code'],
          'client_id' => $consumer->key,
          'client_secret' => $consumer->secret,
          'redirect_uri' => $callback_url,

        ),
      );
      $access_token = $client->getAccessToken(NULL, $options);

      if ($access_token) {
        // We recieved a new token - save it
        if (!$access_token->in_database) {
          $access_token->write();
        }
        $request_token->delete();
        module_invoke_all('oauth_common_authorized', $consumer, $access_token, $request_token);
      }
    }
  }
  return t('The application has been authorized');
}