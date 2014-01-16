<?php
/**
 * @file
 * Theme for Yottaa.
 */

/**
 * Generates the output.
 *
 * @param array $variables
 *   The variables.
 *
 * @return array
 *   A render array.
 */
function theme_yottaa_info($variables) {
  $yottaa_api = yottaa_api_drupal();
  drupal_add_css(drupal_get_path('module', 'yottaa') . '/css/yottaa.css');
  $output = array(
    'header' => array(
      '#type' => 'markup',
      '#prefix' => '<h1>',
      '#markup' => theme('image', array('path' => drupal_get_path('module', 'yottaa') . '/img/logo320.png')) . '<strong>My Yottaa Page</strong></h1>',
      '#postfix' => '</h1>',
    ),
  );

  $parameters = $yottaa_api->getParameters();
  $yottaa_site_id = $parameters['site_id'];
  $yottaa_user_id = $parameters['user_id'];
  $yottaa_api_key = $parameters['api_key'];

  $json_output = $yottaa_api->getStatus();
  $yottaa_status = $json_output["optimizer"];

  if (!isset($json_output["error"])) {
    $yottaa_preview_url = $json_output["preview_url"];
    if ($yottaa_status == 'preview') {
      $output['status'] = array(
        '#type' => 'markup',
        '#markup' => '<div>Your site is currently in <span class="status-preview">' . $yottaa_status . '</span>. This allows you to access an optimized'
        . ' version of your website using the preview URL (<a href="' . $yottaa_preview_url . '" target="_blank">' . $yottaa_preview_url . '</a>).'
        . ' Before making your site live look over the links and configuration below.</div>',
      );
    }
    elseif ($yottaa_api->isLive($yottaa_status)) {
      $output['status'] = array(
        '#type' => 'markup',
        '#markup' => '<div>Your site is currently in <span class="status-live">Live</span>.</div>',
      );
    }
    elseif ($yottaa_api->isBypass($yottaa_status)) {
      $output['status'] = array(
        '#type' => 'markup',
        '#markup' => '<div>Your site is currently in <span class="status-paused">Bypass Mode</span>.</div>',
      );
    }
    elseif ($yottaa_api->isTransparent($yottaa_status)) {
      $output['status'] = array(
        '#type' => 'markup',
        '#markup' => '<div>Your site is currently in <span class="status-paused">Transparent Proxy Mode</span>.</div>',
      );
    }
  }
  else {
    $error = $json_output["error"];
    $output['status'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="status-error">',
      '#markup' => '<div class="status-error">Error: ' . json_encode($error) . '</div>',
    );
  }

  $output['links'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Links</h3>',
  );

  $output['link_a'] = array(
    '#type' => 'markup',
    '#markup' => '<div><a href="https://apps.yottaa.com/" target="_blank">Yottaa Dashboard</a></div>',
  );

  $output['link_b'] = array(
    '#type' => 'markup',
    '#markup' => '<div><a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/optimizer" target="_blank">Yottaa Site Overview</a></div>',
  );

  $output['link_c'] = array(
    '#type' => 'markup',
    '#markup' => '<div><a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings" target="_blank">Yottaa Optimization Configuration</a></div>',
  );

  $output['actions'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Actions</h3><h5>Switch Optimizer Mode</h5>',
  );

  if (!isset($json_output["error"])) {
    if ($yottaa_status == 'preview') {
      $output['action_a'] = array(
        '#type' => 'markup',
        '#markup' => drupal_render((drupal_get_form('yottaa_optimizer_activate_form'))),
      );
    }
    elseif ($yottaa_api->isLive($yottaa_status)) {
      $output['action_a'] = array(
        '#type' => 'markup',
        '#markup' => drupal_render((drupal_get_form('yottaa_optimizer_pause_form'))),
      );
      $output['action_c'] = array(
        '#type' => 'markup',
        '#markup' => drupal_render((drupal_get_form('yottaa_optimizer_transparent_form'))),
      );
    }
    elseif ($yottaa_api->isPaused($yottaa_status)) {
      $output['action_a'] = array(
        '#type' => 'markup',
        '#markup' => drupal_render((drupal_get_form('yottaa_optimizer_resume_form'))),
      );
    }
    $output['actions_other'] = array(
      '#type' => 'markup',
      '#markup' => '<h5>Other Actions</h5>',
    );
    $output['action_b'] = array(
      '#type' => 'markup',
      '#markup' => drupal_render((drupal_get_form('yottaa_clear_cache_form'))),
    );
  }

  $output['settings'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Settings</h3>',
  );

  $output['setting_a'] = array(
    '#type' => 'markup',
    '#markup' => drupal_render((drupal_get_form('yottaa_auto_clear_cache_form'))),
  );

  $output['setting_b'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="setting"><span class="setting-item-title">User Id</span>' . $yottaa_user_id . '</div>',
  );

  $output['setting_c'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="setting"><span class="setting-item-title">API Key</span>' . $yottaa_api_key . '</div>',
  );

  $output['setting_d'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="setting"><span class="setting-item-title">Site Id</span>' . $yottaa_site_id . '</div>',
  );

  $output['setting_e'] = array(
    '#type' => 'markup',
    '#markup' => '<div class="setting">' . l(t('Advanced Configuration'), 'admin/config/services/yottaa/settings') . '</div>',
  );

  $output['checklist'] = array(
    '#type' => 'markup',
    '#markup' => '<h3>Checklist</h3>',
  );

  $settings = $yottaa_api->getSettings();

  if (!isset($settings["error"])) {

    $output['checklist_items'] = array(
      '#type' => 'markup',
      '#prefix' => '<div class="checklist-items">',
      '#postfix' => '</div>',
    );

    // Site pages.
    $checklist_items_1 = '<div class="checklist-item"><span class="checklist-item-title">Enable caching of site pages</span>';
    if ($settings["site_pages_caching"] == 'included') {
      $checklist_items_1 .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $checklist_items_1 .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $checklist_items_1 .= '<div class="checklist-item"><span class="checklist-item-helper">Turn on <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a>.</div>';
    }

    $output['checklist_items_1'] = array(
      '#type' => 'markup',
      '#markup' => $checklist_items_1,
    );

    // Only cache pages for anonymous users.
    $checklist_items_2 = '<div class="checklist-item"><span class="checklist-item-title">Only cache pages for anonymous users</span>';
    if ($settings["only_cache_anonymous_users"] == 'excluded') {
      $checklist_items_2 .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $checklist_items_2 .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $checklist_items_2 .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> which excludes resource from optimization if Request-Header whose name equals to "Cookie" and whose value contains "SESS".</div>';
    }

    $output['checklist_items_2'] = array(
      '#type' => 'markup',
      '#markup' => $checklist_items_2,
    );

    // Edit pages.
    $checklist_items_3 = '<div class="checklist-item"><span class="checklist-item-title">Disable caching of node editing pages</span>';
    if ($settings["edit_pages_caching"] == 'excluded') {
      $checklist_items_3 .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $checklist_items_3 .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $checklist_items_3 .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exclude exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> with a "contains" expression of <strong>/edit</strong>.</div>';
    }

    $output['checklist_items_3'] = array(
      '#type' => 'markup',
      '#markup' => $checklist_items_3,
    );

    // Configure pages.
    $checklist_items_4 = '<div class="checklist-item"><span class="checklist-item-title">Disable caching of configuration pages</span>';
    if ($settings["configure_pages_caching"] == 'excluded') {
      $checklist_items_4 .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }
    else {
      $checklist_items_4 .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $checklist_items_4 .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exclude exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> with a "contains" expression of <strong>admin/</strong>.</div>';
      $checklist_items_4 .= '<div class="checklist-item"><span class="checklist-item-helper">Add an exclude exception into the <a href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '/settings?tab=1" target="_blank">HTML Caching</a> with a "contains" expression of <strong>admin%252F</strong>.</div>';
    }

    $output['checklist_items_4'] = array(
      '#type' => 'markup',
      '#markup' => $checklist_items_4,
    );

    // Proxy mode.
    $checklist_items_5 = '<div class="checklist-item"><span class="checklist-item-title">Enable drupal proxy mode</span>';
    if (variable_get('reverse_proxy', 0) == 0 && $yottaa_api->isLive($yottaa_status)) {
      $checklist_items_5 .= '<span class="checklist-item-status checklist-item-status-failed">Failed</span></div>';
      $checklist_items_5 .= '<div class="checklist-item"><span class="checklist-item-helper">Need to enable the proxy mode for drupal using the form in Settings section.<div>';
    }
    else {
      $checklist_items_5 .= '<span class="checklist-item-status checklist-item-status-passed">Passed</span></div>';
    }

    $output['checklist_items_5'] = array(
      '#type' => 'markup',
      '#markup' => $checklist_items_5,
    );
  }

  return $output;
}

