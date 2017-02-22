/*jshint -W079 */

// Note that provision of jQuery is optional (it is **only** needed if you use fetch on a remote file)
(function(recline) {
  "use strict";
  
  recline = recline || {};
  recline.Backend = recline.Backend || {};
  recline.Backend.XLS = {};  
  var my = recline.Backend.XLS;

  my.__type__ = "xls";

  // use either jQuery or Underscore Deferred depending on what is available
  var Deferred = (typeof jQuery !== "undefined" && jQuery.Deferred) || _.Deferred;

  my.fetch = function(dataset) {
    var dfd = new Deferred();

    var oReq = new XMLHttpRequest();
    oReq.open("GET", dataset.url, true);
    oReq.responseType = "arraybuffer";
    oReq.onload = function() {
      var out = {};
      var data = new Uint8Array(oReq.response);
      var arr = [];
      for(var i = 0; i !== data.length; ++i) arr[i] = String.fromCharCode(data[i]);
      var bstr = arr.join("");
      var workbook = XLSX.read(bstr, {type: "binary"});
      var sheet = dataset.sheet || _.first(_.keys(workbook.Sheets));
      out.fields = my.extractFields(workbook.Sheets[sheet]);
      out.useMemoryStore = true;
      out.records = my.extractData(workbook.Sheets[sheet], out.fields);
      dfd.resolve(out);
    };
    oReq.send();
    return dfd.promise();
  };

  my.extractFields = function(sheet){
    var headers = [];
    var range = XLSX.utils.decode_range(sheet["!ref"]);
    var C, R = range.s.r;

    for(C = range.s.c; C <= range.e.c; ++C) {
        var cell = sheet[XLSX.utils.encode_cell({c:C, r:R})];
        var hdr = "UNKNOWN " + C;
        if(cell && cell.t) hdr = XLSX.utils.format_cell(cell);
        headers.push(hdr);
    }
    return headers;
  };

  my.extractData = function(sheet, headers) {
    var result = [];
    var range = XLSX.utils.decode_range(sheet["!ref"]);
    var row = {};
    var C, R, value;

    for(R = range.s.r + 1; R <= range.e.r; ++R) {
      row = {};
      for(C = range.s.c; C <= range.e.c; ++C) {
        var cell = sheet[XLSX.utils.encode_cell({c:C, r:R})];
        if(cell && cell.t) value = XLSX.utils.format_cell(cell);
        row[headers[C]] = value;
      }
      result.push(row);
    }
    return result;
  };
}(recline));