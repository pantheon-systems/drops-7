(function ($) {
  if ($(window).width() > Drupal.settings.backstretchMinWidth) {
    $.backstretch(Drupal.settings.backstretchURL);
    
    $(document).ready(function () { 
      if (Drupal.settings.backstretchScroller) {
        var height = $(window).height() + parseInt(Drupal.settings.backstretchScrollerAdj);
        if ($('#toolbar').length > 0) { var height = height - $('#toolbar').height(); }
        if (Drupal.settings.backstretchScrollerAdj != 0) {
          $('body').append('<div id="backstretchmargin"></div>');
        }
        $('#backstretchmargin').css('margin-bottom', height);
      }

      if (Drupal.settings.backstretchScrollTo) {

        $("#footer").waypoint(function(){
          $("#backstretch-scrollto").text("Back to Top").attr("href", "#page");
        });

        $("#main-menu").waypoint(function(){
          $("#backstretch-scrollto").text("View Photo").attr("href", "#backstretchmargin");
        });
        
        // Scroll to right place on click.
        $("#backstretch-scrollto").click(function(e){
          var s = $(this).attr("href");
          console.log(s);
          if (s == '#backstretchmargin') {
            var s = 'max';
            console.log('hi');
          }
          $.scrollTo(s, 1000);
          e.preventDefault();
          return false;
        });
      }
    });
  }
})(jQuery);
