;(function(){
  'use strict';
  // cartodb backend usage example
  $(document).on('ready', function(){
    var datasetWithLabels = demoFieldAsSeries();
    var datasetWithValues = demoValuesAsSeries();
    var cartoData = demoCartoDB();
    console.log('aa1', cartoData);
    cartoData.fetch().done(function () {
      // default population for carto dataset here:
      console.log('carto fetch complete', cartoData, query);

      // now get a subset via query
      var query = new recline.Model.Query();
      query.set('filters', 
        [
          {term : {'statename' : 'Texas'}}
        ]
      );

      cartoData.query(query, cartoData).done(function (data) {
        console.log('queried dataset', cartoData, data);
      });
    });
    
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
     * Multi Bar Chart
     */
    var multiBarChart = new recline.View.nvd3.multiBarChart({
        model: datasetWithLabels,
        state: twoDimensionWithLabels,
        el: $('#multiBarChart'),
    });
    multiBarChart.render();


    /**
     * Line Chart
     */
    var lineChart = new recline.View.nvd3.lineChart({
        model: datasetWithValues,
        state: twoDimensionWithValues,
        el: $('#lineChart'),
    });
    lineChart.render();


    /**
     * Multi Horizontal Bar Chart
     */
    var multiBarHorizontalChart = new recline.View.nvd3.multiBarHorizontalChart({
        model: datasetWithLabels,
        state: twoDimensionWithLabels,
        el: $('#multiBarHorizontalChart'),
    });
    multiBarHorizontalChart.render();


    /**
     * Pie Chart
     */
    var pieChart = new recline.View.nvd3.pieChart({
        model: datasetWithLabels,
        state: oneDimensionWithLabels,
        el: $('#pieChart'),
    });
    pieChart.render();


    /**
     * Stacked Area Chart
     */
    var stackedAreaChart = new recline.View.nvd3.stackedAreaChart({
        model: datasetWithValues,
        state: twoDimensionWithValues,
        el: $('#stackedAreaChart'),
    });
    stackedAreaChart.render();


    /**
     * Cumulative Line Chart
     */
    var cumulativeLineChart = new recline.View.nvd3.cumulativeLineChart({
        model: datasetWithValues,
        state: twoDimensionWithValues,
        el: $('#cumulativeLineChart'),
    });
    cumulativeLineChart.render();


    /**
     * Scatter Chart
     */
    var scatterChart = new recline.View.nvd3.scatterChart({
        model: datasetWithValues,
        state: twoDimensionWithValues,
        el: $('#scatterChart'),
    });
    scatterChart.render();


    /**
     * Line With Focus Chart
     */
    var lineWithFocusChart = new recline.View.nvd3.lineWithFocusChart({
        model: datasetWithValues,
        state: twoDimensionWithValues,
        el: $('#lineWithFocusChart'),
    });
    lineWithFocusChart.render();

    /**
     * Line Plus Bar Chart
     **/
     var linePlusBarChart = new recline.View.nvd3.linePlusBarChart({
        model: datasetWithValues,
        state: twoDimensionWithValues,
        el: $('#linePlusBarChart')
     });
     linePlusBarChart.render();

 /**
  * Line Plus Bar Chart - STATIC
 	**/
 var data = [{
    'key': 'foo',
        'bar': true,
        'color': 'skyblue',
        'values': [
        [1431993600000, 31.6882],
        [1432080000000, 76.1706],
        [1432166400000, 76.2297],
        [1432252800000, 75.1944],
        [1432339200000, 75.1536],
        [1432425600000, 74.528],
        [1432512000000, 75.7265],
        [1432598400000, 75.8659],
        [1432684800000, 74.6283],
        [1432771200000, 73.3533]
    ]
}, {
    'key': 'bar',
        'color': 'steelblue',
        'values': [
        [1431993600000, 0.0002997961386257345],
        [1432080000000, 0.0004418193656404055],
        [1432166400000, 0.0003122142681920564],
        [1432252800000, 0.00031651293181407124],
        [1432339200000, 0.0003845457835685849],
        [1432425600000, 0.00031934306569343066],
        [1432512000000, 0.0005163317993040745],
        [1432598400000, 0.00042575122683577205],
        [1432684800000, 0.00025057518394496457],
        [1432771200000, 0.00041715914621428076]
    ]
}];
nv.addGraph(function () {
		console.log('00');
    var chart = nv.models.linePlusBarChart()
        .margin({
            top: 30,
            right: 60,
            bottom: 50,
            left: 70
        })
        .x(function (d, i) { return i; })
        .y(function (d, i) { return d[1]; })
        .options({focusEnable: false});

		console.log('01');
    chart.xAxis.showMaxMin(true)
        .tickFormat(function (d) {
        var dx = data[0].values[d] && data[0].values[d][0] || 0;
        return d3.time.format('%x')(new Date(dx));
    });

		console.log('02');
    chart.y1Axis.tickFormat(d3.format(',f'));

		console.log('03');
    chart.y2Axis.tickFormat(function (d) {
        return d3.format('g')(d)
    });

		console.log('04');
    chart.bars.forceY([0, 200]);
    chart.lines.forceY([0]);

		console.log('05');
    d3.select('#linePlusBarChartStatic svg')
        .datum(data)
        .transition()
        .duration(0)
        .call(chart);

		console.log('06');
    nv.utils.windowResize(chart.update);

		console.log('07');
    return chart;
});

});
  
})(window);
