(function( $ ){
  $(document).ready(function() {
    $('.article-slider').owlCarousel({
      loop: true,
      margin: 40,
      responsiveClass: true,
      responsive: {
        0: {
          items: 1,
          nav: false
        },
        600: {
          items: 2,
          nav: true
        },
        960: {
          items: 3,
          nav: true,
          loop: false,
          margin: 40
        }
      }
    })
  })
})( jQuery );
