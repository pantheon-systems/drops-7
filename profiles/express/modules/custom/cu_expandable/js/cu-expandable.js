(function ($) {
  $(document).ready(function(){

    $(".expandable").expandable({
      type: 'accordion',
      class: 'outline',
      animation: 'fade',
      responsive: {
        breakpoint: 3000,
        headingTagName: "strong",
      },
    });
    
  });
}(jQuery));
