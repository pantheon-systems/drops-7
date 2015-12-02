(function ($) {

  Drupal.wysiwyg.plugins['video'] = {

    /**
     * Return whether the passed node belongs to this plugin.
     */
    isNode: function(node) {
      return ($(node).is('img.wysiwyg-video'));
    },

    /**
     * Execute the button.
     */
    invoke: function(data, settings, instanceId) { 
      Drupal.behaviors.video_wysiwyg.videoBrowser(function(nid, data, settings, instanceId){
        if (data.format == 'html') {
          // Prevent duplicating
          if ($(data.node).is('img.wysiwyg-video')) {
            return;
          }
          var width = 176;
          var height = 144;
          var dimensions = Drupal.settings.wysiwyg.plugins.drupal.video.golbal.dimensions;
          if(dimensions){
            var wxh = dimensions.split('x');
            var width = parseInt(wxh[0]);
            var height = parseInt(wxh[1]);
          }
          var content = Drupal.wysiwyg.plugins['video']._getPlaceholder(settings, Drupal.settings.basePath +'video/embed/' +nid+'/'+width+'/'+height, width, height);
        }
        else {
          // Prevent duplicating.
          // @todo data.content is the selection only; needs access to complete content.
          if (data.content.match(/[content:video]/)) {
            return;
          }
          var content = nid;
        }
        if (typeof content != 'undefined') {
          Drupal.wysiwyg.instances[instanceId].insert(content);
        }
      }, data, settings, instanceId);
    },

    /**
     * Replace all [[content:video]] tags with images.
     */
    attach: function(content, settings, instanceId) {
      return content;
    },

    /**
     * Replace images with [[content:video]] tags in content upon detaching editor.
     */
    detach: function(content, settings, instanceId) {
      //      return $content.html();
      return content;
    },

    /**
     * Helper function to return a HTML placeholder.
     */
    _getPlaceholder: function (settings, src, width, height) {
      return '<iframe width="'+width+'" height="'+height+'" src="'+src+'" frameborder="0" allowfullscreen scrolling="no"></iframe>';
    }
  };

})(jQuery);
