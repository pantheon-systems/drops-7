/**
 * @file
 * HTML Link insert plugin for Linkit.
 */
(function($) {
  Drupal.linkit.registerInsertPlugin('html_link', {
    insert: function(data) {
      var linkitInstance = Drupal.settings.linkit.currentInstance,
          selection = linkitInstance.selection,
          text = linkitInstance.linkContent || data.path;

       // Delete all attributes that are empty.
      for (var attr in data.attributes) {
        if (data.attributes.hasOwnProperty(attr)) {
          delete data.attributes[attr];
        }
      }

      if (selection && selection.text.length >= 1) {
        text = selection.text;
      }

      // Use document.createElement as it is mush fasten then $('<a/>).
      return $(document.createElement('a'))
        .attr(data.attributes)
        .attr('href', data.path)
        .html(text)
        // Convert the element to a string.
        .get(0)
        .outerHTML;
    }
  });
})(jQuery);
