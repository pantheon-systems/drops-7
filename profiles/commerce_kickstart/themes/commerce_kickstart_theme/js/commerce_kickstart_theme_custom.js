(function ($) {
  // Handle user toolbar when user is admin and have admin toolbar enabled.
  Drupal.behaviors.commerce_kickstart_theme_custom_toolbar = {
    attach: function(context, settings) {
      if ($('body').hasClass('toolbar')) {
        $(window, context).resize(function() {
          var toolbarHeight = $('div#toolbar').height();
          $('.zone-user-wrapper').css('top', toolbarHeight + 'px');
        });
      }
    }
  }
  // Disable input fields on price range when viewing the site
  // on normal devices.
  Drupal.behaviors.commerce_kickstart_theme_custom_search_api_ranges = {
    attach:function (context, settings) {
      $('body').bind('responsivelayout', function(e, d) {
        if ($(this).hasClass("responsive-layout-normal")) {
          $('div.search-api-ranges-widget').each(function() {
            $(this).find('input[name=range-from]').attr('readonly', true).unbind('keyup');
            $(this).find('input[name=range-to]').attr('readonly', true).unbind('keyup');
          });
        }
        else {
          $('body').unbind('responsivelayout');
        }
      });
    }
  }
  // Switch list elements to select lists on faceted blocks.
  Drupal.behaviors.commerce_kickstart_theme_custom_search = {
    attach: function(context, settings) {
      $('body').bind('responsivelayout', function(e, d) {
        if($(this).hasClass("responsive-layout-mobile")) {
          $('.block-facetapi', context).each(function(index) {
            $('.facetapi-checkbox').remove();
            $('.element-invisible').remove();

            // Get block title.
            var list_title;
            $(this).find('.block-title').each(function() {
              list_title = $(this).text().toLowerCase();
            });

            // Get list elements.
            var list_element;
            $(this).find('ul').each(function() {
              list_element = $(this).attr('id');
              $(this).addClass('facetapi-lists');
            });

            if(list_element != 'undefined') {
              selectnav(list_element, {
                label: 'Select a ' + list_title + '...',
                activeclass: 'false'
              });
            }
          });
        }
        else {
          $('body').unbind('responsivelayout');
        }
      });
    }
  }
})(jQuery);
