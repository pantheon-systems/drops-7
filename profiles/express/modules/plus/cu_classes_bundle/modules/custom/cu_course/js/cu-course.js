(function( $ ){
  $(document).ready(function(){
    $("#other-classes-trigger").click(function(){
      var clicks = $(this).data('clicks');
      if (clicks) {
        var t = $(this).attr("href");
        $(t).slideToggle("fast");
        $("i.fa", this).removeClass("fa-minus-square");
        $("i.fa", this).addClass("fa-plus-square");
        $(this).removeClass("expand-active");
        $(this).attr('aria-expanded', 'false');
        return false;
      } else {
        var t = $(this).attr("href");
        $(t).slideToggle("fast");
        $("i.fa", this).removeClass("fa-plus-square");
        $("i.fa", this).addClass("fa-minus-square");
        $(this).addClass("expand-active");
        $(this).attr('aria-expanded', 'true');
        return false;
      }
      $(this).data("clicks", !clicks);
    });
  });
})( jQuery );
