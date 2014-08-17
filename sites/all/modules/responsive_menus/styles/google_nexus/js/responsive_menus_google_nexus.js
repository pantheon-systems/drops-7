/**
 * @file
 * Integrate GoogleNexus (codrops) library with Responsive Menus module.
 */

(function ($) {
  Drupal.behaviors.responsive_menus_google_nexus = {
    attach: function (context, settings) {
      settings.responsive_menus = settings.responsive_menus || {};
      $.each(settings.responsive_menus, function(ind, iteration) {
        if (iteration.responsive_menus_style != 'google_nexus') {
          return true;
        }
        if (!iteration.selectors.length) {
          return;
        }
        // Main loop.
        $(iteration.selectors).once('responsive-menus-google-nexus', function() {
          $(this).attr('class', 'gn-menu responsive-menus-google-nexus-processed').removeAttr('id');
          if (iteration.use_ecoicons == '1') {
            $(this).addClass('ecoicons');
          }
          // Add icons in front of menu items.
          $(this).find('a').each(function(a_ind) {
            if (iteration.icons[a_ind]) {
              // Un-escape unicode or html entities.
              var $icon = $('<div />').html(JSON.parse('"' + iteration.icons[a_ind] + '"')).text();
              $(this).attr('data-content', $icon);
            }
            else {
              $icon = $('<div />').html(JSON.parse('"' + iteration.icon_fallback + '"')).text();
              $(this).attr('data-content', $icon);
            }
          });
          // Add other required classes.
          $(this).find('ul').attr('class', 'gn-submenu');
          $(this).find('li').removeAttr('class');

          $(this).before('<div class="gn-menu-container"></div>');
          // Wrap with the structure Google Nexus Menu needs.
          $('.gn-menu-container').append('<ul id="gn-menu" class="gn-menu-main" style="z-index: 99;">'
           + '<li class="gn-trigger">'
           + '<a class="gn-icon gn-icon-menu"><span>Menu</span></a>'
           + '<nav class="gn-menu-wrapper">'
           + '<div class="gn-scroller">'
           + $(this)[0].outerHTML
           + '</div>'
           + '</nav>'
           + '</li>'
           + '<li></li>'
           + '</ul>');

          $(this).remove();
          // Create the menu.
          new gnMenu(document.getElementById('gn-menu'));

        });
      });
    }
  };
}(jQuery));
