<?php

function cuminimal_form_system_theme_settings_alter(&$form, &$form_state) {
  $theme = $form_state['build_info']['args'][0];
	$form['expressbase_theme_settings']['layout'] = array(
		'#type' => 'fieldset',
		'#title' => t('Layout'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);

	$form['expressbase_theme_settings']['layout']['layout_style'] = array(
	  '#type' => 'select',
	  '#title' => t('Layout Style'),
	  '#default_value' => theme_get_setting('layout_style', $theme) ? theme_get_setting('layout_style', $theme) : 'layout-wide',
	  '#description' => t('Pick a layout style for your site.'),
	  '#options' => array(
      'layout-wide' => t('Wide'),
      'layout-boxed' => t('Boxed'),
    ),
	);
}
