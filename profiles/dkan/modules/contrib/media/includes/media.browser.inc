<?php

/**
 * @file
 * Summon plugins and render the media browser.
 */

/**
 * Media browser page callback.
 */
function media_browser($selected = NULL) {
  $output = array();
  $output['#attached']['library'][] = array('media', 'media_browser_page');

  $params = media_get_browser_params();

  // If one or more files have been selected, the browser interaction is now
  // complete. Return empty page content to the dialog which now needs to close,
  // but populate Drupal.settings with information about the selected files.
  if (isset($params['fid'])) {
    $fids = is_array($params['fid']) ? $params['fid'] : array($params['fid']);
    if (!is_numeric($fids[0])) {
      throw new Exception('Error selecting media, fid param is not an fid or an array of fids');
    }
    $files = file_load_multiple($fids);
    foreach ($files as $file) {
      media_browser_build_media_item($file);
    }
    $setting = array('media' => array('selectedMedia' => array_values($files)));
    drupal_add_js($setting, 'setting');
    return $output;
  }

  $plugins = media_get_browser_plugin_info();

  // Allow parameters to provide a list of enabled or disabled media browser
  // plugins.
  if (!empty($params['enabledPlugins'])) {
    $plugins = array_intersect_key($plugins, array_fill_keys($params['enabledPlugins'], 1));
  }
  elseif (!empty($params['disabledPlugins'])) {
    $plugins = array_diff_key($plugins, array_fill_keys($params['disabledPlugins'], 1));
  }

  // Render plugins.
  $plugin_output = array();
  foreach ($plugins as $key => $plugin_info) {
    // Support the old CTools style handler definition.
    if (!isset($plugin_info['class']) && !empty($plugin_info['handler'])) {
      if (is_string($plugin_info['handler'])) {
        $plugin_info['class'] = $plugin_info['handler'];
      }
      elseif (isset($plugin_info['handler']['class'])) {
        $plugin_info['class'] = $plugin_info['handler']['class'];
      }
    }

    if (empty($plugin_info['class']) || !class_exists($plugin_info['class'])) {
      continue;
    }

    $plugin = new $plugin_info['class']($plugin_info, $params);
    if ($plugin->access()) {
      $plugin_output[$key] = $plugin->view();
      if (!empty($plugin_output[$key]) && is_array($plugin_output[$key])) {
        $plugin_output[$key] += array(
          '#title' => $plugin_info['title'],
          '#weight' => isset($plugin_info['weight']) ? $plugin_info['weight'] : 0,
        );
      }
      else {
        unset($plugin_output[$key]);
        continue;
      }
    }
    else {
      continue;
    }

    // We need to ensure that a submit button is available on each tab. If the
    // plugin is not returning a form element we need to add a submit button.
    // This is a fairly broad assumption.
    if (empty($plugin_output[$key]['#form']) && !empty($plugin_output[$key]['#markup'])) {
      $fake_buttons = '<div class="form-actions form-wrapper">';
      $fake_buttons .= l(t('Submit'), '', array(
        'attributes' => array(
          'class' => array('button', 'button-yes', 'fake-submit', $key),
        ),
      ));
      $fake_buttons .= '</div>';
      $plugin_output[$key]['#markup'] .= $fake_buttons;
    }
  }

  // Allow modules to change the tab names or whatever else they want to change
  // before we render.  Perhaps this should be an alter on the theming function
  // that we should write to be making the tabs.
  drupal_alter('media_browser_plugins', $plugin_output);

  $tabs = array();
  $settings = array('media' => array('browser' => array()));

  foreach (element_children($plugin_output, TRUE) as $key) {
    // Add any JavaScript settings from the browser tab.
    if (!empty($plugin_output[$key]['#settings'])) {
      $settings['media']['browser'][$key] = $plugin_output[$key]['#settings'];
    }

    // If this is a "ajax" style tab, add the href, otherwise an id. jQuery UI
    // will use an href value to load content from that url
    $tabid = 'media-tab-' . check_plain($key);
    if (!empty($plugin_output[$key]['#callback'])) {
      $href = $plugin_output[$key]['#callback'];
    }
    else {
      $attributes = array(
        'class' => array('media-browser-tab'),
        'id' => $tabid,
        'data-tabid' => $key,
      );
      // Create a div for each tab's content.
      $plugin_output[$key] += array(
        '#prefix' => '<div '. drupal_attributes($attributes) . ">\n",
        '#suffix' => "</div>\n",
      );
    }

    $attributes = array(
      'href' => '#' . $tabid,
      'data-tabid' => $key,
      'title' => $plugin_output[$key]['#title'],
    );
    $tabs[]['element'] = array(
      '#markup' => '<li><a' . drupal_attributes($attributes) . '>' . check_plain($plugin_output[$key]['#title']) . "</a></li>\n",
    );
  }

  drupal_add_js($settings, 'setting');

  $output['tabset']['tabs'] = array(
    '#theme' => 'menu_local_tasks',
    '#attributes' => array('class' => array('tabs', 'primary')),
    '#primary' => $tabs,
  );

  $output['tabset']['panes'] = $plugin_output;

  return $output;
}

