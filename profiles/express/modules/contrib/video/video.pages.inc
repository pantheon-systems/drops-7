<?php
/**
 * @file
 * This file stores all menu callbacks for the Video module
 */


/**
 * Menu callback for postback/jobs
 */
function video_transcoder_postback_jobs() {
  // Postback setting test
  if (isset($_GET['test'])) {
    echo variable_get('video_postback_test');
    exit;
  }

  $transcoder = new Transcoder();
  if ($transcoder->hasTranscoder()) {
    return $transcoder->getTranscoder()->processPostback();
  }

  return MENU_NOT_FOUND;
}

/**
 * video browser menu callback
 */
function video_file_browser() {
  global $user;
  $output = array();
  // width and height of the player
  $output['video']['wxh'] = array(
    '#title' => t('Player dimensions'),
    '#type' => 'select',
    '#default_value' => '352x288',
    '#description' => t('Select the desired widthxheight of the video player. You can add your own dimensions from !settings.', array('!settings' => l(t('Video module settings'), 'admin/config/media/video'))),
    '#options' => video_utility::getDimensions(),
    '#attributes' => array('class' => array('video-file-browser-dimensions')),
  );
  // field get instances
  $fields = field_info_instances();
  $node_field = array();
  foreach ($fields['node'] as $content_type => $fields) {
    foreach ($fields as $field_name => $field) {
      if ($field['widget']['type'] == 'video_upload')
      $node_field[$content_type][$field_name] = $field;
    }
  }
  foreach ($node_field as $content_type => $field) {
    $result = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('status', 0, '>')
    ->condition('type', $content_type) // @todo place dynamic node types
    ->condition('uid', $user->uid) // @todo add permissions and full access to admin #1
    ->execute()
    ->fetchAll();
    foreach ($result as $value) {
      $node = node_load($value->nid);
      $node_view = node_view($node);
      foreach ($field as $field_name => $field_settings) {
        $item = $node_view[$field_settings['field_name']];
        $item[0]['#theme'] = 'video_formatter_thumbnail';
        $item[0]['#image_style'] = 'thumbnail';
        $item[0]['#path'] = '';
        $form = array();
        $form['video_browser']['image'] = array(
          '#prefix' => '<div class="video-item"><a href="#" ref="' . $node->nid . '">',
          '#markup' => '<div class="video-image">' . render($item[0]) . '</div>',
        );
        $form['video_browser']['title'] = array(
          '#markup' => '<div class="video-title">' . $node->title . '</div>',
        );
        $form['video_browser']['nid'] = array(
          '#type' => 'hidden',
          '#value' => $node->nid,
          '#suffix' => '</a></div>'
        );
        $output['video'][] = $form;
      }
    }
  }
  return render($output);
}

/**
 * Menu callback : video/embed
 */
function video_file_embed(stdClass $node, $width, $height) {
  $fields = field_info_instances('node', $node->type);
  foreach ($fields as $field_type => $field) {
    if ($field['widget']['type'] == 'video_upload') {
      $video_field = $field;
    }
  }

  $element = array(
    '#theme' => 'video_formatter_player',
    '#item' => $node->{$video_field['field_name']}['und'][0],
    '#entity' => $node,
    '#field' => $video_field,
    '#instance' => $video_field,
    '#player_dimensions' => $width . 'x' . $height,
  );
  return drupal_render($element);
}

