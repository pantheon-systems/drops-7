(function ($) {
  $(document).ready(function(){
    $(".bean-collection-grid").each(function(){

    });
    $(".collection-items-categories").hide();

    $("ul.collection-items-navigation a").first().addClass('active');
    $("ul.collection-items-navigation a").click(function(){
      $(".collection-items").hide();
      var target = $(this).attr("href");
      $(target).fadeIn();
      $("ul.collection-items-navigation a").removeClass('active');
      $(this).addClass('active');
      return false;
    });

    $("select.collection-filter").change(function(){
      $(".collection-items").hide();
      var target = $(this).val();
      //alert(target);
      $(target).fadeIn();
    });
  });
})(jQuery);
