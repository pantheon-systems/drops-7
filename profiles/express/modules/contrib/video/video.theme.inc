<?php
/**
 * @file
 * Theme related functions for the Video module.
 */

/**
 * Returns HTML for an video field widget.
 *
 * @param $variables
 *   An associative array containing:
 *   - element: A render element representing the image field widget.
 *
 * @ingroup themeable
 */
function theme_video_widget(array $variables) {
  $element = $variables['element'];

  $output = '<div class="video-widget form-managed-file clearfix">';

  if (isset($element['preview'])) {
    $output .= drupal_render($element['preview']);
  }

  $output .= '<div class="video-widget-data">';
  if ($element['fid']['#value'] != 0) {
    $element['filename']['#markup'] .= ' <span class="file-size">(' . format_size($element['#file']->filesize) . ')</span> ';
  }
  $output .= drupal_render_children($element);
  $output .= '</div>';
  $output .= '</div>';

  return $output;
}

/**
 * Default video cck formatter.
 *
 * Makes sure the video being displayed exists, has been converted (if in
 * queue). If not or the video is processed, then it will get the default
 * player for the specific video type for output.
 */
function theme_video_formatter_player(array $variables) {
  $item = $variables['item'];
  if (empty($item['fid'])) {
    return '';
  }

  // Only needs to be ran if they are converting videos
  if ($item['autoconversion'] && !$item['conversioncompleted']) {
    switch ($item['conversionstatus']) {
      case VIDEO_RENDERING_ACTIVE:
      case VIDEO_RENDERING_PENDING:
        return theme('video_inprogress');
      case VIDEO_RENDERING_FAILED:
        return theme('video_conversion_failed');
    }
  }

  // override player dimensions
  if (!empty($variables['player_dimensions'])) {
    $player_dimensions = explode('x', $variables['player_dimensions'], 2);
    $player_width = intval(trim($player_dimensions[0]));
    $player_height = intval(trim($player_dimensions[1]));
  }
  else {
    $player_width = 640;
    $player_height = 360;
  }

  // Poster image style
  if (!empty($item['thumbnailfile'])) {
    if (empty($variables['poster_image_style'])) {
      $item['thumbnailfile']->url = file_create_url($item['thumbnailfile']->uri);
    }
    else {
      $item['thumbnailfile']->url = image_style_url($variables['poster_image_style'], $item['thumbnailfile']->uri);
    }
  }

  // Determine theme function based on the playable files.
  // If one of the video types is configured to use an HTML5 player then the
  // HTML5 theme function is used.
  // If the last or only video type is configured to use an FLV player then the
  // FLV theme function is used.
  // In all other cases the player configured for the last or only video type
  // is used.
  $defaults = video_utility::getVideoExtensions();
  foreach ($item['playablefiles'] as $playablefile) {
    $extension = video_utility::getExtension($playablefile->uri);
    $theme_function = variable_get('video_extension_' . $extension, $defaults[$extension]);
    if ($theme_function == 'video_play_html5') {
      $theme_function = 'video_html5';
      break;
    }
    elseif ($theme_function == 'video_play_flv') {
      $theme_function = 'video_flv';
    }
  }

  return theme($theme_function, array('item' => $item, 'width' => $player_width, 'height' => $player_height, 'entity' => $variables['entity'], 'entity_type' => $variables['entity_type']));
}

/**
 * Renders the video thumbnail as a link to the node page.
 */
function theme_video_formatter_thumbnail($variables) {
  $item = $variables['item'];

  // Inside a view $variables may contain null data. In that case, just return.
  if (empty($item['fid']) || empty($item['thumbnailfile'])) {
    return '';
  }

  $thumbnail = $item['thumbnailfile'];
  $title = isset($variables['entity']->title) ? $variables['entity']->title : '';

  $image = array(
    'path' => $thumbnail->uri,
    'alt' => $title,
  );

  // Do not output an empty 'title' attribute.
  if (!empty($title)) {
    $image['title'] = $title;
  }

  if (!empty($variables['image_style'])) {
    $image['style_name'] = $variables['image_style'];
    $output = theme('image_style', $image);
  }
  else {
    $output = theme('image', $image);
  }

  if ($variables['path']) {
    $path = $variables['path']['path'];
    $options = $variables['path']['options'];
    // When displaying an image inside a link, the html option must be TRUE.
    $options['html'] = TRUE;
    $output = l($output, $path, $options);
  }

  return $output;
}

/**
 * Returns HTML for an image using a specific image style.
 *
 * @param $variables
 *   An associative array containing:
 *   - style_name: The name of the style to be used to alter the original image.
 *   - path: The path of the image file relative to the Drupal files directory.
 *     This function does not work with images outside the files directory nor
 *     with remotely hosted images.
 *   - alt: The alternative text for text-based browsers.
 *   - title: The title text is displayed when the image is hovered in some
 *     popular browsers.
 *   - attributes: Associative array of attributes to be placed in the img tag.
 *   - getsize: If set to TRUE, the image's dimension are fetched and added as
 *     width/height attributes.
 *
 * @ingroup themeable
 */
