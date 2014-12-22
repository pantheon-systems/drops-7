/*jshint multistr:true */

this.recline = this.recline || {};
this.recline.DeepLink = this.recline.DeepLink || {};

(function($, my) {
  'use strict';

  /**
   * Router object
   * @param {recline.view.MultiView} multiview
   */
  my.Router = function(multiview){
    var self = this;
    var currentView = null;
    var router;
    var changes = {};
    var parser = new my.Parser();
    var deep = DeepDiff.noConflict();
    var firstState = _.clone(_.omit(multiview.state.attributes, 'dataset'));

    /**
     * Update the multiview state and render the new state.
     * @param  {String} state
     */
    self.updateState = function(serializedState){
      var multiviewState = self.transform(serializedState, self.toState);
      changes = multiviewState || {};
      if (multiviewState) {
        multiviewState = _.extend(multiview.state.attributes, multiviewState);
        multiview.model.queryState.set(multiviewState.query);
        multiview.updateNav(multiviewState.currentView);

        _.each(multiview.pageViews, function(view, index){
          var viewKey ='view-' + view.id;
          var pageView = multiview.pageViews[index];
          pageView.view.state.set(multiviewState[viewKey]);
          if(typeof pageView.view.redraw === 'function' && pageView.id === 'graph'){
            setTimeout(pageView.view.redraw, 0);
          } else if(pageView.id === 'grid') {
            pageView.view.render();
          }
        });
      }
    };

    /**
     * Applies a transform function to the input and return de result.
     * @param  {String} input
     * @param  {Function} transformFunction
     * @return {String}
     */
    self.transform = function(input, transformFunction){
      var result;
      try{
        result = transformFunction(input);
      } catch(e){
        console.log(e);
        result = null;
      }
      return result;
    };

    /**
     * Converts a serialized state string to an object.
     * @param  {String} serializedState
     * @return {Object}
     */
    self.toState = function(serializedState){
      var stringObject = parser.inflate(decodeURI(serializedState));
      return JSON.parse(stringObject);
    };

    /**
     * Converts an object state to a string.
     * @param  {Object} state
     * @return {String}
     */
    self.toParams = function(state){
      var stringObject = JSON.stringify(_.omit(state.attributes, 'dataset'));
      return parser.compress(stringObject);
    };

    /**
     * Listen for changes in the multiview state. It computes the differences
     * between the initial state and the current state. Creates a patch object
     * from this difference. Converts this new object to params and finally
     * navigates to that state.
     * @param  {Event} event
     */
    self.onStateChange = function(event){
      var ch = deep.diff(firstState, _.omit(multiview.state.attributes, 'dataset'));
      var tempChanges = {};
      _.each(ch, function(c){
        if(c.kind === 'E'){
          self.createNestedObject(tempChanges, c.path, c.rhs);
        } else if(c.kind === 'A') {
          self.createNestedObject(tempChanges, c.path, c);
        }
      });
      changes = _.extend(changes, tempChanges);
      var newState = new recline.Model.ObjectState();
      newState.attributes = changes;
      router.navigate(self.transform(newState, self.toParams));
      self.updateControls();
    };

    /**
     * Creates a nested object following the props path.
     * @param  {Object} base
     * @param  {Array} prop
     * @param  {*} value
     * @return {Object}
     */
    self.createNestedObject = function( base, props, value ) {
        var names = _.clone(props);
        var lastName = arguments.length === 3 ? names.pop() : false;

        for( var i = 0; i < names.length; i++) {
            base = base[names[i]] = base[names[i]] || {};
        }

        if(lastName && !_.isArray(value) && !_.isObject(value)){
          base = base[lastName] = value;
        }

        if(_.isObject(value) && value.kind === 'A'){
          if(_.isUndefined(base[lastName])){
            base[lastName] = [];
          }
          if(value.item.kind == 'N'){
            base = base[lastName][value.index] = value.item.rhs;
          }
          if(value.item.kind == 'D'){
            base[lastName].splice(value.index, value.item.rhs);
            base = base[lastName];
          }
        }
        return base;
    };

    /**
     * Updates controls based on the new state.
     */
    self.updateControls = function(){
      var id = multiview.state.get('currentView');
      multiview.pager.render();
      if(id === 'graph' || id === 'map') {
        var index = self.getCurrentViewIndex();
        var menuMap = {graph:'editor', map:'menu'};
        var menuName = menuMap[id];
        var menu = multiview.pageViews[index].view[menuName];
        var viewState = self.getCurrentView().view.state;
        menu.state.set(viewState.attributes);
        menu.render();
      }
    };

    /**
     * Gets the current displayed view of the multiview.
     * @return {Object}
     */
    self.getCurrentView = function(){
      var id = multiview.state.get('currentView');
      return _.findWhere(multiview.pageViews, {id:id});
    };

    /**
     * Gets the index of current displayed view.
     * @return {[Integer]}
     */
    self.getCurrentViewIndex = function(){
      var id = multiview.state.get('currentView');
      var index;
      _.each(multiview.pageViews, function(item, i){
        if(item.id === id){
          index = i;
        }
      });
      return index;
    };

    /**
     * Initializes the router object.
     */
    self.initialize = function(){
      var Router = Backbone.Router.extend({
        routes: {
          '*state': 'defaultRoute',
        },
        defaultRoute: function(state) {
          self.updateState(state);
        }
      });
      router = new Router();
      multiview.listenTo(multiview.state, 'all', self.onStateChange);
      multiview.model.bind('all', self.onStateChange);
      Backbone.history.start();
    };

    // Entry point.
    self.initialize();
  };

  /**
   * Url parser
   */
  my.Parser = function(){
    var self = this;

    /**
     * TODO
     * Use this compress map to reduce even more the url size.
     */
    var compressMap = {
      'backend':'b',
      'currentView': 'c',
      'dataset':'d',
      'fields': 'f',
      'records': 'r',
      'query': 'qy',
      'facets': 'fc',
      'filters':'fl',
      'from': 'fr',
      'q':'q',
      'size':'sz',
      'readOnly':'ro',
      'url':'ul',
      'view-graph': 'vga',
      'graphType': 'gt',
      'group': 'gp',
      'series': 'sr',
      'view-grid':'vgi',
      'columnsEditor': 'ce',
      'columnsOrder': 'co',
      'columnsSort': 'cs',
      'columnsWith':'cw',
      'fitColumns': 'fcm',
      'gridOptions': 'go',
      'hiddenColumns': 'hc',
      'options':'op',
      'view-map':'vm',
      'autoZoom': 'az',
      'cluster': 'cl',
      'geomField': 'gf',
      'latField': 'laf',
      'lonField': 'lof',
    };

    /**
     * Reduces the size of the url removing unnecesary characters.
     * @param  {String} str
     * @return {String}
     */
    self.compress = function(str){
      //replace words
      //remove start and end brackets
      //replace true by 1 and false by 0
      return self.escapeStrings(str);
    };

    /**
     * Inflates a compressed url string.
     * @param  {String} str
     * @return {String}
     */
    self.inflate = function(str){
      return self.parseStrings(str);
    };

    /**
     * Escape all the string prepending a ! character to each one.
     * @param  {String} str
     * @return {String}
     */
    self.escapeStrings = function(str){
      //stripping quotes from keys
      str = str.replace(/"([a-zA-Z-_.]+)"\s?:/g ,  "$1:");
      //replacing spaces between quotes with underscores
      str = str.replace(/\x20(?![^"]*("[^"]*"[^"]*)*$)/g, "_");
      return str.replace(/"([a-zA-Z-#_.-]+)?"/g ,  "!$1");
    };

    /**
     * Converts all escaped strings to javascript strings.
     * @param  {String} str
     * @return {String}
     */
    self.parseStrings = function(str){
      //adding quotes to keys
      str = str.replace(/([a-zA-Z-_.]+)\s?:/g ,  "\"$1\":");
      //replacing underscores with spaces for any word that start with !
      str = str.replace(/![a-zA-Z0-9_. -]+/g, function(x) { return x.replace(/_/g, ' '); });
      return str.replace(new RegExp('!([a-zA-Z-# .-]+)?', 'g'),  "\"$1\"");
    };
  };

})(jQuery, this.recline.DeepLink);
