(function($){
     $.fn.expandable = function (options) {
         var settings = $.extend({}, $.fn.expandable.defaults, options);

         return this.each(function(){

            var $tabs = $(this);
            prepareTabs($tabs);
            tabEvents($tabs);
            accordionEvents($tabs);
            selectEvents($tabs);
            displayTabs($tabs);


        });
    };

    $.fn.expandable.defaults = {


    }

    function prepareTabs($tabs) {
      //var config = this.config;

      // Add aria, accordion headers
      $('.expandable-tablist .expandable-tablist-item', $tabs).each(function(i){
        var $tab = $('a', this);

        $tab.attr({
          'aria-selected': 'false',
        });
        $('.expandable-tabcontent:eq(' + i + ')', $tabs).attr({
          'aria-hidden':'true',
          'aria-labelledby':$tab.attr('id')
        });
        $('.expandable-panel', $tabs).hide();

        var id = $($tab).data('expandable-panel');
        var label = $($tab).text();
        var link = '<a href="#' + id +'" role="tab" tabindex="0" aria-controls="' + id + '" aria-expanded="false" id="accordion-section-' + id + '">' + label + '</a>';
        $('.expandable-tabcontent:eq(' + i + ')', $tabs).before('<strong class="expandable-accordion-heading">' + link + '</strong>');
        // $('.expandable-accordion-heading', $tabs).hide();



        $(".expandable-tablist li[role='tab']").keydown(function(ev) {
          if (ev.which ==13) {
            $('a', this).click();
          }
          if ((ev.which ==39)||(ev.which ==37)) {
           var selected= $(this).attr("aria-selected");
           if (selected =="true"){
             $("li[aria-selected='false']").attr("aria-selected","true").focus() ;
             $(this).attr("aria-selected","false");
             var tabpanid= $("li[aria-selected='true']").attr("aria-controls");
             var tabpan = $("#"+tabpanid);
             $("div[role='tabpanel']").attr("aria-hidden","true");
             tabpan.attr("aria-hidden","false");
             }
          }
        });

      });
    }
    function tabEvents($tabs) {
      //var config = this.config;
      $('.expandable-tablist a', $tabs).click(function(event){
        event.preventDefault();
        var $tab = $(this);

        $('.expandable-tabcontent', $tabs).hide().attr({'aria-hidden':'true'});
        $('.expandable-tablist li', $tabs).removeClass('is-active');
        $('.expandable-tablist li a', $tabs).attr({
          'aria-selected': 'false',
        });

        $tab.attr({
          'aria-selected': 'true',
        }).parent().addClass('is-active');
        $tab.focus();

        var $panel = $tab.attr('href');
        //alert($tabs.attr('id'));
        $($panel).fadeIn().attr({'aria-hidden':'false'});
        expandableHashUpdate($panel);
      });
    }
    function accordionEvents($tabs) {
      $('.expandable-accordion-heading a', $tabs).click(function(event){
        event.preventDefault();
        var $tab = $(this);
        var $panel = $tab.attr('href');
        $('.expandable-accordion-heading a', $tabs).removeClass('is-active').attr({'aria-expanded':'false'});
        $('.expandable-tabcontent', $tabs).slideUp().attr({'aria-hidden':'true'});
        if($($panel).is(':visible')) {
          $($panel).slideUp().attr({'aria-hidden':'false'});
          $tab.attr({'aria-expanded':'false'}).removeClass('is-active');
        }
        else {
          $($panel).slideDown().attr({'aria-hidden':'true'});
          $tab.attr({'aria-expanded':'true'}).addClass('is-active');
          expandableHashUpdate($panel);
          var $accordionID = '#' + $tab.attr('id');
          setTimeout(function(){
            $('html, body').animate({
              scrollTop: $($accordionID).offset().top - 100
            }, 500);
          }, 600);
        }

        //$tab.toggleClass('is-active').attr({'aria-expanded':'true'});
      });
    }
    function selectEvents($tabs) {
      if ( $tabs.hasClass('expandable-select') ) {
        $('a.expandable-prompt', $tabs).click(function(event){
          event.preventDefault();
          if ($(this).attr('aria-expanded') == 'false') {
            $(this).attr('aria-expanded', 'true');
          }
          else {
            $(this).attr('aria-expanded', 'false');
          }
          $('.expandable-tab-group', $tabs).toggle('fast');
        });
        $('a.expandable-tablist-link', $tabs).click(function(event) {

          event.preventDefault();
          $tabset = '#' + $(this).data('tabset');
          $($tabset + ' .expandable-tab-group').fadeOut('fast');
        });
        $('body').click(function() {
          $('.expandable-select .expandable-tab-group').fadeOut('fast');
          $('a.expandable-prompt').attr('aria-expanded', 'false');
        });
        $('.expandable-select-prompt').click(function(event){
          event.stopPropagation();
        });
      }

    }
    function displayTabs($tabs) {
      // Open first panel
      if ( $tabs.hasClass('expandable-open') ) {

        $('.expandable-accordion-heading:first a', $tabs).attr({
          'aria-expanded':'true',
        }).addClass('is-active');
        $('.expandable-tablist li:first', $tabs).addClass('is-active');
        $('.expandable-tablist li:first a', $tabs).attr({
          'aria-selected':'true',
        }).addClass('is-active');
        $('.expandable-tabcontent:first', $tabs).show().attr({
          'aria-hidden':'false',
        });
      }
      // Check for hash
      if(window.location.hash) {
        var $hash = window.location.hash;
        if ( $($hash, $tabs).length ) {
          // reset tabs
          $('.expandable-tabcontent', $tabs).hide().attr({'aria-hidden':'true'});
          $('.expandable-tablist li, .expandable-tablist li a', $tabs).removeClass('is-active');
          $('.expandable-tablist li a', $tabs).attr({
            'aria-selected': 'false',
          });
          $('.expandable-accordion-heading a', $tabs).removeClass('is-active').attr({'aria-expanded':'false'});
          // show panel
          $($hash, $tabs).show().attr({'aria-expanded':'true'});
          $('.expandable-tablist li a[href="' + $hash + '"]', $tabs).addClass('isActive').attr({
            'aria-selected': 'true',
          }).parent().addClass('is-active');
          $('.expandable-accordion-heading a[href="' + $hash + '"]', $tabs).addClass('is-active').attr({'aria-expanded':'true'});
        }
      }
    }
    function expandableHashUpdate($panel) {
      history.pushState(null,null,$panel);
    }
})(jQuery);
