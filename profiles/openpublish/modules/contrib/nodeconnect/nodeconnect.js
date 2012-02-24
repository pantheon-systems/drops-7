(function ($) {
  Drupal.behaviors.nodeconnect = {
    'attach': function(context) {
      ref_field_buttons = {};
      $(".nodeconnect-add.single-value", context).each( function() {
        $(this).insertAfter($(this).next().find("label"));
      });
      $(".nodeconnect-edit.single-value", context).each( function() {
        $(this).insertAfter($(this).next().find("label"));
      });
      $(".nodeconnect-edit", context).each( function() {
        edit = $(this).find('input');
        text = $(this).siblings("[type='text']");
        if(text.length == 0 ) {
          text = $(this).siblings().find("[type='text']");
        }
        text
          .bind('change', function(e) {
            if($(this).val() == '') {
              $(edit).attr('disabled', 'disabled');
            }
            else {
              $(edit).attr('disabled', '');
            }
          })
          .trigger('change');
      });
      
    },
  };
})(jQuery);
