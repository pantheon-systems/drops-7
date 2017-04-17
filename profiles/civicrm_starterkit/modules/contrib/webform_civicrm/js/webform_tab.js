jQuery(function($) {
  'use strict';
  // Highlight the CiviCRM tab when hovering the link to it
  function toggleTab() {
    $('a[href$=civicrm]', '.tabs.primary').parent().toggleClass('active');
  }
  $('.webform-civicrm-tab-link').hover(toggleTab, toggleTab);
});