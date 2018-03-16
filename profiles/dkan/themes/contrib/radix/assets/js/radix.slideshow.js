(function($) {
  Drupal.behaviors.field_slideshow = {
    attach: function(context) {

      for (i in Drupal.settings.field_slideshow) {
        var settings = Drupal.settings.field_slideshow[i],
          slideshow = $('div.' + i),
          num_slides = slideshow.children().length,
          $this = false;

        if (!slideshow.hasClass('field-slideshow-processed')) {
          slideshow.addClass('field-slideshow-processed');

          // Add padding if needed
          var max_outerWidth = 0;
          var max_outerHeight = 0;
          $('.field-slideshow-slide img', slideshow).each(function() {
            $this = $(this);
            max_outerWidth = Math.max(max_outerWidth, $this.outerWidth(true));
            max_outerHeight = Math.max(max_outerHeight, $this.outerHeight(true));
          });
          $('.field-slideshow-slide a', slideshow).each(function() {
            $this = $(this);
            max_outerWidth = Math.max(max_outerWidth, $this.outerWidth(true));
            max_outerHeight = Math.max(max_outerHeight, $this.outerHeight(true));
          });
          $('.field-slideshow-slide', slideshow).each(function() {
            $this = $(this);
            max_outerWidth = Math.max(max_outerWidth, $this.outerWidth(true));
            max_outerHeight = Math.max(max_outerHeight, $this.outerHeight(true));
          });
          slideshow.css({
            'padding-right': (max_outerWidth - parseInt(slideshow.css('width'))) + 'px',
            'padding-bottom': (max_outerHeight - parseInt(slideshow.css('height'))) + 'px'
          });

          // Add options
          var options = {
            resizing: 0,
            fx: settings.fx,
            speed: settings.speed,
            timeout: parseInt(settings.timeout),
            index: i,
            settings: settings
          };

          if (settings.speed == "0" && settings.timeout == "0") options.fastOnEvent = true;
          if (settings.controls) {
            options.prev = "#" + i + "-controls .prev";
            options.next = "#" + i + "-controls .next";
          }
          if (settings.pause) options.pause = true;

          if (settings.pager != '') {
            if (settings.pager == 'number' || settings.pager == 'image') options.pager = "#" + i + "-pager";
            if ((settings.pager == 'image' || settings.pager == 'carousel') && num_slides > 1) {
              options.pagerAnchorBuilder = function(idx, slide) {
                return '#' + i + '-pager li:eq(' + idx + ') a';
              };
              if (settings.pager == 'carousel') {
                var carouselops = {
                  buttonNextHTML: null,
                  buttonPrevHTML: null,
                  visible: parseInt(settings.carousel_visible),
                  scroll: parseInt(settings.carousel_scroll),
                  animation: parseInt(settings.carousel_speed),
                  vertical: settings.carousel_vertical,
                  initCallback: function(carousel) {
                    if (carousel.options.visible && num_slides <= carousel.options.visible) {
                      // hide the carousel next and prev if all slide thumbs are displayed
                      $(".carousel-prev, .carousel-next", carousel.container.parent()).addClass("hidden");
                      return false;
                    }
                    $(".carousel-next", carousel.container.parent()).bind('click', function() {
                      carousel.next();
                      return false;
                    });
                    $(".carousel-prev", carousel.container.parent()).bind('click', function() {
                      carousel.prev();
                      return false;
                    });
                  }
                };
                if (parseInt(settings.carousel_circular)) carouselops.wrap = 'circular';

                $("#" + i + "-carousel").jcarousel(carouselops);
                // the pager is the direct item's parent element
                options.pager = "#" + i + "-carousel .field-slideshow-pager";
              }
            }
          }

          // Configure the cycle.before callback, it's called each time the slide change
          options.before = function(currSlideElement, nextSlideElement, options, forwardFlag) {
            // In this function we access the settins with options.settings
            // since the settings variable will be equal to the last slideshow settings
            // Acessing directly settings may cause issues if there are more than 1 slideshow

            // The options.nextSlide sometimes starts with 1 instead of 0, this is safer
            var nextIndex = $(nextSlideElement).index();

            // Add activeSlide manually for image pager
            if (options.settings.pager == 'image') {
              $('li', options.pager).removeClass("activeSlide");
              $('li:eq(' + nextIndex + ')', options.pager).addClass("activeSlide");
            }

            // Add activeSlide manually for number pager
            if (options.settings.pager == 'number') {
              $('li', options.pager).removeClass("active");
              $('li:eq(' + nextIndex + ')', options.pager).addClass("active");
            }

            // If we are using the carousel make it follow the activeSlide
            // This will not work correctly with circular carousel until the version 0.3 of jcarousel
            // is released so we disble this until then
            if (options.settings.pager == 'carousel' && parseInt(options.settings.carousel_follow) && parseInt(options.settings.carousel_circular) == 0) {
              var carousel = $("#" + options.index + "-carousel").data("jcarousel");
              carousel.scroll(nextIndex, true);
            }
          };

          if (num_slides > 1) {

            if (settings.start_on_hover) {
              //If start_on_hover is set, stop cycling onload, and only activate
              //on hover
              slideshow.cycle(options).cycle("pause").hover(function() {
                $(this).cycle('resume');
              },function(){
                $(this).cycle('pause');
              });
            }
            else {
              // Cycle!
              slideshow.cycle(options);
            }

            // After the numeric pager has been built by Cycle, add some classes for theming
            if (settings.pager == 'number') {
              $('.field-slideshow-pager a').each(function(){
                $this = $(this);
                $this.wrap('<li>');
                $this.addClass('slide-' + $this.html());
              });
            }
            // Keep a reference to the slideshow in the buttons since the slideshow variable
            // becomes invalid if there are multiple slideshows (equal to the last slideshow)
            $("#" + i + "-controls .play, #" + i + "-controls .pause").data("slideshow", slideshow);
            // if the play/pause button is enabled link the events
            $("#" + i + "-controls .play").click(function(e) {
              e.preventDefault();
              var target_slideshow = $(this).data("slideshow");
              target_slideshow.cycle("resume", true);
              $(this).hide();
              $(this).parent().find(".pause").show();
            });
            $("#" + i + "-controls .pause").click(function(e) {
              e.preventDefault();
              var target_slideshow = $(this).data("slideshow");
              target_slideshow.cycle("pause");
              $(this).hide();
              $(this).parent().find(".play").show();
            });
          }

        }

      }

      // Recalculate height for responsive layouts
      var rebuild_max_height = function(context) {
        var max_height = 0;
        var heights = $('.field-slideshow-slide',context).map(function ()
        {
          return $(this).height();
        }).get(),
        max_height = Math.max.apply(Math, heights);
        if (max_height > 0) {
          context.css("height", max_height);
        }
      };

      if (jQuery.isFunction($.fn.imagesLoaded)) {
        $('.field-slideshow').each(function() {
          $('img',this).imagesLoaded(function() {
            rebuild_max_height($(this).parents('.field-slideshow'));
          });
        });
      }
      else {
        $(window).load(function(){
          $('.field-slideshow').each(function(){
            rebuild_max_height($(this));
          });
        });

      }
      $(window).resize(function(){
        $('.field-slideshow').each(function(){
          rebuild_max_height($(this));
        });
      });

    }
  };
})(jQuery);