function theme_video_thumb_style($variables) {
  $style_name = $variables['style_name'];
  $path = $variables['path'];

// theme_image() can only honor the $getsize parameter with local file paths.
// The derivative image is not created until it has been requested so the file
// may not yet exist, in this case we just fallback to the URL.
  $style_path = image_style_path($style_name, $path);
  if (!file_exists($style_path)) {
    $style_path = image_style_url($style_name, $path);
  }
  $variables['path'] = $style_path;
  return theme('image', $variables);
}

/**
 * Displays a "encoding in progress message"
 */
function theme_video_inprogress() {
  return '<div class="video-inprogress">' . t('This video is currently being processed. Please wait.') . '</div>';
}

/**
 * Display an "encoding failed" message"
 */
function theme_video_conversion_failed() {
  return '<div class="video-conversion-failed">' . t('The video conversion process has failed. You might want to submit a simpler video format like <em>mpeg</em> or <em>divx avi</em>.<br />If the problem persists contact your website administrator. Please check logs for further debugging.') . '</div>';
}

/**
 * Theme wraper on Flash play on 3rd party
 */
function theme_video_flv($variables) {
  $item = $variables['item'];

  $video = NULL;
  foreach ($item['playablefiles'] as $file) {
    if ($file->filemime == 'video/mp4' || $file->filemime == 'video/flv' || $file->filemime == 'video/x-flv') {
      $video = $file;
      break;
    }
  }

  if ($video == NULL) {
    return '';
  }

  $extension = video_utility::getExtension($video->uri);
  $flash_player = variable_get('video_extension_' . $extension . '_flash_player', '');
  switch ($flash_player) {
    case 'flowplayer':
      // kjh: use a playlist to display the thumbnail if not auto playing
      if (!$variables['autoplay'] && !empty($item['thumbnailfile'])) {
        $options = array(
          'playlist' => array(
            $item['thumbnailfile']->url,
            array(
              'url' => file_create_url($video->uri),
              'autoPlay' => $variables['autoplay'],
              'autoBuffering' => $variables['autobuffering'],
            ),
          ),
        );
      }
      else {
        $options = array(
          'clip' => array(
            'url' => file_create_url($video->uri),
            'autoPlay' => $variables['autoplay'],
            'autoBuffering' => $variables['autobuffering'],
          ),
        );
      }
      $themed_output = theme('flowplayer', array('config' => $options, 'id' => 'flowplayer-' . $item['fid'], 'attributes' => array('style' => 'width:' . $variables['width'] . 'px;height:' . ($variables['height'] + 24) . 'px;')));
      break;

    case 'jwplayer':
      $preset = variable_get('video_jwplayer_preset');
      $options = array(
        'width' => $variables['width'],
        'height' => $variables['height'],
        'autoplay' => $variables['autoplay'],
      );
      $config = array();
      if (!empty($item['thumbnailfile'])) {
        $config['image'] = $item['thumbnailfile']->url;
      }
      $themed_output = theme('jw_player', array('file_object' => $video, 'options' => $options, 'config' => $config, 'preset' => $preset));
      break;

    default:
      $themed_output = t('No flash player has been set up.') . ' ' . l(t('Please select a player to play Flash videos.'), 'admin/config/media/video/players');
      break;
  }

  return theme('video_play_flv', array('item' => $item, 'themed_output' => $themed_output));
}

/**
 * Theme wrapper for HTML5
 */
function theme_video_html5($variables) {
  $themed_output = NULL;
  $item = $variables['item'];

  $files = $item['playablefiles'];
  $extension = video_utility::getExtension($files[0]->uri);

  $html5_player = variable_get('video_extension_' . $extension . '_html5_player', 'video');
  switch ($html5_player) {
    case 'video':
      $variables['files'] = $files;
      return theme('video_play_html5', $variables);

    case 'audio':
      $variables['files'] = $files;
      return theme('video_play_html5_audio', $variables);

    case 'videojs':
      $items = video_utility::objectToArray($files);
      if (!empty($item['thumbnailfile'])) {
        $thumbnail = video_utility::objectToArray($item['thumbnailfile']);
        $thumbnail['uri'] = $thumbnail['url'];
        $items[] = $thumbnail;
      }

      $attributes = array(
        'width' => $variables['width'],
        'height' => $variables['height'],
      );
      return theme('videojs', array('items' => $items, 'player_id' => 'video-' . $item['fid'], 'attributes' => $attributes, 'entity' => $variables['entity'], 'entity_type' => $variables['entity_type']));

    case 'mediaelement':
      $file = reset($files);

      return theme('mediaelement_video', array('attributes' => array('src' => file_create_url($file->uri), 'height' => $variables['height'], 'width' => $variables['width']), 'settings' => array('download_link' => FALSE)));
  }
}

/**
 * Add messages to the page.
 */
function template_preprocess_video_dialog_page(&$variables) {
  $variables['messages'] = theme('status_messages');
}
