(function ($) {
  $(document).ready(function(){ 
     $("#cu-feedback a.cu-feedback-link-embed").click(function() {
        $("#cu-feedback-content").slideToggle('fast');
        $("#cu-feedback a.cu-feedback-link-embed i.fa").toggleClass("fa-chevron-down");
        return false;
     });
  });
})(jQuery);