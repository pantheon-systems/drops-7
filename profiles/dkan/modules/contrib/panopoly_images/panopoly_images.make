; Panopoly Images Makefile

api = 2
core = 7.x

; Cropping images

projects[manualcrop][version] = 1.x-dev
projects[manualcrop][subdir] = contrib
projects[manualcrop][download][type] = git
projects[manualcrop][download][revision] = d6c449d
projects[manualcrop][download][branch] = 7.x-1.x

; jquery.imagesLoaded library for manualcrop
libraries[jquery.imagesloaded][download][type] = file
libraries[jquery.imagesloaded][download][url] = https://github.com/desandro/imagesloaded/archive/v2.1.2.tar.gz
libraries[jquery.imagesloaded][download][subtree] = imagesloaded-2.1.2

; jquery.imgAreaSelect library for manualcrop
libraries[jquery.imgareaselect][download][type] = file
libraries[jquery.imgareaselect][download][url] = https://github.com/odyniec/imgareaselect/archive/v0.9.11-rc.1.tar.gz
libraries[jquery.imgareaselect][download][subtree] = imgareaselect-0.9.11-rc.1
