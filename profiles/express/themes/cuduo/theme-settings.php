<?php

function cuduo_form_system_theme_settings_alter(&$form, &$form_state) {
  $theme = $form_state['build_info']['args'][0];
  $form['cu_omega_theme_settings']['page_title_image']['page_title_image_width']['#access'] = FALSE;
}
