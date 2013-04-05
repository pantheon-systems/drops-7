<?php


function crumbs_admin_form() {
  $form = array();

  // drupal_add_js(drupal_get_path('module', 'crumbs') .'/crumbs.admin.js');
  // drupal_add_css(drupal_get_path('module', 'crumbs') .'/crumbs.admin.css');

  $text_sections = '';
  foreach (array(
    '(enabled)' => 'This section does not have a label, it is just anything at
the top. Criteria in here are considered explicitly enabled, and can be
prioritized by moving them up and down. Criteria that are further up, have a
higher priority. By default, only the asterisk * wildcard is in this section.',
    'disabled' => 'Criteria in this section are considered explicitly
disabled. By default, this section is empty.<br/>(*)',
    'disabled by default' => 'Criteria in this section are implicitly
disabled. New critera end up here, if the plugin code says so.<br/>(**)',
    'inherit' => 'Criteria in this section "inherit" their weight and
enabled/disabled status from the best matching wildcard criterion. See the
documentation for details. New criteria end up in this section by default.<br/>(**)',
  ) as $title => $desc) {
    $desc = t($desc);
    $text_sections .= <<<EOT
<dt><pre>---- $title ----</pre></dt>
<dd>$desc</dd>
EOT;
  }
  $text_placeholders['!text_sections'] = '<dl>' . $text_sections . '</dl>';

  $text_footnotes = '';
  foreach (array(
    '*' => 'The order of criteria in this section has no meaning, and is not saved.
Instead, whenever the form is displayed, these criteria are sorted by name.',
    '**' => 'Whatever you put in these sections, is not saved. Instead, whenever
the form is displayed, the section will be filled with any criteria that are
not already in "enabled" or "disabled", sorted by name.',
  ) as $title => $desc) {
    $desc = t($desc);
    $text_footnotes .= <<<EOT
<p>($title) $desc</p>
EOT;
  }
  $text_placeholders['!text_footnotes'] = '<dl>' . $text_footnotes . '</dl>';

  $text = <<<EOT
<p>To build a breadcrumb trail, Crumbs takes the system path of the current
page, and determines a "parent path". This process is repeated with the parent,
until it arrives at the front page path, or until a loop is detected.</p>
<p>There are plenty of criteria available, that Crumbs can use to find a parent
path. This settings form allows to enable, disable and prioritize these
criteria, by moving (cut+paste) lines of text up and down.</p>
<p>The textarea has these sections:</p>
!text_sections
!text_footnotes
<p>Labels on the criteria have no meaning when you save the form, and are reset
whenever the form is displayed.</p>
EOT;
  $form['instructions'] = array(
    '#markup' => t($text, $text_placeholders),
  );

  $form['settings'] = array(
    '#type' => 'textarea',
    '#title' => t('Order of criteria for parent-finding.'),
    '#rows' => 24,
    // TODO: Add a CSS file? Or totally revamp this settings form?
    '#attributes' => array('style' => 'font-family:"Courier new", monospace;'),
  );
  $form['show_current_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show the current page at the end of the breadcrumb trail.'),
    '#default_value' => variable_get('crumbs_show_current_page', FALSE),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  $form['settings']['#default_value'] = _crumbs_get_default_text();

  return $form;
}


function _crumbs_get_default_text() {

  list($plugins, $disabled_keys) = crumbs_get_plugins();
  list($available_keys, $keys_by_plugin) = _crumbs_load_available_keys($plugins);
  $weights = crumbs_get_weights();

  return _crumbs_build_default_text($available_keys, $keys_by_plugin, $weights, $disabled_keys);
}


