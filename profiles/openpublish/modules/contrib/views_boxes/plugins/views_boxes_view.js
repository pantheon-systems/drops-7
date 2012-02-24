
(function ($) {
  Drupal.behaviors.view_boxes = { 
    attach: function(context) {
      //All of this code is for the custom id field
     
      // Bind an update event to all of our id-value fields
      // this takes the values from all of the corresponding
      // id-fields parse and update our value
      $(".id-value").bind("update", function(e) {
         values = [];
        groupid = $(this).attr('group');
        $("#" + groupid).find(".id-field").each(function(i) {
          value = $(this).val();
          re = /.*id:(.*)\]/;
          if(value.match(re)) {
            id = value.replace(re, "$1");
            values.push(id);
          }
        });
        $(this).val(values.join("+"));
      });
      // make all wrappers sortable
      // and set up an update whenever a field is left
      $(".id-group", context).each(function () {
        $(this, context).find(".id-field").focusout(function (e){
          valueid = $(this).attr("key").replace(/_/g,"-");
          $("#edit-" + valueid).trigger("update");
        });
        $(this, context).find("input.form-submit").click(function (e){
          valueid = $(this).attr("key").replace(/_/g,"-");
          $("#edit-" + valueid).trigger("update");
        });
        $(this, context).find(".id-sortable", context).sortable({
          stop: function(event, ui) {
          valueid = $(this).attr("key").replace(/_/g,"-");
          $("#edit-" + valueid).trigger("update");
          }
        });
      });
      
      // Redefining this here as the popup wasn't hiding properly following a click
      if (Drupal.jsAC != null) {
        Drupal.jsAC.prototype.select = function (node) {
          this.input.value = $(node).data('autocompleteValue');
          $(this.popup).css({ visibility: 'hidden' });
          valueid = $(this.input).attr("key").replace(/_/g,"-");
          $("#edit-" + valueid).trigger("update");
        };      	
      }
	}
  };
})(jQuery);
