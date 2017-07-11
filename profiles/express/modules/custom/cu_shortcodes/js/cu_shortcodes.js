(function( $ ){
  $(document).ready(function(){
    $(".expand-content").hide();
    $("a.expand-title span").addClass("expand");
    $("a.expand-title").toggle(function(){
      var t = $(this).attr("href");
      $(t).slideToggle("fast");
      $("span", this).removeClass("expand");
      $("span", this).addClass("collapse");
      $(this).addClass("expand-active");
      $(this).attr('aria-expanded', 'true');
      return false;
    }, function() {
      var t = $(this).attr("href");
      $(t).slideToggle("fast");
      $("span", this).removeClass("collapse");
      $("span", this).addClass("expand");
      $(this).removeClass("expand-active");
      $(this).attr('aria-expanded', 'false');
      return false;
    });

    $(".small-expand-content, .tooltip-expand-content").hide();
    $("a.small-expand-title").toggle(function(){
      var t = $(this).attr("href");
      $(t).slideToggle("fast");
      $("i.fa", this).removeClass("fa-plus-square");
      $("i.fa", this).addClass("fa-minus-square");
      $(this).attr('aria-expanded', 'true');
      return false;
    }, function() {
      var t = $(this).attr("href");
      $(t).slideToggle("fast");
      $("i.fa", this).removeClass("fa-minus-square");
      $("i.fa", this).addClass("fa-plus-square");
      $(this).attr('aria-expanded', 'false');
      return false;
    });
    $("a.tooltip-expand-title").click(function(){
      var t = $(this).attr("href");
      $(t).fadeIn();
      $("i.fa", this).removeClass("fa-plus-square");
      $("i.fa", this).addClass("fa-minus-square");
      $(this).attr('aria-expanded', 'true');
      return false;
    });
    $("a.close-tip").click(function(){
      var t = $(this).attr("href");
      var r = $(this).attr("rel");
      $(t).fadeOut();
      $("." + r + " i.fa").removeClass("fa-minus-square");
      $("." + r + " i.fa").addClass("fa-plus-square");
      $("." + r).attr('aria-expanded', 'false');
      return false;
    });
    // Countup
    $('.counter').counterUp({
      delay: 10,
      time: 2000,
    });
  });
})( jQuery );


function cu_shortcodes_achors_js(selector) {
  var count = 0;
  var anchorText = '';
  jQuery(selector).each(function(){
		count++;
		var thisText = jQuery(this).text();
		thisText = jQuery.trim(thisText);
		var anchorTextURL = thisText.replace(/ /g, "-");
		var anchorLink = '<a name="' + anchorTextURL + '" id="' + anchorTextURL + '"></a>';
		anchorText += '<li><i class="fa fa-arrow-down arrow"></i> <a href="#' + anchorTextURL + '">' + thisText + '</a></li>';
		jQuery(this).before(anchorLink);
	});
	anchorText = '<div class="auto-anchor"><ul>' + anchorText + '</ul><div>';
  if (count > 0) {
    jQuery(".anchors-links").html(anchorText);
    jQuery(".anchors").fadeIn();
  }

  jQuery('.anchors a').click(function(){
    var scrollTarget = jQuery(this).attr("href");
    jQuery("html, body").animate({scrollTop: (jQuery(scrollTarget).offset().top)-100}, 300);
    return false;
  });
}