/**
 * Attaches media browser javascript to an element.
 *
 * @param array $element
 *   The element array to attach to.
 */
function media_attach_browser_js(&$element) {
  $javascript = media_browser_js();
  foreach ($javascript as $key => $definitions) {
    foreach ($definitions as $definition) {
      $element['#attached'][$key][] = $definition;
    }
  }
}

/**
 * Helper function to define browser javascript.
 */
function media_browser_js() {
  $settings = array(
    'browserUrl' => url('media/browser', array(
      'query' => array(
        'render' => 'media-popup'
      ))
    ),
    'styleSelectorUrl' => url('media/-media_id-/format-form', array(
      'query' => array(
        'render' => 'media-popup'
      ))
    ),
    'dialogOptions' => array(
      'dialogclass' => variable_get('media_dialogclass', 'media-wrapper'),
      'modal' => (boolean)variable_get('media_modal', TRUE),
      'draggable' => (boolean)variable_get('media_draggable', FALSE),
      'resizable' => (boolean)variable_get('media_resizable', FALSE),
      'minwidth' => (int)variable_get('media_minwidth', 500),
      'width' => (int)variable_get('media_width', 670),
      'height' => (int)variable_get('media_height', 280),
      'position' => variable_get('media_position', 'center'),
      'overlay' => array(
        'backgroundcolor' => variable_get('media_backgroundcolor', '#000000'),
        'opacity' => (float)variable_get('media_opacity', 0.4),
      ),
      'zindex' => (int)variable_get('media_zindex', 10000),
    ),
  );

  return array(
    'library' => array(
      array('media', 'media_browser'),
    ),
    'js' => array(
      array(
       'data' => array('media' => $settings),
       'type' => 'setting',
      ),
    ),
  );
}

/**
 * Menu callback for testing the media browser.
 */
function media_browser_testbed($form) {
  media_attach_browser_js($form);

  $form['test_element'] = array(
    '#type' => 'media',
    '#media_options' => array(
      'global' => array(
        'types' => array('video', 'audio'),
      ),
    ),
  );

  $launcher = '<a href="#" id="launcher"> Launch Media Browser</a>';

  $form['options'] = array(
    '#type' => 'textarea',
    '#title' => 'Options (JSON)',
    '#rows' => 10,
  );

  $form['launcher'] = array(
    '#markup' => $launcher,
  );

  $form['result'] = array(
    '#type' => 'textarea',
    '#title' => 'Result',
  );

  $js = <<<EOF
    Drupal.behaviors.mediaTest = {
    attach: function(context, settings) {
      var delim = "---";
      var recentOptions = [];
      var recentOptionsCookie = jQuery.cookie("recentOptions");
      if (recentOptionsCookie) {
        recentOptions = recentOptionsCookie.split("---");
      }

      var recentSelectBox = jQuery('<select id="recent_options" style="width:100%"></select>').change(function() { jQuery('#edit-options').val(jQuery(this).val())});

      jQuery('.form-item-options').append('<label for="recent_options">Recent</a>');
      jQuery('.form-item-options').append(recentSelectBox);
      jQuery('.form-item-options').append(jQuery('<a href="#">Reset</a>').click(function() {alert('reset'); jQuery.cookie("recentOptions", null); window.location.reload(); }));

      jQuery.each(recentOptions, function (idx, val) {
        recentSelectBox.append(jQuery('<option></option>').val(val).html(val));
      });


      jQuery('#launcher').click(function () {
        jQuery('#edit-result').val('');
        var options = {};
        var optionsTxt = jQuery('#edit-options').val();
        if (optionsTxt) {
          // Store it in the recent box
          recentOptionsCookie += "---" + optionsTxt
          jQuery.cookie("recentOptions", recentOptionsCookie, { expires: 7 });
          recentSelectBox.append(jQuery('<option></option>').val(optionsTxt).html(optionsTxt));
          options = eval('(' + optionsTxt + ')');
        }
        Drupal.media.popups.mediaBrowser(Drupal.behaviors.mediaTest.mediaSelected, options);
        return false;
      });
    },

    mediaSelected: function(selectedMedia) {
      var result = JSON.stringify(selectedMedia);
        jQuery('#edit-result').val(result);
    }
  }

EOF;

  drupal_add_js($js, array('type' => 'inline'));
  return $form;
}

/**
 * Adds properties to the file.
 *
 * Additional properties on this file are needed by the media browser JS code.
 */
function media_browser_build_media_item($file) {
  $preview = media_get_thumbnail_preview($file);
  $file->preview = drupal_render($preview);
  $file->url = file_create_url($file->uri);
}
