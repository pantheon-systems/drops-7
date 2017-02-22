;(function($) {
  "use strict";

  $(document).on("ready", function(){

    var backend = {
      backend: "Excel",
      url: "data/testxls1.xlsx",
    };
    var dataset = new recline.Model.Dataset(backend);

    dataset.fetch().done(function(data){
      console.log(data);
    });

  });
})(jQuery);
