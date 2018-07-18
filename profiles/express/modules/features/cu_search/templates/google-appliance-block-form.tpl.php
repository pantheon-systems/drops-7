<?php
// $Id$
/**
 * @file
 *    default theme implementation for the search block form
 * variables of interest
 * - variables['form'] : the form elements array, pre-render
 * - variables['block_search_form']['hidden'] : hidden form elements collapsed + rendered
 * - variables['block_serach_form'] : form elements rendered and keyed by original form keys
 * - variables['block_search_form_complete'] : the entire form collapsed and rendered
 *
 * @see template_preprocess_google_appliance_block_form()
 */

 $options = variable_get('cu_search_options', array('this' => 'this'));
 foreach ($options as $key => $option) {
   if (!$option) {
     unset($options[$key]);
   }
 }
 $active = TRUE;
 $configs = array();
 $configs['this'] = array(
   'value' => 'This site',
   'placeholder' => 'Search this site',
   'label' => 'This site',
   'action' => base_path() . '/gsearch',
 );
 $configs['all'] = array(
   'value' => 'Colorado.edu',
   'placeholder' => 'Search Colorado.edu',
   'label' => 'Colorado.edu',
   'action' => 'http://www.colorado.edu/gsearch',
 );
?>
<?php if (!empty($options)): ?>
<div class="search-container animated">
  <?php if (empty($variables['form']['#block']->subject)) : ?>
    <h2 class="element-invisible"><?php print t('Search'); ?></h2>
  <?php endif; ?>
  <?php //print $block_search_form_complete; ?>
  <div class="search-form-wrapper">
    <div class="search-options">
      <?php if (count($options) > 1): ?>
        <?php foreach ($options as $option): ?>
          <?php if ($option): ?>
            <?php
              $checked = $active ? 'checked="checked"' : '';
            ?>


            <div class="search-option">
              <input type="radio" name="search-option" <?php print $checked; ?> value="<?php print $configs[$option]['value']; ?>" data-placeholder="<?php print $configs[$option]['placeholder']; ?>" data-action="<?php print $configs[$option]['action']; ?>" id="search-<?php print $option; ?>"/> <label for="search-<?php print $option; ?>"><?php print $configs[$option]['label']; ?></label>
            </div>
            <?php
              $active = FALSE;
            ?>
          <?php endif; ?>
        <?php endforeach; ?>
      <?php endif; ?>

    </div>
    <div class="search-fields">
      <?php print render($block_search_form['search_keys']); ?>
      <?php print render($block_search_form['actions']); ?>
    </div>
  </div>
  <?php print render($block_search_form['url']); ?>
  <?php print render($block_search_form['hidden']); ?>
</div>
<?php else: ?>
  <div></div>
<?php endif; ?>
