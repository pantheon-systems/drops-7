/**
 * @file
 * Custom javascript.
 */
var lingotek;
if(!lingotek) lingotek = {};
if(!lingotek.translate) lingotek.translate = {};
if(!lingotek.translations) lingotek.translations = {};

lingotek.Translate = function(nid, engine, sourceLanguage, targetLanguages) {
  var count = 0;
  var retry = new Array();
  var xliff = new lingotek.Xliff("[tag='lingotek-mt-xliff'][nid='" + nid + "']");
  lingotek.mt.processes.setSegmentCount(nid, xliff.getSegmentCount());

  var targetLanguagesParam = "";
  for(var i = 0; i < targetLanguages.length; i++) {
    targetLanguagesParam += "&langpair=" + sourceLanguage + "%7C" + lingotek.util.parseLanguage(targetLanguages[i]);
  }

  this.googleTranslate = function(index) {
    var encodedText = encodeURIComponent(xliff.getSource(index));
    retry[index] = 5;
    lingotek.translations[nid] = this;

    lingotek.translate["googleResult_" + nid + "_" + index] = function(json) {
      eval("var val = " + index + ";");
      eval("var lnid = " + nid + ";");
      lingotek.translations[lnid].googleResult(json, val);
    }

    var url = 'http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=' + encodedText + targetLanguagesParam + "&callback=lingotek.translate.googleResult_" + nid + "_" + index + "&prettyprint=true";
    if(url.length < 2000) {
      jQuery.getScript(url);
    }
    else { //The call won't work, so let's skip it.
      count = count + (count * targetLanguages.length);
    }
  };

  this.googleResult = function(json, index) {
    //Let's make sure before calling the callback function that it returned correctly
    if(json.responseStatus == 200 || retry[index] == 0) {
      var translations = json.responseData;
      if(json.responseData && json.responseData != null) {
        if(json.responseData.translatedText) { //only one language
          if(json.responseStatus == 200) {
            text = json.responseData.translatedText;
            this.addTarget(index, text, targetLanguages[0]);
          }
          else {
            this.saved();
          }
        }
        for(var i = 0; i < translations.length; i++) {
          var translation = translations[i];
          var text = "";
          if(translation.responseStatus == 200) {
            text = translation.responseData.translatedText;
            this.addTarget(index, text, targetLanguages[i]);
          }
          else {
            this.saved();
          }
        }
      }
      else {
        for(var i = 0; i < targetLanguages.length; i++) {
          this.saved();
        }
      }
    }
    else { //Otherwise, let's try this again up to the default retry count:
      retry[index]--;
      setTimeout(function() {
        jQuery.getScript('http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&q=' + encodeURIComponent(xliff.getSource(index)) + targetLanguagesParam + "&callback=lingotek.translate.googleResult_" + nid + "_" + index + "&prettyprint=true");
      }, 1000);
    }
  };

  this.microsoftTranslate = function(index) {
    for(var i = 0; i < targetLanguages.length; i++) {
      var function_counter = (index * targetLanguages.length) + i;
      this.microsoftExecute(index, lingotek.util.parseLanguage(targetLanguages[i]), function_counter);
    }
  };

  this.microsoftExecute = function(index, targetLanguage, function_counter) {
    lingotek.translations[nid] = this;

    //Prepare the callback:
    lingotek.translate["microsoftResult_" + nid + "_" + function_counter] = function(targetText) {
      eval("var val = " + function_counter + ";");
      eval("var lnid = " + nid + ";");
      lingotek.translations[lnid].microsoftResult(targetText, val);
    }

    try {
      var mtLang = targetLanguage.substring(0,2);
      if(mtLang == "zh") {
        if(targetLanguage == "zh-hans") {
          mtLang = "zh-chs";
        }
        else {
          mtLang = "zh-cht";
        }
      }
      else if (mtLang == "nb" || mtLang == "nn") {
        mtLang = "no";
      }
      else if (mtLang == "iw") {
        mtLang = "he";
      }

      if(jQuery.inArray(mtLang, Microsoft.Translator.GetLanguages()) == -1) {
        throw "Target Language not supported: " + mtLang;
      }
      Microsoft.Translator.translate(xliff.getSource(index), sourceLanguage, mtLang, lingotek.translate["microsoftResult_" + nid + "_" + function_counter]);
    }
    catch(err) {
      this.saved();
    }
  };

  this.microsoftResult = function(targetText, counter) {
    var currentSegmentCount = Math.floor(counter / targetLanguages.length);
    var currentLanguage = counter % targetLanguages.length;

    this.addTarget(currentSegmentCount, lingotek.util.escapeForXml(targetText), targetLanguages[currentLanguage])
  };

  this.saved = function() {
    count++;
    lingotek.mt.processes.updateProcess(nid, Math.floor(count / targetLanguages.length), targetLanguages);
  };

  this.translate = function(index) {
    this[engine + 'Translate'](index);
  };

  this.addTarget = function(index, text, targetLanguage) {
    var params = {
      "targetText" : text,
      "targetLanguage" : targetLanguage,
      "sourceText" : xliff.getSource(index),
      "document" : xliff.getDocId()
    };
    lingotek.queue.push({"params" : params, "callback" : this.saved});
  }

  for(var i = 0; i < xliff.getSegmentCount(); i++) {
    this.translate(i);
  }
};
