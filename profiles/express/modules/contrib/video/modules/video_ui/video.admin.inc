<?php
/**
 * @file
 * Provides the administration settings for the Video Drupal module.
 */

/**
 * Video transcoder admin settings
 */
function video_transcoder_admin_settings() {
  $transcoder = new Transcoder();
  $options = $transcoder->getAllTranscoders();

  $form = array();
  $form['video_convertor'] = array(
    '#type' => 'radios',
    '#title' => t('Video transcoder'),
    '#default_value' => variable_get('video_convertor', 'TranscoderAbstractionFactoryFfmpeg'),
    '#options' => $options['radios'],
    '#description' => '<p>' . t('Select a video transcoder will help you convert videos and generate thumbnails.') . '</p>' . theme('item_list', array('items' => $options['help'])),
  );
  $form = $form + $options['admin_settings'];

  return system_settings_form($form);
}

/**
 * Form API callback to validate the upload settings form.
 */
function video_transcoder_admin_settings_validate($form, &$form_state) {
  $transcodername = $form_state['values']['video_convertor'];
  if ($transcodername == '') {
    return;
  }

  Transcoder::createTranscoder($transcodername)->adminSettingsValidate($form, $form_state);

  if (!form_get_errors() && $transcodername != variable_get('video_convertor')) {
    drupal_set_message(t('Because the selected transcoder was changed, you need to update the codec settings for your presets.'), 'warning');
    $form_state['redirect'] = 'admin/config/media/video/presets';
  }
}

/**
 * Video preset admin settings
 */
function video_preset_admin_settings($form_state) {
  $form = array(
    '#theme' => 'video_preset_admin_settings',
    '#submit' => array('video_preset_admin_settings_submit'),
  );

  $presets = Preset::getAllPresets();

  $form['video_use_preset_wxh'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use preset dimensions for video conversion.'),
    '#default_value' => variable_get('video_use_preset_wxh', FALSE),
    '#description' => t('Override the user selected dimensions with the value from the presets (recommended).')
  );

  if (!empty($presets)) {
    $selected = array_filter(variable_get('video_preset', array()));

    $form['video_preset'] = array(
      '#tree' => TRUE,
    );

    foreach ($presets as $preset) {
      $delete = NULL;
      if (empty($preset['module']) && !in_array($preset['name'], $selected)) {
        $delete = array('#type' => 'link', '#title' => t('delete'), '#href' => 'admin/config/media/video/presets/preset/' . $preset['name'] . '/delete');
      }
      elseif ($preset['overridden']) {
        $delete = array('#type' => 'link', '#title' => t('revert'), '#href' => 'admin/config/media/video/presets/preset/' . $preset['name'] . '/revert');
      }

      $form['video_preset'][$preset['name']] = array(
        'status' => array(
          '#type' => 'checkbox',
          '#title' => check_plain($preset['name']),
          '#default_value' => in_array($preset['name'], $selected),
        ),
        'description' => array('#markup' => !empty($preset['description']) ? check_plain($preset['description']) : ''),
        'edit' => array('#type' => 'link', '#title' => t('edit'), '#href' => 'admin/config/media/video/presets/preset/' . $preset['name']),
        'delete' => $delete,
        'export' => array('#type' => 'link', '#title' => t('export'), '#href' => 'admin/config/media/video/presets/preset/' . $preset['name'] . '/export'),
      );
    }
  }

  return system_settings_form($form);
}

function video_preset_admin_settings_validate($form, $form_state) {
  if (isset($form['video_preset'])) {
    $hasany = FALSE;

    foreach ($form_state['values']['video_preset'] as $name => $values) {
      if ($values['status'] == 1) {
        $hasany = TRUE;
        break;
      }
    }

    if (!$hasany) {
      form_error($form['video_preset'], t('You must enable at least one preset.'));
    }
  }
}

function video_preset_admin_settings_submit($form, &$form_state) {
  $form_state['values']['video_use_preset_wxh'] = (bool)$form_state['values']['video_use_preset_wxh'];

  $selected = array();

  if (isset($form['video_preset'])) {
    foreach ($form_state['values']['video_preset'] as $name => $values) {
      if ($values['status'] == 1) {
        $selected[] = $name;
      }
    }
  }

  $form_state['values']['video_preset'] = $selected;
}

