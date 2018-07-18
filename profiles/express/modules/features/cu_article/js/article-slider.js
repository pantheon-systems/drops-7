(function( $ ){
  $(document).ready(function() {
    $('.article-slider').flickity({
      'wrapAround': false,
      'adaptiveHeight': true,
      'draggable': false,
      'cellAlign': 'left',
      'groupCells': true,
      'contain': true,
      'lazyLoad': true,
    });
  })
})( jQuery );
