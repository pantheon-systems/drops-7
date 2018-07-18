<?php

/**
 * @file
 * Admin page callbacks for the elysia cron module.
 */

/**
 * Page callback for 'admin/config/system/cron' path.
 *
 * @return array
 *   Renderable array.
 */
function elysia_cron_admin_page() {
  $aoutput = array();

  if (elysia_cron_access('execute elysia_cron')) {
    $aoutput[] = drupal_get_form('elysia_cron_run_form');
  }

  $output = '';
  elysia_cron_initialize();
  global $_elysia_cron_settings_by_channel;

  $global_disabled = variable_get('elysia_cron_disabled', FALSE);
  $last_channel = elysia_cron_last_channel();
  $output .= '<p>' . t('Global disable') . ': <i>' . ($global_disabled ? '<span class="warn">' . t('YES') . '</span>' : t('no')) . '</i></p>';
  $output .= '<p>' . t('Last channel executed') . ': <i>' . ($last_channel ? $last_channel : t('n/a')) . '</i></p>';

  $running = '';
  foreach ($_elysia_cron_settings_by_channel as $channel => $data) {
    if (elysia_cron_is_channel_running($channel)) {
      $running .= $channel . ' ';
    }
  }
  if ($running) {
    $output .= '<p>' . t('Running channels') . ': <span class="warn">' . $running . '</span></p>';
  }

  $output .= '<p>' . t('Last run') . ': ' . elysia_cron_date(_ec_variable_get('elysia_cron_last_run', 0)) . '</p>';
  $rows = array();
  $ipath = drupal_get_path('module', 'elysia_cron') . '/images/icon_';

  foreach ($_elysia_cron_settings_by_channel as $channel => $data) {
    $running = elysia_cron_is_channel_running($channel);
    $rows[] = array(
      array(
        'data' => '<h3>' . t('Channel') . ': ' . $channel . ($data['#data']['disabled'] ? ' <span class="warn">(' . t('DISABLED') . ')</span>' : '') . '</h3>',
        'colspan' => 2,
        'header' => TRUE,
      ),
      array(
        'data' => elysia_cron_date($data['#data']['last_run']),
        'header' => TRUE,
      ),
      array(
        'data' => $data['#data']['last_execution_time'] . 's ' . t('(Shutdown: !shutdown)', array('!shutdown' => $data['#data']['last_shutdown_time'] . 's')),
        'header' => TRUE,
      ),
      array(
        'data' => $data['#data']['execution_count'],
        'header' => TRUE,
      ),
      array(
        'data' => $data['#data']['avg_execution_time'] . 's / ' . $data['#data']['max_execution_time'] . 's',
        'header' => TRUE,
      ),
    );
    $messages = '';
    if ($running) {
      $messages .= t('Running since !date', array('!date' => elysia_cron_date($running))) . '<br />';
    }
    if ($data['#data']['last_aborted'] || $data['#data']['abort_count']) {
      $msg = array();
      if ($data['#data']['last_aborted']) {
        $msg[] = t('Last aborted') . (!empty($data['#data']['last_abort_function']) ? ': <span class="warn">' . t('On function !function', array('!function' => $data['#data']['last_abort_function'])) . '</span>' : '');
      }
      if ($data['#data']['abort_count']) {
        $msg[] = t('Abort count') . ': <span class="warn">' . $data['#data']['abort_count'] . '</span>';
      }
      $messages .= implode(', ', $msg) . '<br />';
    }
    if ($messages) {
      $rows[] = array(
        '',
        '',
        array('data' => $messages, 'colspan' => 4, 'header' => TRUE),
      );
      $rows[] = array(array('data' => '', 'colspan' => 6));
    }

    foreach ($data as $job => $conf) {
      $icon = 'idle';
      $caption = '<b>' . $job . '</b>';
      $tip = t('Idle');
      if ($conf['disabled']) {
        $icon = 'disabled';
        $caption = '<strike><b>' . $job . '</b></strike>';
        $tip = t('Disabled');
      }
      elseif (!empty($conf['running'])) {
        $icon = 'running';
        $caption = '<b><u>' . $job . '</u></b>';
        $tip = t('Running');
      }
      elseif (elysia_cron_should_run($conf)) {
        $icon = 'waiting';
        $tip = t('Waiting for execution');
      }

      if ($job != '#data') {
        $force_run = elysia_cron_access('execute elysia_cron')
          ? l(t('Force run'), 'admin/config/system/cron/execute/' . $job, array('attributes' => array('onclick' => 'return confirm("' . t('Force execution of !job?', array('!job' => $job)) . '");')))
          : '';

        $icon_image = theme('image', array(
          'path' => $ipath . $icon . '.png',
          'alt' => $tip,
          'title' => $tip,
        ));
        $rows[] = array(
          array('data' => $icon_image, 'align' => 'right'),
          array('data' => $caption . ': <i>' . elysia_cron_description($job) . '</i> ', 'colspan' => 4),
          array('data' => $force_run, 'align' => 'right'),
        );
        $rows[] = array(
          '',
          check_plain($conf['rule']) . (!empty($conf['weight']) ? ' <small>(' . t('Weight') . ': ' . $conf['weight'] . ')</small>' : ''),
          elysia_cron_date($conf['last_run']),
          $conf['last_execution_time'] . 's',
          $conf['execution_count'],
          $conf['avg_execution_time'] . 's / ' . $conf['max_execution_time'] . 's',
        );
      }
    }

    $rows[] = array('&nbsp;', '', '', '', '', '');
  }

  $output .= theme('table', array(
    'header' => array(
      '',
      t('Job / Rule'),
      t('Last run'),
      t('Last exec time'),
      t('Exec count'),
      t('Avg/Max Exec time'),
    ),
    'rows' => $rows,
  ));
  $output .= '<br />';

  $legend_icons = array(
    'idle' => theme('image', array(
      'path' => $ipath . 'idle.png',
      'alt' => t('Idle'),
      'title' => t('Idle'),
    )),
    'waiting' => theme('image', array(
      'path' => $ipath . 'waiting.png',
      'alt' => t('Waiting for execution'),
      'title' => t('Waiting for execution'),
    )),
    'running' => theme('image', array(
      'path' => $ipath . 'running.png',
      'alt' => t('Running'),
      'title' => t('Running'),
    )),
    'disabled' => theme('image', array(
      'path' => $ipath . 'disabled.png',
      'alt' => t('Disabled'),
      'title' => t('Disabled'),
    )),
  );
  $output .= theme('table', array(
    'header' => array(t('Legend')),
    'rows' => array(
      array($legend_icons['idle'] . ' - ' . t("Job shouldn't do anything right now")),
      array($legend_icons['waiting'] . ' - ' . t('Job is ready to be executed, and is waiting for system cron call')),
      array($legend_icons['running'] . ' - ' . t('Job is running right now')),
      array($legend_icons['disabled'] . ' - ' . t("Job is disabled by settings, and won't run until enabled again")),
      array(t("Notes: job times don't include shutdown times (only shown on channel times).")),
      array(t('If an abort occurs usually the job is not properly terminated, and so job timings can be inaccurate or wrong.')),
    ),
  ));

  $aoutput[] = array(
    '#type' => 'markup',
    '#markup' => $output,
  );

  return $aoutput;
}