function theme_video_preset_admin_settings($variables) {
  $form = $variables['form'];

  $header = array(
    array('data' => t('Name')),
    array('data' => t('Description')),
    array('data' => t('Operations'), 'colspan' => 3),
  );

  $rows = array();

  if (!empty($form['video_preset'])) {
    foreach (element_children($form['video_preset']) as $id) {
      $rows[] = array(
        'data' => array(
          drupal_render($form['video_preset'][$id]['status']),
          drupal_render($form['video_preset'][$id]['description']),
          drupal_render($form['video_preset'][$id]['edit']),
          drupal_render($form['video_preset'][$id]['delete']),
          drupal_render($form['video_preset'][$id]['export']),
        ),
      );
    }
  }

  $actions = drupal_render_children($form['actions']);
  $output = drupal_render_children($form);
  $output .= theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'video-preset'),
    'empty' => t('No presets were found. Please use the <a href="@create-preset">Add preset</a> link to create a new Video preset or upload an existing Video preset feature module to your modules directory.', array('@create-preset' => url('admin/config/media/video/presets/add'))),
  ));
  $output .= $actions;

  return $output;
}

/**
 * Video general admin settings
 */
function video_general_admin_settings() {
  $hastranscoder = variable_get('video_convertor') !== '';

  $form = array();
  $form['video_autoplay'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically start video on page load'),
    '#default_value' => variable_get('video_autoplay', FALSE),
    '#description' => t('Start the video when the page and video loads'),
  );
  $form['video_autobuffering'] = array(
    '#type' => 'checkbox',
    '#title' => t('Automatically start video buffering'),
    '#default_value' => variable_get('video_autobuffering', TRUE),
    '#description' => t('Start buffering video when the page and video loads'),
  );
  $form['video_bypass_conversion'] = array(
    '#type' => 'checkbox',
    '#title' => t('Bypass video conversion'),
    '#default_value' => variable_get('video_bypass_conversion', FALSE),
    '#description' => t('Bypass video conversion when creating videos.'),
    '#access' => $hastranscoder,
  );
  $form['video_convert_on_save'] = array(
    '#type' => 'checkbox',
    '#title' => t('Video convert on node submit'),
    '#default_value' => variable_get('video_convert_on_save', FALSE),
    '#description' => t('Convert videos on node submit will enable by default for all users.'),
    '#access' => $hastranscoder,
  );
  $form['video_use_default_thumb'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override video thumbnails with default thumbnail'),
    '#default_value' => variable_get('video_use_default_thumb', FALSE),
    '#description' => t('Override auto thumbnails with default thumbnail.'),
    '#access' => $hastranscoder,
  );
  $form['video_publish_on_complete'] = array(
    '#type' => 'checkbox',
    '#title' => t('Publish when conversion complete'),
    '#default_value' => variable_get('video_publish_on_complete', FALSE),
    '#description' => t('This feature is now fully controlled by !rules module. Download the module and configure rules to take effect when video conversions have completed or failed.', array('!rules' => l(t('Rules'), 'http://drupal.org/project/rules'))),
    '#disabled' => TRUE,
    '#access' => $hastranscoder,
  );
  $form['video_dimensions'] = array(
    '#type' => 'textarea',
    '#title' => t('Available dimensions for converting and displaying videos'),
    '#description' => t('Enter one dimension per line. Each resolution must be in the form of <code>width</code>x<code>height</code>. Example dimensions: <code>1280x720</code>.'),
    '#default_value' => variable_get('video_dimensions', video_utility::getDefaultDimensions()),
    '#required' => TRUE,
    '#wysiwyg' => FALSE,
  );

  return system_settings_form($form);
}

function video_general_admin_settings_validate($form, $form_state) {
  $v =& $form_state['values'];
  if (!empty($v['video_dimensions'])) {
    $v['video_dimensions'] = str_replace("\r", '', $v['video_dimensions']);
    if (!video_utility::validateDimensionList($v['video_dimensions'])) {
      form_error($form['video_dimensions'], t('The list of dimensions contains errors: only enter new lines and dimensions in the form widthxheight.'));
    }
  }
}

/**
 * Video player admin settings
 */
