
(function($) {

/**
 * Add behaviors to submit buttons with the 'files-undo-remove' class.
 */
Drupal.behaviors.filesUndoRemove = {
  attach: function (context) {

    // Check existing states.
    $('.files-undo-remove-hidden-state').once().each(function() {
      var $state = $(this);
      if ($state.val() == 1) {
        var $table_row = $state.parents('tr').first();
        var $button = $table_row.find('.files-undo-remove');
        Drupal.filesUndoRemoveSetState($state.val(0), $button, $table_row);
      }
    });

    // Attach click behavior on the Remove buttons.
    $('.files-undo-remove').once().click(function(e) {

      // Do not submit.
      e.preventDefault();

      // Prepare button and row variable.
      var $button = $(this);
      var $table_row = $button.parents('tr').first();

      // Get the current state.
      var $state = $table_row.find('.files-undo-remove-hidden-state');

      // Based on the state, change the button text and make the table row
      // of this file look like it's disabled or enabled and change
      // the state value.
      Drupal.filesUndoRemoveSetState($state, $button, $table_row);
    });
  }
};

/**
 * State function for a file row.
 */
Drupal.filesUndoRemoveSetState = function(state, button, table_row) {
  if (state.val() == 1) {
    state.val(0);
    button.val(Drupal.t('Remove'));
    table_row.removeClass('warning');
    $('.files-undo-remove-message', table_row).remove();
  }
  else {
    state.val(1);
    table_row.addClass('warning');
    button.val(Drupal.t('Undo'));
    $('.file-size', table_row).append('<div class="files-undo-remove-message">'+ Drupal.t('This file will be removed when saving the form.') +'</div>');
  }
};

})(jQuery);