/**
 * Form builder for general settings form.
 *
 * @return array
 *   From API array.
 */
function elysia_cron_settings_form() {
  global $_elysia_cron_settings_by_channel;
  elysia_cron_initialize();

  $form = array();

  $form['prefix_1'] = array(
    '#type' => 'fieldset',
    '#title' => t('Click for help and cron rules and script syntax'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t(<<<EOT
<h3>Fields order</h3>
<pre>
 +---------------- minute (0 - 59)
 |  +------------- hour (0 - 23)
 |  |  +---------- day of month (1 - 31)
 |  |  |  +------- month (1 - 12)
 |  |  |  |  +---- day of week (0 - 6) (Sunday=0)
 |  |  |  |  |
 *  *  *  *  *
</pre>
<p>Each of the patterns from the first five fields may be either * (an asterisk),
which matches all legal values, or a list of elements separated by commas (see below).</p>
<p>For "day of the week" (field 5), 0 is considered Sunday, 6 is Saturday
(7 is an illegal value)</p>
<p>A job is executed when the time/date specification fields all match the current
time and date. There is one exception: if both "day of month" and "day of week"
are restricted (not "*"), then either the "day of month" field (3) or the "day of week"
field (5) must match the current day (even though the other of the two fields
need not match the current day).</p>

<h3>Fields operators</h3>
<p>There are several ways of specifying multiple date/time values in a field:</p>
<ul>
<li>The comma (',') operator specifies a list of values, for example: "1,3,4,7,8"</li>
<li>The dash ('-') operator specifies a range of values, for example: "1-6", which is equivalent to "1,2,3,4,5,6"</li>
<li>The asterisk ('*') operator specifies all possible values for a field. For example, an asterisk in the hour time field would be equivalent to 'every hour' (subject to matching other specified fields).</li>
<li>The slash ('/') operator (called "step") can be used to skip a given number of values. For example, "*/3" in the hour time field is equivalent to "0,3,6,9,12,15,18,21".</li>
</ul>

<h3>Examples</h3>
<pre>
 */15 * * * * : Execute job every 15 minutes
 0 2,14 * * *: Execute job every day at 2:00 and 14:00
 0 2 * * 1-5: Execute job at 2:00 of every working day
 0 12 1 */2 1: Execute job every 2 month, at 12:00 of first day of the month OR at every monday.
</pre>

<h3>Script</h3>
<p>You can use the script section to easily create new jobs (by calling a php function)
or to change the scheduling of an existing job.</p>
<p>Every line of the script can be a comment (if it starts with #) or a job definition.</p>
<p>The syntax of a job definition is:</p>
<code>
&lt;-&gt; [rule] &lt;ch:CHANNEL&gt; [job]
</code>
<p>(Tokens between [] are mandatory)</p>
<ul>
<li>&lt;-&gt;: a line starting with "-" means that the job is DISABLED.</li>
<li>[rule]: a crontab schedule rule. See above.</li>
<li>&lt;ch:CHANNEL&gt;: set the channel of the job.</li>
<li>[job]: could be the name of a supported job (for example: 'search_cron') or a function call, ending with ; (for example: 'process_queue();').</li>
</ul>
<p>A comment on the line just preceding a job definition is considered the job description.</p>
<p>Remember that script OVERRIDES all settings on single jobs sections or channel sections of the configuration</p>

<h3>Examples of script</h3>
<pre>
# Search indexing every 2 hours (i'm setting this as the job description)
0 */2 * * * search_cron

# I'll check for module status only on sunday nights
# (and this is will not be the job description, see the empty line below)

0 2 * * 0 update_status_cron

# Trackback ping process every 15min and on a channel called "net"
*/15 * * * * ch:net trackback_cron

# Disable node_cron (i must set the cron rule even if disabled)
- */15 * * * * node_cron

# Launch function send_summary_mail('test@test.com', FALSE); every night
# And set its description to "Send daily summary"
# Send daily summary
0 1 * * *  send_summary_mail('test@test.com', FALSE);
</pre>
EOT
    ),
  );

  $form['prefix_2'] = array(
    '#markup' => '<hr>',
  );

  $form['main'] = array(
    '#title' => t('Main'),
    '#type' => 'fieldset',
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['main']['elysia_cron_disabled'] = array(
    '#title' => t('Global disable'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('elysia_cron_disabled', FALSE),
  );
  $form['main']['elysia_cron_run_maintenance'] = array(
    '#title' => t('Run in maintenance'),
    '#description' => t('Allow to run cron in maintenance mode.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('elysia_cron_run_maintenance', FALSE),
  );

  $form['installation'] = array(
    '#title' => t('Installation settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $options[0] = t('Never / Use external crontab');
  $options += drupal_map_assoc(array(3600, 10800, 21600, 43200, 86400, 604800), 'format_interval');
  $form['installation']['cron_safe_threshold'] = array(
    '#type' => 'select',
    '#title' => t("Run cron on visitor's requests, every"),
    '#default_value' => variable_get('cron_safe_threshold', DRUPAL_CRON_DEFAULT_THRESHOLD),
    '#description' => t('Setting a time here will enable the "poormanscron" method, which runs the Drupal cron operation using normal browser/page requests instead of having to set up a crontab to request the cron.php script. This approach requires that your site gets regular traffic/visitors in order to trigger the cron request.')
    . '<br>'
    . t("This way is fine if you don't need a great control over job starting times and execution frequency.")
    . '<br />'
    . t('If you need fine-grained control over cron timings use the crontab method, as <a href="!cron_url">described in Drupal installation guide</a>.', array('!cron_url' => url('http://drupal.org/cron')))
    . '<br />'
    . t("If you have a very large site, or you need to execute some jobs very often (more than once an hour) refer to Elysia cron's INSTALL.TXT to improve main cron setup."),
    '#options' => $options,
  );

  $form['installation']['elysia_cron_queue_show_count'] = array(
    '#title' => t('Show the number of items in queues'),
    '#description' => t('Some queue backends may have performance issue related with counting items in queue. If you faced with it, just disable this option.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('elysia_cron_queue_show_count', TRUE),
  );

  $form['installation']['cron_key'] = array(
    '#title' => t('Cron key'),
    '#type' => 'textfield',
    '#default_value' => variable_get('cron_key'),
    '#description' => t("This is used to avoid external cron calling. If you set this cron will by accessible only by calling <em>http://site/cron.php?cron_key=XXX</em>, so you'll need to modify system crontab to support this (Logged users with <em>execute elysia_cron</em> permission avoid this check).
    <br>If you left this field empty, you can run cron without cron_key parameter, like this <em>http://site/cron.php</em>, but it <b>HIGHLY NOT RECOMMENDED</b>."),
  );

  $form['installation']['elysia_cron_allowed_hosts'] = array(
    '#title' => t('Allowed hosts'),
    '#type' => 'textfield',
    '#default_value' => variable_get('elysia_cron_allowed_hosts', ''),
    '#description' => t('Insert a list of ip addresses separated by , that can run cron.php (Logged user with [execute elysia_cron] permission avoid this check).'),
  );

  $form['installation']['elysia_cron_default_rule'] = array(
    '#title' => t('Default schedule rule'),
    '#type' => 'textfield',
    '#default_value' => variable_get('elysia_cron_default_rule', FALSE),
    '#description' => t("If you don't specify a rule for a process, and if it has not a module specified one, this rule will apply"),
  );

  if (!ini_get('safe_mode')) {
    $form['installation']['elysia_cron_time_limit'] = array(
      '#title' => t('Time limit'),
      '#type' => 'textfield',
      '#default_value' => variable_get('elysia_cron_time_limit', 240),
      '#description' => t('Set the number of seconds a channel is allowed to run. If you have some jobs that needs more time to execute increase it or set to 0 to disable the limit (WARN: that way a stuck job will block the channel forever!).'),
    );
  }

  $form['installation']['elysia_cron_stuck_time'] = array(
    '#title' => t('Stuck time'),
    '#type' => 'textfield',
    '#default_value' => variable_get('elysia_cron_stuck_time', 3600),
    '#description' => t('How many seconds the process should wait to consider the job as stuck (so the channel can run again)'),
  );

  $form['installation']['elysia_cron_debug_messages'] = array(
    '#title' => t('Debug'),
    '#type' => 'select',
    '#default_value' => variable_get('elysia_cron_debug_messages', 0),
    '#options' => array(
      0 => t('Disabled'),
      1 => t('Enabled'),
    ),
    '#description' => t('Enable extended logging (in watchdog)'),
  );

  $default_rules_human = '';
  $default_rules = variable_get('elysia_cron_default_rules', _elysia_cron_default_rules());
  foreach ($default_rules as $dk => $dr) {
    $default_rules_human .= $dr . ' = ' . $dk . PHP_EOL;
  }

  $form['installation']['elysia_cron_default_rules'] = array(
    '#title' => t('Predefined rules'),
    '#type' => 'textarea',
    '#rows' => 5,
    '#default_value' => $default_rules_human,
    '#description' => t('You can put here standard rules used in your system, each one with its own caption. Put each rule in a separate line, in the form "caption = rule". For example: <i>"every 15 minutes = */15 * * * *"</i>.'),
  );

  $form['installation']['elysia_cron_alert_fieldset'] = array(
    '#title' => t('External cron tracking'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('This lets you use an external tracking system like <a href="http://www.host-tracker.com/">Host Tracker</a> to be used to monitor the health of cron on your site. Point the tracking service to <a href="!cron-ping-url">!cron-ping-url</a>. If Elysia cron has been called within the time interval specified below, the ping page will return HTTP 200.  If not, the ping page will throw a 404 (page not found).', array('!cron-ping-url' => url('admin/build/cron/ping'))),
  );
  $form['installation']['elysia_cron_alert_fieldset']['elysia_cron_alert_interval'] = array(
    '#title' => t('Lapse interval (minutes)'),
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => variable_get('elysia_cron_alert_interval', 60),
    '#description' => t('Specify the number of minutes to allow to lapse before the cron ping page returns a 404 (page not found).'),
  );

  $form['elysia_cron_script_fieldset'] = array(
    '#title' => t('Script'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => !variable_get('elysia_cron_script', ''),
  );
  $form['elysia_cron_script_fieldset']['elysia_cron_script'] = array(
    '#type' => 'textarea',
    '#rows' => 20,
    '#default_value' => variable_get('elysia_cron_script', ''),
    '#description' => t('You can specify new cron jobs or modify existing schedules by adding lines to the script.')
    . '<br />'
    . t('<b>Warning</b> All rules specified in the script will OVERRIDE single job settings and channel settings (sections below).'),
  );

  $form['single_job'] = array(
    '#title' => t('Single job settings'),
    '#description' => '<b>' . t('Disabled') . '</b>: ' . t('Flag this to disable job execution') . '<br />'
    . '<b>' . t('Schedule rule') . '</b>: ' . t('Timing rule for the job. Leave empty to use default rule (shown after the field in parenthesis)') . '<br />'
    . '<b>' . t('Weight') . '</b>: ' . t('Use this to specify execution order: low weights are executed before high weights. Default value shown in parenthesis') . '<br />'
    . '<b>' . t('Channel') . '</b>: ' . t('Specify a channel for the job (create the channel if not exists)') . '<br /><br />',
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );

  $jobchannels = array(
    '#title' => t('Job channel associations'),
    '#description' => t('Leave empty for default channel'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  foreach ($_elysia_cron_settings_by_channel as $channel => $cconf) {
    foreach ($cconf as $job => $conf) {
      if ($job != '#data' && empty($conf['expression'])) {
        $form['single_job']['elysia_cron_' . $job] = array(
          '#title' => $job,
          '#description' => elysia_cron_description($job),
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => !elysia_cron_get_job_rule($job) && !elysia_cron_get_job_weight($job) && !elysia_cron_is_job_disabled($job) && !elysia_cron_get_job_channel($job),
        );

        $rule = elysia_cron_get_job_rule($job);
        $options = array_merge(array('default' => t('Default') . ' (' . (!empty($default_rules[$conf['default_rule']]) ? $default_rules[$conf['default_rule']] : $conf['default_rule']) . ')'), $default_rules);
        if ($rule && !isset($options[$rule])) {
          $options[$rule] = $rule;
        }
        $options['custom'] = t('Custom') . ' ...';

        $form['single_job']['elysia_cron_' . $job]['_elysia_cron_seljob_rule_' . $job] = array(
          '#title' => t('Schedule rule'),
          '#type' => 'select',
          '#options' => $options,
          '#default_value' => $rule ? $rule : 'default',
        );

        $form['single_job']['elysia_cron_' . $job]['_elysia_cron_job_rule_' . $job] = array(
          '#title' => t('Schedule rule'),
          '#type' => 'textfield',
          '#size' => 20,
          '#default_value' => $rule ? $rule : $conf['default_rule'],
        );

        $form['single_job']['elysia_cron_' . $job]['_elysia_cron_job_weight_' . $job] = array(
          '#title' => t('Weight'),
          '#type' => 'textfield',
          '#size' => 4,
          '#default_value' => elysia_cron_get_job_weight($job),
          '#description' => '(' . $conf['default_weight'] . ')',
        );

        $form['single_job']['elysia_cron_' . $job]['_elysia_cron_job_disabled_' . $job] = array(
          '#title' => t('Disabled'),
          '#type' => 'checkbox',
          '#default_value' => elysia_cron_is_job_disabled($job, FALSE),
        );

        $form['single_job']['elysia_cron_' . $job]['_elysia_cron_job_channel_' . $job] = array(
          '#title' => t('Channel'),
          '#type' => 'textfield',
          '#size' => 20,
          '#default_value' => elysia_cron_get_job_channel($job),
        );
      }
    }
  }

  $form['channels'] = array(
    '#title' => t('Channels settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );

  foreach ($_elysia_cron_settings_by_channel as $channel => $conf) {
    $form['channels']['elysia_cron_ch_' . $channel] = array(
      '#title' => $channel,
      '#type' => 'fieldset',
    );
    $form['channels']['elysia_cron_ch_' . $channel]['_elysia_cron_ch_disabled_' . $channel] = array(
      '#title' => t('Disabled'),
      '#type' => 'checkbox',
      '#default_value' => elysia_cron_is_channel_disabled($channel, ''),
    );
    $form['channels']['elysia_cron_ch_' . $channel]['_elysia_cron_ch_rule_' . $channel] = array(
      '#title' => t('Default schedule rule'),
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => elysia_cron_get_channel_rule($channel),
    );
  }

  $form['buttons'] = array('#type' => 'actions');
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['buttons']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset to defaults'),
  );

  if (!empty($_POST) && form_get_errors()) {
    elysia_cron_error('The settings have not been saved because of the errors.');
  }

  return $form;
}

/**
 * Theme function for general settings form.
 *
 * @param array $variables
 *   Theme vars.
 *
 * @return string
 *   Ready for print HTML.
 */
function theme_elysia_cron_settings_form(array &$variables) {
  $form = &$variables['form'];

  $output = '<script type="text/javascript"><!--' . "\n" .
    'function _ec_select(key, select) { if (select.value == \'custom\') {' .
      '$ = jQuery; $("#_ec_select_"+key).hide();$("#_ec_custom_"+key).show();$("#_ec_custom_"+key).focus();' .
    '}}' .
    "\n" . '--></script>';

  $coutput = '<table>';

  $i = 0;
  foreach (element_children($form['single_job']) as $c) {
    $key = substr($c, 12);
    if ($i++ == 0) {
      $coutput .= '<tr>'
        . '<th>' . $form['single_job'][$c]['_elysia_cron_job_disabled_' . $key]['#title'] . '</th>'
        . '<th>' . $form['single_job'][$c]['_elysia_cron_job_rule_' . $key]['#title'] . '</th>'
        . '<th colspan="2">' . $form['single_job'][$c]['_elysia_cron_job_weight_' . $key]['#title'] . '</th>'
        . '<th>' . $form['single_job'][$c]['_elysia_cron_job_channel_' . $key]['#title'] . '</th>'
        . '</tr>';
    }

    $def_weight = $form['single_job'][$c]['_elysia_cron_job_weight_' . $key]['#description'];

    $posted_key = $form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]['#name'];
    $posted_val = !empty($_REQUEST[$posted_key]) ? $_REQUEST[$posted_key] : FALSE;

    $form['single_job'][$c]['_elysia_cron_job_rule_' . $key]['#prefix'] = '<span id="_ec_custom_' . $key . '" style="' . ($posted_val != 'custom' ? 'display: none;' : '') . '">';
    $form['single_job'][$c]['_elysia_cron_job_rule_' . $key]['#suffix'] = '</span>';
    $form['single_job'][$c]['_elysia_cron_job_rule_' . $key]['#title'] = NULL;
    $form['single_job'][$c]['_elysia_cron_job_rule_' . $key]['#description'] = NULL;
    $form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]['#prefix'] = '<span id="_ec_select_' . $key . '" style="' . ($posted_val == 'custom' ? 'display: none;' : '') . '">';
    $form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]['#suffix'] = '</span>';
    $form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]['#title'] = NULL;
    $form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]['#description'] = NULL;
    $form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]['#attributes']['onchange'] = '_ec_select(\'' . $key . '\', this)';

    $form['single_job'][$c]['_elysia_cron_job_weight_' . $key]['#title'] = NULL;
    $form['single_job'][$c]['_elysia_cron_job_weight_' . $key]['#description'] = NULL;
    $form['single_job'][$c]['_elysia_cron_job_weight_' . $key]['#attributes']['style'] = 'margin: 0';
    $form['single_job'][$c]['_elysia_cron_job_disabled_' . $key]['#title'] = NULL;
    $form['single_job'][$c]['_elysia_cron_job_disabled_' . $key]['#attributes']['style'] = 'margin: 0';
    $form['single_job'][$c]['_elysia_cron_job_channel_' . $key]['#title'] = NULL;
    $form['single_job'][$c]['_elysia_cron_job_channel_' . $key]['#attributes']['style'] = 'margin: 0';

    $coutput .= '<tr><td colspan="6"><b>' . $form['single_job'][$c]['#title'] . '</b>' . (($d = $form['single_job'][$c]['#description']) && $d != '-' ? ' <i>(' . $d . ')</i>' : '') . '</td></tr>';
    $coutput .= '<tr>'
      . '<td align="center">' . drupal_render($form['single_job'][$c]['_elysia_cron_job_disabled_' . $key]) . '</td>'
      . '<td>' . drupal_render($form['single_job'][$c]['_elysia_cron_seljob_rule_' . $key]) . drupal_render($form['single_job'][$c]['_elysia_cron_job_rule_' . $key]) . '</td>'
      . '<td>' . drupal_render($form['single_job'][$c]['_elysia_cron_job_weight_' . $key]) . '</td><td><small>' . $def_weight . '</small></td>'
      . '<td>' . drupal_render($form['single_job'][$c]['_elysia_cron_job_channel_' . $key]) . '</td>'
      . '</tr>';
    drupal_render($form['single_job'][$c]);
  }
  $coutput .= '</table>';

  $form['single_job']['#children'] = $coutput;

  $coutput = '<table>';

  $i = 0;
  foreach (element_children($form['channels']) as $c) {
    $key = substr($c, 15);
    if ($i++ == 0) {
      $coutput .= '<tr>'
        . '<th>' . t('Name') . '</th>'
        . '<th>' . $form['channels'][$c]['_elysia_cron_ch_disabled_' . $key]['#title'] . '</th>'
        . '<th>' . $form['channels'][$c]['_elysia_cron_ch_rule_' . $key]['#title'] . '</th>'
        . '</tr>';
    }

    $form['channels'][$c]['_elysia_cron_ch_disabled_' . $key]['#title'] = NULL;
    $form['channels'][$c]['_elysia_cron_ch_disabled_' . $key]['#attributes']['style'] = 'margin: 0';
    $form['channels'][$c]['_elysia_cron_ch_rule_' . $key]['#title'] = NULL;
    $form['channels'][$c]['_elysia_cron_ch_rule_' . $key]['#attributes']['style'] = 'margin: 0';

    $coutput .= '<tr>'
      . '<td><b>' . $form['channels'][$c]['#title'] . '</b></td>'
      . '<td>' . drupal_render($form['channels'][$c]['_elysia_cron_ch_disabled_' . $key]) . '</td>'
      . '<td>' . drupal_render($form['channels'][$c]['_elysia_cron_ch_rule_' . $key]) . '</td>'
      . '</tr>';
    drupal_render($form['channels'][$c]);
  }
  $coutput .= '</table>';

  $form['channels']['#children'] = $coutput;

  return $output . drupal_render_children($form);
}

/**
 * Validate handler for 'elysia_cron_settings_form' form.
 *
 * @param array $form
 *   Form API array.
 * @param array $form_state
 *   Filled form_state array with input values.
 */
function elysia_cron_settings_form_validate(array $form, array &$form_state) {
  $values = $form_state['values'];

  $script = $form_state['values']['elysia_cron_script'];
  if ($script) {
    $errors = elysia_cron_decode_script($script, FALSE);
    if ($errors) {
      form_set_error('elysia_cron_script', t('Invalid lines:') . implode('<br>', $errors));
    }
  }

  foreach ($values as $key => $value) {
    if ($value && ((preg_match('/^_elysia_cron_([^_]+_[^_]+)_(.*)$/', $key, $r) && ($r[1] == 'job_rule' || $r[1] == 'ch_rule')) || $key == 'elysia_cron_default_rule')) {
      if (!preg_match('/^\\s*([0-9*,\/-]+[ ]+[0-9*,\/-]+[ ]+[0-9*,\/-]+[ ]+[0-9*,\/-]+[ ]+[0-9*,\/-]+)\\s*$/', $value)) {
        form_set_error($key, t('Invalid rule: %rule', array('%rule' => $value)));
      }
    }
  }

  if (!empty($values['elysia_cron_default_rules'])) {
    $rules = explode(PHP_EOL, $values['elysia_cron_default_rules']);
    foreach ($rules as $rule) {
      $rule = trim($rule);
      if (empty($rule)) {
        continue;
      }

      $rule = explode('=', $rule);
      if (empty($rule[1])) {
        form_set_error('elysia_cron_default_rules', t('Invalid rule: %rule', array('%rule' => $rule[0])));
      }
      elseif (!preg_match('/^\\s*([0-9*,\/-]+[ ]+[0-9*,\/-]+[ ]+[0-9*,\/-]+[ ]+[0-9*,\/-]+[ ]+[0-9*,\/-]+)\\s*$/', trim($rule[1]))) {
        form_set_error('elysia_cron_default_rules', t('Invalid rule: %rule', array('%rule' => $rule[0])));
      }
    }
  }
}

/**
 * Submit handler for 'elysia_cron_settings_form' form.
 *
 * @param array $form
 *   Form API array.
 * @param array $form_state
 *   Filled form_state array with input values.
 */
function elysia_cron_settings_form_submit(array $form, array &$form_state) {
  $form_values = $form_state['values'];

  $op = isset($form_values['op']) ? $form_values['op'] : '';

  // Exclude unnecessary elements.
  unset($form_values['submit'], $form_values['reset'], $form_values['form_id'], $form_values['op'], $form_values['form_token']);

  $elysia_cron_default_rules = array();
  $rules = explode(PHP_EOL, $form_values['elysia_cron_default_rules']);
  foreach ($rules as $r) {
    if (trim($r)) {
      $rr = explode("=", $r);
      $elysia_cron_default_rules[trim($rr[1])] = trim($rr[0]);
    }
  }
  variable_set('elysia_cron_default_rules', $elysia_cron_default_rules);

  foreach ($form_values as $key => $value) {
    $value = trim($value);
    if (!preg_match('/^_elysia_cron_([^_]+_[^_]+)_(.*)$/', $key, $r)) {
      if ($op == t('Reset to defaults') || ($key != 'cron_safe_threshold' && !$value)) {
        variable_del($key);
      }
      elseif ($key != 'elysia_cron_default_rules') {
        if (is_array($value) && isset($form_values['array_filter'])) {
          $value = array_keys(array_filter($value));
        }
        variable_set($key, $value);
      }
    }
    else {
      $nullvalue = $r[1] != 'job_weight' ? !$value : !$value && $value !== '0';

      if ($op == t('Reset to defaults') || $nullvalue) {
        switch ($r[1]) {
          case 'job_channel':
            elysia_cron_reset_job_channel($r[2]);
            break;

          case 'job_rule':
            elysia_cron_reset_job_rule($r[2]);
            break;

          case 'job_weight':
            elysia_cron_reset_job_weight($r[2]);
            break;

          case 'job_disabled':
            elysia_cron_reset_job_disabled($r[2]);
            break;

          case 'ch_disabled':
            elysia_cron_reset_channel_disabled($r[2]);
            break;

          case 'ch_rule':
            elysia_cron_reset_channel_rule($r[2]);
            break;
        }
      }
      else {
        switch ($r[1]) {
          case 'job_channel':
            elysia_cron_set_job_channel($r[2], $value);
            break;

          case 'job_rule':
            if ($form_values['_elysia_cron_seljob_rule_' . $r[2]] == 'custom') {
              elysia_cron_set_job_rule($r[2], $value);
            }
            break;

          case 'seljob_rule':
            if ($value != 'custom') {
              if ($value == 'default') {
                elysia_cron_reset_job_rule($r[2]);
              }
              else {
                elysia_cron_set_job_rule($r[2], $value);
              }
            }
            break;

          case 'job_weight':
            elysia_cron_set_job_weight($r[2], $value);
            break;

          case 'job_disabled':
            elysia_cron_set_job_disabled($r[2], $value);
            break;

          case 'ch_disabled':
            elysia_cron_set_channel_disabled($r[2], $value);
            break;

          case 'ch_rule':
            elysia_cron_set_channel_rule($r[2], $value);
            break;
        }
      }

    }
  }

  if ($op == t('Reset to defaults')) {
    elysia_cron_message('The configuration options have been reset to their default values.');
  }
  else {
    elysia_cron_message('The configuration options have been saved.');
  }
}

/**
 * Build time in "ago" format.
 *
 * @param int $timestamp
 *   Time of latest cron.
 *
 * @return string
 *   Date in 'ago' format.
 */
function elysia_cron_date($timestamp) {
  return $timestamp > 0 ? t('!time ago', array('!time' => format_interval(REQUEST_TIME - $timestamp, 2))) : t('n/a');
}

/**
 * Form builder for cron run form.
 *
 * @return array
 *   From API array.
 */
function elysia_cron_run_form() {
  $form['runf'] = array(
    '#type' => 'fieldset',
    '#access' => elysia_cron_access('execute elysia_cron'),
  );
  $form['runf']['run'] = array(
    '#type' => 'submit',
    '#value' => t('Run cron'),
  );

  return $form;
}

/**
 * Submit handler for 'elysia_cron_run_form' form.
 *
 * @param array $form
 *   Form API array.
 * @param array $form_state
 *   Filled form_state array with input values.
 */
function elysia_cron_run_form_submit(array $form, array &$form_state) {
  // Run cron manually from Cron form.
  if (elysia_cron_run(TRUE)) {
    elysia_cron_message('Cron run successfully.');
  }
  else {
    elysia_cron_error('Cron run failed.');
  }

  drupal_goto('admin/config/system/cron');
}

/**
 * Page callback for 'admin/config/system/cron/execute/%' path.
 *
 * @param string $job
 *   Name of cron job to be executed.
 */
function elysia_cron_execute_page($job = '') {
  if (!$job) {
    elysia_cron_error('No job specified');
    drupal_goto('admin/config/system/cron');
  }

  // Run also if disabled or not ready (but not if it's already running).
  elysia_cron_run_job($job, TRUE, TRUE, FALSE);

  drupal_goto('admin/config/system/cron');
}

/**
 * Form builder for statistic reset form.
 *
 * @return array
 *   From API array.
 */
function elysia_cron_reset_statistics_form() {
  $form['fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reset statistics'),
    '#description' => t('Deletes all cron execution statistics (Last run, last exec time, exec count, avg/max exec time...). Do not touch cron settings.<br /><b>This operation can not be reverted</b><br />'),
  );

  $form['fieldset']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#attributes' => array(
      'onclick' => 'return confirm(\'' . htmlentities(t('Are you sure you want to reset statistics?')) . '\')',
    ),
  );

  return $form;
}

/**
 * Submit handler for 'elysia_cron_reset_statistics_form' form.
 *
 * @param array $form
 *   Form API array.
 * @param array $form_state
 *   Filled form_state array with input values.
 */
function elysia_cron_reset_statistics_form_submit(array $form, array &$form_state) {
  elysia_cron_reset_stats();

  elysia_cron_message('Reset done.');
  drupal_goto('admin/config/system/cron/maintenance');
}
