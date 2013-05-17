; This version of the .make will build a local copy of the distribution
; using the version of drupal-org.make that has been committed.
; Modules and libraries will be in profiles/civicrm_starterkit 
; drush make build-local-civicrm_starterkit.make

api = 2
core = 7.x

; Drupal Core
projects[drupal][version] = "7.22"

;Include the definition for how to build Drupal core directly, including patches:
;includes[] = drupal-org-core.make

; Download the Install profile and recursively build all its dependencies:
projects[civicrm_starterkit][type] = profile
projects[civicrm_starterkit][version] = 3.x-dev