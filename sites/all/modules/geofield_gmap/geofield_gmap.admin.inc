<?php

/**
 * @file
 * Admin pages for Geofield Gmap.
 */

/**
 * Form builder; Geofield Gmap admin settings form.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function geofield_gmap_admin_settings($form, &$form_state) {

  $form['geofield_gmap_google_api_key'] = array(
      '#type' => 'textfield',
      '#title' => t("Google Api Key"),
      '#default_value' => variable_get('geofield_gmap_google_api_key', NULL),
  );

  return system_settings_form($form);

}