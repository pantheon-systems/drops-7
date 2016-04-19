;(function(){
  'use strict';
  function demoCSVData() {
    var dataset = 'Test, Teo, Fede, Mariano' +
    '\nlong jump, 5.6, 4.3, 5.1' +
    '\nhigh jump, 1.2, 1.5, 1.8' +
    '\ntriple jump, 12, 14, 10' +
    '\npole vault, 5, 4, 2';
    return dataset;
  }
  function csvToTable(data) {
    var rows = data.split('\n');
    var table = [];
    var t = '<{{tag}}>{{{value}}}</{{tag}}>';
    $.each(rows, function(i, v) {
      var row = [];
      $.each(v.split(','), function(j, w){
        var tag = 'td';
        if (i === 0) {
          tag = 'th';
        }
        row.push(Mustache.render(t, {tag: tag, value: w}));
      });
      table.push(Mustache.render(t, {tag: 'tr', value: row.join('')}));
    });
    table = table.join('');
    return table;
  }
  $(document).on('ready', function(){
    var data = demoCSVData();
    $('#transposee').append(csvToTable(data));
    recline.Backend.CSVT.fetch({
        data: data,
        transpose: true,
        missingHeader: 'Name',
      }
    ).done(function(dataset) {
      var t = '';
      t += dataset.fields.join(',');
      $.each(dataset.records, function (i, v) {
        t += '\n' + v.join(',');
      });
      $('#transposed').append(csvToTable(t));
    });
  });
})(window);
