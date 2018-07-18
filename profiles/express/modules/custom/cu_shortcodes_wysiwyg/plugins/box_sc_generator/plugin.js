(function($) {
 CKEDITOR.plugins.add('box_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'box_generator_command', new CKEDITOR.dialogCommand( 'box_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'box_sc_generator_button', {
    label: 'Box Shortcode Generator', //this is the tooltip text for the button
    command: 'box_generator_command',
    icon: this.path + 'images/box_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'box_sc_generatorDialog', this.path + 'dialogs/box_sc_generator.js' );
  }
 });
 
})(jQuery);