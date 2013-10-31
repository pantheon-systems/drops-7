(function ($) {

/**
 * Webform node form interface enhancments.
 */

Drupal.behaviors.webformAdmin = {};
Drupal.behaviors.webformAdmin.attach = function(context) {
  // On click or change, make a parent radio button selected.
  Drupal.webform.setActive(context);
  Drupal.webform.updateTemplate(context);
  // Update the template select list upon changing a template.
  // Select all link for file extensions.
  Drupal.webform.selectCheckboxesLink(context);
  // Enhance the normal tableselect.js file to support indentations.
  Drupal.webform.tableSelectIndentation(context);
  // Automatically download exports if available.
  Drupal.webform.downloadExport(context);
  // Enhancements for the conditionals administrative page.
  Drupal.webform.conditionalAdmin(context);
}

Drupal.webform = Drupal.webform || {};

Drupal.webform.setActive = function(context) {
  var setActiveOnChange = function(e) {
    if ($(this).val()) {
      $(this).closest('.form-type-radio').find('input[type=radio]').attr('checked', true);
    }
    e.preventDefault();
  };
  var setActiveOnClick = function(e) {
    $(this).closest('.form-type-radio').find('input[type=radio]').attr('checked', true);
  };
  $('.webform-inline-radio', context).click(setActiveOnClick);
  $('.webform-set-active', context).change(setActiveOnChange);

  // Firefox improperly selects the parent radio button when clicking inside
  // a label that contains an input field. The only way of preventing this
  // currently is to remove the "for" attribute on the label.
  // See https://bugzilla.mozilla.org/show_bug.cgi?id=213519.
  if (navigator.userAgent.match(/Firefox/)) {
    $('.webform-inline-radio', context).removeAttr('for');
  }
};

Drupal.webform.updateTemplate = function(context) {
  var defaultTemplate = $('#edit-templates-default').val();
  var $templateSelect = $('#webform-template-fieldset select#edit-template-option', context);
  var $templateTextarea = $('#webform-template-fieldset textarea:visible', context);

  var updateTemplateSelect = function() {
    if ($(this).val() == defaultTemplate) {
      $templateSelect.val('default');
    }
    else {
      $templateSelect.val('custom');
    }
  }

  var updateTemplateText = function() {
    if ($(this).val() == 'default' && $templateTextarea.val() != defaultTemplate) {
      if (confirm(Drupal.settings.webform.revertConfirm)) {
        $templateTextarea.val(defaultTemplate);
      }
      else {
        $(this).val('custom');
      }
    }
  }

  $templateTextarea.keyup(updateTemplateSelect);
  $templateSelect.change(updateTemplateText);
}

Drupal.webform.selectCheckboxesLink = function(context) {
  function selectCheckboxes() {
    var group = this.className.replace(/.*?webform-select-link-([^ ]*).*/, '$1');
    var $checkboxes = $('.webform-select-group-' + group + ' input[type=checkbox]');
    var reverseCheck = !$checkboxes[0].checked;
    $checkboxes.each(function() {
      this.checked = reverseCheck;
    });
    $checkboxes.trigger('change');
    return false;
  }
  $('a.webform-select-link', context).click(selectCheckboxes);
}

Drupal.webform.tableSelectIndentation = function(context) {
  var $tables = $('th.select-all', context).parents('table');
  $tables.find('input.form-checkbox').change(function() {
    var $rows = $(this).parents('table:first').find('tr');
    var row = $(this).parents('tr:first').get(0);
    var rowNumber = $rows.index(row);
    var rowTotal = $rows.size();
    var indentLevel = $(row).find('div.indentation').size();
    for (var n = rowNumber + 1; n < rowTotal; n++) {
      if ($rows.eq(n).find('div.indentation').size() <= indentLevel) {
        break;
      }
      $rows.eq(n).find('input.form-checkbox').attr('checked', this.checked);
    }
  });
}

/**
 * Attach behaviors for Webform results download page.
 */
Drupal.webform.downloadExport = function(context) {
  if (context === document && Drupal.settings && Drupal.settings.webformExport && document.cookie.match(/webform_export_info=1/)) {
    window.location = Drupal.settings.webformExport;
    delete Drupal.settings.webformExport;
  }
}

/**
 * Attach behaviors for Webform conditional administration.
 */
Drupal.webform.conditionalAdmin = function(context) {
  $('.webform-conditional:not(.webform-conditional-processed)').each(function() {
    $(this).addClass('webform-conditional-processed');

    // Rather than binding to click, we have to use mousedown to work with
    // the AJAX handling, which disables the button and prevents "click" events.
    // This handler needs a delay to let the form submit before we remove the
    // table row.
    $(this).find('.webform-conditional-rule-remove').mousedown(function() {
      var button = this;
      window.setTimeout(Drupal.webform.conditionalRemove.apply(button), 10);
    });

    $(this).find('.webform-conditional-source select').each(function() {
      $(this).change(Drupal.webform.conditionalSourceChange).triggerHandler('change');
    });

    $(this).find('.webform-conditional-operator select').each(function() {
      $(this).change(Drupal.webform.conditionalOperatorChange).triggerHandler('change');
    });

    $(this).find('.webform-conditional-andor select').each(function() {
      $(this).change(Drupal.webform.conditionalAndOrChange).triggerHandler('change');
    });
  });
}

/**
 * Event callback for the remove button next to an individual rule.
 */
Drupal.webform.conditionalRemove = function() {
  // See if there are any remaining rules in this element.
  var ruleCount = $(this).parents('.webform-conditional:first').find('.webform-conditional-rule-remove').length;
  if (ruleCount <= 1) {
    var $tableRow = $(this).parents('tr:first');
    var $table = $('#webform-conditionals-table');
    if ($tableRow.length && $table.length) {
      $tableRow.remove();
      Drupal.webform.restripeTable($table[0]);
    }
  }
}

/**
 * Event callback to update the list of operators after a source change.
 */
Drupal.webform.conditionalSourceChange = function() {
  var source = $(this).val();
  var dataType = Drupal.settings.webform.conditionalValues.sources[source]['data_type'];
  var $operator = $(this).parents('.webform-conditional-rule:first').find('.webform-conditional-operator select');

  // Store a the original list of all operators for all data types in the select
  // list DOM element.
  if (!$operator[0]['webformConditionalOriginal']) {
    $operator[0]['webformConditionalOriginal'] = $operator[0].innerHTML;
  }

  // Reference the original list to create a new list matching the data type.
  var $originalList = $($operator[0]['webformConditionalOriginal']);
  var $newList = $originalList.filter('optgroup[label=' + dataType + ']');
  $operator.html($newList[0].innerHTML);

  // Fire the change event handler on the list to update the value field.
  $operator.triggerHandler('change');
}

/**
 * Event callback to update the list of operators after a source change.
 */
Drupal.webform.conditionalOperatorChange = function() {
  var source = $(this).parents('.webform-conditional-rule:first').find('.webform-conditional-source select').val();
  var dataType = Drupal.settings.webform.conditionalValues.sources[source]['data_type'];
  var operator = $(this).val();
  var $value = $(this).parents('.webform-conditional-rule:first').find('.webform-conditional-value');
  var name = $value.find('input, select, textarea').attr('name');
  var originalValue = false;

  // Given the dataType and operator, we can determine the form key.
  var formKey = Drupal.settings.webform.conditionalValues.operators[dataType][operator]['form'];

  // On initial request, save the default field as printed on the original page.
  if (!$value[0]['webformConditionalOriginal']) {
    $value[0]['webformConditionalOriginal'] = $value[0].innerHTML;
    originalValue = $value.find('input:first').val();
  }

  // If using the default (a textfield), restore the original field.
  if (formKey === 'default') {
    $value[0].innerHTML = $value[0]['webformConditionalOriginal'];
  }
  // If the operator does not need a source value (i.e. is empty), hide it.
  else if (formKey === false) {
    $value[0].innerHTML = '&nbsp;';
  }
  // Lastly check if there is a specialized form for this source and operator.
  else {
    // If there is a per-source form for this operator (e.g. option lists), use
    // the specialized value form.
    if (typeof Drupal.settings.webform.conditionalValues.forms[formKey] == 'object') {
      $value[0].innerHTML = Drupal.settings.webform.conditionalValues.forms[formKey][source];
    }
    // Otherwise all the sources use a generic field (e.g. a text field).
    else {
      $value[0].innerHTML = Drupal.settings.webform.conditionalValues.forms[formKey];
    }
  }

  // Set the name attribute to match the original placeholder field.
  var $firstElement = $value.find('input, select, textarea').filter(':first');
  $firstElement.attr('name', name);

  if (originalValue) {
    $firstElement.val(originalValue);
  }
}

/**
 * Event callback to make sure all group and/or operators match.
 */
Drupal.webform.conditionalAndOrChange = function() {
  $(this).parents('.webform-conditional:first').find('.webform-conditional-andor select').val(this.value);
}

/**
 * Given a table's DOM element, restripe the odd/even classes.
 */
Drupal.webform.restripeTable = function(table) {
  // :even and :odd are reversed because jQuery counts from 0 and
  // we count from 1, so we're out of sync.
  // Match immediate children of the parent element to allow nesting.
  $('> tbody > tr, > tr', table)
    .filter(':odd').filter('.odd')
      .removeClass('odd').addClass('even')
    .end().end()
    .filter(':even').filter('.even')
      .removeClass('even').addClass('odd');
};
})(jQuery);