function _crumbs_build_default_text(array $available_keys, array $keys_by_plugin, array $weights, array $disabled_keys) {

  $key_lengths = array();
  foreach ($available_keys as $key => $title) {
    $key_lengths[] = strlen($key);
  }
  $ideal_length = _crumbs_admin_find_ideal_length($key_lengths);

  foreach ($available_keys as $key => $title) {
    $string = $key;
    if (is_string($title)) {
      if (strlen($string) < $ideal_length) {
        $string .= str_repeat(' ', $ideal_length - strlen($string));
      }
      $string .= ' - '. $title;
    }
    $available_keys[$key] = $string;
  }

  $lines = array(
    'inherit' => $available_keys,
    'disabled_by_default' => array(),
    'enabled' => array(),
    'disabled' => array(),
  );

  foreach ($weights as $key => $weight) {
    $section = ($weight === FALSE) ? 'disabled' : 'enabled';
    $string = $key;
    if (isset($available_keys[$key])) {
      $string = $available_keys[$key];
    }
    else if ($key !== '*') {
      // an orphan setting.
      if (strlen($string) < $ideal_length) {
        $string .= str_repeat(' ', $ideal_length - strlen($string));
      }
      $string .= '   (orphan rule)';
    }
    $lines[$section][$key] = $string;
    unset($lines['inherit'][$key]);
  }

  foreach ($disabled_keys as $key => $disabled) {
    if (isset($lines['inherit'][$key])) {
      $lines['disabled_by_default'][$key] = $lines['inherit'][$key];
      unset($lines['inherit'][$key]);
    }
  }

  foreach ($keys_by_plugin as $plugin_key => $keys_for_this_plugin) {
    $lines['inherit'][$plugin_key .':NEWLINE:'] = "";
  }

  ksort($lines['inherit']);
  foreach ($lines['inherit'] as $key => $line) {
    if (isset($prev) && $prev === '' && $line === '') {
      unset($lines['inherit'][$key]);
    }
    $prev = $line;
  }

  return "\n\n"
    . implode("\n", $lines['enabled'])
    . "\n\n\n---- disabled ----\n\n". implode("\n", $lines['disabled'])
    . "\n\n\n---- disabled by default ----\n\n". implode("\n", $lines['disabled_by_default'])
    . "\n\n\n---- inherit ----\n\n". implode("\n", $lines['inherit'])
    . "\n\n"
  ;
}


/**
 * This algorithm is copied 1:1 from blockadminlight
 */
function _crumbs_admin_find_ideal_length(array $key_lengths, $factor = 30) {
  sort($key_lengths, SORT_NUMERIC);
  $n = count($key_lengths);
  $length = 0;
  $best_length = 0;
  $cost = $n * $factor;
  $best_cost = $cost;
  for ($i=0; $i<$n; ++$i) {
    $increment = $key_lengths[$i] - $length;
    $length = $key_lengths[$i];
    $cost += $i * $increment;
    $cost -= $factor;
    if ($cost < $best_cost) {
      $best_cost = $cost;
      $best_length = $length;
    }
  }
  return $best_length;
}


function crumbs_admin_form_submit($form, &$form_state) {
  $weights = crumbs_get_weights();
  asort($weights);
  list($plugins, $disabled_keys) = crumbs_get_plugins();
  list($available_keys, $keys_by_plugin) = _crumbs_load_available_keys($plugins);

  $weights = array();

  $text = $form_state['values']['settings'];
  $lines = explode("\n", $text);
  $weight = 0;
  foreach ($lines as $line) {
    $line = trim($line);
    list($key, $title) = explode(' ', $line, 2) + array(NULL, NULL);
    if (isset($available_keys[$key])) {
      $weights[$key] = $weight;
      ++$weight;
    }
    elseif (preg_match('/^-/', $line)) {
      if ($weight !== FALSE) {
        $weight = FALSE;
      }
      else {
        break;
      }
    }
  }

  variable_set('crumbs_weights', $weights);
  variable_set('crumbs_show_current_page', $form_state['values']['show_current_page']);
}


function _crumbs_load_available_keys(array $plugins) {
  // we can't use the plugin engine,
  // or else we would miss disabled plugins.
  $plugin_operation = new crumbs_PluginOperation_describe();
  foreach ($plugins as $plugin_key => $plugin) {
    $plugin_operation->invoke($plugin, $plugin_key);
  }
  return array($plugin_operation->getKeys(), $plugin_operation->getKeysByPlugin());
}
