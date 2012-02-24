; Drupal Core
api = 2
core = 7.12

projects[drupal][type] = core
projects[drupal][patch][] = http://drupal.org/files/issues/object_conversion_menu_router_build-972536-1.patch
projects[drupal][patch][] = http://drupal.org/files/issues/992540-3-reset_flood_limit_on_password_reset-drush.patch
; allow simpletest to look into profiles for modules
projects[drupal][patch][] = http://drupal.org/files/issues/911354.46.patch

projects[field_group][subdir] = contrib
projects[field_group][type] = module
projects[field_group][version] = 1.1

projects[references][subdir] = contrib
projects[references][type] = module
projects[references][version] = 2.0

projects[ctools][subdir] = contrib
projects[ctools][type] = module
projects[ctools][version] = 1.0-rc1
projects[ctools][patch][] = http://drupal.org/files/1371180-add-export-module.patch

projects[date][subdir] = contrib
projects[date][type] = module
projects[date][version] = 2.1

projects[diff][subdir] = contrib
projects[diff][type] = module
projects[diff][version] = 2.0

projects[entity][subdir] = contrib
projects[entity][type] = module
projects[entity][version] = 1.0-rc1

projects[features][subdir] = contrib
projects[features][type] = module
projects[features][version] = 1.0-beta6

projects[openidadmin][subdir] = contrib
projects[openidadmin][type] = module
projects[openidadmin][version] = 1.0

projects[pathauto][subdir] = contrib
projects[pathauto][type] = module
projects[pathauto][version] = 1.0

projects[strongarm][subdir] = contrib
projects[strongarm][version] = 2.0-beta5

projects[token][subdir] = contrib
projects[token][version] = 1.0-rc1

projects[views][subdir] = contrib
projects[views][version] = 3.3

projects[vntf][subdir] = contrib
projects[vntf][version] = 1.0-beta5

projects[nodequeue][subdir] = contrib
projects[nodequeue][type] = module
projects[nodequeue][version] = 2.0-beta1
; projects[nodequeue][patch][] = http://drupal.org/files/issues/1023606-qid-to-name-6.patch

projects[entitycache][subdir] = contrib
projects[entitycache][type] = module
projects[entitycache][version] = 1.1

projects[conditional_styles][subdir] = contrib
projects[conditional_styles][version] = 2.0

projects[nodeconnect][type] = module
projects[nodeconnect][subdir] = contrib
projects[nodeconnect][version] = 1.0-alpha2

projects[apps][type] = module
projects[apps][subdir] = contrib
projects[apps][version] = 1.0-beta5

projects[imce][subdir] = contrib
projects[imce][version] = 1.5

projects[imce_wysiwyg][subdir] = contrib
projects[imce_wysiwyg][version] = 1.0

projects[filefield_sources][subdir] = contrib
projects[filefield_sources][version] = 1.4

projects[nodeblock][subdir] = contrib
projects[nodeblock][version] = 1.2
;projects[nodeblock][patch][] = http://drupal.org/files/issues/nodeblock.module.block_view.patch

projects[xmlsitemap][subdir] = contrib
projects[xmlsitemap][type] = module
projects[xmlsitemap][version] = 2.0-rc1

projects[wysiwyg][subdir] = contrib
projects[wysiwyg][type] = module
projects[wysiwyg][version] = 2.1
projects[wysiwyg][patch][] = http://drupal.org/files/issues/wysiwyg-835682-12.patch

projects[addthis][subdir] = contrib
projects[addthis][version] = 2.1-beta1

projects[google_analytics][subdir] = contrib
projects[google_analytics][version] = 1.2

projects[captcha][version] = 1.0-alpha3
projects[captcha][subdir] = contrib
projects[captcha][type] = module
projects[captcha][patch][] = http://drupal.org/files/issues/825088-19-captcha_ctools_export.patch

projects[recaptcha][subdir] = contrib
projects[recaptcha][type] = module
projects[recaptcha][version] = 1.7

projects[link][subdir] = contrib
projects[link][type] = module
projects[link][version] = 1.0

projects[video_embed_field][subdir] = contrib
projects[video_embed_field][type] = module
projects[video_embed_field][version] = 2.0-beta4

projects[vntf][subdir] = contrib
projects[vntf][type] = module
projects[vntf][version] = 1.0-beta5
projects[vntf][patch][] = http://drupal.org/files/1169366-patch-add-require-terms-on-node-option-3.patch

projects[ga_stats][subdir] = contrib
projects[ga_stats][type] = module
projects[ga_stats][version] = 1.0-beta1

projects[libraries][subdir] = contrib
projects[libraries][type] = module
projects[libraries][version] = 1.0

projects[jcarousel][subdir] = contrib
projects[jcarousel][type] = module
projects[jcarousel][version] = 2.6

projects[field_collection][subdir] = contrib
projects[field_collection][type] = module
projects[field_collection][version] = 1.0-beta3
projects[field_collection][patch][] = http://drupal.org/files/issue_1329856_1.patch

; Themes
projects[tao][type] = theme
projects[tao][version] = 3.0-beta4

projects[rubik][type] = theme
projects[rubik][version] = 4.0-beta8

projects[omega][type] = theme
projects[omega][version] = 3.1

; Page Layout + Administration
projects[context][subdir] = contrib
projects[context][type] = module
projects[context][version] = 3.0-beta2

projects[delta][subdir] = contrib
projects[delta][type] = module
projects[delta][version] = 3.0-beta9

projects[omega_tools][subdir] = contrib
projects[omega_tools][type] = module
projects[omega_tools][version] = 3.0-rc4

projects[boxes][subdir] = contrib
projects[boxes][type] = module
projects[boxes][version] = 1.0-beta7

projects[context_field][subdir] = contrib
projects[context_field][type] = module
projects[context_field][version] = 1.0-beta2

projects[views_boxes][subdir] = contrib
projects[views_boxes][type] = module
projects[views_boxes][version] = 1.0-beta8

projects[entity_autocomplete][subdir] = contrib
projects[entity_autocomplete][type] = module
projects[entity_autocomplete][version] = 1.0-beta1
projects[entity_autocomplete][patch][] = http://drupal.org/files/fix-issue-with-taxonomy-bundles-1447178-1_0.patch

projects[views_arguments_extras][subdir] = contrib
projects[views_arguments_extras][type] = module
projects[views_arguments_extras][version] = 1.0-beta1

; add feeds, but don't turn it on
projects[feeds][subdir] = contrib
projects[feeds][type] = module
projects[feeds][version] = 2.0-alpha4

libraries[ckeditor][download][type] = get
libraries[ckeditor][download][url] = http://download.cksource.com/CKEditor/CKEditor/CKEditor%203.5/ckeditor_3.5.tar.gz
