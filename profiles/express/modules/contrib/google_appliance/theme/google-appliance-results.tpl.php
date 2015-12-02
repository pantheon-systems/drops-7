<?php
// $Id$
/**
 * @file 
 *    default theme implementation for displaying Google Search Appliance results
 *
 * This template collects each invocation of theme_google_appliance_result(). This and
 * the child template are dependent on one another sharing the markup for the 
 * results listing
 *
 * @see template_preprocess_google_appliance_results()
 * @see template_preprocess_google_appliance_result()
 * @see google-appliance-result.tpl.php
 */
//dsm($variables);
?>
<?php print drupal_render($search_form); ?>

  <?php if (!empty($spelling_suggestion)): ?>
    <?php print $spelling_suggestion; ?>
  <?php endif; ?>
  
 <?php if (isset($show_synonyms) && $show_synonyms) : ?>
  <div class="synonyms google-appliance-synonyms">
    <span class="p"><?php print $synonyms_label ?></span> <ul><?php print $synonyms; ?></ul>
  </div>
 <?php endif; ?>

<h2 id="search-results-heading"><?php print $results_heading; ?></h2>
<?php if (!isset($response_data['error'])) : ?>
  
  <div class="google-appliance-results-control-bar clearfix">
    <?php print $search_stats; ?>
    <?php print $sort_headers; ?>
  </div>

  <?php if (!empty($keymatch_results)) : ?>
    <ol class="keymatch-results google-appliance-keymatch-results">
      <?php print $keymatch_results; ?>
    </ol>
  <?php endif; ?>

  <ol class="search-results google-appliance-results">
    <?php print $search_results; ?>
  </ol>
  
  <div class="google-appliance-results-control-bar clearfix">
    <?php print $search_stats; ?>
    <?php print $sort_headers; ?>
  </div>
  
  <?php print $pager; ?>

<?php else : ?>
  <p><?php print $error_reason ?></p>
<?php endif; ?>
