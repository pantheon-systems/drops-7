<?php
/**
 * @file
 * Documentation for Commerce kickstart servive poroviders.
 */

/**
 * Allow to other module display their services in the getting started page.
 *
 * @return array
 * - 'service-module-name' array
 *  - 'logo_path': The location of the logo.
 *  - 'title': The human readable service title.
 *  - 'category': The category the service belong to.
 *  - 'teaser': A short text about the service. This will be shown in the
 *    getting started page.
 *  - 'description': Full description about the service.
 *  - 'requirements': Requirements for the module.
 *  - 'link': Link to the service.
 *  - 'installation_proccess': List of steps needed to be done for implementation
 *    of the service.
 *  - 'weight': The weight of the service. The service list is ordered by this
 *    property in a ascending order.
 *  - 'requirements_callback': Callback function for the service requirements
 *    status.
 */
function hook_commerce_kickstart_service_provider() {
  return array(
    'name' => array(
      'logo_path' => '',
      'title' => '',
      'module_path' => '',
      'category' => '',
      'teaser' => '',
      'description' => '',
      'requirements' => '',
      'link' => '',
      'installation_proccess' => '',
      'weight' => 1,
      'requirements_callback' => '',
    ),
  );
}

/**
 * Allow other modules to alter the services variables before rendering.
 */
function hook_commerce_kickstart_service_provider_alter(&$variable) {
}
