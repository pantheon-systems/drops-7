;(function(){
  'use strict';
  var global = global || window;
  _.mixin({

    /**
     * Check if a value is undefined or null.
     * @param  {[Object]} x value to check.
     * @return {[Boolean]} Returns if x is undefined or null
     * otherwise returns true.
     */
    truthy: function(x){
      return x != null; // jshint ignore:line
    },

    /**
     * Given a collection of objects, it returns consolidate
     * report by a specific key. Something like group by in SQL.
     * @param  {[Array]}    coll collection to proccess
     * @param  {[String]}   key to group by
     * @return {[Array]}    A consolidated version grouped by key.
     */
    reportBy: function(coll, key, fields){
      return _.map(_.groupBy(coll, key), function(records) {
        return _.reduce(records, function(acum, record){
          return _.sumFields(acum, record, fields);
        }, {});
      });
    },

    /**
    * Given an object, it picks all the keys present in the mappings
    * parameter an renames accordingly their values.
    *
    * @param  {[Object]} obj       Object from we want to get the keys.
    *
    * @param  {[Object]} mappings  Key-value object. Each key in this object
    * represent the original key name. On the other hand, each value represent
    * the new name.
    *
    * @param  {[Function]} getter  Getter function to retrive a value from obj
    * param.
    *
    * @return {[Object]}           New brand object with the picked keys.
    */
    pickAs: function(obj, mappings, getter){
      var result = {}, destKey, key;
      var keys = _.keys(mappings);

      for (var i = 0, length = keys.length; i < length; i++) {
        key = keys[i];
        destKey = (key in mappings)? mappings[key] : key;
        result[destKey] = (getter)? getter(obj, key) : obj[key];
      }
      return result;
    },

    /**
     * Given a collection of object, it tranforms each object using the mappings
     * param. This method it's similar to pickAs but for a collection instead an
     * object.
     * @param  {[Array]} coll     Collection to transform
     * @param  {[Object]} mappings Key-value object. Each key in this object
     * represent the original key name. On the other hand, each value represent
     * the new name.
     * @param  {[Function]} Getter function to retrive a value from obj
     * param.
     * @return {[type]}          Returns a transformed collection.
     */
    mapAndTransform: function(coll, mappings, getter){
      return _.map(coll, function(obj){
        return _.pickAs(obj, mappings, getter);
      });
    },

    /**
     * Sum
     * @param  {[type]} acum [description]
     * @param  {[type]} obj  [description]
     * @return {[type]}      [description]
     */
    sumFields: function(acum, obj, fields){
      for(var field in obj){
        if(!_.has(acum, field)) {
          acum[field] = obj[field];
        } else if(_.isNumber(obj[field]) && _.inArray(fields, field)){
          acum[field] += obj[field];
        }
      }
      return acum;
    },

    /**
     * [keyToIndex description]
     * @param  {[type]} coll  [description]
     * @param  {[type]} field [description]
     * @param  {[type]} start [description]
     * @return {[type]}       [description]
     */
    keyToIndex: function(coll, field, start){
      start = (!_.truthy(start)) ? start : 0;
      return _.map(coll, function(obj, count){
        obj[field] = count + start;
        return obj;
      });
    },

    /**
     * [deepMerge description]
     * @param  {[type]} obj1 [description]
     * @param  {[type]} obj2 [description]
     * @return {[type]}      [description]
     */
    deepMerge: function(obj1, obj2) {
      for (var p in obj2) {
        try {
          if(obj2[p].constructor === Object) {
            obj1[p] = _.deepMerge(obj1[p], obj2[p]);
          } else {
            obj1[p] = obj2[p];
          }
        } catch(e) {
          obj1[p] = obj2[p];
        }
      }
      return obj1;
    },

    /**
     * [always description]
     * @param  {[type]} coll  [description]
     * @return {[type]}       [description]
     */
    always: function(k){
      return function(){
        return k;
      };
    },

    /**
     * [inv description]
     * @param  {[type]} coll  [description]
     * @return {[type]}       [description]
     */
    inv: function(method){
      var args = _.rest(_.toArray(arguments));
      return function(ctrl){
        return _.isFunction(ctrl[method]) && ctrl[method].apply(ctrl, args);
      };
    },
    /**
     * [cloneJSON description]
     * @param  {[type]} coll  [description]
     * @return {[type]}       [description]
     */
    cloneJSON: function(obj){
      return JSON.parse(JSON.stringify(obj));
    },
    /**
     * [inArray description]
     * @param  {[type]} coll  [description]
     * @return {[type]}       [description]
     */
    inArray: function(array, item){
      return (_.indexOf(array, item) === -1)? false : true;
    },
    validator: function(predicate){
      return function(item){
        if(_.isFunction(predicate)) {
          return predicate(item);
        }
        return false;
      };
    },
    validateField: function(coll, field, validator){
      return _.every(coll, function(item){
        return validator(item[field]);
      });
    },
    negate: function (predicate) {
      return function() {
        return !predicate.apply(this, arguments);
      };
    },
    cast: function(value){
      var type = _.inferType(value);
      if(type === 'undefined') return undefined;
      if(type === 'null') return null;
      if(type === 'NaN') return NaN;
      if(type === 'Array') return value;
      if(type === 'Date'){
        return Date.parse(value);
      }

      return global[type](value);
    },
    inferType:function(value){
      if(_.isUndefined(value)){
        return 'undefined';
      } else if(_.isNull(value)){
        return 'null';
      } else if(_.isDate(value)){
        return 'Date';
      } else if (_.isArray(value)){
        return 'Array';
      } else if(!isNaN(value) && value !== ''){
        return 'Number';
      } else if(value !== '' && !isNaN(Date.parse(value))) {
        return 'Date';
      } else if (_.isObject(value)){
        return 'Object';
      } else if (_.isString(value)){
        return 'String';
      } else if (_.isBoolean(value)){
        return 'Boolean';
      } else if (_.isRegExp(value)){
        return 'RegExp';
      } else if (_.isNaN(value)){
        return 'NaN';
      }
    },
    iteratee: function(key){
      return function(obj){
        return obj[key];
      };
    },
    instantiate: function(Constructor){
      return function(options){
        return new Constructor(options);
      };
    }
  });
})();