(function($) {
 CKEDITOR.plugins.add('map_sc_generator', {
  init: function( editor )
  {
   
   
   editor.addCommand( 'map_generator_command', new CKEDITOR.dialogCommand( 'map_sc_generatorDialog' ) );
   
   editor.ui.addButton( 'map_sc_generator_button', {
    label: 'Map Shortcode Generator', //this is the tooltip text for the button
    command: 'map_generator_command',
    icon: this.path + 'images/map_sc_generator_button.gif'
   });
   CKEDITOR.dialog.add( 'map_sc_generatorDialog', this.path + 'dialogs/map_sc_generator.js' );
  }
 });
 
})(jQuery);