/**
 * Custom theme for yottaa optimizer activate form.
 *
 * @param array $variables
 *   The variables.
 */
function theme_yottaa_optimizer_activate_form($variables) {
  $yottaa_site_id = variable_get('yottaa_site_id', '');
  $active_link = '<a class="button" href="https://apps.yottaa.com/framework/web/sites/' . $yottaa_site_id . '" target="_blank"> Activate Optimizations</a>';
  $rows = array(array($active_link, t("Activating your site allows all e-commerce visitors to receive the benefits out Yottaa's site speed optimizer.")));
  return theme('table', array('rows' => $rows));
}

/**
 * Custom theme for yottaa optimizer bypass form.
 *
 * @param array $variables
 *   The variables.
 */
function theme_yottaa_optimizer_pause_form($variables) {
  $form = $variables['form'];
  $rows = array(array(drupal_render($form['submit']), t('In Bypass Mode, Yottaa DNS will route all traffic to your origin server directly, by-passing optimizations, SSL termination and origin server shielding etc. Your origin servers IP address are visible to anyone in this mode.')));
  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa optimizer transparent form.
 *
 * @param array $variables
 *   The variables.
 */
function theme_yottaa_optimizer_transparent_form($variables) {
  $form = $variables['form'];
  $rows = array(array(drupal_render($form['submit']), t('In Transparent Proxy Mode, Yottaa will proxy your site traffic through the Yottaa Network without applying any optimization. Other features such as SSL termination, site protection and Private Test are in effect as usual. Your origin servers IP address are shielded by Yottaa.')));
  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa optimizer resume form.
 *
 * @param array $variables
 *   The variables.
 */
function theme_yottaa_optimizer_resume_form($variables) {
  $form = $variables['form'];
  $rows = array(array(drupal_render($form['submit']), t('Starting optimization will apply optimizations on your website within 5 minutes.')));
  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa clear cache form.
 *
 * @param array $variables
 *   The variables.
 */
function theme_yottaa_clear_cache_form($variables) {
  $form = $variables['form'];
  $rows = array(array(drupal_render($form['submit']), t('Clearing the cache will remove all of your sites resources from our CDN. Use this option if you have updated a resource (gif, css, JavaScript).')));
  return theme('table', array('rows' => $rows)) . drupal_render_children($form);
}

/**
 * Custom theme for yottaa auto clear cache setting form.
 *
 * @param array $variables
 *   The variables.
 */
function theme_yottaa_auto_clear_cache_form($variables) {
  $form = $variables['form'];
  $rows = array(array(drupal_render($form['submit'])));
  return drupal_render_children($form) . theme('table', array('rows' => $rows));
}
