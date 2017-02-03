/**
 * @file
 * Provides behaviour for visualization views.
 */

(function ($) {
  Drupal.behaviors.VisualizationEntity = {
    attach: function (context) {
      $('.visualization-embed #embed-height').on('keyup', renderIframeCode);
      $('.visualization-embed #embed-width').on('keyup', renderIframeCode);

      $('.visualization-embed .embed-code-wrapper').hide();
      $('.visualization-embed').on('click', 'a.embed-link', function(){
        $(this).parents('.visualization-embed').find('.embed-code-wrapper').toggle();
        return false;
      });
      function renderIframeCode(e){
        var prop = (e.currentTarget.id === 'embed-height') ? 'height' : 'width';
        var value = ($(this).val()) ? $(this).val() : (prop === 'height') ? '600' : '960';

        var iframe = $('.visualization-embed #embed-code').text();
        var newCode = $(iframe).prop(prop, value).get(0).outerHTML;
        $('.visualization-embed #embed-code').text(newCode);
      }
    },
  };
})(jQuery);
