(function ($) {

// Written by
// Bram ten Hove, bramth@goalgorilla.com
// Daniel Beeke, daniel@goalgorilla.com

// Explain link in query log
Drupal.behaviors.search_krumo_search = {
  attach: function() {

    // Define krumo root.
    var k = $('.krumo-root:not(".processed")');

    // Check if there is a krumo.
    if ($(k).addClass('processed').length > 0) {
      var form  = '<div class="search-krumo">';
      form     += '  <form id="search-krumo">';
      form     += '    <input class="form-text" type="text" name="search-query" />';
      // If there are more than one krumo's.
      if ($(k).length > 1) {
        form   += '    <select class="form-select" name="search-option">';
        form   += '      <option value="all">search all</option>';
        // For each krumo.
        $(k).each(function(i) {
          i++;
          form += '      <option value="'+ i +'">search krumo #'+ i +'</option>';
        });
        form   += '    </select>';
      }
      form     += '    <input class="form-submit" type="submit" value="submit" name="submit" />';
      form     += '  </form>';
      form     += '</div>';
      form     += '<div class="search-krumo-results"></div>';

      // Insert the form before the first krumo.
      k.eq(0).before(form);
    }

    // On submit execute the following.
    $('form#search-krumo').submit(function() {
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
        if (o && o != 'all') {
          k = $('.messages.status ul li:nth-child('+ o +') .krumo-root');
        }
        else {
          k = $('.krumo-root');
        }
        // Find all elements with the query.
        $('.krumo-element > a:contains('+ q +'), .krumo-element > strong:contains('+ q +')', k).each(function(i) {
          // Add result class.
          $(this).parent().addClass('krumo-query-result');

          // Expand parents until the query result is layed open before the user.
          $(this).parent().parents(".krumo-nest").show().prev().addClass('krumo-opened');
        });
        // Show result overview.
        $('.search-krumo-results').html('Found '+ $('.krumo-element > a:contains('+ q +'), .krumo-element > strong:contains('+ q +')', k).length +' elements');
      }
      else {
        $('.search-krumo-results').html('Empty query');
      }
      return false;
    });

  }
}

})(jQuery);
