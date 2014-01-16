<?php

/**
 * @file
 * Provides a license type plugin.
 */
if (module_exists('commerce_cardonfile')) {
  $plugin = array(
    'title' => t('Card on File via First Data TransArmor'),
    'description' => t('Integrates Card on File with this payment method. This requires a First Data Global Gateway e4 account upgraded to include support for TransArmor.'),
    'class' => 'CommerceFirstDataGGE4ComponentCardonFile',
    'weight' => 0,
    'requires' => array('web_service'),
  );
}
