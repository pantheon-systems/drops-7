core = 7.x
api = 2

projects[] = drupal

projects[dkan][type] = profile
projects[dkan][download][type] = git
projects[dkan][download][url] = git://github.com/nucivic/dkan.git
projects[dkan][download][revision] = 7.x-1.x

# RECLINE
libraries[recline][type] = libraries
libraries[recline][download][type] = git
libraries[recline][download][url] = "https://github.com/NuCivic/recline.js.git"
libraries[recline][download][revision] = "d2640036dbb08ca3ab1a269b0544c35a41aabe03"
