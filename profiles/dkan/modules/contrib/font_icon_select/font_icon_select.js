/**
 * @file
 * Javascript for font_icon_select.
 *
 * Provides js that runs admin selection functionality in the black/whitelist.
 * Applied to the global form and field specific form.
 */

Drupal.behaviors.font_icon_select = {
  attach: function (context, settings) {
    jQuery('.field-type-font-icon-select-icon', context).once('bind_font_icon_select_handlers', font_icon_select_options_behavior_each);
  }
}

function font_icon_select_options_behavior_each(){
  // Defaults cardinality to 0 if it can't find the correct field name.
  var propt,
      cardinality = 0;

  // Get the current field name from the structure.
  for (propt in Drupal.settings.font_icon_select) {
    // If this is a property of the font_icon_select object check if
    // the this frame includes the named class.
    if (Drupal.settings.font_icon_select.hasOwnProperty(propt) && jQuery(this).hasClass('field-name-' + propt.replace(/_/g, '-'))) {
      cardinality = Drupal.settings.font_icon_select[propt].cardinality;
    }
  }

  // Check if the field is already full and should be disabled.
  if (cardinality > 1 && jQuery('.selectionInner.checked', this).length == cardinality) {
    disable_unchecked(this)
  }

  // Bind the click event.
  jQuery(this).delegate('label', 'click', {cardinality : cardinality}, default_options_onclick);
}

/**
 * Disables unchecked options once cardinality is reached.
 */
function disable_unchecked(parent){
  // Switched from parents('label') to parent().parent() because of
  // a noticeable speed increase.
  jQuery('div.selectionInner:not(.checked)', parent).parent().parent().siblings('input').attr('disabled', 'disabled');
  jQuery('div.selectionInner:not(.checked)', parent).addClass('disabled');
  return true;
}

/**
 * Re-enables unchecked options after cardinality is no longer full.
 */
function enable_unchecked(parent){
  jQuery('input.font_icon_select_options', parent).removeAttr('disabled');
  jQuery('.selectionInner', parent).removeClass('disabled');
  return true;
}

/**
 * Onclick handler for defualt option selection.
 *
 * Ensures that cardinality is observed.
 * Drives classes that show coloration of (un)selected options.
 *
 * @see disable_unchecked()
 * @see enable_unchecked()
 */
function default_options_onclick(e){
  var cardinality = e.data.cardinality,
      outer_parent = jQuery(e.currentTarget).parents('.field-widget-font-icon-select-icon-widget');

  if (jQuery('.selectionInner', e.currentTarget).hasClass('disabled')) {
    return false;
  }

  if (cardinality == 1) {
    // If this is the selected item remove the checked class and return, no action needed.
    if (jQuery('.checked', e.currentTarget).length) {
      jQuery('.selectionInner', e.currentTarget).removeClass('checked');
      // Enable any disabled checkboxes.
      if (jQuery('.disabled', outer_parent).length) {
        if (jQuery('.checked', outer_parent).length <= 1) {
          enable_unchecked(jQuery('.font_icon_select_instance_options', outer_parent));
        }
        else {
          disable_unchecked(jQuery('.font_icon_select_instance_options', outer_parent));
        }
      }
      return;
    }

    // Enable any disabled checkboxes.
    if (jQuery('.disabled', outer_parent).length) {
       enable_unchecked(jQuery('.font_icon_select_instance_options', outer_parent));
    }

    // Uncheck all of the other options in this field as this setting needs
    // to behave like a set of radio buttons.
    jQuery('.font_icon_select_instance_options div.selectionInner.checked', outer_parent).each(function remove_checked_anon(){
      jQuery(this).parent().parent().siblings('.form-item').children('input').attr('checked', false);
    });

    // Uncolor the recently unchecked options.
    jQuery('.font_icon_select_instance_options div.selectionInner', outer_parent).removeClass('checked');

    // Check the selected option.
    jQuery('div.selectionInner', e.currentTarget).addClass('checked');

    return true;
  }

  if (jQuery('div.checked', e.currentTarget).length === 1) {
    jQuery('div.selectionInner', e.currentTarget).removeClass('checked');
    /*
     * It is possible for the cardinality to be lower than the number
     * of selected options.
     * This can happen in the field instance settings if the cardinality is
     * reduced without first reducing the selected defaults.
     */
    if (cardinality == 0 || cardinality > jQuery('.font_icon_select_instance_options div.selectionInner.checked', outer_parent).length) {
      return enable_unchecked(jQuery('.font_icon_select_instance_options', outer_parent));
    }
    // If we have too many checked still we need to disable the item
    // that was just unchecked.
    else if (cardinality <= jQuery('.font_icon_select_instance_options div.selectionInner.checked', outer_parent).length) {
      return disable_unchecked(jQuery('.font_icon_select_instance_options', outer_parent));
    }
  }
  else {
    jQuery('div.selectionInner', e.currentTarget).addClass('checked');
    if (cardinality > 1 && cardinality == jQuery('.font_icon_select_instance_options div.selectionInner.checked', outer_parent).length) {
      return disable_unchecked(jQuery('.font_icon_select_instance_options', outer_parent));
    }
    return true;
  }
}
