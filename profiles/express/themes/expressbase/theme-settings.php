<?php

function expressbase_form_system_theme_settings_alter(&$form, &$form_state) {
  $theme = $form_state['build_info']['args'][0];
	$form['expressbase_theme_settings'] = array(
		'#type' => 'fieldset',
		'#title' => t('Theme Settings'),
    '#description' => 'Customizable options for the design and layout of site content.',
	);
  $collapsed = isset($_GET['responsive']) ? FALSE : TRUE;
  $form['expressbase_theme_settings']['responsive'] = array(
		'#type' => 'fieldset',
		'#title' => t('Responsive/Mobile Friendly'),
		'#collapsible' => TRUE,
		'#collapsed' => $collapsed,
	);
	$form['expressbase_theme_settings']['responsive']['alpha_responsive'] = array(
	  '#type' => 'checkbox',
	  '#title' => t('Enable responsive/mobile friendly design'),
	  '#default_value' => theme_get_setting('alpha_responsive', $theme),
	);
	$form['expressbase_theme_settings']['responsive']['primary_sidebar'] = array(
	  '#type' => 'select',
	  '#title' => t('Primary Sidebar'),
	  '#default_value' => theme_get_setting('primary_sidebar', $theme) ? theme_get_setting('primary_sidebar', $theme) : 'primary-sidebar-second',
	  '#options' => array(
      'primary-sidebar-first' => t('First/Left'),
      'primary-sidebar-second' => t('Second/Right'),
    ),
    '#description' => 'This setting sets which is the primary sidebar for tablet displays',
	);
	$form['expressbase_theme_settings']['typography'] = array(
		'#type' => 'fieldset',
		'#title' => t('Typography'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);

	$form['expressbase_theme_settings']['typography']['headings'] = array(
	  '#type' => 'select',
	  '#title' => t('Heading Style'),
	  '#default_value' => theme_get_setting('headings', $theme) ? theme_get_setting('headings', $theme) : 'headings-bold',
	  '#description' => t('Pick a style for your sites headings.'),
	  '#options' => array(
      'headings-bold' => t('Bold'),
      'headings-light' => t('Light'),
    ),
	);
	if (module_exists('cu_title_image')) {
    $form['expressbase_theme_settings']['page_title_image'] = array(
  		'#type' => 'fieldset',
  		'#title' => t('Page Title Image'),
  		'#collapsible' => TRUE,
  		'#collapsed' => TRUE,
  	);
  	$form['expressbase_theme_settings']['page_title_image']['page_title_image_background'] = array(
  	  '#type' => 'select',
  	  '#title' => t('Page Title Image Style'),
  	  '#default_value' => theme_get_setting('page_title_image_background', $theme) ? theme_get_setting('page_title_image_background', $theme) : 'page-title-image-background-white',
  	  '#description' => t('Pick a style for page title image text.'),
  	  '#options' => array(
        'page-title-image-background-white' => t('Solid'),
        'page-title-image-background-transparent' => t('Transparent'),
      ),
  	);
  	$form['expressbase_theme_settings']['page_title_image']['page_title_image_width'] = array(
  	  '#type' => 'select',
  	  '#title' => t('Page Title Image Width'),
  	  '#default_value' => theme_get_setting('page_title_image_width', $theme) ? theme_get_setting('page_title_image_width', $theme) : 'page-title-image-width-content',
  	  '#description' => t('Pick a width for page title image. The effect is more dramatic when the theme layout option is set to wide.'),
  	  '#options' => array(
        'page-title-image-width-full' => t('Wide'),
        'page-title-image-width-content' => t('Boxed'),
      ),
  	);
	}

	$form['expressbase_theme_settings']['columns'] = array(
		'#type' => 'fieldset',
		'#title' => t('Column Options'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	$form['expressbase_theme_settings']['columns']['after_content_columns'] = array(
	  '#type' => 'select',
	  '#title' => t('After Content Columns'),
	  '#default_value' => theme_get_setting('after_content_columns', $theme) ? theme_get_setting('after_content_columns', $theme) : '3',
	  '#description' => t('Pick how many columns for blocks after the content'),
	  '#options' => array(
      '6' => t('6'),
      '4' => t('4'),
      '3' => t('3'),
      '2' => t('2'),
      '1' => t('1'),
    ),
	);
	 $form['expressbase_theme_settings']['columns']['lower_columns'] = array(
	  '#type' => 'select',
	  '#title' => t('After Content 2 Columns'),
	  '#default_value' => theme_get_setting('lower_columns', $theme) ? theme_get_setting('lower_columns', $theme) : '2',
	  '#description' => t('Pick how many columns for blocks in the second after content region'),
	  '#options' => array(
      '6' => t('6'),
      '4' => t('4'),
      '3' => t('3'),
      '2' => t('2'),
      '1' => t('1'),
    ),
	);
  $form['expressbase_theme_settings']['columns']['footer_columns'] = array(
	  '#type' => 'select',
	  '#title' => t('Footer Columns'),
	  '#default_value' => theme_get_setting('footer_columns', $theme) ? theme_get_setting('footer_columns', $theme) : '4',
	  '#description' => t('Pick how many columns for blocks in the footer'),
	  '#options' => array(
      '6' => t('6'),
      '4' => t('4'),
      '3' => t('3'),
      '2' => t('2'),
      '1' => t('1'),
    ),
	);

	$form['expressbase_theme_settings']['breadcrumbs'] = array(
		'#type' => 'fieldset',
		'#title' => t('Breadcrumbs'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	$form['expressbase_theme_settings']['breadcrumbs']['use_breadcrumbs'] = array(
    '#type' => 'checkbox',
   	'#title' => t('Use Breadcrumbs'),
   	'#default_value' => theme_get_setting('use_breadcrumbs', $theme) ? theme_get_setting('use_breadcrumbs', $theme) : FALSE,
   	'#description' => t('Enable breadcrumb navigation.'),
  );
  $form['expressbase_theme_settings']['action_menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Secondary Menu'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['expressbase_theme_settings']['action_menu']['use_action_menu'] = array(
    '#type' => 'checkbox',
    '#title' => t('Place secondary menu inline with main menu'),
    '#default_value' => theme_get_setting('use_action_menu', $theme) ? theme_get_setting('use_action_menu', $theme) : FALSE,
    '#description' => t('Place secondary menu above or inline with the main menu. Secondary menu label does not display when placed inline.'),
  );
  $form['expressbase_theme_settings']['action_menu']['action_menu_color'] = array(
	  '#type' => 'select',
	  '#title' => t('Color'),
	  '#default_value' => theme_get_setting('action_menu_color', $theme) ? theme_get_setting('action_menu_color', $theme) : 'action-none',
	  '#description' => t('Pick color for inline secondary menu'),
	  '#options' => array(
      'action-blue' => t('Blue'),
      'action-gray' => t('Gray'),
      'action-gold' => t('Gold'),
      'action-none' => t('None (same as main menu navigation)'),
    ),
	);
	$form['expressbase_theme_settings']['footer_menu'] = array(
    '#type' => 'fieldset',
    '#title' => t('Footer Menu'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['expressbase_theme_settings']['footer_menu']['footer_menu_color'] = array(
	  '#type' => 'select',
	  '#title' => t('Color'),
	  '#default_value' => theme_get_setting('footer_menu_color', $theme) ? theme_get_setting('footer_menu_color', $theme) : 'footer-menu-gray',
	  '#description' => t('Pick color for footer menu.'),
	  '#options' => array(
      'footer-menu-gray' => t('Gray'),
      'footer-menu-gold' => t('Gold'),
    ),
	);

	$form['expressbase_theme_settings']['block_icons'] = array(
    '#type' => 'fieldset',
    '#title' => t('Block Icons'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['expressbase_theme_settings']['block_icons']['block_icons_color'] = array(
	  '#type' => 'select',
	  '#title' => t('Color'),
	  '#default_value' => theme_get_setting('block_icons_color', $theme) ? theme_get_setting('block_icons_color', $theme) : 'block-icons-inherit',
	  '#description' => t('Pick color for block title icons.'),
	  '#options' => array(
	    'block-icons-inherit' => t('Same as block title text'),
      'block-icons-gray' => t('Gray'),
      'block-icons-gold' => t('Gold'),
    ),
	);
}