function video_players_admin_settings() {
  $form = array(
    '#submit' => array('video_players_admin_settings_submit'),
  );
  $form['extensions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Video extensions'),
    '#description' => t('Here you can map specific players to each video extension type.') . ' ' . t('This player will be selected when you choose the !formatter formatter for a Video field.', array('!formatter' => t('Video player'))),
  );
  // lets get all our supported extensions and players.
  $extensions = video_utility::getVideoExtensions();
  $players = video_utility::getVideoPlayers();
  $flv_players = video_video_flv_players();
  $html5_players = video_video_html5_players();

  foreach ($extensions as $ext => $player) {
    $form['extensions']['video_extension_' . $ext] = array(
      '#type' => 'select',
      '#title' => t('Extension:') . '  ' . $ext,
      '#default_value' => variable_get('video_extension_' . $ext, $player),
      '#options' => $players,
      '#required' => TRUE,
    );

    // For Flash
    if (!empty($flv_players)) {
      $value = variable_get('video_extension_' . $ext . '_flash_player', '');
      if (empty($value) || !isset($flv_players[$value])) {
        $value = key($flv_players);
      }
      $form['extensions']['video_extension_' . $ext . '_flash_player'] = array(
        '#type' => 'radios',
        '#title' => t('Flash player for @extension', array('@extension' => $ext)),
        '#options' => $flv_players,
        '#default_value' => $value,
        '#required' => TRUE,
        '#states' => array(
          'visible' => array(
            ':input[id=edit-video-extension-' . $ext . ']' => array('value' => 'video_play_flv'),
          ),
        ),
      );
    }
    else {
      $form['extensions']['video_extension_' . $ext . '_flash_player']['type'] = array(
        '#type' => 'markup',
        '#markup' => t('No Flash players detected.<br />You need to install !flowplayer or !jwplayer.', array('!flowplayer' => l(t('Flowplayer API'), 'http://www.drupal.org/project/flowplayer'), '!jwplayer' => l(t('JW Player'), 'http://www.drupal.org/project/jw_player'))),
        '#states' => array(
          'visible' => array(
            ':input[id=edit-video-extension-' . $ext . ']' => array('value' => 'video_play_flv'),
          ),
        ),
      );
    }

    // for HTML5
    if (!empty($html5_players)) {
      $value = variable_get('video_extension_' . $ext . '_html5_player', '');
      if (empty($value) || !isset($html5_players[$value])) {
        $value = key($html5_players);
      }
      $form['extensions']['video_extension_' . $ext . '_html5_player'] = array(
        '#type' => 'radios',
        '#title' => t('HTML5 player for @extension', array('@extension' => $ext)),
        '#options' => $html5_players,
        '#markup' => t('Additional HTML5 players module.<br />You can install !videojs.', array('!videojs' => l(t('Video.js'), 'http://drupal.org/project/videojs'))),
        '#default_value' => $value,
        '#required' => TRUE,
        '#states' => array(
          'visible' => array(
            ':input[id=edit-video-extension-' . $ext . ']' => array('value' => 'video_play_html5'),
          ),
        ),
      );
    }
  }

  // Miscellaneous player settings
  $form['playersettings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Player settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  if (module_exists('jw_player')) {
    $presets = array();
    foreach (jw_player_preset_load() as $preset => $item) {
      $presets[$preset] = $item['preset_name'];
    }

    $form['playersettings']['video_jwplayer_preset'] = array(
      '#type' => 'select',
      '#title' => t('JW Player preset'),
      '#options' => $presets,
      '#default_value' => variable_get('video_jwplayer_preset', NULL),
      '#empty_value' => '',
    );
  }

  if (count(element_children($form['playersettings'])) == 0) {
    unset($form['playersettings']);
  }

  return system_settings_form($form);
}

/**
 * Submit helper to remove unneeded variables before they are saved.
 */
function video_players_admin_settings_submit($form, &$form_state) {
  // Clear all existing variables
  db_query('DELETE FROM {variable} WHERE name LIKE \'video_extension_%\'');

  // Remove flv player or html5 player when that player hasn't been selected
  $extensions = video_utility::getVideoExtensions();
  foreach (array_keys($extensions) as $ext) {
    $selected = $form_state['values']['video_extension_' . $ext];
    if ($selected != 'video_play_flv') {
      unset($form_state['values']['video_extension_' . $ext . '_flash_player']);
    }
    if ($selected != 'video_play_html5') {
      unset($form_state['values']['video_extension_' . $ext . '_html5_player']);
    }
  }
}

/**
 * Video cron admin settings
 */
