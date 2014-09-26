
/**
 * @file
 * Add JavaScript behaviors for the "options" form element type.
 */

(function($) {

Drupal.optionElements = Drupal.optionElements || {};
Drupal.behaviors.optionsElement = Drupal.behaviors.optionsElement || {};

Drupal.behaviors.optionsElement.attach = function(context) {
  $('div.form-options:not(.options-element-processed)', context).each(function() {
    $(this).addClass('options-element-processed');
    var optionsElement = new Drupal.optionsElement(this);
    Drupal.optionElements[optionsElement.identifier] = optionsElement;
  });
};

/**
 * Constructor for an options element.
 */
Drupal.optionsElement = function(element) {
  var self = this;

  // Find the original "manual" fields.
  this.element = element;
  this.manualElement = $(element).find('div.options').get(0);
  this.manualOptionsElement = $(element).find('textarea').get(0);
  this.manualDefaultValueElement = $(element).find('input.form-text').get(0);
  this.keyTypeToggle = $(element).find('input.key-type-toggle').get(0);
  this.multipleToggle = $(element).find('input.multiple-toggle').get(0);

  // Setup variables containing the current status of the widget.
  this.optgroups = $(element).is('.options-optgroups');
  this.multiple = $(element).is('.options-multiple');
  this.keyType = element.className.replace(/^.*?options-key-type-([a-z]+).*?$/, '$1');
  this.customKeys = Boolean(element.className.match(/options-key-custom/));
  this.identifier = this.manualOptionsElement.id + '-widget';
  // jQuery 1.6 API change: http://api.jquery.com/prop/
  this.enabled = $.fn.prop ? !$(this.manualOptionsElement).prop('readonly') : !$(this.manualOptionsElement).attr('readonly');
  this.defaultValuePattern = $(element).find('input.default-value-pattern').val();

  if (this.defaultValuePattern) {
    this.defaultValuePattern = new RegExp(this.defaultValuePattern);
  }

  // Warning messages.
  this.keyChangeWarning = Drupal.t('Custom keys have been specified in this list. Removing these custom keys may change way data is stored. Are you sure you wish to remove these custom keys?');

  // Setup new DOM elements containing the actual options widget.
  this.optionsElement = $('<div></div>').get(0); // Temporary DOM object.
  this.optionsToggleElement = $(Drupal.theme('optionsElementToggle')).get(0);
  this.optionAddElement = $(Drupal.theme('optionsElementAdd')).get(0);
  this.removeDefaultElement = $(Drupal.theme('optionsElementRemoveDefault')).get(0);

  // Add the options widget and toggle elements to the page.
  $(this.manualElement).css('display', 'none').before(this.optionsElement).after(this.optionsToggleElement).after(this.optionAddElement);
  if (this.manualDefaultValueElement) {
    $(this.manualElement).after(this.removeDefaultElement);
  }

  // Enable add item link.
  $(this.optionAddElement).find('a').click(function() {
    var newOption = self.addOption($('table tr:last', self.optionsElement).get(0));
    $(newOption).find('input[type=text]:visible:first').focus();
    return false;
  });

  // Enable the toggle action for manual entry of options.
  $(this.optionsToggleElement).find('a').click(function() {
    self.toggleMode();
    return false;
  });

  // Enable the remove default link.
  $(this.removeDefaultElement).find('a').click(function() {
    $(self.element).find('input.option-default').removeAttr('checked').trigger('change');
    return false;
  });

  // Add a handler for key type changes.
  if (this.keyTypeToggle) {
    $(this.keyTypeToggle).click(function() {
      var checked = $(this).attr('checked');
      // Before switching the key type, ensure we're not destroying user keys.
      if (!checked) {
        var options = self.optionsFromText();
        var confirm = false;
        if (self.keyType == 'associative') {
          for (var n = 0; n < options.length; n++) {
            if (options[n].key != options[n].value) {
              confirm = true;
              break;
            }
          }
        }
        if (confirm) {
          if (window.confirm(self.keyChangeWarning)) {
            self.setCustomKeys(false);
          }
        }
        else {
          self.setCustomKeys(false);
        }
      }
      else {
        self.setCustomKeys(true);
      }
    });
  }

  // Add a handler for multiple value changes.
  if (this.multipleToggle) {
    $(this.multipleToggle).click(function(){
      self.setMultiple($(this).attr('checked'));
    });
  }

  // Be sure to show the custom keys if we have any errors.
  if (Drupal.settings.optionsElement && Drupal.settings.optionsElement.errors) {
    this.customKeys = true;
  }

  // Update the options widget with the current state of the textarea.
  this.updateWidgetElements();

  // Highlight errors that may have occurred during Drupal validation.
  if (Drupal.settings.optionsElement && Drupal.settings.optionsElement.errors) {
    this.checkKeys(Drupal.settings.optionsElement.errors, 'error');
  }
}

/**
 * Update the widget element based on the current values of the manual elements.
 */
Drupal.optionsElement.prototype.updateWidgetElements = function() {
  var self = this;

  // Create a new options element and replace the existing one.
  var newElement = $(Drupal.theme('optionsElement', this)).get(0);
  if ($(this.optionsElement).css('display') == 'none') {
    $(newElement).css('display', 'none');
  }
  $(this.optionsElement).replaceWith(newElement);
  this.optionsElement = newElement;

  // Manually set up table drag for the created table.
  Drupal.settings.tableDrag = Drupal.settings.tableDrag || {};
  Drupal.settings.tableDrag[this.identifier] = {
    'option-depth': {
      0: {
        action: 'depth',
        hidden: false,
        limit: 0,
        relationship: 'self',
        source: 'option-depth',
        target: 'option-depth'
      }
    }
  };

  // Allow indentation of elements if optgroups are supported.
  if (this.optgroups) {
    Drupal.settings.tableDrag[this.identifier]['option-parent'] = {
      0: {
        action: 'match',
        hidden: false,
        limit: 1,
        relationship: 'parent',
        source: 'option-value',
        target: 'option-parent'
      }
    };
  }

  // Enable button for adding options.
  $('a.add', this.optionsElement).click(function() {
    var newOption = self.addOption($(this).parents('tr:first').get(0));
    $(newOption).find('input[type=text]:visible:first').focus();
    return false;
  });

  // Enable button for removing options.
  $('a.remove', this.optionsElement).click(function() {
    self.removeOption($(this).parents('tr:first').get(0));
    return false;
  });

  // Add the same update action to all textfields and radios.
  $('input', this.optionsElement).change(function() {
    self.updateOptionElements();
    self.updateManualElements();
  });

  // Add a delayed update to textfields.
  $('input.option-value', this.optionsElement).keyup(function(e) {
    self.pendingUpdate(e);
  });

  // Attach behaviors as normal to the new widget.
  Drupal.attachBehaviors(this.optionsElement);

  // Remove the "Show row weights" link
  $(".tabledrag-toggle-weight-wrapper").remove();

  // Add an onDrop action to the table drag instance.
  Drupal.tableDrag[this.identifier].onDrop = function() {
    // Update the checkbox/radio buttons for selecting default values.
    if (self.optgroups) {
      self.updateOptionElements();
    }
    // Update the options within the hidden text area.
    self.updateManualElements();
  };

  // Add an onIndent action to the table drag row instances.
  Drupal.tableDrag[this.identifier].row.prototype.onIndent = function() {
    if (this.indents) {
      $(this.element).addClass('indented');
    }
    else {
      $(this.element).removeClass('indented');
    }
  };

  // Update the default value and optgroups.
  this.updateOptionElements();
}

/**
 * Update the original form element based on the current state of the widget.
 */
Drupal.optionsElement.prototype.updateManualElements = function() {
  var options = {};

  // Build a list of current options.
  var previousOption = false;
  $(this.optionsElement).find('input.option-value').each(function() {
    var $row = $(this).is('tr') ? $(this) : $(this).parents('tr:first');
    var depth = $row.find('input.option-depth').val();
    if (depth == 1 && previousOption) {
      if (typeof(options[previousOption]) != 'object') {
        options[previousOption] = {};
      }
      options[previousOption][this.value] = this.value;
    }
    else {
      options[this.value] = this.value;
      previousOption = this.value;
    }
  });
  this.options = options;

  // Update the default value.
  var defaultValue = this.multiple ? [] : '';
  var multiple = this.multiple;
  $(this.optionsElement).find('input.option-default').each(function() {
    if (this.checked && this.value) {
      if (multiple) {
        defaultValue.push(this.value);
      }
      else {
        defaultValue = this.value;
      }
    }
  });
  this.defaultValue = defaultValue;

  // Update with the new text and trigger the change action on the field.
  this.optionsToText();

  if (this.manualDefaultValueElement) {
    // Don't wipe out custom pattern-matched default values.
    defaultValue = multiple ? defaultValue.join(', ') : defaultValue;
    if (defaultValue || !(this.defaultValuePattern && this.defaultValuePattern.test(this.manualDefaultValueElement.value))) {
      this.manualDefaultValueElement.value = defaultValue;
      $('.default-value-pattern-match', this.element).remove();
    }
  }

  $(this.manualOptionsElement).change();
}

/**
 * Several maintenance routines to update all rows of the options element.
 *
 * - Disable options for optgroups if indented.
 * - Disable add and delete links if indented.
 * - Match the default value radio button value to the key of the text element.
 */
Drupal.optionsElement.prototype.updateOptionElements = function() {
  var self = this;
  var previousRow = false;
  var previousElement = false;
  var $rows = $(this.optionsElement).find('tbody tr');

  $rows.each(function(index) {
    var optionValue = $(this).find('input.option-value').val();
    var optionKey = $(this).find('input.option-key').val();

    // Update the elements key if matching the key and value.
    if (self.keyType == 'associative') {
      $(this).find('input.option-key').val(optionValue);
    }

    // Match the default value checkbox/radio button to the option's key.
    $(this).find('input.option-default').val(optionKey ? optionKey : optionValue);

    // Hide the add/remove links the row if indented.
    var depth = $(this).find('input.option-depth').val();
    var defaultInput = $(this).find('input.option-default').get(0);

    if (depth == 1) {
      // Affect the parent row, adjusting properties for optgroup items.
      $(previousElement).attr('disabled', true).attr('checked', false);
      $(previousRow).addClass('optgroup').find('a.add, a.remove').css('display', 'none');
      $(this).find('a.add, a.remove').css('display', '');
      $(defaultInput).attr('disabled', false);

      // Hide the key column for the optgroup. It would be nice if hiding
      // columns worked in IE7, but for now this only works in IE8 and other
      // standards-compliant browsers.
      if (self.customKeys && (!$.browser.msie || $.browser.version >= 8)) {
        $(previousRow).find('td.option-key-cell').css('display', 'none');
        $(previousRow).find('td.option-value-cell').attr('colspan', 2);
      }
    }
    else {
      // Set properties for normal options that are not optgroups.
      $(defaultInput).attr('disabled', false);
      $(this).removeClass('optgroup').find('a.add, a.remove').css('display', '');

      // Hide the key column. See note above for compatibility concerns.
      if (self.customKeys && (!$.browser.msie || $.browser.version >= 8)) {
        $(this).find('td.option-key-cell').css('display', '');
        $(this).find('td.option-value-cell').attr('colspan', '');
      }
      previousElement = defaultInput;
      previousRow = this;
    }
  });

  // Do not allow the last item to be removed.
  if ($rows.size() == 1) {
    $rows.find('a.remove').css('display', 'none')
  }

  // Disable items if needed.
  if (this.enabled == false) {
    this.disable();
  }
}

/**
 * Add a new option below the current row.
 */
Drupal.optionsElement.prototype.addOption = function(currentOption) {
  var self = this;
  var windowHieght = $(document).height();
  var newOption = $(currentOption).clone()
    .find('input.option-key').val(self.keyType == 'numeric' ? self.nextNumericKey() : '').end()
    .find('input.option-value').val('').end()
    .find('input.option-default').attr('checked', false).end()
    .find('a.tabledrag-handle').remove().end()
    .removeClass('drag-previous')
    .insertAfter(currentOption)
    .get(0);

  // Scroll down to accomidate the new option.
  $(window).scrollTop($(window).scrollTop() + $(document).height() - windowHieght);

  // Make the new option draggable.
  Drupal.tableDrag[this.identifier].makeDraggable(newOption);

  // Enable button for adding options.
  $('a.add', newOption).click(function() {
    var newOption = self.addOption($(this).parents('tr:first').get(0));
    $(newOption).find('input[type=text]:visible:first').focus();
    return false;
  });

  // Enable buttons for removing options.
  $('a.remove', newOption).click(function() {
    self.removeOption(newOption);
    return false;
  });

  // Add the update action to all textfields and radios.
  $('input', newOption).change(function() {
    self.updateOptionElements();
    self.updateManualElements();
  });

  // Add a delayed update to textfields.
  $('input.option-value', newOption).keyup(function(e) {
    self.pendingUpdate(e);
  });

  this.updateOptionElements();
  this.updateManualElements();

  return newOption;
}

/**
 * Remove the current row.
 */
Drupal.optionsElement.prototype.removeOption = function(currentOption) {
  $(currentOption).remove();

  this.updateOptionElements();
  this.updateManualElements();
}

/**
 * Toggle link for switching between the JavaScript and manual entry.
 */
Drupal.optionsElement.prototype.toggleMode = function() {
  if ($(this.optionsElement).is(':visible')) {
    var height = $(this.optionsElement).height();
    $(this.optionsElement).css('display', 'none');
    $(this.optionAddElement).css('display', 'none');
    $(this.manualElement).css('display', '').find('textarea').height(height);
    $(this.optionsToggleElement).find('a').text(Drupal.t('Normal entry'));
  }
  else {
    this.updateWidgetElements();
    $(this.optionsElement).css('display', '');
    $(this.optionAddElement).css('display', '');
    $(this.manualElement).css('display', 'none');
    $(this.optionsToggleElement).find('a').text(Drupal.t('Manual entry'));
  }
}

/**
 * Enable the changing of options.
 */
Drupal.optionsElement.prototype.enable = function() {
  this.enabled = true;
  $(this.manualOptionsElement).attr('readonly', '');
  $(this.element).removeClass('options-disabled');

  $('a.add, a.remove, a.tabledrag-handle, div.form-option-add a', this.element).css('display', '');
  $('input.form-text', this.optionsElement).attr('disabled', '');
};

/**
 * Disable the changing of options.
 */
Drupal.optionsElement.prototype.disable = function() {
  this.enabled = false;
  $(this.manualOptionsElement).attr('readonly', true);
  $(this.element).addClass('options-disabled');

  $('a.add, a.remove, a.tabledrag-handle, div.form-option-add a', this.element).css('display', 'none');
  $('input.form-text', this.optionsElement).attr('disabled', 'disabled');
};

/**
 * Enable entering of custom key values.
 */
Drupal.optionsElement.prototype.setCustomKeys = function(enabled) {
  if (enabled) {
    $(this.element).addClass('options-key-custom');
  }
  else {
    $(this.element).removeClass('options-key-custom');
  }

  this.customKeys = enabled;
  // Rebuild the options widget.
  this.updateManualElements();
  this.updateWidgetElements();
}

/**
 * Change the current key type (associative, custom, numeric, none).
 */
Drupal.optionsElement.prototype.setKeyType = function(type) {
  $(this.element)
    .removeClass('options-key-type-' + this.keyType)
    .addClass('options-key-type-' + type);
  this.keyType = type;
  // Rebuild the options widget.
  this.updateManualElements();
  this.updateWidgetElements();
}

/**
 * Set the element's #multiple property. Boolean TRUE or FALSE.
 */
Drupal.optionsElement.prototype.setMultiple = function(multiple) {
  if (multiple) {
    $(this.element).addClass('options-multiple');
  }
  else {
    // Unselect all default options except the first.
    $(this.optionsElement).find('input.option-default:checked:not(:first)').attr('checked', false);
    this.updateManualElements();
    $(this.element).removeClass('options-multiple');
  }
  this.multiple = multiple;
  // Rebuild the options widget.
  this.updateWidgetElements();
};

/**
 * Highlight duplicate keys.
 */
Drupal.optionsElement.prototype.checkKeys = function(duplicateKeys, cssClass){
  $(this.optionsElement).find('input.option-key').each(function() {
    if (duplicateKeys[this.value]) {
      $(this).addClass(cssClass);
    }
  });
};

/**
 * Update a field after a delay.
 *
 * Similar to immediately changing a field, this field as pending changes that
 * will be updated after a delay. This includes textareas and textfields in
 * which updating continuously would be a strain the server and actually slow
 * down responsiveness.
 */
Drupal.optionsElement.prototype.pendingUpdate = function(e) {
  var self = this;

  // Only operate on "normal" keys, excluding special function keys.
  // http://protocolsofmatrix.blogspot.com/2007/09/javascript-keycode-reference-table-for.html
  if (!(
    e.keyCode >= 48 && e.keyCode <= 90 || // 0-9, A-Z.
    e.keyCode >= 93 && e.keyCode <= 111 || // Number pad.
    e.keyCode >= 186 && e.keyCode <= 222 || // Symbols.
    e.keyCode == 8) // Backspace.
    ) {
    return;
  }

  if (this.updateDelay) {
    clearTimeout(this.updateDelay);
  }

  this.updateDelay = setTimeout(function(){
    self.updateOptionElements();
    self.updateManualElements();
  }, 500);
};

/**
 * Given an object of options, convert it to a text string.
 */
Drupal.optionsElement.prototype.optionsToText = function() {
  var $rows = $('tbody tr', this.optionsElement);
  var output = '';
  var inGroup = false;
  var rowCount = $rows.size();
  var defaultValues = [];

  for (var rowIndex = 0; rowIndex < rowCount; rowIndex++) {
    var isOptgroup = $rows.eq(rowIndex).is('.optgroup');
    var isChild = $rows.eq(rowIndex).is('.indented');
    var key = $rows.eq(rowIndex).find('input.option-key').val();
    var value = $rows.eq(rowIndex).find('input.option-value').val();

    // Handle groups.
    if (this.optgroups && value !== '' && isOptgroup) {
      output += '<' + ((key !== '') ? (key + '|') : '') + value + '>' + "\n";
      inGroup = true;
    }
    // Typical key|value pairs.
    else {
      // Exit out of any groups.
      if (this.optgroups && inGroup && !isChild) {
        output += "<>\n";
        inGroup = false;
      }

      // Add the row for the option.
      if (this.keyType == 'none' || this.keyType == 'associative') {
        output += value + "\n";
      }
      else if (value == '') {
        output += "\n";
      }
      else {
        output += ((key !== '') ? (key + '|') : '') + value + "\n";
      }
    }
  }

  this.manualOptionsElement.value = output;
};

/**
 * Given a text string, convert it to an object.
 */
Drupal.optionsElement.prototype.optionsFromText = function() {
  // Use jQuery val() instead of value because it fixes Windows line breaks.
  var rows = $(this.manualOptionsElement).val().match(/^.*$/mg);
  var parent = '';
  var options = [];
  var defaultValues = {};

  // Drop the last row if empty.
  if (rows.length && rows[rows.length - 1] == '') {
    rows.pop();
  }

  if (this.manualDefaultValueElement) {
    if (this.multiple) {
      var defaults = this.manualDefaultValueElement.value.split(',');
      for (var n = 0; n < defaults.length; n++) {
        var defaultValue = defaults[n].replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
        defaultValues[defaultValue] = defaultValue;
      }
    }
    else {
      var defaultValue = this.manualDefaultValueElement.value.replace(/^[ ]*(.*?)[ ]*$/, '$1'); // trim().
      defaultValues[defaultValue] = defaultValue;
    }
  }

  for (var n = 0; n < rows.length; n++) {
    var row = rows[n].replace(/^[ \r\n]*(.*?)[ \r\n]*$/, '$1'); // trim().
    var key = '';
    var value = '';
    var checked = false;
    var hasChildren = false;
    var groupClear = false;

    var matches = {};
    // Row is a group.
    if (this.optgroups && (matches = row.match(/^\<((([^>|]*)\|)?([^>]*))\>$/))) {
      if (matches[0] == '<>') {
        parent = '';
        groupClear = true;
      }
      else {
        key = matches[3] ? matches[3] : '';
        parent = value = matches[4];
        hasChildren = true;
      }
    }
    // Check if this row is a key|value pair.
    else if ((this.keyType == 'mixed' || this.keyType == 'numeric' || this.keyType == 'custom') && (matches = row.match(/^([^|]+)\|(.*)$/))) {
      key = matches[1];
      value = matches[2];
      checked = defaultValues[key];
    }
    // Row is a straight value.
    else {
      key = (this.keyType == 'mixed' || this.keyType == 'numeric') ? '' : row;
      value = row;
      if (!key && this.keyType == 'mixed') {
        checked = defaultValues[value];
      }
      else {
        checked = defaultValues[key];
      }
    }

    if (!groupClear) {
      options.push({
        key: key,
        value: value,
        parent: (value !== parent ? parent : ''),
        hasChildren: hasChildren,
        checked: (checked ? 'checked' : false)
      });
    }
  }

  // Convert options to numeric if no key is specified.
  if (this.keyType == 'numeric') {
    var nextKey = this.nextNumericKey();
    for (var n = 0; n < options.length; n++) {
      if (options[n].key == '') {
        options[n].key = nextKey;
        nextKey++;
      }
    }
  }

  return options;
};

/**
 * Utility method to get the next numeric option in a list of options.
 */
Drupal.optionsElement.prototype.nextNumericKey = function(options) {
  this.keyType = 'custom';
  options = this.optionsFromText();
  this.keyType = 'numeric';

  var maxKey = -1;
  for (var n = 0; n < options.length; n++) {
    if (options[n].key.match(/^[0-9]+$/)) {
      maxKey = Math.max(maxKey, options[n].key);
    }
  }
  return maxKey + 1;
};

/**
 * Theme function for creating a new options element.
 *
 * @param optionsElement
 *   An options element object.
 */
Drupal.theme.prototype.optionsElement = function(optionsElement) {
  var output = '';
  var options = optionsElement.optionsFromText();
  var hasDefault = optionsElement.manualDefaultValueElement;
  var defaultType = optionsElement.multiple ? 'checkbox' : 'radio';
  var keyType = optionsElement.customKeys ? 'textfield' : 'hidden';

  // Helper function to print out a single draggable option row.
  function tableDragRow(key, value, parent, indent, status) {
    var output = '';
    output += '<tr class="draggable' + (indent > 0 ? ' indented' : '') + '">'
    output += '<td class="' + (hasDefault ? 'option-default-cell' : 'option-order-cell') + '">';
    for (var n = 0; n < indent; n++) {
      output += Drupal.theme('tableDragIndentation');
    }
    output += '<input type="hidden" class="option-parent" value="' + parent.replace(/"/g, '&quot;') + '" />';
    output += '<input type="hidden" class="option-depth" value="' + indent + '" />';
    if (hasDefault) {
      output += '<input type="' + defaultType + '" name="' + optionsElement.identifier + '-default" class="form-radio option-default" value="' + key.replace(/"/g, '&quot;') + '"' + (status == 'checked' ? ' checked="checked"' : '') + (status == 'disabled' ? ' disabled="disabled"' : '') + ' />';
    }
    output += '</td><td class="' + (keyType == 'textfield' ? 'option-key-cell' : 'option-value-cell') +'">';
    output += '<input type="' + keyType + '" class="' + (keyType == 'textfield' ? 'form-text ' : '') + 'option-key" value="' + key.replace(/"/g, '&quot;') + '" />';
    output += keyType == 'textfield' ? '</td><td class="option-value-cell">' : '';
    output += '<input class="form-text option-value" type="text" value="' + value.replace(/"/g, '&quot;') + '" />';
    output += '</td><td class="option-actions-cell">'
    output += '<a class="add" title="' + Drupal.t('Add new option') + '" href="#"' + (status == 'disabled' ? ' style="display: none"' : '') + '><span class="add">' + Drupal.t('Add') + '</span></a>';
    output += '<a class="remove" title="' + Drupal.t('Remove option') + '" href="#"' + (status == 'disabled' ? ' style="display: none"' : '') + '><span class="remove">' + Drupal.t('Remove') + '</span></a>';
    output += '</td>';
    output += '</tr>';
    return output;
  }

  output += '<div class="options-widget">';
  output += '<table id="' + optionsElement.identifier + '">';

  output += '<thead><tr>';
  output += '<th>' + (hasDefault ? Drupal.t('Default') : '&nbsp;') + '</th>';
  output += keyType == 'textfield' ? '<th>' + Drupal.t('Key') + '</th>' : '';
  output += '<th>' + Drupal.t('Value') + '</th>';
  output += '<th>&nbsp;</th>';
  output += '</tr></thead>';

  output += '<tbody>';

  // Make sure that at least a few options exist if empty.
  if (!options.length) {
    var newOption = {
      key: '',
      value: '',
      parent: '',
      hasChildren: false,
      checked: false
    }
    options.push(newOption);
    options.push(newOption);
    options.push(newOption);
  }

  for (var n = 0; n < options.length; n++) {
    var option = options[n];
    var depth = option.parent === '' ? 0 : 1;
    var checked = !option.hasChildren && option.checked;
    output += tableDragRow(option.key, option.value, option.parent, depth, checked);
  }

  output += '</tbody>';
  output += '</table>';

  if (optionsElement.defaultValuePattern && optionsElement.manualDefaultValueElement && optionsElement.defaultValuePattern.test(optionsElement.manualDefaultValueElement.value)) {
    output += Drupal.theme('optionsElementPatternMatch', optionsElement.manualDefaultValueElement.value);
  }

  output += '</div>';

  return output;
};

Drupal.theme.prototype.optionsElementPatternMatch = function(matchedValue) {
  return '<div class="default-value-pattern-match"><span>' + Drupal.t('Manual default value') + '</span>: ' + matchedValue + '</div>';
};

Drupal.theme.prototype.optionsElementAdd = function() {
  return '<div class="form-option-add"><a href="#">' + Drupal.t('Add item') + '</a></div>';
};

Drupal.theme.prototype.optionsElementRemoveDefault = function() {
  return '<div class="remove-default"><a href="#">' + Drupal.t('No default') + '</a></div>';
};

Drupal.theme.prototype.optionsElementToggle = function() {
  return '<div class="form-options-manual"><a href="#">' + Drupal.t('Manual entry') + '</a></div>';
};

Drupal.theme.tableDragChangedMarker = function () {
  return ' ';
};

Drupal.theme.tableDragChangedWarning = function() {
  return '<span></span>';
};

/**
 * Field module support for Options widgets.
 */
Drupal.behaviors.optionsElementFieldUI = {};
Drupal.behaviors.optionsElementFieldUI.attach = function(context) {
  var $cardinalityField = $(context).find('#edit-field-cardinality');
  if ($cardinalityField.length) {
    $cardinalityField.change(function() {
      var optionsElementId = $(this).parents('fieldset:first').find('.form-type-options table').attr('id');
      if (Drupal.optionElements[optionsElementId]) {
        Drupal.optionElements[optionsElementId].setMultiple(this.value != 1);
      }
    }).trigger('change');
  }
}

})(jQuery);
