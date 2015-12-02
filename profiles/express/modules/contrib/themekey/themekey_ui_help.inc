<?php

/**
 * @file
 * Provides content for help pages.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 */


/**
 * Adds tutorials to themekey_help_form().
 *
 * @see themekey_help()
 * @see themekey_help_form()
 * @see themekey_ui_form_alter()
 *
 * @param $form
 *   reference to a Drupal form
 */
function themekey_ui_help_tutorials(&$form) {
  $form['themekey_help_tutorials']['Allowing users to select a theme for all content they create'] = array(
    '#type' => 'fieldset',
    '#title' => t('Allowing users to select a theme for all content they create'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_tutorials']['Allowing users to select a theme for all content they create']['author'] = array(
    '#type' => 'item',
    '#title' => t('Author'),
    '#markup' => l(t('!path', array('!path' => 'Sansui')), 'http://drupal.org/user/297165'),
  );

  $img_path = base_path() . drupal_get_path('module', 'themekey_ui') . '/img/tutorials/';

  $form['themekey_help_tutorials']['Allowing users to select a theme for all content they create']['item'] = array(
    '#type' => 'item',
    '#markup' => '<p>' . t('In this tutorial, you will learn how to enable users to select a theme for content they create. For example, if you would like each of your bloggers to display their own theme on each of their blog entries, this tutorial will allow you to grant this option.</p>') .
'<p>' . t('First, you must enable any themes you would like available from under !link (Administer -> Appearance)</p>', array('!link' => l(t('!path', array('!path' => 'admin/appearance')), 'admin/appearance'))) .
'<p>' . t('Once you have enabled your themes, navigate to !link (Administer -> Configuration -> Themekey -> Settings -> User Interface). You will see a checkbox for "Show theme option in user profile". Click on the checkbox and save your settings.</p>', array('!link' => l(t('!path', array('!path' => 'admin/config/user-interface/themekey/settings/ui')), 'admin/config/user-interface/themekey/settings/ui'))) .
'<p>' . '<img src="' . $img_path . 'themekey_user_profiles_2.png" alt="' . t('Enabling theme selection in user profiles') . '" style="border:solid #d4e7f3 2px;" /></p>' .
'<p>' . t('Next, you must grant permission to the roles that you would like to be able to change themes for their own content. Navigate to !link (Administer -> People -> Permissions) and check the boxes for "Assign theme to own nodes" for all roles you want to have this option.</p>', array('!link' => l(t('!path', array('!path' => 'admin/people/permissions')), 'admin/people/permissions'))) . '<br />' .
'<p>' . '<img src="' . $img_path . 'themekey_user_profiles_3.png" alt="' . t('Enable correct permissions to use themekey for your users') . '" style="border:solid #d4e7f3 2px;" /></p>' .
'<p>' . t('Once you have done this, your users can log in, navigate to My Account -> Edit and a new option will appear under their account settings. A full select list of your enabled themes will appear for them to select from. Once they have chosen a new theme, any node that is authored by this user will display the theme they have selected.</p>') .
'<p>' . '<img src="' . $img_path . 'themekey_user_profiles_4.png" alt="' . t('ThemeKey rule chain that switches to different themes on content creation forms for different user roles') . '" style="border:solid #d4e7f3 2px;" /></p>' .
'<p>' . t('Have Fun!') . '</p>',
  );

  $form['themekey_help_tutorials']['Allowing users to select a theme for all content they create']['versions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Versions used to create this tutorial'),
    '#collapsible' => FALSE,
  );

  $form['themekey_help_tutorials']['Allowing users to select a theme for all content they create']['versions']['themekey'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey'),
    '#markup' => '6.x-2.2',
  );

  $form['themekey_help_tutorials']['Allowing users to select a theme for all content they create']['versions']['themekey_ui'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey UI'),
    '#markup' => '6.x-2.2',
  );

  return $form;
}