function video_scheduling_admin_settings() {
  $form = array();
  $form['video_cron'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Drupal\'s built-in cron'),
    '#default_value' => variable_get('video_cron', TRUE),
    '#description' =>
        t('Check this box to use Drupal\'s built in cron execution to transcode videos. Be aware that video transcoding is very resource-intensive. If you use poor man\'s cron, this option is strongly discouraged.') . '<br/>' .
        t('Alternatives to using cron are the video_scheduler.php script that is located in the Video module directory or the video-scheduler <a href="@drush-url">Drush</a> command (recommended).', array('@drush-url' => url('http://drupal.org/project/drush'))) . '<br/>' .
        t('If you use none of these options, you can only transcode videos by using the %convertonsave option when uploading a video.', array('%convertonsave' => t('Convert video on save'))),
  );
  $form['video_transcode_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Video transcode timeout'),
    '#default_value' => variable_get('video_transcode_timeout', 10),
    '#description' => t('The maximum time allowed for a video transcode to complete. Use a larger value when you regularly transcode large or long videos. When a video has been transcoding for more than this amount of time, it will be marked as failed. Leave this field empty to disable this behavior.'),
    '#field_suffix' => t('minutes'),
    '#maxlength' => 5,
    '#size' => 10,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['video_queue_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Video queue timeout'),
    '#default_value' => variable_get('video_queue_timeout', 60),
    '#description' => t('The maximum time allowed for a video to be in the queue. When processing of the queue is aborted, the video will be requeued after this amount of time. Leave this field empty to disable this behavior.'),
    '#field_suffix' => t('minutes'),
    '#maxlength' => 5,
    '#size' => 10,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['video_ffmpeg_instances'] = array(
    '#type' => 'textfield',
    '#title' => t('Total videos to convert during each scheduled run'),
    '#default_value' => variable_get('video_ffmpeg_instances', 5),
    '#description' => t('How many videos do you want to process on each cron/video_scheduler.php/drush run?'),
    '#maxlength' => 5,
    '#size' => 10,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  return system_settings_form($form);
}

/**
 * Return our possible flash players.
 *
 * When extending this method, also change the error message in
 * video_players_admin_settings().
 */
function video_video_flv_players() {
  $options = array();
  if (module_exists('flowplayer')) {
    $options['flowplayer'] = t('Flowplayer');
  }
  if (module_exists('jw_player')) {
    $options['jwplayer'] = t('JW Player');
  }
  return $options;
}

/**
 * Return our possible HTML5 players.
 */
function video_video_html5_players() {
  $options = array(
    'video' => t('Plain HTML5 video player with Flash fallback'),
    'audio' => t('Plain HTML5 audio player with Flash fallback'),
  );
  if (module_exists('videojs')) {
    $options['videojs'] = t('Video.js');
  }
  if (module_exists('mediaelement')) {
    $options['mediaelement'] = t('MediaElement');
  }
  return $options;
}

/**
 * Page callback that displays FFmpeg information.
 */
function video_admin_ffmpeg_info($form_state) {
  $ffmpegpath = variable_get('video_ffmpeg_path', '/usr/bin/ffmpeg');
  if (isset($_GET['ffmpegpath'])) {
    $ffmpegpath = $_GET['ffmpegpath'];
  }

  $form = array();
  $form['ffmpegpath'] = array(
    '#type' => 'textfield',
    '#title' => t('Path to FFmpeg or avconv executable'),
    '#default_value' => $ffmpegpath,
    '#required' => TRUE,
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Show information'));

  if ($ffmpegpath != '') {
    $transcoder = new PHPVideoToolkit($ffmpegpath, realpath(file_directory_temp()) . '/');
    $info = $transcoder->getFFmpegInfo(FALSE);
    $infotxt = var_export($info, TRUE);

    $form['info'] = array(
      '#type' => 'textarea',
      '#title' => t('Debug information'),
      '#default_value' => $infotxt,
      '#weight' => 1000,
      '#rows' => min(20, substr_count($infotxt, "\n")),
    );
  }

  return $form;
}

function video_admin_ffmpeg_info_submit($form, &$form_state) {
  $form_state['redirect'] = array(
    'admin/config/media/video/ffmpeg-info',
    array(
      'query' => array(
        'ffmpegpath' => $form_state['values']['ffmpegpath'],
      ),
    ),
  );
}
