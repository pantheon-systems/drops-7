(function( $ ){


function parallax_browser_check() {
  if (navigator.userAgent.match(/(iPod|iPhone|iPad|Android)/)) {
    return false;
  }
  return true;
}

$(document).ready(function(){
  if (parallax_browser_check()) {
    $('.parallax-window').parallaxie();
  }
});


}( jQuery ));
