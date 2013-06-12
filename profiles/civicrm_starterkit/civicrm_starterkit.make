; This version of the .make will build a local copy of the distribution
; using the versions of modules and patches listed.
; Modules and libraries will be in sites/all 
; This is used to test the packaging BEFORE committing
; drush make civicrm_starterkit.make

core = 7.22
api = 2

; Drupal Core
projects[drupal][version] = "7.22"

; ====== CIVICRM RELATED =========

libraries[civicrm][download][type] = get
libraries[civicrm][download][url] = "http://downloads.civicrm.org/civicrm-4.3.4-starterkit.tgz"
libraries[civicrm][destination] = modules
libraries[civicrm][directory_name] = civicrm

;PATCHES THAT SHOULD BE ADDRESSED IN FUTURE CIVICRM RELEASES
libraries[civicrm][patch][1844558] = http://drupal.org/files/1844558-run-civicrm-from-profile-dir-config-2.patch
libraries[civicrm][patch][1940074] = http://drupal.org/files/1940074-openFlashChart_tpl_javascript-4.patch
libraries[civicrm][patch][1946820] = http://drupal.org/files/1946820-turn_IDS_off_in_civicrm.settings.php_.tpl_.patch
libraries[civicrm][patch][1967972] = http://drupal.org/files/1967972-bootsrap-fixes.patch
libraries[civicrm][patch][2010430] = https://drupal.org/files/2010430-htmlpurifier_path.patch

;PANTHEON RELATED PATCHES
libraries[civicrm][patch][1978142] = http://drupal.org/files/1978142-pantheon-settings-11.patch
libraries[civicrm][patch][1978796] = http://drupal.org/files/1978796-session.save-as_file.patch
libraries[civicrm][patch][1978838] =  http://drupal.org/files/1978838-pre-populate-db-settings.patch

;IMPROVING PROFILE INSTALL UX WHEN INSTALLING FROM A PROFILE
libraries[civicrm][patch][1849424] = http://drupal.org/files/1849424-use-vars-in-link.patch
libraries[civicrm][patch][] = http://drupal.org/files/1849424-pass-vars-in-link-2.patch

;PATCHES THAT ADD LIBRARIES BACK
libraries[jquery][download][type] = get
libraries[jquery][download][url] = "http://code.jquery.com/jquery-1.8.3.min.js"
libraries[jquery][destination] = "modules/civicrm/packages"
libraries[jquery][directory_name] = jquery
libraries[jquery][download][filename] = jquery-1.8.3.min.js
libraries[jquery][patch][1787976] = http://drupal.org/files/1787976-jquery-missing-files-9.patch
libraries[jquery][patch][] = http://drupal.org/files/textarearesizer-4.patch
libraries[jquery][patch][1950068] = http://drupal.org/files/1950068-more-js-files-2.patch
libraries[jquery][patch][] = http://drupal.org/files/1787976-updated-fo-4-3-3.patch

libraries[jquery_ui][download][type] = get
libraries[jquery_ui][download][url] = "http://jquery-ui.googlecode.com/files/jquery-ui-1.9.0-rc.1.zip"
libraries[jquery_ui][destination] = "modules/civicrm/packages/jquery/jquery-ui-1.9.0"
libraries[jquery_ui][directory_name] = development-bundle

libraries[jstree][download][type] = get
libraries[jstree][download][url] = "https://github.com/vakata/jstree/archive/v.pre1.0.zip"
libraries[jstree][destination] = "modules/civicrm/packages/jquery/plugins"
libraries[jstree][directory_name] = jstree

; waiting on whitelist change #1971760
; once approved, patch for #1950068 will need to be updated
;libraries[jquery_validate][download][type] = get
;libraries[jquery_validate][download][url] = "https://raw.github.com/jzaefferer/jquery-validation/master/jquery.validate.js"
;libraries[jquery_validate][destination] = "modules/civicrm/packages/jquery/plugins"

libraries[phpids][download][type] = get
libraries[phpids][download][url] = "http://phpids.org/files/phpids-0.7.zip"
libraries[phpids][destination] = "modules/civicrm/packages"
libraries[phpids][directory_name] = IDS
libraries[phpids][patch][1787984] = http://drupal.org/files/1787984-IDS-directory-fix-IDS.patch

libraries[dompdf][download][type] = get
libraries[dompdf][download][url] = "http://dompdf.googlecode.com/files/dompdf_0-6-0_beta3.tar.gz"
libraries[dompdf][destination] = "modules/civicrm/packages"
libraries[dompdf][directory_name] = dompdf

; CKEDitor and TinyMCE are included twice... really need to change that
; with a patch that allows CiviCRM to use sites/all/libraries

libraries[ckeditor-civicrm][download][type] = get
libraries[ckeditor-civicrm][download][url] = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.6.2/ckeditor_3.6.2.tar.gz"
libraries[ckeditor-civicrm][destination] = "modules/civicrm/packages"
libraries[ckeditor-civicrm][directory_name] = ckeditor

libraries[tinymce-civicrm][download][type] = get
libraries[tinymce-civicrm][download][url] = "https://github.com/downloads/tinymce/tinymce/tinymce_3.4.8.zip"
libraries[tinymce-civicrm][destination] = "modules/civicrm/packages"
libraries[tinymce-civicrm][directory_name] = tinymce


; ====== POPULAR CONTRIB MODULES =========

projects[backup_migrate][subdir] = "contrib"
projects[backup_migrate][version] = "2.7"

projects[civicrm_cron][subdir] = "contrib"
projects[civicrm_cron][version] = "2.0-beta1"

projects[ctools][subdir] = "contrib"
projects[ctools][version] = "1.3"

projects[captcha][subdir] = "contrib"
projects[captcha][version] = "1.0-beta2"

projects[features][subdir] = "contrib"
projects[features][version] = "2.0-rc1"

projects[fontyourface][subdir] = "contrib"
projects[fontyourface][version] = "2.8"

projects[imce][subdir] = "contrib"
projects[imce][version] = "1.7"

projects[imce_wysiwyg][subdir] = "contrib"
projects[imce_wysiwyg][version] = "1.0"

projects[libraries][subdir] = "contrib"
projects[libraries][version] = "2.1"

projects[recaptcha][subdir] = "contrib"
projects[recaptcha][version] = "1.9"

projects[views][subdir] = "contrib"
projects[views][version] = "3.7"

projects[webform][subdir] = "contrib"
projects[webform][version] = "3.19"

projects[webform_civicrm][subdir] = "contrib"
projects[webform_civicrm][version] = "3.5"

projects[wysiwyg][subdir] = "contrib"
projects[wysiwyg][version] = "2.2"


; ====== DRUPAL LIBRARIES =========

libraries[ckeditor][download][type] = get
libraries[ckeditor][download][url] = "http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.6.2/ckeditor_3.6.2.tar.gz"
libraries[ckeditor][destination] = libraries
libraries[ckeditor][directory_name] = ckeditor

libraries[tinymce][download][type] = get
libraries[tinymce][download][url] = "https://github.com/downloads/tinymce/tinymce/tinymce_3.5.8.zip"
libraries[tinymce][destination] = libraries
libraries[tinymce][directory_name] = tinymce
