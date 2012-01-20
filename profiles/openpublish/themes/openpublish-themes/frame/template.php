<?php

/**
 * @file
 * This file is empty by default because the base theme chain (Alpha & Omega) provides
 * all the basic functionality. However, in case you wish to customize the output that Drupal
 * generates through Alpha & Omega this file is a good place to do so.
 * 
 * Alpha comes with a neat solution for keeping this file as clean as possible while the code
 * for your subtheme grows. Please read the README.txt in the /preprocess and /process subfolders
 * for more information on this topic.
 */
 
/**
 * Implements HOOK_form_FORM_ID_alter
 * This hook into the search block form adds HTML5 placeholder text for search
 */
function frame_form_search_block_form_alter(&$form, &$form_state) {
  $form['search_block_form']['#attributes']['placeholder'] = t('Search this site…');
}

/**
 *  Implements hook_form_FORM_ID_alter().
 *  Alter the search form and add our js to submit on enter keydown.
 *  We're hiding the submit button and the form removes the enter to submit functionality
 */
function frame_form_search_form_alter (&$form, &$form_state, $form_id) {
  drupal_add_js(drupal_get_path('theme', 'frame') . '/js/search.js');
}