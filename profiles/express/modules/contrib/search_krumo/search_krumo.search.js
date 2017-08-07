(function ($) {
  Drupal.behaviors.search_krumo_search = {
    attach: function() {
      // Define krumo root.
      var k = $('.krumo-root:not(".processed")');

      // Check if there is a krumo.
      if ($(k).addClass('processed').length > 0) {
        // Create the search krumo form.
        var form  = '<div class="search-krumo">';
        form     += '  <form id="search-krumo">';
        form     += '    <input class="form-text" type="text" name="search-query" />';
        // If there are more than one krumo's.
        if ($(k).length > 1) {
          form   += '    <select class="form-select" name="search-option">';
          form   += '      <option value="all">' + Drupal.t('Search all') + '</option>';
          // For each krumo.
          $(k).each(function(i) {
            i++;
            form += '      <option value="'+ i +'">' + Drupal.t('Search krumo') + ' #'+ i +'</option>';
          });
          form   += '    </select>';
        }
        form     += '    <input class="form-submit" type="submit" value="' + Drupal.t('Submit') + '" name="submit" />';
        form     += '  </form>';
        form     += '</div>';
        form     += '<div class="search-krumo-results"></div>';

        // Insert the form before the first krumo.
        k.eq(0).before(form);
      }

      // On submit execute the following.
      $('form#search-krumo').submit(function(e) {
        // Remove result and classes from previous query.
        $('.krumo-element.krumo-query-result').removeClass('krumo-query-result');
        $('.krumo-nest').hide().prev().removeClass('krumo-opened');
        $('.search-krumo-results').html('');

        // Get query value and option value as variables.
        var q = $('input[name=search-query]', this).val();
        var o = $('select[name=search-option]', this).val();

        // If the query is not empty, we can proceed.
        if (q) {
          var k;
          // Check if we're just trying to look through a single krumo.
          if (o && o != 'all') {
            k = $('.messages.status ul li:nth-child('+ o +') .krumo-root');
          }
          else {
            // Let's search through all of them!
            k = $('.krumo-root');
          }
          // Find all elements with the query.
          $('.krumo-element > a:contains('+ q +'), .krumo-element > strong:contains('+ q +'), .krumo-preview:contains('+ q +')', k).each(function(i) {
            if (!$(this).hasClass('krumo-preview')) {
              // Add result class.
              $(this).parent().addClass('krumo-query-result');

              // Expand parents until the query result is layed open before the user.
              $(this).parent().parents(".krumo-nest").show().prev().addClass('krumo-opened');
            }
            else  {
              // We have a result in the preview (body/summary).
              $(this).closest(".krumo-nest").prev().addClass('krumo-query-result');

              // Expand parents until the query result is layed open before the user.
              $(this).parent().parents(".krumo-nest").show().prev().addClass('krumo-opened');
            }
          });
          // Show result overview.
          $('.search-krumo-results').html(Drupal.formatPlural($('.krumo-element > a:contains('+ q +'), .krumo-element > strong:contains('+ q +'), .krumo-preview:contains('+ q +')', k).length, 'Found 1 element', 'Found @count elements'));
        }
        else {
          $('.search-krumo-results').html(Drupal.t('Empty query'));
        }

        // Prevent the form from being submitted.
        e.preventDefault();
      });
    }
  }
})(jQuery);
