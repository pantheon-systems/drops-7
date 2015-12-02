/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'expand_sc_generatorDialog', function( editor ) {
  
    
	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Expand Content Shortcode Generator',
		minWidth: 400,
		minHeight: 300,

		// Dialog window contents definition.
		contents: [
 			{
				// Definition of the Basic Settings dialog tab (page).
				id: 'tab-settings',
				label: 'Expand Content Settings',

				// The tab contents.
				elements: [
				  
					{
						// Text input for expand title.
						type: 'text',
						id: 'expand-title',
						label: 'Title',
						validate: CKEDITOR.dialog.validate.notEmpty( "Expand Content Title cannot be empty" )
					},
					{
					// Text input for expand text.
						type: 'textarea',
						id: 'expand-text',
						label: 'Expand Content Text',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "Expand Content Text field cannot be empty" )
					},
					{
						// Select input for expand style.
						type: 'select',
						id: 'expand-style',
						label: 'Expand Content Style',
						items: [ [ 'Regular', 'regular' ], [ 'Small', 'small' ], [ 'ToolTip', 'tooltip' ] ],
    'default': 'regular',
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
			var expandTitle = dialog.getValueOf( 'tab-settings', 'expand-title' );
			var expandContent = dialog.getValueOf( 'tab-settings', 'expand-text' );
			var expandStyle = dialog.getValueOf( 'tab-settings', 'expand-style' );
			 
			 
			editor.insertHtml( '[expand title="' + expandTitle + '" style="' + expandStyle + '"]' + expandContent + '[/expand]');
		}
	};
});