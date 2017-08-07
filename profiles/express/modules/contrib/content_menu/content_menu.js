(function($) {
  Drupal.behaviors.content_menu = {
    attach:function(context, settings) {
      // Show menu item title input field when clicking on the trigger.
      $('.menu-overview-title-edit-trigger', context).click(function(e) {
        var textfield = $(this).siblings('.form-type-textfield');
        textfield.show();
        textfield.addClass('visible');
        textfield.children('input').focus();
        $(this).siblings('.menu-overview-title-link').hide();
        $(this).hide();
        e.stopPropagation();
        e.stopImmediatePropagation();
      });
      // Hide when clicking outside the item
      $('html', context).not('.form-type-textfield.visible').click(function(e) {
        var editTrigger = $(this).find('.menu-overview-title-edit-trigger');
        var textfield = $(editTrigger).siblings('.form-type-textfield');
        textfield.hide();
        textfield.addClass('visible');
        $(editTrigger).siblings('.menu-overview-title-link').show();
        $(editTrigger).removeAttr('style');
      });
    }
  };
})(jQuery);
