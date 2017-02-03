## Introduction
This library provides recline views and UI controls to create NVD3 charts.

## Supported charts
 - Cumulative line
 - Discrete bar
 - Line
 - Line with explorer
 - Multibar
 - Multibar horizontal
 - Pie
 - Scatter
 - Stacked area 

## Getting started

If you want handle all the configuration stuff by code you only need to add this file to your html:

```html
<script type="text/javascript" src="recline.view.nvd3.min.js"></script>
```

However, if you want to provide a UI for chart's manipulation you will need to add another one:

```html
<script type="text/javascript" src="recline.view.nvd3.controls.min.js"></script>
```


### Dummy data
```javascript
/* jshint ignore:start */
function demoFieldAsSeries() {
  var dataset = new recline.Model.Dataset({
     records: [
      {id: 0, state: 'Idaho', total: 861, ratio: 776},
      {id: 1, state: 'Minnesota', total: 3017, ratio: 778},
      {id: 2, state: 'Hawaii', total: 652, ratio: 797},
      {id: 3, state: 'Iowa', total: 1365, ratio: 979},
      {id: 4, state: 'Oregon', total: 1630, ratio: 1028},
      {id: 5, state: 'Idaho', total: 1000, ratio: 500},
    ]
  });
  return dataset;
}

function demoValuesAsSeries(){
  var dataset = new recline.Model.Dataset({
    records: [
      {id: 0, date: '2011-01-01', x: 1, y: 210, z: 100, country: 'DE', title: 'first', lat:52.56, lon:13.40},
      {id: 1, date: '2011-02-02', x: 2, y: 312, z: 200, country: 'UK', title: 'second', lat:54.97, lon:-1.60},
      {id: 2, date: '2011-03-03', x: 3, y: 645, z: 150, country: 'US', title: 'third', lat:40.00, lon:-75.5},
      {id: 3, date: '2011-04-04', x: 4, y: 123, z: 300, country: 'DE', title: 'fourth', lat:57.27, lon:-6.20},
      {id: 4, date: '2011-05-04', x: 5, y: 756, z: 800, country: 'UK', title: 'fifth', lat:51.58, lon:0},
      {id: 6, date: '2011-06-02', x: 6, y: 132, z: 120, country: 'US', title: 'sixth', lat:51.04, lon:7.9},
    ]
  });
  return dataset;
}

var datasetWithLabels = demoFieldAsSeries();
var datasetWithValues = demoValuesAsSeries();

var oneDimensionWithLabels = new recline.Model.ObjectState({
  xfield: 'state',
  seriesFields: ['total'],
  group: true
});

var twoDimensionWithLabels = new recline.Model.ObjectState({
  xfield: 'state',
  seriesFields: ['total', 'ratio'],
  group: true
});

var twoDimensionWithValues = new recline.Model.ObjectState({
  xfield: 'date',
  seriesFields: ['y', 'z'],
});

/* jshint ignore:end */
```

## Examples

```javascript
/**
 * Discrete Bar Chart
 */
var discreteBar = new recline.View.nvd3.discreteBarChart({
    model: datasetWithLabels,
    state: oneDimensionWithLabels,
    el: $('#discreteBar'),
});
discreteBar.render();

/**
 * Line Chart
 */
var lineChart = new recline.View.nvd3.lineChart({
    model: datasetWithValues,
    state: twoDimensionWithValues,
    el: $('#lineChart'),
});
lineChart.render();  
```

More examples in the directory examples.

## Caveats
All the nvd3 options are available through the option key in the state object:

```javascript

var state = new recline.Model.ObjectState({
  xfield: 'state',
  seriesFields: ['total'],
  group: true,
  options: {
    donut: true
  }
});

/**
 * Pie Chart
 */
var pieChart = new recline.View.nvd3.pieChart({
    model: datasetWithLabels,
    state: state,
    el: $('#pieChart'),
});
pieChart.render();
```

## How to run tests and examples

### Requirements
- NodeJS
- Bower
- NPM

### Run examples
```bash
git clone git@github.com:NuCivic/recline.view.nvd3.js.git recline.view.nvd3.js
cd recline.view.nvd3.js
npm install
bower install
grunt
```

### Run tests
```bash
node_modules/karma/bin/karma start --single-run --browsers PhantomJS
```
