(function ($) {
  $(document).ready(function(){
    $('.search-options1 input').change(function(){
      var label = $(this).attr('data-placeholder');
      var action = $(this).attr('data-action');
      $('.cu-search .form-text').attr('placeholder', label)
      $('#search form, #mobile-search form').attr('action', action);
    });
  });
}(jQuery));
