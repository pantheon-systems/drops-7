/**
 * The abbr dialog definition.
 *
 * Created out of the CKEditor Plugin SDK:
 * http://docs.ckeditor.com/#!/guide/plugin_sdk_sample_1
 */

// Our dialog definition.
CKEDITOR.dialog.add( 'soundcloud_embedDialog', function( editor ) {


	return {

		// Basic properties of the dialog window: title, minimum size.
		title: 'Soundcloud Shortcode Generator',
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
						type: 'textarea',
						id: 'soundcloud-embed',
						label: 'Soundcloud Embed Code',
						// Validation checking whether the field is not empty.
						validate: CKEDITOR.dialog.validate.notEmpty( "Soundcloud Embed Code field cannot be empty" )
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
			var embedCode = dialog.getValueOf( 'tab-settings', 'soundcloud-embed' );

      embedCode = embedCode.replace('</iframe>', '[/soundcloud]');
      embedCode = embedCode.replace('iframe', 'soundcloud');
      embedCode = embedCode.replace('<', '[');
      embedCode = embedCode.replace('>', ']');

			editor.insertHtml(embedCode);
		}
	};
});
