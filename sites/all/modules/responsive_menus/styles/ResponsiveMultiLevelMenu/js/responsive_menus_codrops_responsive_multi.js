/**
 * @file
 * Integrate codrops' ResponsiveMultiLevelMenu library with Responsive Menus.
 */
(function ($) {
  Drupal.behaviors.responsive_menus_codrops_responsive_multi = {
    attach: function (context, settings) {
      settings.responsive_menus = settings.responsive_menus || {};
      var $windowWidth = document.documentElement.clientWidth || document.body.clientWidth;
      $.each(settings.responsive_menus, function(ind, iteration) {
        if (iteration.responsive_menus_style != 'codrops_responsive_multi') {
          return true;
        }
        if (!iteration.selectors.length) {
          return;
        }
        // Only apply if window size is correct.  Runs once on page load.
        var $media_size = iteration.media_size || 768;
        if ($windowWidth <= $media_size) {
          // Call codrops ResponsiveMultiLevelMenu with our settings.
          $(iteration.selectors).once('responsive-menus-codrops-multi-menu', function() {
            $(this).prepend('<button class="dl-trigger">Open Menu</button>');
            // Removing other classes / IDs.
            $(this)
              .attr('class', 'dl-menuwrapper')
              .attr('id', 'dl-menu')
              .css('z-index', '999');
            // Find the parent ul.
            var $parent_ul = $(this).find('ul:not(.contextual-links)').first();
            $parent_ul
              .attr('class', 'dl-menu')
              .attr('id', 'rm-dl-menu')
              .find('li').removeAttr('id').removeAttr('class')
              .find('a').removeAttr('id').removeAttr('class');
            // Add submenu classes.
            $parent_ul.find('ul').each(function(subIndex, subMenu) {
              $(this).removeAttr('id').attr('class', 'dl-submenu');
              $subnav_link = $(this).parent('li').find('a').first()[0].outerHTML;
              $(this).prepend('<li>' + $subnav_link + '</li>');
            });
            // Call the ResponsiveMultiLevelMenu dlmenu function.
            $(this).dlmenu({
                animationClasses : {
                  classin : iteration.animation_in || 'dl-animate-in-1',
                  classout : iteration.animation_out || 'dl-animate-out-1'
                }
            });
          });
        }

      });

    }
  };
}(jQuery));
