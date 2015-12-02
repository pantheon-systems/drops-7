<?php

function cushadow_form_system_theme_settings_alter(&$form, &$form_state) {
  $form['cu_omega_theme_settings']['page_title_image']['page_title_image_width']['#access'] = FALSE;
}
