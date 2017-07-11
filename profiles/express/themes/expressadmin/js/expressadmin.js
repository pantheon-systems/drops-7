(function ($) {
  $(document).ready(function(){
    // Get field column config
    var expressFieldColumnsMinimum = Drupal.settings.express_field_columns;
    // Apply to all multi checkbox/radio fields
     $('.node-form .form-checkboxes, .node-form .form-radios, #bean-form .form-checkboxes, #bean-form .form-radios').each(function() {
        var fieldCount = $('.form-item', this).length;
        // If field contains more than the config amount of options,
        // apply column class
        if (fieldCount > expressFieldColumnsMinimum) {
          $(this).addClass('express-field-columns');
        }
     });

     // Select links
     $('.express-select-links').each(function(){
       var target = $('ul', this).attr('id');
       $('button', this).attr('aria-controls', target);
     });
     $('.express-select-links > ul').hide().attr('tabindex', '-1');
     $('.express-select-links > button').attr('aria-expanded', 'false');
     $('.express-select-links > button').click(function(event){
       event.preventDefault();
       if ($(this).attr('aria-expanded') == 'true') {
        $(this).attr('aria-expanded', 'false');
        $(this).next().slideUp().attr('aria-expanded', 'false');
      } else {
          $(this).attr('aria-expanded', 'true');
          $(this).next().slideDown().attr('aria-expanded', 'true');
      }
     });
  });
})(jQuery);
