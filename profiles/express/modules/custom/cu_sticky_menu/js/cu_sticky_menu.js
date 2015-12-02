jQuery(window).scroll(function() {
  if (jQuery(this).scrollTop() > 160) {
      jQuery('#sticky-menu').fadeIn('slow');
  } else {
      jQuery('#sticky-menu').fadeOut('slow');
  }
});