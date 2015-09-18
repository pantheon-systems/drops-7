# Visualization Entity Charts
This modules provides the ability to create embedable NVD3 charts.

### Installation 
- Make sure you have [visualization_entity](https://github.com/NuCivic/visualization_entity) module already installed and enabled.

## Install from github zip
```
# cd into your site's docroot and:
mkdir -p sites/all/modules/nucivic/
cd sites/all/modules/nucivic
wget https://github.com/NuCivic/visualization_entity_charts/archive/master.zip
unzip master.zip
mv visualization_entity_charts-master visualization_entity_charts
cd ../../../../
drush make --no-core sites/all/modules/nucivic/visualization_entity_charts/visualization_entity_charts.make
drush -y en visualization_entity_charts
drush cc all
```

## Install from git working copy

```
# cd into your site's docroot and:
mkdir -p sites/all/modules/nucivic/
cd sites/all/modules/nucivic/
git clone git@github.com:NuCivic/visualization_entity_charts.git
cd ../../../../
drush make --no-core sites/all/modules/nucivic/visualization_entity_charts/visualization_entity_charts.make
drush -y en visualization_entity_charts
drush cc all
```

### Usage

Once the module is enabled, new chart entities can be created by going to /admin/structure/entity-type/visualization/ve_chart/add. A multi-step process will guide you through creation of a chart based on an uploaded data file.

The bundle also includes an integration with the [Recline module](https://github.com/NuCivic/recline). If you have a content type with a recline file field, you can add a Recline Field Reference field to your chart bundle. This field type is defined in a module that comes bundled with [Visualization Entity](https://github.com/NuCivic/visualization_entity). The included DKAN integration module adds a Recline Field Reference pointing specifically at DKAN's Resource content type. In this case, entering an existing Resource node in the reference field will automatically populate the resource file into the chart entity's file field.