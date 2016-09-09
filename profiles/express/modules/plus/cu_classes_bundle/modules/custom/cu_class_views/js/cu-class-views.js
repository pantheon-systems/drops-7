(function( $ ){
  $(document).ready(function(){
    $(".form-item-combine").addClass("element-invisible");
    $(".views-widget-filter-combine label").addClass("icon-closed");
    $(".views-widget-filter-combine label").click(function(){
      var clicks = $(this).data('clicks');
      if (clicks) {
        $(".form-item-combine").addClass("element-invisible");
        $(this).addClass("icon-closed");
        $(this).removeClass("icon-open");

      } else {
        $(".form-item-combine").removeClass("element-invisible");
        $(this).removeClass("icon-closed");
        $(this).addClass("icon-open");
        $(".form-item-combine input").focus();
      }
      $(this).data("clicks", !clicks);
      return false;
    });
  });
})( jQuery );
