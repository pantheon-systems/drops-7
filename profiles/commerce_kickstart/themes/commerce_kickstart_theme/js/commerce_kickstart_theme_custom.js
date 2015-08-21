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
      $('body', context).bind('responsivelayout', function(e, d) {
        if($(this).hasClass("responsive-layout-mobile")) {
          $('.block-facetapi', context).each(function(index) {
            var $facetBlock = $(this);

            // Clean up <li> children so select list looks okay.
            $facetBlock.find('label.element-invisible', context).remove();
            $facetBlock.find('span.element-invisible', context).remove();
            $facetBlock.find('input.facetapi-checkbox', context).remove();

            // Get block title.
            var list_title = $facetBlock.find('.block-title', context).text().toLowerCase();

            // Get list elements.
            var $list_element = $facetBlock.find('ul', context);
            $list_element.addClass('facetapi-lists');

            if(typeof $list_element !== 'undefined') {
              selectnav($list_element.attr('id'), {
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
