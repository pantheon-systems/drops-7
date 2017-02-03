# Open Data Schema Map: DCAT-AP

Schema module to provide endpoints for [DCAT-AP, the DCAT Application profile for data portals in Europe](https://joinup.ec.europa.eu/asset/dcat_application_profile/asset_release/dcat-ap-v11). The schema provided is in JSON but includes XML prefixes, and is indended primarily for output to XML/RDF.

Two schemas are actually provided:

* **DCAT Catalog:** Can be used to create an entire data catalog, usually in XML format and exposed at `/catalog.xml`.
* **DCAT Dataset**: Schema for providing a single-dataset endpoint

[DKAN's implementation](https://github.com/NuCivic/open_data_schema_map_dkan) adds meta links in page headers to catalog.xml, and links in each dataset page (as well as header links) to the dataset-specific endpoints.
