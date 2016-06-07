(function($){
  /**
   * Hide submit button in select widget facet.
   **/
  Drupal.behaviors.ExampleHideSubmit = {
  attach: function(context) {
    $('.facetapi-bonus-select-facet input.ctools-auto-submit-click:not(.facetapi_bonus-hide-submit-processed)', context)
    .addClass('facetapi_bonus-hide-submit-processed').hide();
  }}

})(jQuery);