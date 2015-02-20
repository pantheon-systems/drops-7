[![Build Status](https://travis-ci.org/NuCivic/open_data_schema_map.svg?branch=master)](https://travis-ci.org/NuCivic/open_data_schema_map)

Open Data Schema Map
====================

This module provides a flexible way to expose your Drupal content via APIs following specific Open Data schemas. Currently, the [CKAN](http://docs.ckan.org/en/ckan-1.8/domain-model-dataset.html) and [Project Open Data schemas](http://project-open-data.github.io/schema/) are provided, but new schemas can be easily added through your own modules. A user interface is in place to create endpoints and map fields from the chosen schema to Drupal content using tokens.

DKAN specific implmentation: https://github.com/NuCivic/open_data_schema_map_dkan

## Basic concepts

### Schema
A schema is a list of field definitions, usually representing a community specification for presenting machine-readable data. The core Open Data Schema Map module does not include any schemas; they are provided by additional modules. A schema module includes 

* a stanard Drupal .module file -- with an implementation of ```hook_open_data_schema()``` to expose the schema to the core Open Data Schema Map module, plus _alter functions for any needed modifications of the UI form or the data output itself.
* the schema itself, expressed as a .json file. For instance, see the [Project Open Data schema file](https://github.com/NuCivic/open_data_schema_map/blob/master/modules/open_data_schema_pod/data/single_entry.json) to see how these schema are defined in JSON


### API
An API in this module is a configuration set that exposes a specific set of machine-readable data at a specific URL (known as the API's endpoint). This module allows you to create multiple APIs that you save as database records and/or export using [Features](http://drupal.org/project/features). An API record will contain 

* an endpoint URL
* a schema (chosen from the available schemas provided by the additional modules as described above)
* a mapping of fields defined in that schema to Drupal tokens (usually referencing fields from a node)
* optionally, one or more arguments passed through the URL to filter the result set

## Usage

### Installation

Enable the main _Open Data Schema Map_ module as usual, and additionally enable any schema modules you will need to create your API. 

### Creating APIs

Navigate to admin/config/services/odsm and click "Add API."  

![screen shot 2014-07-14 at 3 24 03 pm](https://cloud.githubusercontent.com/assets/309671/3575902/c7ff24e6-0b8c-11e4-92c3-9ba2e163bf56.png)

Give the API a title, machine name, chose which entity type (usually _node_) and bundle (in [DKAN](https://github.com/NuCivic/dkan), this is usually _Dataset_). 

![screen shot 2014-07-14 at 3 46 39 pm](https://cloud.githubusercontent.com/assets/309671/3576163/b3e6ea90-0b8f-11e4-9d9e-33b4515310f0.png)

You will need to create the API record before adding arguments and mappings.

### Arguments

The results of the API call can be filtered by a particular field via arguments in the URL. To add an argument, first choose the schema field then, if you are filtering by a custom field API field (ie, a field whose machine name begins with "field\_"), identify the database column that would contain the actual argument value. Leave off the field name prefix; for instance, if filtering by a DKAN tag (a term reference field), the correct column is field_tags_tid, so you would enter "tid". Which Drupal field to use will be extrapolated from the token you map to that schema field.

![Screen Shot 2014-07-14 at 3.55.49 PM.png | uploaded via ZenHub](https://cloud.githubusercontent.com/assets/512243/5281816/992d1138-7ac6-11e4-8e7b-bcaefa733648.png)

### Field Mapping

The API form presents you with a field for each field in your schema. Map the fields using Drupal's token system. Note: using more than one token in a single field may produce unexpected results and is not recommended. 

#### Multi-value fields

For Drupal multi-value entity reference fields, the schema can use an array to instruct the API to iterate over each value and map the referenced data to multiple schema fields. For instance, in the CKAN schema, tags are described like this in schema_ckan.json:

```    
      "tags": {
      "title":"Tags",
      "description":"",
      "anyOf": [
        {
          "type": "array",                    
          "items": {
            "type": "object",
            "properties": {
              "id": {
                "title": "UUID",
                "type": "string"
              },
              "vocabulary_id": {
                "title": "Vocaulary ID",
                "type": "string"
              },
              "name": {
                "title": "Name",
                "type": "string"
              },
              "revision_timestamp": {
                "title": "Revision Timestamp",
                "type": "string"
              },
              "state": {
                "title": "state",
                "description": "",
                "type": "string",
                "enum": ["uncomplete", "complete", "active"]
              }
            }
          }
        }
      ]
    },
```

You can chose which of the available multivalue fields on your selected bundle to map to the "tags" array, exposing all of the referenced "tag" entities (taxonomy terms in this example) to use as the context for your token mappings on the schema fields within that array. First, simply chose the multivalue field, leaving the individual field mappings blank, and save the form. 

![screen shot 2014-07-16 at 12 14 29 am](https://cloud.githubusercontent.com/assets/309671/3594511/c3ca9cd4-0c9f-11e4-8fd0-1ea7c3c8b2b3.png)

When you return to the tags section of the form after saving, you will now see a special token navigator you can use to find tokens that will work with this iterative approach (using "Nth" in place of the standard delta value in the token):

![screen shot 2014-07-16 at 12 22 00 am](https://cloud.githubusercontent.com/assets/512243/5281826/ad5e3eac-7ac6-11e4-8c7d-91076527c84d.png)

## Customizing

### Adding new schemas

You are not limited by the schemas included with this module; any Open Data schema may be defined in a custom module. Use the open_data_schema_ckan module as a model to get started.

### Date format
Date formats can be chanaged manually by changing the "Medium" date time format in "admin/config/regional/date-time" or in code by using one of the alter hooks:
![screen shot 2014-09-04 at 11 15 01 am](https://cloud.githubusercontent.com/assets/512243/4152408/a9cb06b2-344e-11e4-84c8-c2174b5fc566.png)


