core = 7.x
api = 2

projects[geofield][version] = 1.2
projects[geofield][subdir] = contrib

projects[geophp][version] = 1.7
projects[geophp][subdir] = contrib

libraries[leaflet_draw][type] = libraries
libraries[leaflet_draw][download][type] = git
libraries[leaflet_draw][download][url] = "https://github.com/Leaflet/Leaflet.draw.git"
libraries[leaflet_draw][download][revision] = "82f4d960a44753c3a9d98001e49e03429395b53a"

libraries[leaflet_core][type] = libraries
libraries[leaflet_core][download][type] = git
libraries[leaflet_core][download][url] = "https://github.com/Leaflet/Leaflet.git"
libraries[leaflet_core][download][revision] = "81221ae4cd9772a8974b2e3c867d4fb35abd052d"
