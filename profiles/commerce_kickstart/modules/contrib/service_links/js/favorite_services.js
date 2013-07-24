(function ($) {
  Drupal.behaviors.sl_fs = {
    attach: function(context, settings) {
      var $favorites = $("a.service-links-favorite", context).once('service-links-favorite');
      var message = '';

      $favorites.show();
      if (window.chrome) {
        message= Drupal.t('Use CTRL + D to add this to your bookmarks');
      } else if (window.opera && window.print) {
        $favorites.each(function(){
          var url = $(this).attr('href').split('&favtitle=');
          var title = decodeURI(url[1]);
          url = url[0];
          $(this).attr('rel', 'sidebar').attr('href', url).attr('title', title);
        });
      } else if (window.sidebar || window.external) {
        $favorites.click(function(event){
          event.preventDefault();
          var url = $(this).attr('href').split('&favtitle=');
          var title = decodeURI(url[1]);
          url = url[0];
          if (window.sidebar) {
            window.sidebar.addPanel(title, url, '');
          } else if (window.external) {
            window.external.AddFavorite(url, title);
          }
        });
      } else {
        message = Drupal.t('Please use your browser to bookmark this page.');
      }
      if (message) {
        $favorites.click(function(event){
          event.preventDefault();
          alert(message);
        });
      }
    }
  }
})(jQuery);
