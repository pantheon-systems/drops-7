/**
 * @file
 * JS for Radix.
 */
(function ($, Drupal, window, document, undefined) {
  // Allow dropdown links to be clickable by showing dropdowns on hover/focus.
  Drupal.behaviors.radix_dropdown = {
    attach: function(context, setting) {
      var dropdown_disabled = false;

      // Prevent the dropdown from re-opening if a menu link was focused before
      // the window was re-focused.
      $(window).focus(function() {
        dropdown_disabled = true;
        setTimeout(function () {
          dropdown_disabled = false;
        }, 0);
      });

      $('.dropdown').once('radix-dropdown', function() {
        var dropdown = this;

        // Helper function to show the dropdown.
        function show() {
          if (!$(dropdown).hasClass('open') && !dropdown_disabled) {
            $('>[data-toggle="dropdown"]', dropdown).trigger('click.bs.dropdown');
          }
        }

        // Helper function to hide the dropdown.
        function hide() {
          if ($(dropdown).hasClass('open')) {
            $('>[data-toggle="dropdown"]', dropdown).trigger('click.bs.dropdown');
          }
        }

        // Show dropdown on hover and focus.
        $(this).on('mouseenter.radix.dropdown', function(e) {
          show();
        });
        $(this).on('mouseleave.radix.dropdown', function() {
          hide();
        });
        $(this).on('focusin.radix.dropdown', function() {
          show();
        });

        $(this).on('keydown.radix.dropdown', function(e) {
          // Prevent up/down arrow from doing anything -- they conflict with
          // using focus to show the dropdown, and the default Bootstrap keydown
          // handler will trigger our click handler to visit the link.
          if (e.keyCode == 38 || e.keyCode == 40) {
            return false;
          }
          // Show/hide dropdown with spacebar.
          if (e.keyCode == 32) {
            $('>[data-toggle="dropdown"]', dropdown).trigger('click.bs.dropdown');
            return false;
          }
          // Hide the dropdown with the escape hey.
          if (e.keyCode == 27) {
            // Leave focus on the parent after it's hidden.
            $('>[data-toggle="dropdown"]', dropdown).focus();
            hide();
            return false;
          }
        });

        // Allow a.dropdown-toggle to be clickable.
        if ($(this).has('> a.dropdown-toggle')) {
          $(this).on('click.radix.dropdown', function(e) {
            var $target = $(e.target);
            if ($target.parent().get(0) == dropdown && $target.is('a.dropdown-toggle') && $target.attr('href')) {
              e.preventDefault();
              window.location.href = $target.attr('href');
            }
          });
        }
      });

      // Make submenus work.
      $('.dropdown-submenu').once('radix-dropdown', function() {
        var dropdown = this,
            toggle = $(dropdown).children('.dropdown-toggle'),
            menu = $(dropdown).children('.dropdown-menu');

        function show() {
          $(dropdown).addClass('open');
        }

        function hide() {
          $(dropdown).removeClass('open');
        }

        $(dropdown)
          .on('mouseenter.radix.dropdown', show)
          .on('mouseleave.radix.dropdown', hide)
          .on('focusin.radix.dropdown', show);
      });

      // Hide dropdowns when focus is lost.
      $('body').once('radix-dropdown').on('focusout.radix.dropdown', function(e) {
        var parent = $(e.target).closest('li.radix-dropdown-processed.open').get(0);
        if (parent) {
          // Defer to after all handlers so we can see where focus landed.
          setTimeout(function () {
            // Don't do anything if no element is focused - that can only
            // happen with the mouse and this is meant to close the menu
            // when the keyboard is used to change focus.
            if (!document.activeElement || document.activeElement === document.body) {
              return;
            }
            // Hide the parent if it doesn't contain the now focused element
            // and is still open.
            if (!parent.contains(document.activeElement) && $(parent).hasClass('open')) {
              if ($(parent).hasClass('dropdown-submenu')) {
                $(parent).removeClass('open');
              }
              else {
                $(parent).trigger('click.bs.dropdown');
              }
            }
          }, 0);
        }
      });

    }
  };

  // Bootstrap tooltip.
  Drupal.behaviors.radix_tooltip = {
    attach: function(context, setting) {
      if ($.fn.tooltip) {
        $("[data-toggle='tooltip']").tooltip();
      }
    }
  };

  // Bootstrap popover.
  Drupal.behaviors.radix_popover = {
    attach: function(context, setting) {
      if ($.fn.popover) {
        $("[data-toggle='popover']").popover();
      }
    }
  };

  $(document).ready(function() {
    // Show first tab by default.
    // Ignore the "primary" tabs on the node edit page.
    if ($.fn.tab) {
      var tabs = $('.nav-tabs').not('.primary');
      tabs.children('li').first().find('a').tab('show');

      if (hash = window.location.hash) {
        $('.nav-tabs > li > a[href$="' + hash + '"]').tab('show');
      }
    }
  });
})(jQuery, Drupal, this, this.document);
