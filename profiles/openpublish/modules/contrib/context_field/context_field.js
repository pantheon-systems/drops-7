(function ($) {
  Drupal.behaviors.context_field = { attach: function(context) {
    $('#context_field-context-ui').dialog({
      width: 350,
      height: 450,
      //position: 'left',
      position: [0, 75],
      zIndex: 0,
      draggable: false,
      resizable: false,
      title: 'Context Editor',
      hide: 'slide',
      open: function () {
        $('#context_field-context-ui').show();
        $('#context_field-context-ui').parent().css('position', 'fixed');

        $('div.ui-dialog-titlebar').click(function () {
          $('#context_field-context-ui').toggle(400);
        });

        $(".context-block-addable").mousedown(function () {
          $('#context_field-context-ui').attr('location', $('#context_field-context-ui').parent().css('left'));
          $('#context_field-context-ui').parent().animate({'left':-300}, 1000);
          $('body').one('mouseup',function () {
            $('#context_field-context-ui').parent().animate({'left':$('#context_field-context-ui').attr('location')}, 1000);
          });
        });
      },
      close: function () {
        window.location.href = $('.tabs.primary li a').first().attr('href');
      }
    });

    // Hide the context editor if we're editing or adding a box
    if ($('#boxes-box-form').length || $('.boxes-box-editing').length) {
      $('#context_field-context-ui').hide();
      $('#context-ui-editor .links a.done').click();
    }
    else {
      $('#context_field-context-ui').show();
    }
    
    // Trigger Edit mode (init)
    $('#context-ui-editor .links a.edit').first().click();
    
    // Conceal Section title, subtitle and class
    $('div.context-block-browser').nextAll('.form-item').hide();

    // Add a class to body
    $('body').once().addClass('context-field-editor');
  }
  };
})(jQuery);
