<?php

function cuswatch_form_system_theme_settings_alter(&$form, &$form_state) {
  $theme = $form_state['build_info']['args'][0];
  $form['cu_omega_theme_settings']['page_title_image']['page_title_image_width']['#access'] = FALSE;
	$form['cuswatch_theme_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Swatch Theme Settings'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);

	$form['cuswatch_theme_settings']['layout_style'] = array(
	  '#type' => 'radios',
	  '#title' => t('Layout Style'),
	  '#default_value' => theme_get_setting('layout_style', 'cuswatch') ? theme_get_setting('layout_style', 'cuswatch') : 'layout-wide',
	  '#description' => t('Pick a layout style for your site.'),
	  '#options' => array(
      'layout-wide' => t('Wide'),
      'layout-boxed' => t('Boxed'),
    ),
	);
}
