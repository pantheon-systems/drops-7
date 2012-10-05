<?php
define('APPS_VOTING_PATH', '/apps/vote/');
/**
 *  @file
 *  Adds voting component for Apps.  Each client site will be able to submit one vote, 
 *  which is tied to their site cliet id.  Has a corresponding file and component in the
 *  Appserver module.
 */

/**
 *  Menu callback - this will be hit via an AJAX call to a menu hook
 *  @param $server is the machine name of the server
 *  @param $app is the machine name of the app
 *  @param $vote is a number between 0 and 100 representing the percentage vote
 */
function apps_vote_for_app($server, $app, $vote) {
  apps_include('manifest');
  //get out client id
  $id = apps_client_id();

  //get our appserver url
  $server = apps_servers($server);
  $url = $server['manifest'];

  //unfortunately server includes the full url, which we don't actuall need, so we need to just get the domain
  $parts = parse_url($url);
  if ($parts) {
    $port = isset($parts['port']) ? ':' . $parts['port'] : '';
    $url = $parts['scheme'] . '://' . $parts['host'] . $port;
    $url .= APPS_VOTING_PATH . $app . '/' . $vote;
    
    $result = drupal_http_request($url, array(
      'data' => 'client_id=' . $id,
    ));

    if ($result->code == '200') {
      $output = json_decode($result->data);

      //record our vote
      if (lock_acquire('apps_voting_config')) {
        $user_votes = variable_get('apps_user_ratings', array());
        $user_votes[$server['name']][$app] = $vote;
        variable_set('apps_user_ratings', $user_votes);
        lock_release('apps_voting_config');
      }
    }
    else {
      $output = array('error' => 'Server connection failed, vote could not be saved.');
    }
  } 
  else {
    $output = array('error' => 'Server URL failed to parse: ' . $url);
  }

  drupal_json_output($output);
}

/**
 *  Theme callback to create a voting widget for apps
 *  Produced widget allows users to vote for apps
 */
function apps_theme_voting_widget($vars) {
  if (!empty($vars['app']['rating'])) {
    $ratings = $vars['app']['rating'];
  } else {
    $ratings = array(
      'count' => 0,
      'average' => 100,
    );
  }
  
  $teaser = !empty($vars['app']['teaser']); 
  
  $server = $vars['app']['server']['name'];
  $mname = $vars['app']['machine_name'];
  
  //retrieve the user's current rating or the defaults
  $app_ratings = variable_get('apps_user_ratings', array());
  $user_rating = isset($app_ratings[$server][$mname]) ? $app_ratings[$server][$mname] : FALSE;

  $stars = t('@num stars', array('@num' => round($ratings['average'] / 20, 1)));

  $js_settings = array(
    'apps' => array(
      $mname => array(
        'url' => '/apps/vote/' . $server . '/' . $mname . '/',
        'rating' => array (
          'average' => $ratings['average'],
          'count' => $ratings['count'],
          'stars' => $stars,
          'user' => $user_rating,
        ), 
      ),
    ),
  );
  // check if there is a vote for this app already - otherwise hide the label until there is.
  $hasvote = ($user_rating === FALSE) ? 'no-vote' : '';

  $render_arr = array (
    '#prefix' => '<div class="app-rating clearfix" id="app-rating-' . $vars['app']['machine_name'] . '">',
    'rating' => array(
      '#prefix' => '<div class="app-stars-holder" title="' . $stars . '">',
      '#markup' => '<span class="app-stars" style="width:'. $ratings['average'] . '%"></span>',
      '#suffix' => '</div>'
    ),
    'numratings' => array(
      '#prefix' => '<div class="app-rating-count">',
      '#markup' => '(' . t('!count ratings', array('!count' => '<span class="rating-count">' . $ratings['count'] . '</span>')) . ')',
      '#suffix' => '</div>',
    ),
    '#suffix' => '</div>',
  );
  // Only add the components for voting to non teaser pages
  if (!$teaser && variable_get('apps_allow_voting', TRUE)) {
    $render_arr['totalrating'] = array(
      '#prefix' => '<div class="app-average-rating">',
      '#markup' => t('Average rating: !stars stars', array(
          '!stars' => '<span class="stars-count">' . round($ratings['average'] / 20, 1) . '</span>'
        )
      ),
      '#suffix' => '</div>',
    );
    $render_arr['userrating'] = array(
      '#prefix' => '<div class="app-user-rating ' . $hasvote . '">',
      '#markup' => t('Your rating: !stars stars', array(
          '!stars' => '<span class="stars-count">' . round($user_rating / 20, 1) . '</span>'
        )
      ),
      '#suffix' => '</div>',
    );
    $render_arr['#attached'] = array(
      'js' => array(
        drupal_get_path('module', 'apps') . '/theme/js/apps-voting.js',
        array('type' => 'setting', 'data' => $js_settings),
      ),
    );
  }

  return $render_arr;
}
