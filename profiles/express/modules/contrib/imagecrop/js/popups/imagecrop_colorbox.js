(function($) { 

$(document).ready(function() {
  $('#cancel-crop').click(Drupal.Imagecrop.closePopup);
});

/**
 * Event listener, close the current popup.
 */
Drupal.Imagecrop.closePopup = function() {
  parent.jQuery.colorbox.close();
}

/**
 * Force an update from the imagefield widgets.
 */
Drupal.Imagecrop.forceUpdate = function() {
  $('.user-picture img', parent.document).each(Drupal.Imagecrop.refreshImage);
  $('.image-preview img', parent.document).each(Drupal.Imagecrop.refreshImage);  
}


})(jQuery);