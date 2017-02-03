/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'map_sc_generatorDialog', function( editor ) {
  
    
	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Map Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'Map Settings',

				// The tab contents.
				elements: [

					{
					// Text input for map url.
						type: 'textarea',
						id: 'map-url',
						label: 'Map Embed Code',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "Map URL field cannot be empty" )
					},					
					
					
				]
			},

		],

		// This method is invoked once a user clicks the OK button, confirming the dialog.
		onOk: function() {

			// The context of this function is the dialog object itself.
			// http://docs.ckeditor.com/#!/api/CKEDITOR.dialog
			var dialog = this;

			// Get Fields	
			var mapURL = dialog.getValueOf( 'tab-settings', 'map-url' );
			var re1='.*?';	// Non-greedy match on filler
      var re2='((?:http|https)(?::\\/{2}[\\w]+)(?:[\\/|\\.]?)(?:[^\\s"]*))';	// HTTP URL 1

      var p = new RegExp(re1+re2,["i"]);
      var m = p.exec(mapURL);
      var httpurl1=m[1];			 
			editor.insertHtml( '[map]' + httpurl1 + '[/map]');
		}
	};
});