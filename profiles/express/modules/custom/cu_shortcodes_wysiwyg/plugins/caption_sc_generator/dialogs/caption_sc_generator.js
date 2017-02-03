/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'caption_sc_generatorDialog', function( editor ) {
  
    
	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'caption Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'Caption Settings',

				// The tab contents.
				elements: [
				  
					{
						// Text input for caption title.
						type: 'textarea',
						id: 'caption-text',
						label: 'Image Caption',
					},
										{
						// Select input for caption style.
						type: 'select',
						id: 'caption-align',
						label: 'Align/Float',
						items: [ [ 'none', 'none' ], [ 'right', 'right' ], [ 'left', 'left' ] ],
    'default': 'none',
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
			var captionText = dialog.getValueOf( 'tab-settings', 'caption-text' );
			var captionAlign = dialog.getValueOf( 'tab-settings', 'caption-align' );
			
			 
			editor.insertHtml( '[imagecaption align="' + captionAlign + '"]<p>' + CKEDITOR.instances[this.getParentEditor().name].getSelection().getSelectedElement().$.outerHTML  +'</p><p>' + captionText + '</p>[/imagecaption]');
		}
	};
});