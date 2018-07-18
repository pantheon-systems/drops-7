(function ($) {
  $(document).ready(function(){
    $('.search-options input').change(function(){
      var label = $(this).attr('data-placeholder');
      var action = $(this).attr('data-action');
      $('.cu-search .form-text').attr('placeholder', label);
      $('.cu-search-box form').attr('action', action);
    });
  });
}(jQuery));
