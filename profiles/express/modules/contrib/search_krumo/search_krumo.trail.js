(function ($) {
  Drupal.behaviors.search_krumo_trail = {
    attach: function() {
      $('.krumo-element:not(".processed")').addClass('processed').append('<span class="krumo-get-path"><a href="#">' + Drupal.t('Get path') + '</a></span>');

      // The function to return the path.
      $('.krumo-get-path').once().click(function(e) {
        // Function for getting a path to an element in PHP.
        var pathItems = [];
        var parent = $(this).parents('.krumo-root');
        var krumoIndex = parent.index('.krumo-root');

        // Array which will hold all the pieces of the trail.
        var currentItem = ['Trail', $(this).parent().children('.krumo-name').text()];
        pathItems.push(currentItem);

        // Filling the trail array.
        $(this).parents('.krumo-nest').each(function(i) {
          // Get the element type.
          var elementType = $(this).prev('.krumo-element').children('.krumo-type').text().toString().split(' ');
          // Objects.
          if (elementType[0] == 'Object') {
            var currentItem = ['Object', $(this).prev('.krumo-element').children('.krumo-name').text()];
          }
          // Arrays.
          else if (elementType[0] == 'Array,') {
            var currentItem = ['Array', $(this).prev('.krumo-element').children('.krumo-name').text()];
          }
          pathItems.push(currentItem);
        });

        // The string with the whole trail which will be returned at the end.
        var trail = '';
        // For each item in the trail array we are going to add it to the trail.
        $.each(pathItems, function(i) {
          // Fix the trail for arrays.
          if (pathItems[i +1] && pathItems[i +1][0] == 'Array') {
            // Integers should be returned as integers.
            if (parseInt($(this)[1]) == $(this)[1]) {
              trail = '[' + $(this)[1] + ']' + trail;
            }
            // Replace 'und' by the Drupal constant LANGUAGE_NONE.
            else if ($(this)[1] == 'und') {
              trail = '[LANGUAGE_NONE]' + trail;
            }
            // Else we return the item as a string in the trail.
            else {
              trail = "['" + $(this)[1] + "']" + trail;
            }
          }
          // Fix the trail for objects.
          else if (pathItems[i +1] && pathItems[i +1][0] == 'Object') {
            // Replace 'und' by the Drupal constant LANGUAGE_NONE.
            if ($(this)[1] == 'und') {
              trail = '->{LANGUAGE_NONE}' + trail;
            }
            // Else we add the item to the trail.
            else {
              trail = '->' + $(this)[1] + trail;
            }
          }
          else {
            // Add the variable name if it could be found.
            if (Drupal.settings.searchKrumo !== undefined && Drupal.settings.searchKrumo.variable[krumoIndex] !== undefined) {
              trail = Drupal.settings.searchKrumo.variable[krumoIndex] + trail;
            }
            // Otherwise we return the default variable name.
            else {
              trail = '$var' + trail;
            }
          }
        });

        $(this).addClass('hidden').hide().before('<input id="trail-input" value="' + trail + '" />');

        $('#trail-input').select().blur(function() {
          $(this).remove();
          $('.krumo-get-path.hidden').show();
        });

        e.preventDefault();
      });
    }
  }
})(jQuery);
