core = 7.x
api = 2

projects[geofield][version] = 1.2
projects[geofield][subdir] = contrib

projects[geophp][version] = 1.7
projects[geophp][subdir] = contrib

libraries[leaflet_draw][type] = libraries
libraries[leaflet_draw][download][type] = git
libraries[leaflet_draw][download][url] = "https://github.com/Leaflet/Leaflet.draw.git"
libraries[leaflet_draw][download][tag] = "v0.4.9"

# LEAFLET
libraries[leaflet][type] = libraries
libraries[leaflet][download][type] = git
libraries[leaflet][download][url] = "https://github.com/NuCivic/Leaflet.git"
libraries[leaflet][download][tag] = "v1.0.2-alt-marker-shadow-5258"