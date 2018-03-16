var CSVT = _.clone(recline.Backend.CSV);
(function(my) {
  'use strict';
  my.__type__ = 'csvt';
  // use either jQuery or Underscore Deferred depending on what is available
  var Deferred = (typeof jQuery !== 'undefined' && jQuery.Deferred) || _.Deferred;
  my.fetch = function(dataset) {
    var dfd = new Deferred();
    if (dataset.file) {
      var reader = new FileReader();
      var encoding = dataset.encoding || 'UTF-8';
      reader.onload = function(e) {
        var out = my.extractFields(my.parse(e.target.result, dataset), dataset);
        out.useMemoryStore = true;
        out.metadata = {
          filename: dataset.file.name
        };
        dfd.resolve(out);
      };
      reader.onerror = function (e) {
        dfd.reject({error: {message: 'Failed to load file', code: e.target.error.code}});
      };
      reader.readAsText(dataset.file, encoding);
    } else if (dataset.data) {
      var out = my.extractFields(my.parse(dataset.data, dataset), dataset);
      out.useMemoryStore = true;
      dfd.resolve(out);
    } else if (dataset.url) {
      jQuery.get(dataset.url).done(function(data) {
        var out = my.extractFields(my.parse(data, dataset), dataset);
        out.useMemoryStore = true;
        dfd.resolve(out);
      }).fail(function(req, status){
        dfd.reject({error: {message: status, request: req}});
      });
    }
    return dfd.promise();
  };
  my.parse = function(s, dialect) {
    var out = recline.Backend.CSV.parse(s, dialect);
    if (dialect.transpose && dialect.missingHeader) {
      out = my.transpose(out);
      out[0] = out[0] || [];
      out[0][0] = dialect.missingHeader;
    }
    return out;
  };
  my.transpose = function(transposee) {
    if (!transposee.length) {
      return [];
    }
    if (transposee[0] instanceof Array) {
      var rlen = transposee.length;
      var clen = transposee[0].length;
      var transposed = new Array(clen);
      for (var i = rlen; i > 0; i--) {
        if (transposee[i-1].length !== clen) {
          console.log('Index Error!');
        }
      }
      for (i = 0; i < clen; i++) {
        transposed[i] = [];
        for (var j = 0; j < rlen; j++) {
          transposed[i][j] = transposee[j][i];
        }
      }
      return transposed;
    }
    else return [];
  };
}(CSVT));

this.recline = this.recline || {};
this.recline.Backend = this.recline.Backend || {};
this.recline.Backend.CSVT = CSVT;