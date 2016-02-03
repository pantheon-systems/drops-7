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

#### Step One - Choose a Resource
Use the form to upload or link to a data source for the visulization.
Valid source data include:
* CSV
* Google Spreadsheet
* Data Proxy

#### Step Two - Define Variables
Add metadata fields to the visualization including *categories* and *tags*.
Choose a single *x-field* and one or more *y-fields* for the visualization.

#### Step Three - Choose Chart Type

#### Step Four - Preview and Adjust
##### Query Editor

##### Filter Editor
###### Add Filter
* **Add filter** function to filter the data. Select a field and choose a value, range or, if appropriate, geo data.
Multiple filters can be applied to data.

##### Chart Configuration
**NOTE**:X and Y Axis Fields are not supported by chart type *Pie Chart*
###### X Axis
* **Format** Select an appropriate format for the X Axis labels.
* **Axis Label**  will provide a custom label for the x axis. **Note:** Axis labels do not display for Pie Charts.
* **Label rotation** will change angle of label values.
* **Tick Values** with **step value** will update the number of ticks for the X axis. **NOTE:** If the range set for tick values is smaller than the range of complete data represented, the chart will be abreviated.


###### Y Axis
* **Axis Label** Provide a custom label for the x axis. **Note:** Axis labels do not display for Pie Charts. **Increase *distance* field until Label is visible on chart.
* **Tick Values** with **step value** to update the number of ticks for the Y axis. **NOTE:** If the range set for tick values is smaller than the range of complete data represented, the chart will be abreviated.

###### General
* **Color**: Set the color the chart is drawn in. Use either a [HEX color code](http://www.w3schools.com/tags/ref_colorpicker.asp) or a [valid css color name ](http://www.w3schools.com/cssref/css_colornames.asp)
* **Transition Time**: Time in ms it takes for graph to animate.
* Goal
**From** - 
**To** - 
**Label Outside** - 
**Show Label** - 
* **Margin** - Enter value of margin in the order: *top, right, bottom, left*
* **Show Title**
* **Show Controls**
* **Show Legend**
* **Group By X Field** - For multiple series values Y will show values grouped by X
* **Show Tooltips** - Shows data and label on hover
* **Reduce Ticks** - 

### Recline
The bundle also includes an integration with the [Recline module](https://github.com/NuCivic/recline). If you have a content type with a recline file field, you can add a Recline Field Reference field to your chart bundle. This field type is defined in a module that comes bundled with [Visualization Entity](https://github.com/NuCivic/visualization_entity). The included DKAN integration module adds a Recline Field Reference pointing specifically at DKAN's Resource content type. In this case, entering an existing Resource node in the reference field will automatically populate the resource file into the chart entity's file field.
