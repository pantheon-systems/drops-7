core = 7.x
api = 2

projects[eck][version] = 2.0-rc8
projects[eck][subdir] = contrib

projects[geo_file_entity][subdir] = contrib
projects[geo_file_entity][download][type] = git
projects[geo_file_entity][download][url] = https://github.com/NuCivic/geo_file_entity.git
projects[geo_file_entity][download][branch] = master
projects[geo_file_entity][type] = module

projects[uuidreference][subdir] = contrib
projects[uuidreference][version] = 1.x-dev
projects[uuidreference][patch][238875] = https://www.drupal.org/files/issues/uuidreference-alternative_to_module_invoke_all_implementation_for_query_alter_hook-238875-0.patch

; Libraries
libraries[chroma][download][type] = "file"
libraries[chroma][download][url] = "https://github.com/gka/chroma.js/zipball/master"

libraries[numeral][download][type] = "file"
libraries[numeral][download][url] = "https://github.com/adamwdraper/Numeral-js/zipball/master"

libraries[recline_choropleth][download][type] = "file"
libraries[recline_choropleth][download][url] = "https://github.com/NuCivic/recline.view.choroplethmap.js/archive/master.zip"

libraries[leaflet_zoomtogeometries][download][type] = "file"
libraries[leaflet_zoomtogeometries][download][url] = "https://github.com/NuCivic/leaflet.map.zoomToGeometries.js/zipball/master"
