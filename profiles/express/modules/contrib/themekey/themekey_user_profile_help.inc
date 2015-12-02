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
function themekey_user_profile_help_tutorials(&$form) {
  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site'] = array(
    '#type' => 'fieldset',
    '#title' => t('Allowing users to select a personal theme for this site (Drupal 7 replacement of a Drupal 6 Core feature)'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site']['author'] = array(
    '#type' => 'item',
    '#title' => t('Author'),
    '#markup' => l(t('!path', array('!path' => 'mkalkbrenner')), 'http://drupal.org/user/124705'),
  );

  $img_path = base_path() . drupal_get_path('module', 'themekey_user_profile') . '/img/tutorials/';

  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site']['item'] = array(
    '#type' => 'item',
    '#markup' => '<p>' . t('In this tutorial, you will learn how to enable users to select a personal theme for your site. This feature was part of Drupal 6 Core but has been removed in Drupal 7 Core. The ThemeKey User Profile module replaces this functionality for Drupal 7 now.') . '</p>' .
'<p>' . t('Quick and dirty (screenshots will follow):<ol>
<li>Install ThemeKey User Profile which is part of the ThemeKey module package</li>
<li>Assign the permission "Select different theme" to the roles at !link1</li>
<li>Activate "Add theme option to user profile" at !link2</li>
<li>Configure "Selectable Thems" at !link3</li>
<li>Optional: Prioritize the static ThemeKey property "user:profile_triggers_theme" at !link4</li>
</ol>', array(
      '!link1' => l(t('!path', array('!path' => '/admin/people/permissions')), 'admin/people/permissions'),
      '!link2' => l(t('!path', array('!path' => '/admin/config/user-interface/themekey/settings/ui')), 'admin/config/user-interface/themekey/settings/ui'),
      '!link3' => l(t('!path', array('!path' => '/admin/config/user-interface/themekey/settings/ui')), 'admin/config/user-interface/themekey/settings/ui'),
      '!link4' => l(t('!path', array('!path' => '/admin/config/user-interface/themekey')), 'admin/config/user-interface/themekey'),
    )) . '</p>',
  );

  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site']['versions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Versions used to create this tutorial'),
    '#collapsible' => FALSE,
  );

  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site']['versions']['themekey'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey'),
    '#markup' => '7.x-1.0-beta2',
  );

  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site']['versions']['themekey_ui'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey UI'),
    '#markup' => '7.x-1.0-beta2',
  );

  $form['themekey_help_tutorials']['Allowing users to select a personal theme for this site']['versions']['themekey_user_profile'] = array(
    '#type' => 'item',
    '#title' => t('ThemeKey User Profile'),
    '#markup' => '7.x-1.0-beta2',
  );

  return $form;
}
