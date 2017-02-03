/**
 * @file
 * Hook on the bind events from media field buttons.
 *
 */

(function ($) {

/**
 * Loads media browsers and callbacks, specifically for media as a field.
 * This is overridden, because there is no js event available to hook in.
 */
Drupal.behaviors.mediaElement = {
    
  attach: function (context, settings) {  
    
    $('.media-widget', context).once('mediaBrowserLaunch', function () {
      
      var options = settings.media.elements[this.id];
      globalOptions = {};
      if (options.global != undefined) {
        var globalOptions = options.global;
      }
      //options = Drupal.settings.media.fields[this.id];
      var fidField = $('.fid', this);
      var previewField = $('.preview', this);
      var removeButton = $('.remove', this); // Actually a link, but looks like a button.
      var imagecropDiv = $('.imagecrop', this);
      var imagecropLink = imagecropDiv.find('a');
      
      // Show the Remove button if there's an already selected media.
      if (fidField.val() != 0) {
        removeButton.css('display', 'inline-block');
      }
  
      // When someone clicks the link to pick media (or clicks on an existing thumbnail)
      $('.launcher', this).bind('click', function () {
        // Launch the browser, providing the following callback function
        // @TODO: This should not be an anomyous function.
        Drupal.media.popups.mediaBrowser(function (mediaFiles) {
          if (mediaFiles.length < 0) {
            return;
          }
          var mediaFile = mediaFiles[0];
          // Set the value of the filefield fid (hidden).
          fidField.val(mediaFile.fid);
          // Set the preview field HTML.
          previewField.html(mediaFile.preview);
          // Show the Remove button.
          removeButton.show();
          
          // Show the imagecrop link
          imagecropDiv.css({display : 'inline-block'});

          var oldHref = imagecropLink.attr('href');
          var queryStringStart = oldHref.indexOf('?');
          var queryString = '';
          if (queryStringStart >= 0) {
            queryString = '?' + oldHref.slice(queryStringStart + 1);  
          }
          
          // Set correct file
          var href = Drupal.settings.imagecrop.cropUrl.replace('/fid/', '/' + mediaFile.fid + '/');
          if (queryString != '') {
            href = href + queryString;
          }
          
          imagecropLink.attr('href', href);
          
        }, globalOptions);
        return false;
      });
  
      // When someone clicks the Remove button.
      $('.remove', this).bind('click', function () {
        // Set the value of the filefield fid (hidden).
        fidField.val(0);
        // Set the preview field HTML.
        previewField.html('');
        // Hide the Remove button.
        removeButton.hide();
        return false;
      });
  
      $('.media-edit-link', this).bind('click', function () {
        var fid = fidField.val();
        if (fid) {
          Drupal.media.popups.mediaFieldEditor(fid, function (r) { alert(r); });
        }
        return false;
      });
  
    });
    
  }
  
}

Drupal.Imagecrop = {}

/**
 * Open the imagecrop popup to the given link
 */
Drupal.Imagecrop.openPopup = function(link) {
  
  var url = $(link).attr('href');
  window.open(url, 'imagecrop', 'menubar=0,scrollbars=1,resizable=1,width=' + Drupal.settings.imagecrop.popupWidth + ',height=' + Drupal.settings.imagecrop.popupWidth + "'");  
  
}

})(jQuery);