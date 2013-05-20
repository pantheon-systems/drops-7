/**
 * @file
 * Custom xliff javascript.
 */
var lingotek;
if(!lingotek) lingotek = {};

lingotek.Xliff = function(selector) {
  var object = jQuery(selector);
  var xliff = unescape(object.text());
  var results = new Array();

  var header = xliff.substring(0, xliff.match("<body>").index) + "<body>";
  var footer = xliff.substring(xliff.match("</body>").index);
  var sourceLanguage = xliff.match(/source-language="([^"]*)"/)[1];

  var found;
  var sourceText = new Array();
  var re = /<source[^>]*>([^<]*)<\/source>/g;
  while(found = re.exec(xliff)) {
    sourceText.push(found[1]);
  }

  var tuIds = new Array();
  re = /<trans-unit[^>]*id="([^"]*)"/g;
  while(found = re.exec(xliff)) {
    tuIds.push(found[1]);
  }

  var count = sourceText.length;
  var completed = 0;

  this.getSource = function(index) {
    return sourceText[index];
  };

  this.getSegmentCount = function() {
    return sourceText.length;
  };

  this.getDocId = function() {
    return object.attr("doc");
  }

  this.getTuId = function(index) {
    return tuIds[index];
  };
};
