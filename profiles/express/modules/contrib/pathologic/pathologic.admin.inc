<?php

/**
 * @file
 * Configuration form for Pathologic.
 */

/**
 * Form callback for the Pathologic configuration form.
 */
function pathologic_configuration_form($form, $form_state) {
  $form['reminder'] = array(
    '#type' => 'markup',
    '#markup' => '<p>' . t('Reminder: The settings on this form only affect text formats for which Pathologic is configured to use the global Pathologic settings; if it&rsquo;s configured to use per-format settings, these settings will have no effect. If you have upgraded to the current 7.x-3.x version of Pathologic from version 7.x-2.x, all of your current formats will use their existing per-format settings by default, so that behavior will be the same as the 7.x-2.x version until they are configured to use the global settings instead. Please <a href="!docs">consult Pathologic&rsquo;s documentation</a> for a step-by-step guide to configuring Pathologic.', array('!docs' => 'https://www.drupal.org/node/257026')) . '</p>',
    '#weight' => 0,
  );
  $config_form = _pathologic_configuration_form(array(
    'protocol_style' => variable_get('pathologic_protocol_style', 'full'),
    'local_paths' => variable_get('pathologic_local_paths', ''),
  ));
  $form['pathologic_protocol_style'] = $config_form['protocol_style'];
  $form['pathologic_local_paths'] = $config_form['local_paths'];
  return system_settings_form($form);
}

/**
 * Common elements for the Pathologic configuration form.
 *
 * This reduces redundancy in code for form elements that will appear on both
 * the global settings form and the per-format filter settings form.
 *
 * @param $defaults
 *   An array of default values for the configuration form fields.
 * @return
 *   The common form elements
 */
function _pathologic_configuration_form($defaults) {
  return array(
    'protocol_style' => array(
      '#type' => 'radios',
      '#title' => t('Processed URL format'),
      '#default_value' => $defaults['protocol_style'],
      '#options' => array(
        'full' => t('Full URL (<code>http://example.com/foo/bar</code>)'),
        'proto-rel' => t('Protocol relative URL (<code>//example.com/foo/bar</code>)'),
        'path' => t('Path relative to server root (<code>/foo/bar</code>)'),
      ),
      '#description' => t('The <em>Full URL</em> option is best for stopping broken images and links in syndicated content (such as in RSS feeds), but will likely lead to problems if your site is accessible by both HTTP and HTTPS. Paths output with the <em>Protocol relative URL</em> option will avoid such problems, but feed readers and other software not using up-to-date standards may be confused by the paths. The <em>Path relative to server root</em> option will avoid problems with sites accessible by both HTTP and HTTPS with no compatibility concerns, but will absolutely not fix broken images and links in syndicated content.'),
      '#weight' => 10,
    ),
    'local_paths' => array(
      '#type' => 'textarea',
      '#title' =>  t('All base paths for this site'),
      '#default_value' => $defaults['local_paths'],
        '#description' => t('If this site is or was available at more than one base path or URL, enter them here, separated by line breaks. For example, if this site is live at <code>http://example.com/</code> but has a staging version at <code>http://dev.example.org/staging/</code>, you would enter both those URLs here. If confused, please read <a href="!docs">Pathologic&rsquo;s documentation</a> for more information about this option and what it affects.', array('!docs' => 'https://www.drupal.org/node/257026')),
      '#weight' => 20,
    ),
  );
}

