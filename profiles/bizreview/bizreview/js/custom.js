
(function($) {
  /**
   * @todo
   */
  
  
  Drupal.behaviors.bizreviewDropdownMenu = {
    attach: function (context) {
        $('.dropdown').hover(
        function () {
			$(this).addClass('open');
		},
		
        function () {
			$(this).removeClass('open');
		}
		);
    }
  };
  
  
  Drupal.behaviors.bizreviewEqualHeights = {
    attach: function (context) {
      $('body', context).once('views-row-equalheights', function () {
        $(window).resize(function() {
          $($('.view-list-business-grid .view-content, .view-categories .view-content, .view-list-modern .view-content').get().reverse()).each(function () {
            var elements = $(this).children('.views-row').css('height', '');
            if($(window).width() > 960) {
              var tallest = 0;
              elements.each(function () {    
                if ($(this).height() > tallest) {
                  tallest = $(this).height();
                }
              }).each(function() {
                if (($(this).height() < tallest) || ($(this).height() == tallest)) {
                  $(this).css('height', tallest);
				  $('.views-row-inner',this).css('height', tallest);
                }
              });
			}
			else {
				elements.each(function () {
				  $(this).css('height', 'auto');
				  $('.views-row-inner',this).css('height', 'auto');
				});
			}
          });
        });
      });
    }
  };
  
  Drupal.behaviors.bizreviewGalleryPage = {
    attach: function (context) {
      $('.block-featured-business .views-field-field-image, .view-member .views-field-picture, .view-meet-our-team .views-field-field-image').hover(
        function () {
		  $(this).addClass('hover');
        },
        function () {
		  $(this).removeClass('hover');
        }
      );
    }
  };
  Drupal.behaviors.bizreviewThemeColors = {
    attach: function (context) {
      $('body', context).once('block-theme-colors-showhide', function () {													   
        jQuery('.block-theme-colors .close').click(function(e){
		  e.preventDefault();
		  jQuery('.block-theme-colors .block-theme-color-content ').hide();
		  jQuery(this).hide();
		  jQuery('.block-theme-colors .open').show();
		});
		jQuery('.block-theme-colors .open').click(function(e){
          e.preventDefault();
		  jQuery('.block-theme-colors .block-theme-color-content ').show();
		  jQuery(this).hide();
		  jQuery('.block-theme-colors .close').show();
		});  
      });
    }
  };
})(jQuery);

