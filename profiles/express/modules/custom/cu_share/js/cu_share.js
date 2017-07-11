(function ($) {
  $(document).ready(function(){
     // encodeURI(uri)
     $('a.share-on-twitter').each(function(){
       var share_url = $(this).attr('href');
       $(this).click(function(){
         window.open(share_url,'ShareOnTwitterWindow', 'width=600, height=400');
         return false;
       });
     });
  });

})(jQuery);
