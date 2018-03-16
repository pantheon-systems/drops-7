/**
 * @file
 * Javascript for font_icon_select field instance settings form.
 *
 * Builds off global functionality provided in font_icon_select.js.
 *
 * @see font_icon_select.js
 */

/**
 * Bind click and change events.
 */
jQuery(document).ready(function(){
  if (jQuery('#edit-instance-settings-blacklist-fieldset-blacklist').length) {
    var list_container = jQuery('#edit-instance-settings-blacklist-fieldset-suppress')

    // Fire the update to hide the black/whitelisted items.
    update_defaults_helper(false, list_container);
  
    // Fires when the black/whitelist toggle changes.
    jQuery('#edit-instance-settings-blacklist-fieldset-blacklist input').bind('change', update_list_type)

    jQuery('#edit-instance-settings-blacklist-fieldset-suppress label').bind('click', {container: list_container}, update_defaults_helper);
  
    // Watch to see if the cardinality changes.
    jQuery('#edit-field-cardinality').bind('change', field_cardinality_onchange);
  }

  // Black/whitelist settings.
  jQuery('div.icon_option_list_selection').delegate('label', 'click', black_white_options_onclick);
});

/**
 * Updates the type of list if the toggle has been changed. Fires update_defaults_helper.
 */
function update_list_type(e) {
  var blacklist;
  blacklist = jQuery(e.currentTarget).attr('id') == 'edit-instance-settings-blacklist-fieldset-blacklist-1';
  if (jQuery(e.currentTarget).attr('id') == 'edit-instance-settings-blacklist-fieldset-blacklist-1') {
    jQuery('.font_icon_select_options').removeClass('whitelist').addClass('blacklist')
  }
  else {
    jQuery('.font_icon_select_options').removeClass('blacklist').addClass('whitelist')
  }
}

/**
 * Onchange handler for cardinality selection.
 *
 * Updates default options selection enabled/disabled swap.
 */
function field_cardinality_onchange(e){
  var cardinality = jQuery('#edit-field-cardinality').val() != -1 ? jQuery('#edit-field-cardinality').val() : 0;
	if (cardinality == Drupal.settings.font_icon_select.cardinality) {
    return;
  }

  if (cardinality > 1 && jQuery('#edit-field-cardinality').val() <= jQuery('.font_icon_select_instance_options div.selectionInner.checked').length) {
    disable_unchecked(jQuery('.font_icon_select_instance_options'));
  }
  else if (cardinality == 0 || cardinality > jQuery('.font_icon_select_instance_options div.selectionInner.checked').length) {
    enable_unchecked(jQuery('.font_icon_select_instance_options'));
  }
  Drupal.settings.font_icon_select.field_icon_select.cardinality = cardinality;

  // Undelegate the default options onclick so that it can be rebound with the correct cardinality.
  jQuery('.field-type-font-icon-select-icon').undelegate('label', 'click', default_options_onclick);

  font_icon_select_options_behavior_each.apply(jQuery('.field-type-font-icon-select-icon'));
}

/**
 * Updates the available defaults after the black/white list has changed.
 *
 * @arg object event.
 *   Undefined if not passed from an event. event.data will contain a container attribute.
 * @arg object container.
 *   The container being updated. Unused if event is used.
 */
function update_defaults_helper(e, container){
  var currentTarget, rangeItems, addClass;

  // We have 3 options, update everything (onload or black/white swap), update many things (shift click), or update one.
  // Test everything!
  if (typeof e == "undefined" || e == false) {
    jQuery('.font_icon_selection_outer_wrapper', container).each(function update_defaults_helper_full_each(index, element) {
      update_defaults(jQuery('input', element).val(), jQuery('input:checked', element).length);
    });
    return;
  }
  currentTarget = jQuery(e.currentTarget).parent();
  
  // Multiple here. This takes care of everything in the shift click range, not including the
  // current item! Don't return here, allow the final call to fire.
  if (jQuery('.lastSelected', e.data.container).length && e.shiftKey) {
    rangeItems = get_shift_click_range(currentTarget, jQuery('.lastSelected', e.data.container))
    addClass = jQuery('div.selectionInner', jQuery('.lastSelected', e.data.container)).hasClass('checked')
    
    rangeItems.each(function range_items_each(index, element) {
      black_white_option_onclick(element, addClass, true)
      update_defaults(jQuery('input', element).val(), jQuery('input:checked', element).length);
    });
  }
  
  black_white_option_onclick(currentTarget, !jQuery('div.selectionInner', currentTarget).hasClass('checked'))
  update_defaults(jQuery('input', currentTarget).val(), !jQuery('input:checked', currentTarget).length);

  // Reset the 'current' selected item.
  jQuery('.font_icon_selection_outer_wrapper', container).removeClass('lastSelected');
  jQuery(currentTarget).addClass('lastSelected');
  return;
}

