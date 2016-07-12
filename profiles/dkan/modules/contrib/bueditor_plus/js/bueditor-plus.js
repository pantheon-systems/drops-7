
/**
 * @file
 * Triggers on changes of input formats to render the appropriate
 * editor from bueditor.
 */

(function($){
  var instances = {}, ckBound = false;
  
  function TextareaToBue(id) {
    var parent = $("#" + id).parents('.form-item');
    var bue = $(".bue-ui", parent);
    return bue.length ? bue : null;
  }
  
  
  Drupal.behaviors.BUEditorPlus = {
    attach: function (context, settings) {
      if (BUE && BUE.behavior){
        BUE.behavior(context, settings); // BUEditor < 7.x-1.5
      } else {
        Drupal.behaviors.BUE.attach(context, settings); // BUEditor >= 7.x-1.5
      }
      for (var id in Drupal.settings.BUEPlus.preset){
        if (!instances[id]) {
          // Create a storage for our BEEditorPlus instances. This is for future
          // use only as of right now.
          instances[id] = new Drupal.bueditorPlus(id);
        }
      }
      
      if (!ckBound) {
        ckBound = true;
        if (typeof CKEDITOR !== 'undefined') {
          CKEDITOR.on('instanceDestroyed', function(evt){
            var bue = TextareaToBue(evt.editor.name);
            if (bue) {
              bue.show().css('visibility', 'visible');
            }
          });
          
          CKEDITOR.on('instanceReady', function(evt){
            var bue = TextareaToBue(evt.editor.name);
            if (bue) {
              bue.hide().css('visibility', 'hidden');
            }
          });          
        }
      }
    }
  };

  Drupal.bueditorPlus = function (id) {
    this.id = id;
    this.body = $("#" + id + "-value");
    if (this.body.hasClass('bupProcessed')){
      return;
    }
    this.body.addClass('bupProcessed');
    this.summary = $("#" + id + "-summary");
    this.format =  $("#" + id + "-format .filter-list");
    this.currentEID = 0;
    this.editor = null;
    this.summaryEditor = null;
    this.pset = Drupal.settings.BUEPlus.preset[id];
    var self = this;
    if (this.format.length > 0){
      this.format.bind('change', function(){

        var selectedFormat = $('option:selected', this).val();
        // Only switch if it isn't the same editor instance
        if (self.pset[selectedFormat] != self.currentEID){
          self.currentEID = self.pset[selectedFormat];
          if (self.editor){
            self.editor.UI.remove();
            delete(BUE.instances[self.body.get(0).bue.index]);
            delete(self.body.get(0).bue);
          }
          if (self.summaryEditor){
            self.summaryEditor.UI.remove();
            delete(BUE.instances[self.summary.get(0).bue.index]);
            delete(self.summary.get(0).bue);
          }

          self.editor = null;
          self.summaryEditor = null;

          if (self.pset[selectedFormat]){

            BUE.preset[id] = self.pset[selectedFormat];
            self.body.show().css('visibility', 'visible');
            self.editor = BUE.processTextarea(self.body.get(0), self.pset[selectedFormat]);
            if (self.summary.length){
              self.summaryEditor = BUE.processTextarea(self.summary.get(0), self.pset[selectedFormat]);
            }
          }
        }
      }).change();
    } else {
      // No format selector so we use the default, if selected.
      if (Drupal.settings.BUEPlus.defaults[id]){
        var format = self.pset[Drupal.settings.BUEPlus.defaults[id]];
        BUE.preset[id] = format;
        self.body.show().css('visibility', 'visible');
        self.editor = BUE.processTextarea(self.body.get(0), format);
        if (self.summary.length){
          self.summaryEditor = BUE.processTextarea(self.summary.get(0), format);
        }
      }
    }
  };
})(jQuery);
