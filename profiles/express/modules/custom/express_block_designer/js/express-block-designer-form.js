(function ($) {
  $(document).ready(function(){
    var search = '<label for"icon-search-field" class="element-invisible">Icon Search</label><div class="icon-search"><input type="search" name="icon-search-field" id="icon-search-field" class="icon-search-field form-text" size="40" placeholder="Search by Icon Name"/></div><div class="icon-search-results element-invisible1"></div>';
    $('#edit-icon #edit-exbd-icon').before(search);

    $('.icon-search-field').on('input', function(event){
      var query = $('.icon-search-field').val().toLowerCase();
      var count = 0;

      if (query == '') {
        $('#edit-exbd-icon .form-item-exbd-icon').removeClass('icon-search-selected').removeClass('icon-search-hidden');
        count = $('.form-item-exbd-icon').length;
        $('.icon-search-results').text('');
      }
      else {
        $('#edit-exbd-icon .form-item-exbd-icon').removeClass('icon-search-selected');

        $('#edit-exbd-icon .form-item-exbd-icon').addClass('icon-search-hidden');

        $(".form-item-exbd-icon input[value*='" + query + "']").parent().addClass('icon-search-selected');

        count = $('.form-item-exbd-icon.icon-search-selected').length;
        $('.icon-search-results').text(count + ' icons found.');
      }

    });

  });
}(jQuery));
