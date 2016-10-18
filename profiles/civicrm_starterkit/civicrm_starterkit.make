; This version of the .make will build a local copy of the distribution
; using the versions of modules and patches listed.
; Modules and libraries will be in sites/all 
; This is used to test the packaging BEFORE committing
; drush make --no-core civicrm_starterkit.make

core = 7.51
api = 2

; Drupal Core
projects[drupal][version] = "7.51"

; ====== CIVICRM RELATED =========

libraries[civicrm][download][type] = get
libraries[civicrm][download][url] = "https://download.civicrm.org/civicrm-4.6.22-drupal.tar.gz"
libraries[civicrm][destination] = modules
libraries[civicrm][directory_name] = civicrm

;PANTHEON RELATED PATCHES
; Add Pantheon settings to civicrm.settings.php (http://forum.civicrm.org/index.php?topic=31570.0)
; Redis caching settings for Pantheon
libraries[civicrm][patch][pantheonsettings] = https://www.drupal.org/files/issues/2082713-pantheon-settings-civicrm-46-2.patch

; Add Redis caching
libraries[civicrm][patch][redis] = https://www.drupal.org/files/issues/2468687-redis-caching-civi46.patch

; Skip config cache on Pantheon
libraries[civicrm][patch][config] = https://www.drupal.org/files/issues/2096467-skip-config-cache-civi46.patch

; INSTALL
; provide modulepath to populate settings
libraries[civicrm][patch][2063371] = http://drupal.org/files/2063371-add-modulePath-var-4-4.patch
libraries[civicrm][patch][1978796] = http://drupal.org/files/1978796-session.save-as_file.patch
; Related to https://issues.civicrm.org/jira/browse/CRM-9683
libraries[civicrm][patch][2130213] = http://drupal.org/files/issues/2130213-ignore-timezone-on-install-2.patch
;IMPROVING PROFILE INSTALL UX WHEN INSTALLING FROM A PROFILE
libraries[civicrm][patch][1849424-use] = https://www.drupal.org/files/issues/1849424-use-vars-in-link-civi46.patch
libraries[civicrm][patch][1849424-pass] = http://drupal.org/files/1849424-pass-vars-in-link-2.patch
; Populate with Pantheon environment settings on install
libraries[civicrm][patch][1978838] = http://drupal.org/files/issues/1978838-pre-populate-db-settings-2.patch

; Required for extern urls to work (e.g. ipn.php, soap.php)
libraries[civicrm][patch][2177647] = https://drupal.org/files/issues/2177647-sessions-fix.patch
libraries[civicrm][patch][cron] = https://www.drupal.org/files/issues/2819697-cron-civi46.patch

; Necessary if in profiles/*/modules/civicrm
libraries[civicrm][patch][1844558] = https://drupal.org/files/issues/1844558-run-civicrm-from-profile-dir-config-3.patch
libraries[civicrm][patch][1967972] = http://drupal.org/files/1967972-bootsrap-fixes.patch

; May be necessary where extension, etc paths are cached but Pantheon changes binding
libraries[civicrm][patch][2347897] = https://www.drupal.org/files/issues/2347897-binding-fix-for-extension-civi46.patch

; [OPTIONAL IF USING REDIS] Use CiviCRM cache functions to use Redis for storing compiled Smarty templates
; Based on github.com/ojkelly commit 85e04b6
;libraries[civicrm][patch][smartyredis] = https://www.drupal.org/files/issues/2570335-smarty-redis-civi-cache-civi46.patch

; This file is added to create the sites/all/extensions directory
libraries[cache][download][type] = get
libraries[cache][download][url] = "https://raw.github.com/PHPIDS/PHPIDS/master/README.md"
libraries[cache][download][filename] = timestamp.txt
libraries[cache][destination] = extensions
libraries[cache][patch][1980088] = https://drupal.org/files/1980088-create-extensions-dir-4.patch

; ====== POPULAR CONTRIB MODULES =========

projects[backup_migrate][subdir] = "contrib"
projects[backup_migrate][version] = "2.8"

projects[civicrm_clear_all_caches][subdir] = "contrib"
projects[civicrm_clear_all_caches][version] = "1.0-beta1"

projects[civicrm_cron][subdir] = "contrib"
projects[civicrm_cron][version] = "2.0-beta2"

projects[ctools][subdir] = "contrib"
projects[ctools][version] = "1.10"

projects[captcha][subdir] = "contrib"
projects[captcha][version] = "1.3"

projects[features][subdir] = "contrib"
projects[features][version] = "2.10"

projects[fontyourface][subdir] = "contrib"
projects[fontyourface][version] = "2.8"

projects[imce][subdir] = "contrib"
projects[imce][version] = "1.10"

projects[imce_wysiwyg][subdir] = "contrib"
projects[imce_wysiwyg][version] = "1.0"

projects[libraries][subdir] = "contrib"
projects[libraries][version] = "2.3"

projects[module_filter][subdir] = "contrib"
projects[module_filter][version] = "1.8"

projects[options_element][subdir] = "contrib"
projects[options_element][version] = "1.12"

projects[profile_status_check][subdir] = "contrib"
projects[profile_status_check][version] = "1.0-beta3"

projects[profile_switcher][subdir] = "contrib"
projects[profile_switcher][version] = "1.0-beta2"

projects[recaptcha][subdir] = "contrib"
projects[recaptcha][version] = "2.2"

projects[views][subdir] = "contrib"
projects[views][version] = "3.14"

projects[webform][subdir] = "contrib"
projects[webform][version] = "4.14"

projects[webform_civicrm][subdir] = "contrib"
projects[webform_civicrm][version] = "4.16"

projects[wysiwyg][subdir] = "contrib"
projects[wysiwyg][version] = "2.2"


; ====== DRUPAL LIBRARIES =========

libraries[ckeditor][download][type] = get
libraries[ckeditor][download][url] = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.6.2/ckeditor_3.6.2.tar.gz"
libraries[ckeditor][destination] = libraries
libraries[ckeditor][directory_name] = ckeditor

libraries[tinymce][download][type] = get
libraries[tinymce][download][url] = "http://download.moxiecode.com/tinymce/tinymce_3.5.11.zip"
libraries[tinymce][destination] = libraries
libraries[tinymce][directory_name] = tinymce