/**
 * Update the default value options to reflect the changed black/whitelist.
 *
 * @arg string value
 *   The value of the toggled input field.
 * @arg boolean checked
 *   Whether the toggled input is off or on.
 */
function update_defaults(value, checked) {
  var defaults_field, input, input_parent, blacklist;

  defaults_field = jQuery('#edit-field-icon-select-und');
  input = jQuery('input[value="' + value + '"]', defaults_field);
  input_parent = input.parent().parent();
  blacklist = jQuery(defaults_field).hasClass('blacklist');

  if (checked) {
    jQuery(input_parent).addClass('suppression_list_toggled');
    // Uncheck the element as it is hidden.
    if (blacklist && jQuery(input).is(':checked')) {
      jQuery('label', input_parent).click();
    }
  }
  else {
    jQuery(input_parent).removeClass('suppression_list_toggled');
    // Uncheck the element as it is hidden.
    if (!blacklist && jQuery(input).is(':checked')) {
      jQuery('label', input_parent).click();
    }
  }
}

/**
 * Onclick handler for the black/whitelist selections.
 *
 * Updates available default options.
 * Unchecks currently checked option if it becomes blacklisted.
 */
function black_white_options_onclick(e){
  var container = jQuery('div.icon_option_list_selection'),
      current = jQuery(e.target).parents('.font_icon_selection_outer_wrapper'),
      previous = jQuery('.lastSelected', container),
      addClass = (previous.length && e.shiftKey ? jQuery('div.selectionInner', previous).hasClass('checked') : !jQuery('div.selectionInner', current).hasClass('checked')),
      rangeItems = [];

  if (e.shiftKey && previous.length) {
    rangeItems = get_shift_click_range(current, previous);
    rangeItems.each(function range_items_each(index, element){
      black_white_option_onclick(element, addClass, true)
    });
  }

  black_white_option_onclick(current, addClass)

  // Reset the 'current' selected item.
  jQuery('.font_icon_selection_outer_wrapper', container).removeClass('lastSelected');
  jQuery(current).addClass('lastSelected');

  /*
   * Trigger an event here in case we are in the instance settings.
   * Instance settings js will catch the click triggered event and
   * update defaults.
   */
  jQuery('div.icon_option_list_selection label').triggerHandler('black_white_option_clicked');
}

/**
 * Toggles checked class and checked status if toggle flag is set.
 *
 * @arg object element.
 *   The DOM object to update.
 * @arg boolean addClass.
 *   Flag that determines if an element's class should be turned on or off.
 * @arg boolean toggle.
 *   Flag that determines if an element's checkbox should be toggled. Used during shift click opps.
 */
function black_white_option_onclick(element, addClass, toggle) {
  toggle = typeof toggle == "undefined" ? false : true;

  if (addClass) {
    jQuery('div.selectionInner', element).addClass('checked');
    if (toggle) {
      jQuery('input', element).attr('checked', true);
    }
    return;
  }
  
  jQuery('div.selectionInner', element).removeClass('checked');
  if (toggle) {
    jQuery('input', element).attr('checked', false);
  }
}

/**
 * returns all elements between the element just clicked and the one previously clicked.
 */ 
function get_shift_click_range(current, previous) {
  var rangeItems = [];

  if (previous[0] == current[0]) {
    return rangeItems;
  }

  if (current.nextAll('.lastSelected').length > 0) {
    rangeItems = current.nextUntil('.lastSelected');
  }
  else {
    // Need the class for nextUntil, dom object doesn't work until 1.6.
    current.addClass('current');
    rangeItems = previous.nextUntil('.current');
    current.removeClass('current');
  }
  
  return rangeItems;
}