jQuery(window).scroll(function() {
  if (jQuery(this).scrollTop() > 300) {
      jQuery('#cu_back_to_top').fadeIn('slow');
  } else {
      jQuery('#cu_back_to_top').fadeOut('slow');
  }
});
jQuery('#cu_back_to_top a').click(function(){
  jQuery("html, body").animate({ scrollTop: 0 }, 600);
  jQuery("#page").focus();
  return false;
});