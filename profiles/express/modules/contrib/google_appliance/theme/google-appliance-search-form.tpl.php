<?php
// $Id$
/**
 * @file 
 *    default theme implementation for the full search form
 *
 * variables of interest
 * - variables['form'] : the form elements array, pre-render
 * - variables['search_form']['hidden'] : hidden form elements collapsed + rendered 
 * - variables['serach_form'] : non-hidden form elements rendered and keyed by original form keys
 * - variables['search_form_complete'] : the entire form collapsed and rendered
 *
 * @see template_preprocess_google_appliance_search_form()
 */
//dsm($variables);
?>
<div class="container-inline">
  <?php print $search_form_complete; ?>
</div>
