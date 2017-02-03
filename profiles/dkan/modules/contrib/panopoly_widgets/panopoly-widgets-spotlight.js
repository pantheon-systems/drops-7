Drupal.settings.spotlight_settings = Drupal.settings.spotlight_settings || {};

(function ($) {

 /**
  * Form behavior for Spotlight
  */
 Drupal.behaviors.panopolySpotlight = {
   attach: function (context, settings) {
     if ($('.field-name-field-basic-spotlight-items').length) {
       $('.field-name-field-basic-spotlight-items').each(function() {
         var rotation_time = $(this).find('.panopoly-spotlight-buttons-wrapper').data('rotation-time');
         var $slides = $(this);
         $slides.tabs().tabs("rotate", rotation_time, true);
         var $controls = $slides.find('.ui-tabs-nav li');
         // $('.field-name-field-basic-spotlight-items').css('height', $('.field-name-field-basic-spotlight-items').height());
         // $('.field-name-field-basic-spotlight-items').css('overflow', 'hidden');

         // Navigation is hidden by default, display it if JavaScript is enabled.
         $slides.find('.panopoly-spotlight-buttons-wrapper').css('display', 'block');

         $slides.find('.panopoly-spotlight-pause-play').once('panopoly-spotlight').bind('click', function(event) {
           event.preventDefault();
           if ($(this).hasClass('paused')) {
             $slides.tabs().tabs("rotate", rotation_time, true);
             $(this).text(Drupal.t('Pause'));
             $(this).removeClass('paused');
           }
           else {
             $slides.tabs().tabs("rotate", 0, false);
             $slides.find('.ui-tabs-nav li a').attr('tabindex', 0);
             $(this).text(Drupal.t('Play'));
             $(this).addClass('paused');
           }
         });
         if ($slides.find('.panopoly-spotlight-previous').length && $slides.find('.panopoly-spotlight-next').length) {
           $slides.find('.panopoly-spotlight-previous').once('panopoly-spotlight').bind('click', function (event) {
             event.preventDefault();
             $slides.find('.panopoly-spotlight-pause-play:not(.paused').trigger('click');
             var activeControl = $($controls.filter('.ui-state-active'));

             if (activeControl.prev().length != 0) {
               activeControl.prev().children('a').trigger('click');
             }
             else {
               $controls.last().children('a').trigger('click');
             }
           });
           $slides.find('.panopoly-spotlight-next').once('panopoly-spotlight').bind('click', function (event) {
             event.preventDefault();
             $slides.find('.panopoly-spotlight-pause-play:not(.paused').trigger('click');
             var activeControl = $($controls.filter('.ui-state-active'));

             if (activeControl.next().length != 0) {
               activeControl.next().children('a').trigger('click');
             }
             else {
               $controls.first().children('a').trigger('click');
             }
           });
         }
       });
     }
   }
 };

})(jQuery);
