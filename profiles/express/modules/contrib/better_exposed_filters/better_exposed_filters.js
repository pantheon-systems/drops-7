/**
 * @file better_exposed_filters.js
 * 
 * Provides some client-side functionality for the Better Exposed Filters module
 */
(function ($) {
  Drupal.behaviors.better_exposed_filters = { 
    attach: function() {

      /*
       * Add Select all/none links to specified checkboxes
       */
      
      // Check for selected that already has the select all/none element for pages with outside ajaxy
      // things going on.
      var selected = $('.form-checkboxes.bef-select-all-none:not(.bef-processed)');
      if (selected.length) {
        var selAll = Drupal.t('Select All');
        var selNone = Drupal.t('Select None');
        
        // Set up a prototype link and event handlers
        var link = $('<a class="bef-toggle" href="#">'+ selAll +'</a>')
        link.click(function() {
          if (selAll == $(this).text()) {
            // Select all the checkboxes
            $(this)
              .html(selNone)
              .siblings('.bef-checkboxes, .bef-tree')
                .find('.form-item input:checkbox').each(function() {
                  $(this).attr('checked', 'checked');
                })
              .end()
  
              // attr() doesn't trigger a change event, so we do it ourselves. But just on 
              // one checkbox otherwise we have many spinning cursors
              .find('input[type=checkbox]:first').change() 
            ;
          }
          else {
            // Unselect all the checkboxes
            $(this)
              .html(selAll)
              .siblings('.bef-checkboxes, .bef-tree')
                .find('.form-item input:checkbox').each(function() {
                  $(this).attr('checked', '');
                })
              .end()
  
              // attr() doesn't trigger a change event, so we do it ourselves. But just on 
              // one checkbox otherwise we have many spinning cursors
              .find('input[type=checkbox]:first').change() 
            ;
          }
          return false;
        });
  
        // Add link to the page for each set of checkboxes.
        selected
          .addClass('bef-processed')
          .each(function(index) {
            // Clone the link prototype and insert into the DOM
            var newLink = link.clone(true);
            
            newLink.insertBefore($('.bef-checkboxes, .bef-tree', this));
            
            // If all checkboxes are already checked by default then switch to Select None
            if ($('input:checkbox:checked', this).length == $('input:checkbox', this).length) {
              newLink.click();
            }
          });
      }
    }                   // attach: function() {
  };                    // Drupal.behaviors.better_exposed_filters = { 
}) (jQuery);
