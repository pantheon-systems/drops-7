/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.DeepLink = this.recline.DeepLink || {};
this.recline.DeepLink.Deps = this.recline.DeepLink.Deps || {};

;(function($, _, my){
  'use strict';
  my.Map = function(map, router){
    var self = this;
    self.name = 'map';

    self.alterState = function(state){
      if(state.currentView === 'map'){
        state[self.name] = {bounds:map.getBounds()};
      }
      return state;
    };

    self.updateUrl = function(){
      router.onStateChange(null);
    };

    self.update = function(state){
      if(map && state && state.map){
        var newBounds = _.map(_.values(state.map.bounds), _.values);
        setTimeout(map.fitBounds.bind(map,newBounds), 0);
      }
    };
    map.on('dragend', self.updateUrl);
    map.on('zoomend', self.updateUrl);
  };

})(jQuery, _, this.recline.DeepLink.Deps);
