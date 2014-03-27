(function ($) {
  // Override core JS so it works with "button" tags.
  if (Drupal.fieldUIOverview && Drupal.fieldUIOverview.AJAXRefreshRows) {
    /**
     * Triggers Ajax refresh of selected rows.
     *
     * The 'format type' selects can trigger a series of changes in child rows.
     * The #ajax behavior is therefore not attached directly to the selects, but
     * triggered manually through a hidden #ajax 'Refresh' button.
     *
     * @param rows
     *   A hash object, whose keys are the names of the rows to refresh (they
     *   will receive the 'ajax-new-content' effect on the server side), and
     *   whose values are the DOM element in the row that should get an Ajax
     *   throbber.
     */
    Drupal.fieldUIOverview.AJAXRefreshRows = function (rows) {
      // Separate keys and values.
      var rowNames = [];
      var ajaxElements = [];
      $.each(rows, function (rowName, ajaxElement) {
        rowNames.push(rowName);
        ajaxElements.push(ajaxElement);
      });

      if (rowNames.length) {
        // Add a throbber next each of the ajaxElements.
        var $throbber = $('<div class="ajax-progress ajax-progress-throbber"><div class="throbber">&nbsp;</div></div>');
        $(ajaxElements)
          .addClass('progress-disabled')
          .after($throbber);

        // Fire the Ajax update.
        $(':input[name=refresh_rows]').val(rowNames.join(' '));
        $(':input#edit-refresh').mousedown();

        // Disabled elements do not appear in POST ajax data, so we mark the
        // elements disabled only after firing the request.
        $(ajaxElements).attr('disabled', true);
      }
    };
  }
})(jQuery);
