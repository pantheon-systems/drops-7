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
  });
})(jQuery);
