; Panopoly Images Makefile

api = 2
core = 7.x

; Cropping images

projects[manualcrop][version] = 1.x-dev
projects[manualcrop][subdir] = contrib
projects[manualcrop][download][type] = git
projects[manualcrop][download][revision] = 7237972
projects[manualcrop][download][branch] = 7.x-1.x
projects[manualcrop][patch][1665130] = http://drupal.org/files/issues/manualcrop-file_entity-settings-1665130-13.patch
projects[manualcrop][patch][2237835] = http://drupal.org/files/issues/manualcrop-two-crop-tools-2237835.patch
projects[manualcrop][patch][2248587] = http://drupal.org/files/issues/manualcrop-make-file-2248587.patch

; Manualcrop has its own .make file which gets these.
;
;libraries[jquery.imagesloaded][download][type] = get
;libraries[jquery.imagesloaded][download][url] = https://github.com/desandro/imagesloaded/archive/v2.1.2.tar.gz
;
;libraries[jquery.imgareaselect][download][type] = get
;libraries[jquery.imgareaselect][download][url] = http://odyniec.net/projects/imgareaselect/jquery.imgareaselect-0.9.10.zip
