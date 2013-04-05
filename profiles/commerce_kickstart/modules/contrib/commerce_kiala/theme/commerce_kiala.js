;(function($) {

  /**
   * Add effects for point selection form
   */
  Drupal.behaviors.commerceKiala = {
    attach: function (context, settings) {
      $('.commerce-kiala-service-details-form .form-radio', context).once('commerce-kiala')
        .click(function() {
          var $t = $(this);
          $t.closest('.form-radios').find('.form-type-radio').removeClass('form-type-radio-checked');
          $t.closest('.form-type-radio').addClass('form-type-radio-checked');
        })
        .filter(':last').closest('.form-type-radio').addClass('form-type-radio-last').end().end()
        .filter(':checked').closest('.form-type-radio').addClass('form-type-radio-checked');
    }
  };

})(jQuery);
