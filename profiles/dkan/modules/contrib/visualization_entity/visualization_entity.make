core = 7.x
api = 2

projects[eck][version] = 2.0-rc8
projects[eck][subdir] = contrib

projects[geo_file_entity][subdir] = contrib
projects[geo_file_entity][download][type] = git
projects[geo_file_entity][download][url] = https://github.com/NuCivic/geo_file_entity.git
projects[geo_file_entity][download][revision] = be45046e636cfebbbb53a314c0f3693fc2e03d39
projects[geo_file_entity][type] = module

projects[uuidreference][subdir] = contrib
projects[uuidreference][version] = 1.x-dev
projects[uuidreference][patch][238875] = https://www.drupal.org/files/issues/uuidreference-alternative_to_module_invoke_all_implementation_for_query_alter_hook-238875-0.patch

; Libraries
libraries[chroma][download][type] = "file"
libraries[chroma][download][url] = "https://github.com/gka/chroma.js/zipball/f1b7ca5cc4156f7d766e45e13ed6496c7b8ff7da"

libraries[numeral][download][type] = "file"
libraries[numeral][download][url] = "https://github.com/adamwdraper/Numeral-js/zipball/7487acb3b9b9d3be80d504b151681d1ff75224ce"

libraries[recline_choropleth][download][type] = "file"
libraries[recline_choropleth][download][url] = "https://github.com/NuCivic/recline.view.choroplethmap.js/archive/aca36cf4c3c408c67e2c20f1188931d61ffdec50.zip"

libraries[leaflet_zoomtogeometries][download][type] = "file"
libraries[leaflet_zoomtogeometries][download][url] = "https://github.com/NuCivic/leaflet.map.zoomToGeometries.js/zipball/08c19374b6f74a9efde979013c3c16266ab2b505"

# NVD3
libraries[nvd3][type] = libraries
libraries[nvd3][download][type] = git
libraries[nvd3][download][url] = "https://github.com/novus/nvd3.git"
libraries[nvd3][download][revision] = 7ebd54ca09061022a248bec9a050a4dec93e2b28

# D3
libraries[d3][type] = libraries
libraries[d3][download][type] = git
libraries[d3][download][url] = "https://github.com/d3/d3.git"
libraries[d3][download][tag] = v3.5.17

# GDOCS BACKEND
libraries[gdocs][type] = libraries
libraries[gdocs][download][type] = git
libraries[gdocs][download][url] = "https://github.com/okfn/recline.backend.gdocs.git"
libraries[gdocs][download][revision] = e81bb237759353932834a38a0ec810441e0ada10

# LODASH DATA
libraries[lodash_data][type] = libraries
libraries[lodash_data][download][type] = git
libraries[lodash_data][download][url] = "https://github.com/NuCivic/lodash.data.git"
libraries[lodash_data][download][revision] = 0dbe0701003b8a45037ab5fada630db2dbf75d9d

# SPECTRUM COLORPICKER
libraries[spectrum][type] = libraries
libraries[spectrum][download][type] = git
libraries[spectrum][download][url] = https://github.com/bgrins/spectrum.git
libraries[spectrum][destination] = libraries
libraries[spectrum][download][tag] = 1.8.0
libraries[spectrum][directory_name]= bgrins-spectrum

# RECLINE NVD3 VIEW
libraries[reclineViewNvd3][type] = libraries
libraries[reclineViewNvd3][download][type] = git
libraries[reclineViewNvd3][download][url] = "https://github.com/NuCivic/recline.view.nvd3.js.git"
libraries[reclineViewNvd3][download][revision] = dcf34811b24f1b48593f06c227a8c14a82972e3a
