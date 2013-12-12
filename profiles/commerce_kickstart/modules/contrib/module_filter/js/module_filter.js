
(function ($) {
  var moduleFilterTimeOut;
  var moduleFilterTextFilter = '';

  Drupal.behaviors.moduleFilter = {
    attach: function() {
      $("#module-filter-wrapper").show();
      $('input[name="module_filter[name]"]').focus();
      $('input[name="module_filter[name]"]').keyup(function(e) {
        switch (e.which) {
          case 13:
            if (moduleFilterTimeOut) {
              clearTimeout(moduleFilterTimeOut);
            }

            moduleFilter(moduleFilterTextFilter);
            break;
          default:
            if (moduleFilterTextFilter != $(this).val()) {
              moduleFilterTextFilter = this.value;
              if (moduleFilterTimeOut) {
                clearTimeout(moduleFilterTimeOut);
              }

              moduleFilterTimeOut = setTimeout('moduleFilter("' + moduleFilterTextFilter + '")', 500);
            }
            break;
        }
      });
      $('input[name="module_filter[name]"]').keypress(function(e) {
        if (e.which == 13) e.preventDefault();
      });

      $('#edit-module-filter-show-enabled').change(function() {
        moduleFilter($('input[name="module_filter[name]"]').val());
      });
      $('#edit-module-filter-show-disabled').change(function() {
        moduleFilter($('input[name="module_filter[name]"]').val());
      });
      $('#edit-module-filter-show-required').change(function() {
        moduleFilter($('input[name="module_filter[name]"]').val());
      });
      $('#edit-module-filter-show-unavailable').change(function() {
        moduleFilter($('input[name="module_filter[name]"]').val());
      });
    }
  }

  moduleFilter = function(string) {
    stringLowerCase = string.toLowerCase();

    $("fieldset table tbody tr td label strong").each(function(i) {
      var $row = $(this).parents('tr');
      var module = $(this).text();
      var moduleLowerCase = module.toLowerCase();
      var $fieldset = $row.parents('fieldset');

      if (string != '') {
        if ($fieldset.hasClass('collapsed')) {
          $fieldset.removeClass('collapsed');
        }
      }

      if (moduleLowerCase.match(stringLowerCase)) {
        if (moduleFilterVisible($('td.checkbox input', $row))) {
          if (!$row.is(':visible')) {
            $row.show();
            if ($fieldset.hasClass('collapsed')) {
              $fieldset.removeClass('collapsed');
            }
            if (!$fieldset.is(':visible')) {
              $fieldset.show();
            }
          }
        }
        else {
          $row.hide();
          if ($row.siblings(':visible').html() == null) {
            $fieldset.hide();
          }
        }
      }
      else {
        if ($row.is(':visible')) {
          $row.hide();
          if ($row.siblings(':visible').html() == null) {
            $fieldset.hide();
          }
        }
      }
    });
  }

  moduleFilterVisible = function(checkbox) {
    if (checkbox.length > 0) {
      if ($('#edit-module-filter-show-enabled').is(':checked')) {
        if ($(checkbox).is(':checked') && !$(checkbox).is(':disabled')) {
          return true;
        }
      }
      if ($('#edit-module-filter-show-disabled').is(':checked')) {
        if (!$(checkbox).is(':checked') && !$(checkbox).is(':disabled')) {
          return true;
        }
      }
      if ($('#edit-module-filter-show-required').is(':checked')) {
        if ($(checkbox).is(':checked') && $(checkbox).is(':disabled')) {
          return true;
        }
      }
    }
    if ($('#edit-module-filter-show-unavailable').is(':checked')) {
      if (checkbox.length == 0 || ($(checkbox).size() > 0 && !$(checkbox).is(':checked') && $(checkbox).is(':disabled'))) {
        return true;
      }
    }
    return false;
  }
})(jQuery);
