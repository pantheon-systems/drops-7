<?php

function culayers_form_system_theme_settings_alter(&$form, &$form_state) {
  $theme = $form_state['build_info']['args'][0];
	$form['culayers_theme_settings'] = array(
		'#type' => 'fieldset', 
		'#title' => t('Layers Theme Settings'), 
		'#collapsible' => TRUE, 
		'#collapsed' => TRUE,
	);
	
	$form['culayers_theme_settings']['layout_style'] = array(
	  '#type' => 'radios', 
	  '#title' => t('Layout Style'), 
	  '#default_value' => theme_get_setting('layout_style', $theme) ? theme_get_setting('layout_style', $theme) : 'layout-wide', 
	  '#description' => t('Pick a layout style for your site.'),
	  '#options' => array(
      'layout-wide' => t('Wide'),
      'layout-boxed' => t('Boxed'),
    ),
	);
}