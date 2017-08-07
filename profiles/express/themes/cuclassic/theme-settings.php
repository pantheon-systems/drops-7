<?php

function cuclassic_form_system_theme_settings_alter(&$form, &$form_state) {
  $form['expressbase_theme_settings']['responsive']['#access'] = FALSE;
}
