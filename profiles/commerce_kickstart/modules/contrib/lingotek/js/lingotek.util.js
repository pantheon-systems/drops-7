/**
 * @file
 * Custom util javascript.
 */
var lingotek;
if(!lingotek) lingotek = {};
if(!lingotek.util) lingotek.util = {};

lingotek.util.escapeForXml = function(text) {
  text = text.replace('&', '&amp;');
  text = text.replace('"', '&quot;');
  text = text.replace("'", '&apos;');
  text = text.replace('>', '&gt;');
  text = text.replace('<', '&lt;');

  return text;
}

/*
 * Returns the language code used by the Machine Translation engine when provided the format Lingotek uses.
 */
lingotek.util.parseLanguage = function(lang) {
  var l;
  if(lang.match("_")) {
    l = lang.toLowerCase().split("_");
  } else {
    l = lang.toLowerCase().split("-");
  }

  if(l[0] == "zh") {
    if(l[1] == "tw" || l[1] == "hk") {
      l[0] += "-TW";
    } else {
      l[0] += "-CN";
    }
  } else if(l[0] == "pt") {
    l[0] += "-PT";
  } else if(l[0] == "he") {
    l[0] = "iw";
  }

  return l[0];
}
