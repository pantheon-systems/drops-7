(function ($) {
  Drupal.behaviors.gazette = {
    attach: function (context, settings) {
      $(window).bind('load resize', function(){
        var breakpoint = 740;
        
        //different browsers calculate page width differently
        if (navigator.userAgent.indexOf("WebKit") != -1) {
          var width = $(document).width();
        } else {
          //webkit
          var width = window.innerWidth;
        }
        
        //check if the menu is created or not and fire appropriate calls
        if (typeof menuInit == 'undefined' || menuInit == false) {
          if(width < breakpoint) {
            menuInit = true;
            Drupal.behaviors.gazette.createMenu();
          }
        } else {
          if (width >= breakpoint) {
            menuInit = false;
            Drupal.behaviors.gazette.destroyMenu();
          }
        }
      })
    },
    createMenu: function (context, settings) {
      $('nav.navigation h2', context).click(function(){
        $('nav.navigation', context).toggleClass('active');
      })
    },
    destroyMenu: function (context, settings) {
      $('nav.navigation h2', context).unbind('click');
    }
  };
})(jQuery);