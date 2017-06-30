/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.View = this.recline.View || {};

;(function ($, my) {
'use strict';

var ALLOWED_TICKS = 100;

my.BaseControl = Backbone.View.extend({
  templateTop:
            '<div id="control-chart-container">' +
              '<div class="recline-nvd3-query-editor"></div>' +
              '<div class="recline-nvd3-filter-editor"></div>' ,
  templateXFormat:
              '<fieldset id="x-axis">' +
                '<legend>X Axis</legend>' +
              '<div class="form-group">' +
                '<label for="control-chart-x-format">X-Format</label>' +
                '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                  'title="X Axis Data Format" data-content="Select the format and display option for the x-axis data."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '<select class="form-control" id="control-chart-x-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value="%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="Percentage" value="p">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageInt" value="d">97 -> 97%</option>' +
                      '<option data-type="PercentageInt" value=",.2f">97 -> 97.00%</option>' +
                    '</optgroup>' +
                '</select>' +
              '</div>' +
              '<div class="form-group">' +
                '<label for="control-chart-label-x-rotation">X Label Rotation</label>' +
                '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                  'title="X Label Rotation" data-content="If your x-axis labels are long text values it will help to rotate them to avoid overlapping text. Enter a value to define the angle of the labels, 45 is recommended."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '<input aria-label="X label rotation" value="{{options.xAxis.rotateLabels}}" type="text" id="control-chart-label-x-rotation" class="form-control" placeholder="e.g: -45"/>' +
              '</div>' +

              // Axis label.
              '<div class="form-group">' +
                '<div class="row">' +
                  '<div class="col-md-8 col-sm-8">' +
                    '<label for="control-chart-x-axis-label">X Axis Label</label>' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="X Axis Label" data-content="Provide a label to appear along the x-axis."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                    '<input aria-label="X axis label" class="form-control" type="text" id="control-chart-x-axis-label" value="{{options.xAxis.axisLabel}}"/>' +
                  '</div>' +
                  '<div class="col-md-4 col-sm-4">' +
                    '<label for="control-chart-y-axis-label-distance">Distance</label>' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-placement="left" data-trigger="focus"' +
                      'title="Adjust x-axis label position" data-content="This only effects the x-axis label if using the horizontal bar chart. If your axis label overlaps the data labels you can move the label left with positive values, and right with negative values. ' +
                      'You may need to adjust the left margin of the chart as well."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                    '<input aria-label="X axis label distance" class="form-control" type="number" id="control-chart-x-axis-label-distance" value="{{options.xAxis.axisLabelDistance}}"/>' +
                  '</div>' +
                '</div>' +
              '</div>' +

              /// Axis ticks
              '<div class="form-group axis-ticks">' +
                '<div class="row">' +
                  '<div class="col-md-8 col-sm-8">' +
                    '<label for="control-chart-x-values">Tick Values' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Tick and Step Values" data-content="Enter a numerical range to set the start and end values to display. Use the Step field to define the value between each tick within the range."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a></label>' +
                    '<input aria-label="Tick values from" class="form-control" type="text" placeholder="From.." id="control-chart-x-values-from" value="{{xValuesFrom}}"/>' +
                    '<input aria-label="Tick values to" class="form-control" type="text" placeholder="To.." id="control-chart-x-values-to" value="{{xValuesTo}}"/>' +
                  '</div>' +
                  '<div class="col-md-4 col-sm-4">' +
                    '<label for="control-chart-x-values-step">Step</label>' +
                    '<input aria-label="x step value" class="form-control" type="number" id="control-chart-x-values-step" value="{{xValuesStep}}"/>' +
                  '</div>' +
                '</div>' +
              '</div>' +
              '<span class="help-block alert-danger">{{errors.xTicks}}</span>' +
            '</fieldset>',
          templateYFormat:

              //////// Y AXIS
              '<fieldset id="y-axis">' +
                '<legend>Y Axis</legend>' +

                // Format.
                '<div class="form-group">' +
                  '<label for="control-chart-y-format">Format</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                  'title="Y Axis Data Format" data-content="Select the format and display option for the y-axis data."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                  '<select aria-label="y axis format" class="form-control" id="control-chart-y-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="PercentageB" value=".2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value=".0f">97 -> 97%</option>' +
                      '<option data-type="PercentageA" value=".2f">97 -> 97.00%</option>' +
                    '</optgroup>' +
                  '</select>' +
                '</div>' +

                // Axis label.
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-8 col-sm-8">' +
                      '<label for="control-chart-y-axis-label">Y Axis Label</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Y Axis Label" data-content="Provide a label to appear along the y-axis"><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                      '<input aria-label="y axis label" class="form-control" type="text" id="control-chart-y-axis-label" value="{{options.yAxis.axisLabel}}"/>' +
                    '</div>' +
                    '<div class="col-md-4 col-sm-4">' +
                      '<label for="control-chart-y-axis-label-distance">Distance</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-placement="left" data-trigger="focus"' +
                      'title="Adjust y-axis label position." data-content="If your axis label overlaps the data labels you can move the label left with positive values, and right with negative values. You may need to adjust the left margin of the chart as well."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                      '<input aria-label="y axis label distance" class="form-control" type="number" id="control-chart-y-axis-label-distance" value="{{options.yAxis.axisLabelDistance}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Axis ticks
                '<div class="form-group axis-ticks">' +
                  '<div class="row">' +
                    '<div class="col-md-8 col-sm-8">' +
                      '<label for="control-chart-y-values">Tick Values' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Tick and Step Values" data-content="Enter a numerical range to set the start and end values to display. Use the Step field to define the value between each tick within the range."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a></label>' +
                      '<input aria-label="y axis tick values from" class="form-control" placeholder="From.." type="text" id="control-chart-y-values-from" value="{{yValuesFrom}}"/>' +
                      '<input aria-label="y axis tick values to" class="form-control" placeholder="To.." type="text" id="control-chart-y-values-to" value="{{yValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-4 col-sm-4">' +
                      '<label for="control-chart-y-values-step">Step</label>' +
                      '<input aria-label="y step value" class="form-control" type="number" id="control-chart-y-values-step" value="{{yValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
                '<span class="help-block alert-danger">{{errors.yTicks}}</span>' +
              '</fieldset>',
  templateY1Format:
              //////// Y1 AXIS
              '<fieldset id="y1-axis">' +
                '<legend>Y-1 Axis</legend>' +

                // Format.
                '<div class="form-group">' +
                  '<label for="control-chart-y1-format">Format</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                  'title="Y Axis Data Format" data-content="Select the format and display option for the y-axis data."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                  '<select aria-label="y1 axis format" class="form-control" id="control-chart-y1-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="Percentage" value="p,.2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value="">97 -> 97.00%</option>' +
                    '</optgroup>' +
                  '</select>' +
                '</div>' +

                // Axis label.
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-8 col-sm-8">' +
                      '<label for="control-chart-y1-axis-label">Y Axis Label</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Y Axis Label" data-content="Provide a label to appear along the y-axis"><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                      '<input aria-label="y1 axis label" class="form-control" type="text" id="control-chart-y1-axis-label" value="{{options.y1Axis.axisLabel}}"/>' +
                    '</div>' +
                    '<div class="col-md-4 col-sm-4">' +
                      '<label for="control-chart-y1-axis-label-distance">Distance</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-placement="left" data-trigger="focus"' +
                      'title="Adjust y-axis label position." data-content="If your axis label overlaps the data labels you can move the label left with positive values, and right with negative values. You may need to adjust the left margin of the chart as well."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                      '<input aria-label="y1 axis label distance" class="form-control" type="number" id="control-chart-y1-axis-label-distance" value="{{options.y1Axis.axisLabelDistance}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Axis ticks
                '<div class="form-group axis-ticks">' +
                  '<div class="row">' +
                    '<div class="col-md-8 col-sm-8">' +
                      '<label for="control-chart-y1-values">Tick Values' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Tick and Step Values" data-content="Enter a numerical range to set the start and end values to display. Use the Step field to define the value between each tick within the range."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a></label>' +
                      '<input aria-label="y1 axis tick values from" class="form-control" placeholder="From.." type="text" id="control-chart-y1-values-from" value="{{y1ValuesFrom}}"/>' +
                      '<input aria-label="y1 axis tick values to" class="form-control" placeholder="To.." type="text" id="control-chart-y1-values-to" value="{{y1ValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-4 col-sm-4">' +
                      '<label for="control-chart-y1-values-step">Step</label>' +
                      '<input aria-label="y1 step value" class="form-control" type="number" id="control-chart-y1-values-step" value="{{y1ValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
                '<span class="help-block alert-danger">{{errors.y1Ticks}}</span>' +
              '</fieldset>',
  templateY2Format:
              //////// Y2 AXIS
              '<fieldset id="y2-axis">' +
                '<legend>Y-2 Axis</legend>' +

                // Format.
                '<div class="form-group">' +
                  '<label for="control-chart-y2-format">Format</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                  'title="Y Axis Data Format" data-content="Select the format and display option for the y-axis data."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                  '<select aria-label="y2 axis format" class="form-control" id="control-chart-y2-format">' +
                    '<optgroup label="Text">' +
                      '<option data-type="String" value="">Text</option>' +
                    '</optgroup>' +
                    '<optgroup label="Numbers">' +
                      '<option data-type="Number" value="d">100,000</option>' +
                      '<option data-type="Number" value=",.1f">100,000.0</option>' +
                      '<option data-type="Number" value=",.2f">100,000.00</option>' +
                      '<option data-type="Number" value="s">100K</option>' +
                    '</optgroup>' +
                    '<optgroup label="Date">' +
                      '<option data-type="Date" value="%m/%d/%Y">mm/dd/yyyy</option>' +
                      '<option data-type="Date" value=""%m-%d-%Y">mm-dd-yyyy</option>' +
                      '<option data-type="Date" value="%Y">Year</option>' +
                    '</optgroup>' +
                    '<optgroup label="Currency">' +
                      '<option data-type="Number" value="$,.2f">$100,000.00</option>' +
                      '<option data-type="Number" value="$,.1f">$100,000.0</option>' +
                      '<option data-type="Number" value="$,">$100,000</option>' +
                    '</optgroup>' +
                    '<optgroup label="Percentage">' +
                      '<option data-type="Percentage" value="%">.97 -> 97%</option>' +
                      '<option data-type="Percentage" value="p,.2f">.97 -> 97.00%</option>' +
                      '<option data-type="PercentageA" value="d">97 -> 97%</option>' +
                      '<option data-type="PercentageA" value="">97 -> 97.00%</option>' +
                    '</optgroup>' +
                  '</select>' +
                '</div>' +

                // Axis label.
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-8 col-sm-8">' +
                      '<label for="control-chart-y2-axis-label">Y Axis Label</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Y Axis label" data-content="Provide a label to appear along the y-axis"><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                      '<input aria-label="y2 axis label" class="form-control" type="text" id="control-chart-y2-axis-label" value="{{options.y2Axis.axisLabel}}"/>' +
                    '</div>' +
                    '<div class="col-md-4 col-sm-4">' +
                      '<label for="control-chart-y2-axis-label-distance">Distance</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-placement="left" data-trigger="focus"' +
                      'title="Adjust y-axis label position." data-content="If your axis label overlaps the data labels you can move the label left with positive values, and right with negative values. You may need to adjust the left margin of the chart as well."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                      '<input aria-label="y2 axis label distance" class="form-control" type="number" id="control-chart-y2-axis-label-distance" value="{{options.y2Axis.axisLabelDistance}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                /// Axis ticks
                '<div class="form-group axis-ticks">' +
                  '<div class="row">' +
                    '<div class="col-md-8 col-sm-8">' +
                      '<label for="control-chart-y2-values">Tick Values' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Tick and Step Values" data-content="Enter a numerical range to set the start and end values to display. Use the Step field to define the value between each tick within the range."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a></label>' +
                      '<input aria-label="y2 axis tick values from" class="form-control" placeholder="From.." type="text" id="control-chart-y2-values-from" value="{{y2ValuesFrom}}"/>' +
                      '<input aria-label="y2 axis tick values to" class="form-control" placeholder="To.." type="text" id="control-chart-y2-values-to" value="{{y2ValuesTo}}"/>' +
                    '</div>' +
                    '<div class="col-md-4 col-sm-4">' +
                      '<label for="control-chart-y2-values-step">Step</label>' +
                      '<input aria-label="y2 step value" class="form-control" type="number" id="control-chart-y2-values-step" value="{{y2ValuesStep}}"/>' +
                    '</div>' +
                  '</div>' +
                '</div>' +
                '<span class="help-block alert-danger">{{errors.y2Ticks}}</span>' +
              '</fieldset>',
  templateGoal:
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-12 col-sm-12">' +
                      '<label>Goal</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Display a goal line" data-content="Setting a goal line will provide your users with a way of tracking performance against expectation. Enter a value for the goal in the first field. Enter a color value in the second field if you do not want to use the default color (red). Check the [Show label] box to label the goal line with the word [TARGET]. The label will display inside the chart unless you also check the [Label outside] box."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                    '</div>' +
                  '</div>' +
                  '<div class="row">' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input id="control-chart-goal-value" type="text" class="form-control" aria-label="Goal value" placeholder="50" value="{{goal.value}}">' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input id="control-chart-goal-color" type="text" class="form-control" aria-label="Goal color" placeholder="red" value="{{goal.color}}">' +
                    '</div>' +
                    '<div class="col-md-6 col-sm-3">' +
                      '<div class="form-group checkbox checkbox-without-margin">' +
                        '<label for="control-chart-goal-outside">' +
                          '<input aria-label="Goal label outside" type="checkbox" id="control-chart-goal-outside" value="{{goal.outside}}" {{#goal.outside}}checked{{/goal.outside}}/> Label outside' +
                        '</label>' +
                      '</div>' +
                      '<div class="form-group checkbox checkbox-without-margin">' +
                        '<label for="control-chart-goal-label">' +
                          '<input type="checkbox" id="control-chart-goal-label" value="{{goal.label}}" {{#goal.label}}checked{{/goal.label}}/> Show label' +
                        '</label>' +
                      '</div>' +
                    '</div>' +
                  '</div>' +
                '</div>',
  templateGeneral:
              // ////// GENERAL.
              '<fieldset>' +
                '<legend>General</legend>' +

                // Color.
                '<div class="form-group">' +
                    '<label for="control-chart-color">Color</label>' +
                    '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Select Colors" data-content="Override the default colors used for the chart by entering hex values or color names here. Separate colors with commas."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                    '<input aria-label="chart colors" class="form-control" type="text" id="control-chart-color" value="{{options.color}}" placeholder="e.g: #FF0000,green,blue,#00FF00"/>' +
                '</div>' +

                // Goal.
                '<div id="goal-controls"></div>' +

                // Data sort.
                '<div class="form-group">' +
                  '<label for="control-chart-sort">Sort</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Adjust Sort Order" data-content="Select the column by which to sort the data."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                  '<select aria-label="Data sort" id="control-chart-sort" class="form-control chosen-select">' +
                    '{{#sortFields}}' +
                      '<option value="{{value}}" {{#selected}} selected{{/selected}}>{{name}}</option>' +
                    '{{/sortFields}}' +
                  '</select>' +
                '</div>' +

                // Margin.
                '<div class="form-group">' +
                  '<div class="row">' +
                    '<div class="col-md-12 col-sm-12">' +
                      '<label>Margin</label>' +
                      '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Adjust space around the chart" data-content="If your labels, legends and controls are being cut off or overlapping, you can increase ' +
                      'the default margins around the chart. The order is Top, Right, Bottom, and Left."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                    '</div>' +
                  '</div>' +
                  '<div class="row">' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input aria-label="Margin top" id="control-chart-margin-top" type="text" class="form-control" placeholder="Top" value="{{options.margin.top}}">' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input aria-label="Margin right" id="control-chart-margin-right" type="text" class="form-control" placeholder="Right" value="{{options.margin.right}}">' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input aria-label="Margin bottom" id="control-chart-margin-bottom" type="text" class="form-control" placeholder="Bottom" value="{{options.margin.bottom}}">' +
                    '</div>' +
                    '<div class="col-md-3 col-sm-3">' +
                      '<input aria-label="Margin left" id="control-chart-margin-left" type="text" class="form-control" placeholder="Left" value="{{options.margin.left}}">' +
                    '</div>' +
                  '</div>' +
                '</div>' +

                // Custom height.
                '<div class="form-group">' +
                  '<label for="control-chart-height">Chart height (optional)</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Adjust height of the chart" data-content="If your y-axis labels appear crowded or are overlapping, you can define a height value here to give the data more space."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                  '<input aria-label="Chart height" value="{{chartHeight}}" type="text" id="control-chart-height" class="form-control" placeholder="480"/>' +
                '</div>' +

                // Show title.
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-title">' +
                    '<input aria-label="Show title" type="checkbox" id="control-chart-show-title" value="{{showTitle}}" {{#showTitle}}checked{{/showTitle}}/> Show title' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                      'title="Show Chart Title" data-content="Click this box to display the title above the cart."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +

                // Show controls.
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-controls">' +
                    '<input aria-label="Show controls" type="checkbox" id="control-chart-show-controls" value="{{options.showControls}}" {{#options.showControls}}checked{{/options.showControls}}/> Show controls' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Show Chart Controls" data-content="Selecting this option will allow users to toggle between a Grouped or Stacked display of the chart."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +

                // Show legend.
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-legend">' +
                  '<input aria-label="show legend" type="checkbox" id="control-chart-show-legend" value="{{options.showLegend}}" {{#options.showLegend}}checked{{/options.showLegend}}/> Show legend' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Show Chart Legend" data-content="Selecting this option will allow users to toggle specific data on or off. The legend consists of legend labels presented alongside a colored bullet. ' +
                    'The bullets are selectable. When nothing is selected, all data will display. Double-clicking a bullet, will turn all the others off. A single click will act to toggle the data on or off."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +

                // Show tooltips.
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-show-tooltips">' +
                    '<input aria-label="show tooltips" type="checkbox" id="control-chart-show-tooltips" {{#options.tooltips}}checked{{/options.tooltips}}/> Show Tooltips' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Show Tooltips" data-content="This option will allow users to see specific data information when hovering over the chart."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +

                // Group.
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-group">' +
                    '<input aria-label="group" type="checkbox" id="control-chart-group" value="{{group}}" {{#group}}checked{{/group}}/> Group by X-Field' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Group by X-Field" data-content="If there are two or more rows that have the same value in the column assigned to the x-axis field, those rows will be combined and display as a single data point. This is only relevant for combining numerical data."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +

                // Fewer x-axis labels.
                '<div class="form-group checkbox">' +
                  '<label for="control-chart-reduce-ticks">' +
                    '<input aria-label="fewer labels" type="checkbox" id="control-chart-reduce-ticks" {{#options.reduceXTicks}}checked{{/options.reduceXTicks}}/> Fewer X-axis Labels' +
                  '</label>' +
                  '<a class="help" tabindex="0" role="button" data-toggle="popover" data-trigger="focus"' +
                    'title="Reduce Ticks" data-content="This option will reduce the number of labels along the x-axis."><i class="fa fa-question-circle-o" aria-hidden="true"></i></a>' +
                '</div>' +
              '</div>' +
            '</fieldset>',
  composeTemplate: function () {
    var template = '';
    template += this.templateTop;
    template += this.templateGeneral;
    template += this.templateXFormat;
    template += this.templateYFormat;
    template += this.customOptions ? this.customOptions : '';
    return template;
  },
  initialize: function (options) {
    _.extend(this, options);
  },
  events: {
    'change input[type="checkbox"]': 'update',
    'change select': 'update',
    'blur input[type="text"]': 'update',
    'keydown input[type="text"]': 'update',
    'keydown input[type="number"]': 'update',
    'change input[type="number"]': 'update',
    'submit #control-chart': 'update'
  },
  render: function () {
    var self = this;
    var sortFields = _.arrayToOptions(_.getFields(self.state.get('model')));
    var formatX, formatY;
    sortFields.unshift({name:'default', label:'Default', selected: false});
    self.state.set('sortFields', _.applyOption(sortFields, [self.state.get('sort')]));

    var options = self.state.get('options') || {};
    options.margin = options.margin || {top: 30, right: 20, bottom: 50, left: 60};
    self.state.set('options', options, {silent : true});
    $('#base-controls').html(Mustache.render(self.composeTemplate(), self.state.toJSON()));
    $('#goal-controls').html(Mustache.render(self.templateGoal, self.state.toJSON()));

    self.$('.chosen-select').chosen({width: '95%'});
    if (self.state.get('xFormat') && self.state.get('xFormat').format) {
      formatX = self.state.get('xFormat');
      self.$('#control-chart-x-format option[value="' + formatX.format + '"][data-type="' + formatX.type + '"]')
        .attr('selected', 'selected');
    }
    if (self.state.get('yFormat') && self.state.get('yFormat').format) {
      formatY = self.state.get('yFormat');
      self.$('#control-chart-y-format option[value="' + formatY.format + '"][data-type="' + formatY.type + '"]')
      .attr('selected', 'selected');
    }
    $('#control-chart-color').on('blur', function (e) {
      self.update(e);
    });
    $('#control-chart-color-picker').spectrum({
      change : function (color) {
        $('#control-chart-color').val(function (i, val) {
          var newVal;
          if (val) {
            newVal = val + ', ' + color.toHexString();
          }
          else {
            newVal = color.toHexString();
          }
          return newVal;
        });
        $('input#control-chart-color').trigger('blur');
      }
    });
    if (self.renderQueryEditor) {
      this.queryEditor = new my.QueryEditor({
        el : '.recline-nvd3-query-editor',
        model: this.model.queryState,
        state: this.state
      });
      this.queryEditor.render();
    }
    if (self.renderFilterEditor) {
      this.filterEditor = new my.FilterEditor({
        el : '.recline-nvd3-filter-editor',
        model: this.model,
        state: this.state
      });
    }
    $('[data-toggle="popover"]').popover({
      placement: 'auto right'
    });
  },
  update: function (e) {
    var self = this;
    var newState = {};
    if (e) {
      if (self.$(e.target).closest('.chosen-container').length) return;
      if (e.type === 'keydown' && e.keyCode !== 13) return;
    }

    // If the form is valid then update the state.
    if (self.validate()) {
      // Get old settings.
      var oldSettings = self.state.toJSON();
      // Get new settings.
      var newSettings = self.getUIState();
      // Merge old and new settings.
      newState = _.merge({}, oldSettings, newSettings);
      // The merge function is recursive so all the settings that are saved as
      // arrays are merged as well. Chart colors are saved on arrays and we need
      // the new settings to replace the old ones so the following was added in order
      // to fix those settings after the merge is done.
      newState.options.color = newSettings.options.color;

      // Update state.
      self.state.set(newState);
    }

    // Render form again to display updated information + errors.
    self.render();
  },
  getUIState: function () {
    var self = this;
    var color;
    var rotationVal = parseInt(self.$('#control-chart-label-x-rotation').val());
    var computedState = {
      group: self.$('#control-chart-group').is(':checked'),
      chartHeight: self.$('#control-chart-height').val(),
      xFormat:{
        type: self.$('#control-chart-x-format option:selected').data('type'),
        format: self.$('#control-chart-x-format option:selected').val()
      },
      yFormat:{
        type: self.$('#control-chart-y-format option:selected').data('type'),
        format: self.$('#control-chart-y-format option:selected').val()
      },
      y1Format:{
        type: self.$('#control-chart-y1-format option:selected').data('type'),
        format: self.$('#control-chart-y1-format option:selected').val()
      },
      y2Format:{
        type: self.$('#control-chart-y2-format option:selected').data('type'),
        format: self.$('#control-chart-y2-format option:selected').val()
      },
      sort: self.$('#control-chart-sort').val(),
      showTitle: self.$('#control-chart-show-title').is(':checked'),
      xValues: [self.$('#control-chart-x-values-from').val(), self.$('#control-chart-x-values-to').val()],
      xValuesFrom: self.$('#control-chart-x-values-from').val(),
      xValuesTo: self.$('#control-chart-x-values-to').val(),      
      xValuesStep: parseInt(self.$('#control-chart-x-values-step').val() || 1),
      yValues: [self.$('#control-chart-y-values-from').val(), self.$('#control-chart-y-values-to').val()],
      yValuesFrom: self.$('#control-chart-y-values-from').val(),
      yValuesTo: self.$('#control-chart-y-values-to').val(),
      yValuesStep: parseInt(self.$('#control-chart-y-values-step').val() || 1),
      y1Values: [self.$('#control-chart-y1-values-from').val(), self.$('#control-chart-y1-values-to').val()],
      y1ValuesFrom: self.$('#control-chart-y1-values-from').val(),
      y1ValuesTo: self.$('#control-chart-y1-values-to').val(),
      y1ValuesStep: parseInt(self.$('#control-chart-y1-values-step').val() || 1),
      y2Values: [self.$('#control-chart-y2-values-from').val(), self.$('#control-chart-y2-values-to').val()],
      y2ValuesFrom: self.$('#control-chart-y2-values-from').val(),
      y2ValuesTo: self.$('#control-chart-y2-values-to').val(),
      y2ValuesStep: parseInt(self.$('#control-chart-y2-values-step').val() || 1),
      lpbBarChartField: self.$('#control-lpb-barchart-field').val(),
    };

    computedState.options = computedState.options || {};
    computedState.options.xAxis = computedState.options.xAxis || {};
    computedState.options.yAxis = computedState.options.yAxis || {};
    computedState.options.y1Axis = computedState.options.y1Axis || {};
    computedState.options.y2Axis = computedState.options.y2Axis || {};
    computedState.options.tooltips = self.$('#control-chart-show-tooltips').is(':checked');
    computedState.options.showControls = self.$('#control-chart-show-controls').is(':checked');
    computedState.options.showLegend = self.$('#control-chart-show-legend').is(':checked');
    computedState.options.reduceXTicks = self.$('#control-chart-reduce-ticks').is(':checked');
    computedState.options.xAxis.rotateLabels = (isNaN(rotationVal)) ? 0 : rotationVal;
    color = _.invoke(self.$('#control-chart-color').val().split(','), 'trim');
    computedState.options.xAxis.axisLabel = self.$('#control-chart-x-axis-label').val();
    computedState.options.xAxis.axisLabelDistance = parseInt(self.$('#control-chart-x-axis-label-distance').val()) || 0;
    computedState.options.yAxis.axisLabel = self.$('#control-chart-y-axis-label').val();
    computedState.options.yAxis.axisLabelDistance = parseInt(self.$('#control-chart-y-axis-label-distance').val()) || 0;
    computedState.options.y1Axis.axisLabel = self.$('#control-chart-y1-axis-label').val();
    computedState.options.y1Axis.axisLabelDistance = parseInt(self.$('#control-chart-y1-axis-label-distance').val()) || 0;
    computedState.options.y2Axis.axisLabel = self.$('#control-chart-y2-axis-label').val();
    computedState.options.y2Axis.axisLabelDistance = parseInt(self.$('#control-chart-y2-axis-label-distance').val()) || 0;
    if (self.$('#control-chart-color').val()) {
      computedState.options.color = color;
    } else {
      delete computedState.options.color;
    }
    var margin = {
      top: parseInt(self.$('#control-chart-margin-top').val()),
      right: parseInt(self.$('#control-chart-margin-right').val()),
      bottom: parseInt(self.$('#control-chart-margin-bottom').val()),
      left: parseInt(self.$('#control-chart-margin-left').val()),
    };
    var goal = {
      value: parseFloat(self.$('#control-chart-goal-value').val()) || '',
      color: self.$('#control-chart-goal-color').val(),
      outside: self.$('#control-chart-goal-outside').is(':checked'),
      label: self.$('#control-chart-goal-label').is(':checked'),
    };

    // Replace NaN Vals with 0.
    _.each(_.keys(margin), function (key) {
      margin[key] = (isNaN(margin[key])) ? 0 : margin[key];
    });
    computedState.goal = goal;
    computedState.options.margin = margin;
    return computedState;
  },
  validate: function() {
    var self = this;
    var currentSettings = self.state.toJSON();
    var newSettings = self.getUIState();

    var error = "The number of ticks should be lower than " + ALLOWED_TICKS;
    var isValid = true;

    // Process tick settings in X axis.
    var valid_x = self.processTicks(newSettings.xValuesFrom, newSettings.xValuesTo, newSettings.xValuesStep);
    if (!valid_x) {
      currentSettings.errors.xTicks = error;
      isValid = false;
    }

    // Process tick settings in Y axis.
    var valid_y = self.processTicks(newSettings.yValuesFrom, newSettings.yValuesTo, newSettings.yValuesStep);
    if (!valid_y) {
      currentSettings.errors.yTicks = error;
      isValid = false;
    }

    // Process tick settings in Y1 axis.
    var valid_y1 = self.processTicks(newSettings.y1ValuesFrom, newSettings.y1ValuesTo, newSettings.y1ValuesStep);
    if (!valid_y1) {
      currentSettings.errors.y1Ticks = error;    
      isValid = false;
    }

    // Process tick settings in Y2 axis.
    var valid_y2 = self.processTicks(newSettings.y2ValuesFrom, newSettings.y2ValuesTo, newSettings.y2ValuesStep);
    if (!valid_y2) {
      currentSettings.errors.y2Ticks = error;    
      isValid = false;
    }

    if (isValid) {
      // Clear all errors if any.
      currentSettings.errors = {};        
    }

    // Update state.
    self.state.set(currentSettings);

    return isValid;
  },
  processTicks: function(fromValue, toValue, stepValue) {
    // Check if the number of ticks is valid.
    // Check if both 'from' and 'to' values are set.
    if (fromValue && toValue) {
      // Calculate the number of ticks.
      var ticks = (toValue - fromValue) / stepValue;
      // If the number of ticks is higher than ALLOWED_TICKS value  
      // then return FALSE.
      if (ticks > ALLOWED_TICKS) {
        return false;
      } 
    }

    return true;
  }
});

my.QueryEditor = Backbone.View.extend({
    template: ' \
      <form action="" method="GET" class="form-inline" role="form"> \
        <div class="form-group"> \
          <div class="input-group text-query"> \
            <div class="input-group-btn"> \
              <button type="button" class="btn btn-default">Go &raquo;</button> \
            </div> \
            <input aria-label="Search query" class="form-control search-query" type="text" name="q" value="{{q}}" placeholder="Search data ..."/> \
          </div> \
        </div> \
      </form> \
    ',

    events: {
      'click button': 'onFormSubmit',
      'change input': 'onFormSubmit'
    },

    initialize: function () {
      _.bindAll(this, 'render');
      this.listenTo(this.model, 'change', this.render);
      this.render();
    },
    onFormSubmit: function (e) {
      e.preventDefault();
      var query = this.$el.find('.search-query').val();
      this.model.set({q: query});
    },
    render: function () {
      var tmplData = this.model.toJSON();
      var templated = Mustache.render(this.template, tmplData);
      this.$el.html(templated);
    }
  });

  my.FilterEditor = Backbone.View.extend({
    template: ' \
      <div class="filters"> \
        <div class="form-stacked js-add"> \
          <div class="form-group"> \
            <label>Field</label> \
            <select aria-label="Field to filter" class="fields form-control"> \
              {{#fields}} \
              <option value="{{id}}">{{label}}</option> \
              {{/fields}} \
            </select> \
          </div> \
          <div class="form-group"> \
            <label>Filter type</label> \
            <select aria-label="Field filter type" class="filterType form-control"> \
              <option value="term">Value</option> \
              <option value="range">Range</option> \
              <option value="geo_distance">Geo distance</option> \
            </select> \
          </div> \
          <button id="add-filter-btn" type="button" class="btn btn-default">Add</button> \
        </div> \
        <div class="form-stacked js-edit"> \
          {{#filters}} \
            {{{filterRender}}} \
          {{/filters}} \
          {{#filters.length}} \
          <button type="button" class="btn btn-default">Update</button> \
          {{/filters.length}} \
        </div> \
      </div> \
    ',
    filterTemplates: {
      term: ' \
        <div class="filter-{{type}} filter"> \
          <div class="form-group"> \
            <label> \
              {{field}} <small>{{type}}</small> \
              <a class="js-remove-filter" href="#" title="Remove this filter" data-filter-id="{{id}}">&times;</a> \
            </label> \
            <input aria-label="Field filter value" class="form-control" type="text" value="{{term}}" name="term" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
          </div> \
        </div> \
      ',
      range: ' \
        <div class="filter-{{type}} filter"> \
          <fieldset> \
            <div class="form-group"> \
              <label> \
                {{field}} <small>{{type}}</small> \
                <a class="js-remove-filter" href="#" title="Remove this filter" data-filter-id="{{id}}">&times;</a> \
              </label> \
            </div> \
            <div class="form-group"> \
              <label for="">From</label> \
              <input aria-label="Field filter value from" class="form-control" type="text" value="{{from}}" name="from" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
            <div class="form-group"> \
              <label for="">To</label> \
              <input aria-label="Field filter value to" class="form-control" type="text" value="{{to}}" name="to" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
          </fieldset> \
        </div> \
      ',
      geo_distance: ' \
        <div class="filter-{{type}} filter"> \
          <fieldset> \
            <legend> \
              {{field}} <small>{{type}}</small> \
              <a class="js-remove-filter" href="#" title="Remove this filter" data-filter-id="{{id}}">&times;</a> \
            </legend> \
            <div class="form-group"> \
              <label class="control-label" for="">Longitude</label> \
              <input aria-label="Field filter longitude value" class="input-sm" type="text" value="{{point.lon}}" name="lon" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
            <div class="form-group"> \
              <label class="control-label" for="">Latitude</label> \
              <input aria-label="Field filter latitude value" class="input-sm" type="text" value="{{point.lat}}" name="lat" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
            <div class="form-group"> \
              <label class="control-label" for="">Distance (km)</label> \
              <input aria-label="Field filter distance value" class="input-sm" type="text" value="{{distance}}" name="distance" data-filter-field="{{field}}" data-filter-id="{{id}}" data-filter-type="{{type}}" /> \
            </div> \
          </fieldset> \
        </div> \
      '
    },
    events: {
      'click .js-remove-filter': 'onRemoveFilter',
      'click .js-add-filter': 'onAddFilterShow',
      'click .js-edit button': 'onTermFiltersUpdate',
      'click #add-filter-btn': 'onAddFilter'
    },
    initialize: function (opts) {
      _.bindAll(this, 'render');
      this.listenTo(this.model.fields, 'all', this.render);
      this.listenTo(this.model.queryState, 'change change:filters:new-blank', this.render);
      _.extend(this, opts);
      this.render();
    },
    render: function () {
      var self = this;
      var tmplData = $.extend(true, {}, this.model.queryState.toJSON());
      // We will use idx in list as the id.
      tmplData.filters = _.map(tmplData.filters, function (filter, idx) {
        filter.id = idx;
        return filter;
      });
      tmplData.fields = this.model.fields.toJSON();
      tmplData.filterRender = function () {
        return Mustache.render(self.filterTemplates[this.type], this);
      };
      var out = Mustache.render(this.template, tmplData);
      this.$el.html(out);
    },
    onAddFilterShow: function (e) {
      e.preventDefault();
      var $target = $(e.target);
      $target.hide();
      this.$el.find('.js-add').show();
    },
    onAddFilter: function (e) {
      e.preventDefault();
      var $target = $(e.target).closest('.form-stacked');
      $target.hide();
      var filterType = $target.find('select.filterType').val();
      var field      = $target.find('select.fields').val();
      this.model.queryState.addFilter({type: filterType, field: field});
    },
    onRemoveFilter: function (e) {
      e.preventDefault();
      var $target = $(e.target);
      var filterId = $target.attr('data-filter-id');
      this.model.queryState.removeFilter(filterId);
    },
    onTermFiltersUpdate: function (e) {
     var self = this;
      e.preventDefault();
      var filters = self.model.queryState.get('filters');
      var $form = $(e.target).closest('.form-stacked');
      _.each($form.find('input'), function (input) {
        var $input = $(input);
        var filterType  = $input.attr('data-filter-type');
        var filterIndex = parseInt($input.attr('data-filter-id'), 10);
        var name        = $input.attr('name');
        var value       = $input.val();

        switch (filterType) {
          case 'term':
            filters[filterIndex].term = value;
            break;

          case 'range':
            filters[filterIndex][name] = value;
            break;

          case 'geo_distance':
            if (name === 'distance') {
              filters[filterIndex].distance = parseFloat(value);
            }
            else {
              filters[filterIndex].point[name] = parseFloat(value);
            }
            break;
        }
      });
      self.model.queryState.set({filters: filters, from: 0});
      self.model.queryState.trigger('change');
    }
  });

})(jQuery, recline.View.nvd3);
