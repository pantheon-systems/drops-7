(function ($) {
  $(document).ready(function(){
    $(".bean-collection-grid .expand-trigger").each(function(i){
      //alert(i);
      $(this).addClass('xxx');
      var trigger = $(this).attr('href');
      var trigger2 = trigger + '-' + i;
      var target = trigger2.substring(1);


      $(this).attr('href', trigger2);
      $(this).next('.expand-content').attr('id', target);
    });
    $(".collection-items-categories").hide();

    $("ul.collection-items-navigation a").first().addClass('active');
    $("ul.collection-items-navigation a").click(function(){
      // Get the collection to operate on
      var collectionTarget = $(this).attr("data-collection");
      // Remove disabled class, aria from all items in collection
      $("#" + collectionTarget + " .collection-item").removeClass('collection-item-disabled').removeAttr('aria-hidden').removeAttr('role');
      // Get the collection category
      var target = $(this).attr("data-collection-category");
      // Apply disabled class, aria to all items not in category
      $('#' + collectionTarget + ' .collection-item').not('.collection-category-' + target).addClass('collection-item-disabled').attr('aria-hidden', 'true').attr('role', 'presentation');
      // Remove active class from category links
      $("ul.collection-items-navigation a").removeClass('active');
      // Apply active class to the clicked link
      $(this).addClass('active');
      return false;
    });
    // Collection ALL link
    $("ul.collection-items-navigation a.collection-category-all").click(function(){
      // Get the collection to operate on
      var collectionTarget = $(this).attr("data-collection");
      // Remove disabled class, aria from all items in collection
      $("#" + collectionTarget + " .collection-item").removeClass('collection-item-disabled').removeAttr('aria-hidden').removeAttr('role');
      // Remove active class from category links
      $("ul.collection-items-navigation a").removeClass('active');
      // Apply active class to the clicked link
      $(this).addClass('active');
      return false;
    });
    $("select.collection-filter").change(function(){
      // Get the collection to operate on
      var collectionTarget = $(this).attr("data-collection");
      // Get the collection category
      var target = $(this).val();
      // Show all collection items
      $("#" + collectionTarget + " .collection-item").removeClass('collection-item-disabled').removeAttr('aria-hidden').removeAttr('role');
      // Add disabled class, aria to collection items that are not part of the category chosen
      if (target != 'all') {
        $('#' + collectionTarget + ' .collection-item').not('.collection-category-' + target).addClass('collection-item-disabled').attr('aria-hidden', 'true').attr('role', 'presentation');
      }
    });
  });
})(jQuery);
