<?php

/**
 * @file
 * Administrative page callbacks for Widget Services module.
 */

/**
 * Menu callback administration settings for widget options.
 */
function  widget_services_admin() {
  $form['twitter_widget'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Twitter Widget'),
  );
  $form['twitter_widget']['service_links_tw_data_count'] = array(
    '#type' => 'select',
    '#title' => t('Counter'),
    '#default_value' => variable_get('service_links_tw_data_count', 'horizontal'),
    '#options' => array(
      'horizontal' => t('Horizontal'),
      'vertical' => t('Vertical'),
      'none' => t('None'),
    ),
  );
  $form['twitter_widget']['service_links_tw_data_via'] = array(
    '#type' => 'textfield',
    '#field_prefix' => '@',
    '#title' => t('Via user'),
    '#description' => t('Add here your screen name.'),
    '#default_value' => variable_get('service_links_tw_data_via', ''),
    '#size' => 40,
  );

  $form['facebook_like'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Facebook Like'),
  );
  $form['facebook_like']['service_links_fl_layout'] = array(
    '#type' => 'select',
    '#title' => t('Layout'),
    '#default_value' => variable_get('service_links_fl_layout', 'button_count'),
    '#options' => array(
      'standard' => t('Standard'),
      'button_count' => t('Button Count'),
      'box_count' => t('Box Count'),
    ),
  );
  $form['facebook_like']['service_links_fl_width'] = array(
    '#title' => t('Width'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_fl_width', 100),
    '#description' => t('A good value for button layout is 100, for box layout is 80 and for standard layout 450'),
    '#size' => 10,
  );
  $form['facebook_like']['service_links_fl_height'] = array(
    '#title' => t('Height'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_fl_height', 21),
    '#description' => t('A good value for button layout is 21, for box layout is 65 and for standard layout is 80'),
    '#size' => 10,
  );
  $form['facebook_like']['service_links_fl_font'] = array(
    '#title' => t('Font'),
    '#type' => 'select',
    '#default_value' => variable_get('service_links_fl_font', ''),
    '#options' => array(
      '' => t('None'),
      'arial' => t('Arial'),
      'lucida grande' => t('Lucida Grande'),
      'segoe ui' => t('Segoe Ui'),
      'tahoma' => t('Tahoma'),
      'trebuchet ms' => t('Trebuchet Ms'),
      'verdana' => t('Verdana'),
    ),
  );
  $form['facebook_like']['service_links_fl_show_faces'] = array(
    '#title' => t('Show Faces'),
    '#type' => 'radios',
    '#default_value' => variable_get('service_links_fl_show_faces', 'false'),
    '#options' => array(
      'true' => t('Yes'),
      'false' => t('No'),
    ),
    '#attributes' => array('class' => array('container-inline')),
    '#description' => t('This works only when the standard layout is set on'),
  );
  $form['facebook_like']['service_links_fl_colorscheme'] = array(
    '#title' => t('Color Scheme'),
    '#type' => 'radios',
    '#default_value' => variable_get('service_links_fl_colorscheme', 'light'),
    '#options' => array(
      'light' => t('Light'),
      'dark' => t('Dark'),
    ),
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['facebook_like']['service_links_fl_action'] = array(
    '#title' => t('Action'),
    '#type' => 'radios',
    '#default_value' => variable_get('service_links_fl_action', 'like'),
    '#options' => array(
      'like' => t('Like'),
      'recommend' => t('Recommend'),
    ),
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['facebook_like']['service_links_fl_locale'] = array(
    '#title' => t('Locale'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_fl_locale', ''),
    '#description' => t('Enter the language code and the country code separated by an underscore (i.e. es_ES, en_GB, en_US, it_IT, fr_FR, pt_BR, pt_PT)'),
    '#size' => 5,
    '#attributes' => array('class' => array('container-inline')),
  );
 
  $form['facebook_share'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Facebook Share'),
  );
  $form['facebook_share']['service_links_fs_type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#default_value' => variable_get('service_links_fs_type', 'button_count'),
    '#options' => array(
      'button' => t('Button'),
      'button_count' => t('Button Count'),
      'box_count' => t('Box Count'),
      'icon_link' => t('Icon Link'),
    ),
  );
  $form['facebook_share']['service_links_fs_app_id'] = array(
    '#type' => 'textfield',
    '#title' => t('App ID'),
    '#default_value' => variable_get('service_links_fs_app_id', '150123828484431'), 
    '#description' => t('You may generate your app ID at the <a href=@url>Facebook apps</a> page', array('@url' => 'https://developers.facebook.com/apps')),
    '#size' => 40,
  );
  $form['facebook_share']['service_links_fs_css'] = array(
    '#type' => 'textfield',
    '#title' => t('Style'),
    '#default_value' => variable_get('service_links_fs_css', ''), 
    '#description' => t('Apply a CSS style providing a single string (i.e. %style)', array('%style' => 'text-align:left;vertical-align:top;margin-top:4px')),
    '#size' => 80,
  );
 
  $form['google_plus_one'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Google Plus One'),
  );
  $form['google_plus_one']['service_links_gpo_size'] = array(
    '#title' => t('Size'),
    '#type' => 'select',
    '#default_value' => variable_get('service_links_gpo_size', ''),
    '#options' => array(
      '' => t('Standard'),
      'small' => t('Small'),
      'medium' => t('Medium'),
      'tall' => t('Tall'),
    ),
  );
  $form['google_plus_one']['service_links_gpo_annotation'] = array(
    '#title' => t('Annotation'),
    '#type' => 'select',
    '#default_value' => variable_get('service_links_gpo_annotation', ''),
    '#options' => array(
      '' => t('Bubble'),
      'none' => t('None'),
      'inline' => t('Inline'),
    ),
  );
  $form['google_plus_one']['service_links_gpo_width'] = array(
    '#title' => t('Max width'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_gpo_width', 300),
    '#description' => t('If annotation is set to "inline", this parameter sets the width in pixels to use for the button and its inline annotation. If the width is omitted, a button and its inline annotation use 450px. A minimum value of 120 is required.
See <a href="@url">Inline annotation widths</a> for examples of how the annotation is displayed for various width settings', array('@url' => 'https://developers.google.com/+/web/+1button/?hl=it#inline-annotation')),
    '#size' => 10,
  );
  $form['google_plus_one']['service_links_gpo_lang'] = array(
    '#title' => t('Language'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_gpo_lang', ''),
    '#description' => t('Sets the language to use for the +1 buttons on the page when annotation is set to "inline". For available language code values, see the <a href="@url">list of supported language codes</a>', array('@url' => 'https://developers.google.com/+/web/+1button/?hl=it#available-languages')),
    '#size' => 5,
  );
  $form['google_plus_one']['service_links_gpo_callback'] = array(
    '#title' => t('Callback function'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_gpo_callback', ''),
    '#description' => t('If specified, this function is called after the user clicks the +1 button.'),
    '#size' => 40,
  );

  $form['linkedin_share_button'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Linkedin Share Button'),
  );
  $form['linkedin_share_button']['service_links_lsb_countmode'] = array(
    '#title' => t('Count Mode'),
    '#type' => 'select',
    '#default_value' => variable_get('service_links_lsb_countmode', 'right'),
    '#options' => array(
      'top' => t('Vertical'),
      'right' => t('Horizontal'),
      '' => t('No Count'),
    ),
  );

  $form['pinterest_button'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#title' => t('Pinterest Button'),
  );
  $form['pinterest_button']['service_links_pb_countlayout'] = array(
    '#title' => t('Pin Count'),
    '#type' => 'select',
    '#default_value' => variable_get('service_links_pb_countlayout', 'horizontal'),
    '#options' => array(
      'horizontal' => t('Horizontal'),
      'vertical' => t('Vertical'),
      'none' => t('No Count'),
    ),
  );
  $form['pinterest_button']['service_links_pb_mediatoken'] = array(
    '#title' => t('Media parameter'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_pb_mediatoken', ''),
    '#description' => t('Fill the media parameter with a token which will be evaluated by <a href=@url>Token</a> module.', array('@url' => 'http://www.drupal.org/project/token')),
    '#size' => 40,
  );
  $form['pinterest_button']['service_links_pb_descriptiontoken'] = array(
    '#title' => t('Description parameter'),
    '#type' => 'textfield',
    '#default_value' => variable_get('service_links_pb_descriptiontoken', ''),
    '#description' => t('Fill the description parameter with a token which will be evaluated by <a href=@url>Token</a> module, if left blank will be replaced by the node teaser.', array('@url' => 'http://www.drupal.org/project/token')),
    '#size' => 40,
  );
  if (module_exists('token')) {
    $form['pinterest_button']['token_help'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => t('Token help'),
      '#theme' => 'token_tree',
      '#token_types' => array('node'),
    );
  }

  return system_settings_form($form);
}
