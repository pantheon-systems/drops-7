(function ($) {
  // innerLabel
  $.fn.innerLabel = function( default_text ){
    var element = this;
    // prevent non-existant elements from being processed
    if( $(element).length <= 0 ) {
      return;
    }

      // If element val() isset, use that as default text.
      var text = (element.find('input[type=text]').val()) ? element.find('input[type=text]').val() : default_text;

      // Set the default text.
      //var text = (default_text) ? default_text : element.find('label').text().trim();

      // add a class to be able to target processed form items
      $(element).addClass('inner-label-processed');

      // set the default value for the input text
      $(element).find('input[type=text]')[0].defaultValue = text;

      // Add focus/blur event handlers

      $(element)
        .find('input[type=text]')
        .val(text)
        .focus( function() {
          // when user clicks, remove the default text
          if ($(this).val() == $(this)[0].defaultValue) {
            $(this).val('');
            $(this).addClass('focus');
          }
        })
        .blur( function() {
          // when user moves away from text,
          // replace the text with default if input is empty
          if ($(this).val().trim() == '') {
            $(this).val($(this)[0].defaultValue);
            $(this).removeClass('focus');
          }
       });
  };
  $(window).load(function(){
    // Put innerLabel on to search field in the header.
    $('#block-google-appliance-ga-block-search-form form .form-item-search-keys').innerLabel('Search this site');
  });
}(jQuery));
