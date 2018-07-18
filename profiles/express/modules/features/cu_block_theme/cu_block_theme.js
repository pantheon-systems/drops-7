(function( $ ){
  $(document).ready(function(){  
    var i=1;
    $('.blockstylesteps ul.menu li a').each(function(){
      $(this).before('<span class="chart-number">' + i + '</span>');
      i++;
    });
  });
})( jQuery );
