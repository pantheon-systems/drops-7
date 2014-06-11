<?php
/**
 * @file
 * Content type enables user to place the selected service links
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_node_form.png',
  'title' => t('Service links Pane'),
  'description' => t('Provides services links'),
  'category' => t('Widgets'),
  'edit form' => 'service_links_service_links_content_type_edit_form',
  'render callback' => 'service_links_service_links_content_type_render'
);

/**
 * Output function for the 'service_links' content type.
 */
function service_links_service_links_content_type_render($subtype, $conf, $panel_args, &$context) {
  $content = NULL;
  $node = menu_get_object('node');

  $style = $conf['service_links_block_style'];
  $panel_services = array();
  $settings = _service_links_load_settings();
  $services = service_links_get_links($settings['link_show']);

  foreach ($services as $service_id => $service) {
    if (isset($conf[$service_id]) && $conf[$service_id]) {
      $panel_services[$service_id] = $service;
    }
  }

  if (isset($node)) {
    switch ($conf['service_links_style']) {
      case 'service_links':
        $content = theme('service_links_block_format', array(
          'items' => panel_service_links_render($node, FALSE, $style, $panel_services),
          'style' => $style)
        );
        break;
      case 'service_links_fisheye':
        $content = theme('service_links_fisheye_format', array(
          'items' => panel_service_links_render($node, FALSE, SERVICE_LINKS_STYLE_FISHEYE, $panel_services))
        );
        break;
    }
  }
  elseif (!isset($node)) {
    switch ($conf['service_links_style']) {
      case 'service_links':
        $options = array(
          'style' => $style,
          'link_to_front' => variable_get('service_links_block_not_node_front', FALSE),
        );
        $content = theme('service_links_block_format', array(
          'items' => panel_service_links_render(NULL, FALSE, $options, $panel_services),
          'style' => $style,
            ));
        break;
      case 'service_links_fisheye':
        $content = theme('service_links_fisheye_format', array(
          'items' => panel_service_links_render(NULL, FALSE, SERVICE_LINKS_STYLE_FISHEYE, $panel_services))
        );
        break;
    }
  }

  // Build the content type block.
  $block = new stdClass();
  $block->content = $content;

  return $block;
}

/**
 * Helper function to render the output of the pane.
 *
 * @param $node
 *   The node on which the links are to be rendered if available, or NULL.
 * @param nodelink
 *   The link to the node if available, or FALSE.
 * @param $options
 *   The style for rendering the service links, fisheye or not.
 * @param $services
 *   An array of services to display.
 */
function panel_service_links_render($node, $nodelink = FALSE, $options = NULL, $services = array()) {
  $links = array('weight' => array(), 'link' => array());
  $settings = _service_links_load_settings();

  if (empty($settings['link_show'])) {
    return array();
  }

  if (isset($options)) {
    if (!is_array($options)) {
      $options = array('style' => $options);
    }
    $settings = array_merge($settings, $options);
  }

  _service_links_get_tags($node, $settings);

  foreach ($services as $service_id => $service) {
    // Load the position.
    $links['weight'][] = isset($settings['link_weight'][$service_id]) ? $settings['link_weight'][$service_id] : 0;

    // Render the Service.
    $links['link'] += _service_links_render($service_id, $service, $settings, $nodelink, $node);
  }

  if (!empty($links['link'])) {
    array_multisort($links['weight'], $links['link']);
  }

  return !empty($links['link']) ? $links['link'] : array();
}

/**
 * The form to add or edit a service_links as content.
 */
function service_links_service_links_content_type_edit_form($form, &$form_state) {
  ctools_include('plugins');
  $conf = $form_state['conf'];

  $form['service_links_style'] = array(
    '#type' => 'select',
    '#title' => t('Service link icon style'),
    '#options' => array(
      'service_links' => t('Normal service links display'),
      'service_links_fisheye' => t('Service links with FishEye effect'),
    ),
    '#default_value' => isset($conf['service_links_style']) ? $conf['service_links_style'] : TRUE,
  );

  $form['service_links_block_style'] = array(
    '#type' => 'select',
    '#title' => t('Service link display'),
    '#options' => array(
      1 => t('Only text'),
      2 => t('Only icon'),
      3 => t('Icon and text'),
      4 => t('Empty'),
    ),
    '#default_value' => isset($conf['service_links_block_style']) ? $conf['service_links_block_style'] : TRUE,
    '#states' => array(
      'invisible' => array(
        ':input[name="service_links_style"]' => array(
          'value' => 'service_links_fisheye'
        )
      )
    ),
  );

  $settings = _service_links_load_settings();
  $services = service_links_get_links($settings['link_show']);

  $form['panel_service_links'] = array('#theme' => 'service_links_drag_table');
  $form['panel_service_links']['panel_service_links_show'] = array('#tree' => TRUE);
  $form['panel_service_links']['panel_service_links_weight'] = array('#tree' => TRUE);

  foreach ($services as $service_id => $service) {
    $icon = array(
      'path' => isset($service['icon']) ? service_links_expand_path($service['icon'], 'preset') : service_links_expand_path("$service_id.png", +'preset'),
    );

    $form[$service_id] = array(
      '#service' => ucwords(str_replace('_', ' ', $service['module'])),
      '#type' => 'checkbox',
      '#title' => theme('image', $icon) . " " . t('Show %name link', array('%name' => $service['name'])),
      '#return_value' => 1,
      '#default_value' => isset($conf[$service_id]) ? $conf[$service_id] : 0,
    );
  }

  if (empty($services)) {
    drupal_set_message(t('You need to load at least one of XXX Services module, please enable them in <a href="@url">admin > modules</a> page', +array('@url' => url('admin/modules'))), 'warning');
  }

  return $form;
}

/**
 * Store the service_links selection.
 */
function service_links_service_links_content_type_edit_form_submit($form, &$form_state) {
  $form_state['conf']['service_links_style'] = $form_state['values']['service_links_style'];
  $form_state['conf']['service_links_block_style'] = $form_state['values']['service_links_block_style'];
  $settings = _service_links_load_settings();
  $services = service_links_get_links($settings['link_show']);

  foreach ($services as $service_id => $service) {
    if (isset($form_state['values'][$service_id]))
      $form_state['conf'][$service_id] = $form_state['values'][$service_id];
  }
}
