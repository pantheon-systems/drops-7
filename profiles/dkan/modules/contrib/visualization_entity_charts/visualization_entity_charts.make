core = 7.x
api = 2

# Recline.js
includes[recline_make] = https://raw.githubusercontent.com/NuCivic/recline/7.x-1.x/recline.make

# NVD3
libraries[nvd3][type] = libraries
libraries[nvd3][download][type] = git
libraries[nvd3][download][url] = "https://github.com/novus/nvd3.git"
libraries[nvd3][download][revision] = "7ebd54ca09061022a248bec9a050a4dec93e2b28"

# D3
libraries[d3][type] = libraries
libraries[d3][download][type] = git
libraries[d3][download][url] = "https://github.com/mbostock/d3.git"
libraries[d3][download][revision] = "f82dd6fb414a15bca4f9c39c7c9442295ddea416"

# GDOCS BACKEND
libraries[gdocs][type] = libraries
libraries[gdocs][download][type] = git
libraries[gdocs][download][url] = "https://github.com/okfn/recline.backend.gdocs.git"
libraries[gdocs][download][revision] = "e81bb237759353932834a38a0ec810441e0ada10"

# LODASH DATA
libraries[lodash_data][type] = libraries
libraries[lodash_data][download][type] = git
libraries[lodash_data][download][url] = "https://github.com/NuCivic/lodash.data.git"
libraries[lodash_data][download][revision] = "0dbe0701003b8a45037ab5fada630db2dbf75d9d"

# RECLINE NVD3 VIEW
libraries[reclineViewNvd3][type] = libraries
libraries[reclineViewNvd3][download][type] = git
libraries[reclineViewNvd3][download][url] = "https://github.com/NuCivic/recline.view.nvd3.js.git"
libraries[reclineViewNvd3][download][revision] = "e20af6f5275b709b20da5f599870a28335394bce"
