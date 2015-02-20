/*jshint multistr:true*/

this.recline = this.recline || {};
this.recline.DeepLink = this.recline.DeepLink || {};

;(function($, my) {
  'use strict';

  /**
   * Router object
   * @param {recline.view.MultiView} multiview
   */
  my.Router = function(multiview){
    var self = this;
    var parser = new my.Parser();

    var deep = DeepDiff.noConflict();

    // TODO: pass firstState as parameter.
    var firstState = _.omit(
      JSON.parse(JSON.stringify(multiview.state)), 'dataset');
    var currentState = {};
    var dependencies = {};
    var router;

    _.extend(self, Backbone.Events);

    function inv(method){
      var args = _.rest(_.toArray(arguments));
      return function(ctrl){
        return _.isFunction(ctrl[method]) && ctrl[method].apply(ctrl, args);
      };
    }

    /**
     * Update the multiview state and render the new state.
     * @param  {String} state
     */
    self.updateState = function(serializedState){
      var multiviewState = self.transform(serializedState, self.toState); 
      _.each(dependencies, inv('update', multiviewState));
      if (multiviewState && !_.isEmpty(multiviewState)) {
        multiviewState = _.extend(multiview.state.attributes, multiviewState);
        multiview.model.queryState.set(multiviewState.query);
        multiview.updateNav(multiviewState.currentView || 'grid');
        _.each(multiview.pageViews, function(view, index){
          var viewKey ='view-' + view.id;
          var pageView = multiview.pageViews[index];
          pageView.view.state.set(multiviewState[viewKey]);
          if(_.isFunction(pageView.view.redraw) && pageView.id === 'graph'){
            setTimeout(pageView.view.redraw, 0);
          } else if(pageView.id === 'grid') {
            pageView.view.render();
          }
        });
      } else {
        multiview.updateNav('grid');
      }
      self.trigger('init', {serializedState:serializedState});
    };

    /**
     * Adds a dependency to this router. Something to track and
     * to execute when tracked thing changes
     * @param  {Function} ctrl Constructor with the implementation
     * of this observer
     * @return {undefined}
     */
    self.addDependency = function(ctrl){
      dependencies[ctrl.name] = ctrl;
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
    self.onStateChange = function(){
      var ch = deep.diff(firstState,
        _.omit(multiview.state.attributes, 'dataset'));
      var changes = {};
      var newState;
      var serializedState;

      _.each(ch, function(c){
        if(c.kind === 'E'){
          self.createNestedObject(changes, c.path, c.rhs);
        } else if(c.kind === 'A') {
          self.createNestedObject(changes, c.path, c);
        }
      });
      newState = new recline.Model.ObjectState();
      newState.attributes = changes;
      newState.attributes = self.alterState(newState.attributes);
      currentState = newState;
      serializedState = self.transform(newState, self.toParams);
      router.navigate(serializedState);
      self.updateControls();
      self.trigger('stateChange',
        {serializedState:serializedState, state:currentState});
    };

    /**
     * Creates a composed function based on alterState function from each
     * dependency and run it through the pipeline passing as parameter
     * the state and returning the altered state object.
     * @param  {Object} state
     */
    self.alterState = function(state){
      if(_.isEmpty(dependencies)) return state;
      var alter = _.compose.apply(null,
        _.without(_.map(dependencies, function(ctrl){
        return ctrl.alterState;
      }), undefined));
      return alter(state);
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
        if(value.item.kind === 'N'){
          base = base[lastName][value.index] = value.item.rhs;
        }
        if(value.item.kind === 'D'){
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
    self.start = function(){
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

  };

  /**
   * Url parser
   */
  my.Parser = function(){
    var self = this;

    /**
     * Reduces the size of the url removing unnecesary characters.
     * @param  {String} str
     * @return {String}
     */
    self.compress = function(str){

      // Replace words
      // Remove start and end brackets
      // Replace true by 1 and false by 0
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

      // % presence could lead to malformed url.
      str = str.replace('%', '@@');

      // Stripping quotes from keys
      str = str.replace(/"([a-zA-Z-_.]+)"\s?:/g ,  '$1:');

      // Replacing spaces between quotes with underscores
      str = str.replace(/\x20(?![^"]*("[^"]*"[^"]*)*$)/g, '++');
      return str.replace(/"([a-zA-Z0-9-#_.-|+]+)?"/g ,  '!$1');
    };

    /**
     * Converts all escaped strings to javascript strings.
     * @param  {String} str
     * @return {String}
     */
    self.parseStrings = function(str){

      // Converting all the @@ to %.
      str = str.replace('@@', '%');

      // Adding quotes to keys
      str = str.replace(/([{,])([a-zA-Z-_.\+]+)\s?:/g ,  '$1\"$2\":');
      // Replacing underscores with spaces for any word that start with !
      // TODO: make space replacement configurable
      str = str.replace(/![a-zA-Z0-9_. -\+]+/g, function(x) {
        return x.replace(/\+\+/g, ' ');
      });
      return str.replace(
        new RegExp('!([a-zA-Z0-9-_# .-:%]+)?', 'g'),  '\"$1\"');
    };
  };

})(jQuery, this.recline.DeepLink);
