<?php

/**
 * @file
 * Definition of GoogleAnalyticsReportsGoogleAnalyticsViewsWizard.
 */

/**
 * Tests creating Google Analytics views with the wizard.
 */
class GoogleAnalyticsReportsGoogleAnalyticsViewsWizard extends ViewsUiBaseViewsWizard {

  /**
  * @override
  */
  protected function default_display_options($form, $form_state) {
    $display_options = parent::default_display_options($form, $form_state);

    // Add permission-based access control.
    $display_options['access']['type'] = 'perm';
    $display_options['access']['perm'] = 'access google analytics reports';

    // Remove the default fields, since we are customizing them here.
    unset($display_options['fields']);

    // Add the title field.
    /* Field: Page tracking: Page Title */
    $display_options['fields']['pageTitle']['id'] = 'pageTitle';
    $display_options['fields']['pageTitle']['table'] = 'google_analytics';
    $display_options['fields']['pageTitle']['field'] = 'pageTitle';
    $display_options['fields']['pageTitle']['label'] = '';
    $display_options['fields']['pageTitle']['element_label_colon'] = FALSE;

    // Remove the default filters, since we are customizing them here.
    unset($display_options['filters']);

    /* Filter criterion: Google Analytics: Start date of report */
    $display_options['filters']['start_date']['id'] = 'start_date';
    $display_options['filters']['start_date']['table'] = 'google_analytics';
    $display_options['filters']['start_date']['field'] = 'start_date';
    $display_options['filters']['start_date']['value']['value'] = '-31 day';
    $display_options['filters']['start_date']['value']['type'] = 'offset';
    $display_options['filters']['start_date']['group'] = 1;
    $display_options['filters']['start_date']['expose']['operator'] = FALSE;
    /* Filter criterion: Google Analytics: End date of report */
    $display_options['filters']['end_date']['id'] = 'end_date';
    $display_options['filters']['end_date']['table'] = 'google_analytics';
    $display_options['filters']['end_date']['field'] = 'end_date';
    $display_options['filters']['end_date']['value']['value'] = '-1 day';
    $display_options['filters']['end_date']['value']['type'] = 'offset';
    $display_options['filters']['end_date']['group'] = 1;
    $display_options['filters']['end_date']['expose']['operator'] = FALSE;

    return $display_options;
  }

}
