---
core: 7.x
api: '2'
projects:
  eck:
    version: 2.0-rc9
    subdir: contrib
  geo_file_entity:
    subdir: contrib
    download:
      type: git
      url: https://github.com/NuCivic/geo_file_entity.git
      revision: be45046e636cfebbbb53a314c0f3693fc2e03d39
    type: module
  uuidreference:
    subdir: contrib
    version: 1.x-dev
    patch:
      238875: https://www.drupal.org/files/issues/uuidreference-alternative_to_module_invoke_all_implementation_for_query_alter_hook-238875-0.patch
libraries:
  chroma:
    download:
      type: file
      url: https://github.com/gka/chroma.js/zipball/1b311f3a45d26086eacfe2d4bae69365c1388e27
  numeral:
    download:
      type: file
      url: https://github.com/adamwdraper/Numeral-js/zipball/7de892ffb438af6e63b9c4f6aff0c9bc3932f09f
  recline_choropleth:
    download:
      type: file
      url: https://github.com/NuCivic/recline.view.choroplethmap.js/archive/402c573a2254bc30cc10041a57be6ed93be590b9.zip
  leaflet_zoomtogeometries:
    download:
      type: file
      url: https://github.com/NuCivic/leaflet.map.zoomToGeometries.js/zipball/08c19374b6f74a9efde979013c3c16266ab2b505
  nvd3:
    download:
      type: git
      url: https://github.com/novus/nvd3.git
      tag: v1.8.5
  d3:
    download:
      type: git
      url: https://github.com/d3/d3.git
      tag: v3.5.17
  gdocs:
    download:
      type: git
      url: https://github.com/okfn/recline.backend.gdocs.git
      revision: e81bb237759353932834a38a0ec810441e0ada10
  lodash_data:
    download:
      type: git
      url: https://github.com/NuCivic/lodash.data.git
      revision: 0dbe0701003b8a45037ab5fada630db2dbf75d9d
  spectrum:
    download:
      type: git
      url: https://github.com/bgrins/spectrum.git
      tag: 1.8.0
      revision: 9e04e5882de98cb9f909300b035d0f38c058c2fb
    destination: libraries
    directory_name: bgrins-spectrum
  reclineViewNvd3:
    download:
      type: git
      url: https://github.com/NuCivic/recline.view.nvd3.js.git
      revision: 18d75592120af4ba16f9b071ea78975b4f050fd8
