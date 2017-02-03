<?php
/**
 * @file
 * Display the node form's Metatag fieldset in a pane.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Node form meta tags'),
  'icon' => 'icon_node_form.png',
  'description' => t('Meta tags on the node form.'),
  'required context' => new ctools_context_required(t('Form'), 'node_form'),
  'category' => t('Form'),
);

/**
 * CTools content type callback to output this pane.
 */
function metatag_node_form_metatags_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();

  $block->title = t('Meta tags');
  $block->module = 'metatag';
  $block->delta = 'metatag';

  if (isset($context->form)) {
    if (isset($context->form['metatags'])) {
      $block->content['metatags'] = $context->form['metatags'];
      unset($block->content['metatags']['#pre_render']);
      unset($block->content['metatags']['#theme_wrappers']);
      $block->content['metatags']['#type'] = '';

      // Set access to false on the original rather than removing so that
      // vertical tabs doesn't clone it. I think this is due to references.
      $context->form['metatags']['#access'] = FALSE;
    }
  }
  else {
    $block->content = t('Meta tags options.');
  }
  return $block;
}

/**
 * CTools content type callback to get the title of this pane.
 */
function metatag_node_form_metatags_content_type_admin_title($subtype, $conf, $context) {
  if (isset($context->identifier)) {
    $message = t('"@s" node form meta tags', array('@s' => $context->identifier));
  }
  else {
    $message = t('Node form meta tags');
  }
  return $message;
}

/**
 * CTools content type callback to display an edit form for this pane.
 */
function metatag_node_form_metatags_content_type_edit_form($form, &$form_state) {
  // Provide a blank form so we have a place to have context setting.
  return $form;
}

/**
 * CTools content type callback to provide extra information about this pane.
 */
function metatag_node_form_metatags_content_type_admin_info($type, $subtype, $conf, $context = NULL) {
  $output = new stdClass();
  $output->title = t('Metatag form');
  $output->content = t('Shows the meta tag fields on this node form if the user has the appropriate permissions.');

  return $output;
}
