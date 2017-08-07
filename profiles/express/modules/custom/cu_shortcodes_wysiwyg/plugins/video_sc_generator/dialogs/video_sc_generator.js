/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'video_sc_generatorDialog', function( editor ) {
  
    
	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Video Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'video Settings',

				// The tab contents.
				elements: [

					{
					// Text input for video url.
						type: 'text',
						id: 'video-url',
						label: 'video URL',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "video URL field cannot be empty" )
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
			var videoURL = dialog.getValueOf( 'tab-settings', 'video-url' );
			
			 
			 
			editor.insertHtml( '[video:' + videoURL + ']');
		}
	};
});