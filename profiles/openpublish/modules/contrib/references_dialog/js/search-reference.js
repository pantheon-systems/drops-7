(function ($) {
  Drupal.behaviors.referencesDialog = {
    attach: function (context, settings) {
      // Make sure the overlay doesn't mess things up for us by unbinding it's
      // event.
      $(document).unbind('click.drupal-overlay mouseup.drupal-overlay');
      // Check what type of display we are dealing with.
      // We can't combine all of these, since that causes
      // JQuery.each() to freak ut.'
      var selector = null;
      if ($('table.views-table').size() > 0) {
        selector = $('table.views-table tbody tr');
      }
      else if ($('table.views-view-grid').size() > 0) {
        selector = $('table.views-view-grid td');
      }
      else if ($('.views-row').size() > 0) {
        selector = $('.views-row');
      }
      else {
        return;
      }
      selector.each(function(index) {
        $(this).click(function(e) {
          // Ignore if the element is a link.
          if (e.target && e.target.nodeName && e.target.nodeName.toLowerCase() !== 'a') {
            // Fetch the entity from wherever it might be.
            var entity = settings.ReferencesDialog.entities[index];
            // Tell our parent that we are done with what we want to do here.
            parent.Drupal.ReferencesDialog.close(entity.entity_type, entity.entity_id, entity.title);
          }
        });
      });
      // Process all links so that they have the render=references_dialog
      // parameter. Also, make sure that we don't close the dialog and enter
      // anything upon entity submittion.'
      $('#references-dialog-page a').each(function(key, element) {
        var href = $(element).attr('href');
        $(element).attr('href', href + (href.indexOf('?') ? '&' : '?')
          + 'render=references-dialog&closeonsubmit=0');
      })
    }
  }
})(jQuery);