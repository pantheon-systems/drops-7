/**
 * @file
 * Provides options for chart visualization.
 */

(function ($) {
  var sharedObject;

  Drupal.behaviors.VisualizationEntityCharts = {
    attach: function (context) {
      var currentState = $('#edit-field-ve-settings-und-0-value').val();
      var state;
      var model;

      // There is not saved state. Neither database or memory.
      if(currentState && !sharedObject){

        state = new recline.Model.ObjectState(JSON.parse(currentState));
        model = state.get('model');
        if(model && !model.records){
          // Ensure url is protocol agnostic
          model = state.get('model');
          model.url = cleanURL(model.url);
          model = new recline.Model.Dataset(model);

          // Hack: check if the file exists before fetch.
          // CSV.JS does not return an ajax promise then
          // we can't know if the request fails.
          $.get(state.get('model').url)
          .done(function(){
            model.fetch().done(init);
            state.set('model', model);
            state.get('model').queryState.attributes = state.get('queryState');
            sharedObject = {state: state};
          })
          .fail(function(){
            sharedObject = {state: state};
            sharedObject.state.set({step:0});
            init();
          });
        }
        else if(model && model.records) {
          jQuery(function(){
            state.set('model', new recline.Model.Dataset(model));
            state.get('model').queryState.attributes = state.get('queryState');
            sharedObject = {state: state};
            init();
          });
        }
      }
      else if(!sharedObject) {
        state = new recline.Model.ObjectState();
        state.set('queryState', new recline.Model.Query());
        sharedObject = {state: state};
        init();
      }

      if(state) {
        setActiveStep(state.get('step'));
      } else {
        setActiveStep(0);
      }

      function cleanURL(url){
        var haveProtocol = new RegExp('^(?:[a-z]+:)?//', 'i');
        if(haveProtocol.test(url)){
          url = url.replace(haveProtocol, '//');
        }
        return url;
      }

      function setActiveStep(n){
        var $stages = $('#ve-chart-form .stages li');
        $stages.removeClass('active');
        $stages
          .eq(n)
          .addClass('active');
      }

      function init(){
        var msv = new MultiStageView({
          state: state,
          el: $('#steps')
        });

        msv.addStep(new LoadDataView(sharedObject));
        msv.addStep(new DataOptionsView(sharedObject));
        msv.addStep(new ChooseChartView(sharedObject));
        msv.addStep(new ChartOptionsView(sharedObject));

        msv.on('multistep:change', function(e){
          setActiveStep(e.step);
        });
        msv.render();

        // Oh my god!!
        $(document).ajaxComplete(function(e, xhr, settings) {
          if(settings.url && settings.url.search('/file/ajax/field_file') !== -1){
            var url = $('.file-widget a').prop('href');
            var source = {backend:'csv', url: url};
            sharedObject.state.set('source', source);
            msv.gotoStep(0);
            msv.render();
          }
        });
        var $resourceField = $('#edit-field-uuid-resource-und-0-target-uuid');

        $resourceField.on('autocompleteSelect', function(event, node) {
          var re = /\[(.*?)\]/;
          var $sourceField = $('#control-chart-source');
          var uuid = re.exec($resourceField.val())[1];
          $sourceField.val('/node/' + uuid + '/download');
        });
        sharedObject.state.on('change', function(){
          $('#edit-field-ve-settings-und-0-value').val(JSON.stringify(sharedObject.state.toJSON()));
        });
        window.msv = msv;
        window.sharedObject = sharedObject;
      }
    }
  };
})(jQuery);
