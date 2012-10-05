(function ($) {

  Drupal.behaviors.openpublish_media = {
    attach: function (context, settings) {
      if ($('.node-openpublish-photo-gallery .field-name-field-op-main-image .field-item a, .node-openpublish-photo-gallery .field-name-field-op-gallery-image-image .field-item a', context).length > 0) {
        var photoswipe = $('.node-openpublish-photo-gallery .field-name-field-op-main-image .field-item a, .node-openpublish-photo-gallery .field-name-field-op-gallery-image-image .field-item a', context).photoSwipe({
          getImageCaption: function(el){
            var caption = $(el).closest('.field-collection-item-field-op-gallery-image', context).find('.field-name-field-op-gallery-image-caption .field-item', context).text();
            if (caption != '') {
              return caption;
            } else {
              return $(el).closest('#region-content', context).find('h1#page-title', context).text();
            }
          },
          captionAndToolbarAutoHideDelay: 10000,
          captionAndToolbarShowEmptyCaptions: false,
          imageScaleMethod: 'fitNoUpscale'
        });
      }
    }
  };

})(jQuery);