/**
 * @file
 * Custom javascript.
 */
var lingotek;
if(!lingotek) lingotek = {};
if(!lingotek.mt) lingotek.mt = {};
if(!lingotek.queue) lingotek.queue = new Array();

lingotek.mt.init = function() {
  var textareas = jQuery("textarea[tag='lingotek-mt-xliff']");

  if(textareas.length > 0) {
    lingotek.mt.processes = new lingotek.ProcessManager();
    textareas.each(function(i, input) {
      var xliff = jQuery(input);
      var nid = xliff.attr('nid');

      lingotek.mt.processes.addProcess(nid);
      var translate = new lingotek.Translate(
        nid,
        xliff.attr('engine'),
        xliff.attr('source'),
        xliff.attr('targets').split(',')
      );
    });
    lingotek.mt.dequeue();
  }
};

lingotek.mt.dequeue = function() {
  if(lingotek.queue.length > 0) {
    var obj = lingotek.queue.pop();
    jQuery.post("?q=lingotek/segment", obj.params, obj.callback);
  }
  else {
    setTimeout(lingotek.mt.dequeue, 1000);
  }
};

lingotek.mt.finalizeTranslation = function(nid, targetLanguages) {
  jQuery.post("?q=lingotek/update/" + nid, {'targets[]' : targetLanguages}, function(json) {
    lingotek.mt.processes.removeProcess(json.nid);
  });
};

jQuery(document).ready(lingotek.mt.init);
