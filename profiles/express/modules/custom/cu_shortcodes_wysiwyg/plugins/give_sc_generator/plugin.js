(function($) {
 CKEDITOR.plugins.add('give_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'give_generator_command', new CKEDITOR.dialogCommand( 'give_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'give_sc_generator_button', {
    label: 'Give Button Shortcode Generator', //this is the tooltip text for the button
    command: 'give_generator_command',
    icon: this.path + 'images/give_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'give_sc_generatorDialog', this.path + 'dialogs/give_sc_generator.js' );
  }
 });
 
})(jQuery);