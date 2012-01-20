/**
 *  Make the search block submit on enter
 */
(function ($) {
  Drupal.behaviors.frame_search_submit = {
    attach : function (context, settings){
      var input = $('.search-form #edit-keys', context);
      if (input.size()) {
        input.keyup(function(e){
          if (e.which == 13) {
            this.form.submit();
          }
        });
      } 
    }
  };
})(jQuery);
