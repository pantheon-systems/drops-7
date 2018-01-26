
CONTENTS OF THIS FILE
---------------------

* Introduction
* Requirements
* Installation
* Configuration
* Maintainers

INTRODUCTION
------------

The Module Missing Message Fixer module displays a list of missing modules that appear in the message log after the Drupal 7.50 release and provides an interface to fix the entries.  This module was designed in sn effort to help those people who didnt want to run SQL commands directly.

* For a full description of the module, visit https://www.drupal.org/project/module_missing_message_fixer
* To submit bug reports and feature suggestions, or to track changes visit https://www.drupal.org/project/issues/module_missing_message_fixer


REQUIREMENTS
------------

This module requires no additional modules outside of Drupal core.


INSTALLATION
------------

Install the Module Missing Message Fixer module as you would normally install a contributed Drupal module. Visit https://www.drupal.org/docs/7/extending-drupal-7/installing-contributed-modules-find-import-enable-configure-drupal-7 for further information.


CONFIGURATION
-------------

Through the UI interface:
1. Enable the Module Missing Message Fixer module.
2. Ensure that you have the proper permissions by navigating to Administration > People > Permissions and selecting the checkbox.
3. Navigate to Administration > Configuration > System > Missing Module Message Fixer. Select the missing modules you would like to fix and select the "Remove These Errors!" tab. The module will now go through and remove the "ghost records" from the systems table.
Through Drush:
1. $ drush en module_missing_message_fixer
2. $ drush module-missing-message-fixer-list OR $ drush mmmfl. This will generate a list of missing modules.
3. $ drush module-missing-message-fixer-fix machine_name_of_module (or --all) OR $ drush mmmff machine_name_of_module (or --all). This will fix the missing modules entities.

For more information visit https://www.drupal.org/node/2487215


MAINTAINERS
-----------

* John Ouellet (labboy0276) - https://www.drupal.org/u/labboy0276

Supporting organization:
* Kalamuna - https://www.drupal.org/kalamuna
