// $Id$
(function ($) {

/**
 * Toggles the collapsible region.
 */
Drupal.behaviors.skyCollapsRegionToggle = {
  attach: function (context, settings) {
    $('.collapsible-toggle a, context').click(function() {
      $('#section-collapsible').toggleClass('toggle-active').find('.region-collapsible').slideToggle('fast');
      return false;
    });
  }
}

Drupal.behaviors.skyCollapsMenuToggle = {
  attach: function (context, settings) {
    $('.menu-toggle a, context').click(function() {
      $('#menu-bar').toggleClass('toggle-active').find('nav').slideToggle('fast');
      return false;
    });
  }
}

/**
 * CSS Help for IE.
 * - Adds even odd striping and containers for images in posts.
 * - Adds a .first-child class to the first paragraph in each wrapper.
 * - Adds a prompt containing the link to a comment for the permalink.
 */
Drupal.behaviors.skyPosts = {
  attach: function (context, settings) {
    // Detects IE6-8.
    if (!jQuery.support.leadingWhitespace) {
      $('.article-content p:first-child').addClass('first-child');
      $('.article-content img, context').parent(':not(.field-item, .user-picture)').each(function(index) {
        var stripe = (index/2) ? 'even' : 'odd';
        $(this).wrap('<div class="content-image-' + stripe  + '"></div>');
      });
    }
    // Comment link copy promt.
    $("time span a").click( function() {
      prompt('Link to this comment:', this.href);
      return false;
    });
  }
}

})(jQuery);
