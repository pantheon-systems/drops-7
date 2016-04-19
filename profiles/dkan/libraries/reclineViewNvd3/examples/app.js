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
  });

})(window);
