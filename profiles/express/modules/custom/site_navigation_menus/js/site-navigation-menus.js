(function ($) {
  $(document).ready(function(){
    var sectionClick = false;
    $(".section-navigation-toggle").click(function () {
      $("#section-navigation").fadeToggle();
      if ($(this).attr('aria-expanded') == 'false') {
        $(this).attr('aria-expanded', 'true');
        $('.section-navigation-toggle .fa').toggleClass('fa-chevron-down');
        $('.section-navigation-toggle .fa').toggleClass('fa-reorder');
      }
      else {
        $(this).attr('aria-expanded', 'false');
        $('.section-navigation-toggle .fa').toggleClass('fa-reorder');
        $('.section-navigation-toggle .fa').toggleClass('fa-chevron-down');
      }
      return false;
    });
  });
})(jQuery);
