(function( $ ){
  $(document).ready(function() {
    $('.article-slider').flickity({
      'wrapAround': true,
      'adaptiveHeight': true,
      'draggable': false,
      'cellAlign': 'left',
      'groupCells': true,
      'contain': true,
      'lazyLoad': true,
    });
  })
})( jQuery );
