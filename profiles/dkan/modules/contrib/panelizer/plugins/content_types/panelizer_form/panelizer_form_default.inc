<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'single' => TRUE,
  'icon' => 'icon_panelizer_form.png',
  'title' => t('Panelizer form default settings'),
  'description' => t('Panelizer default options on the Node form.'),
  'required context' => new ctools_context_required(t('Form'), 'node_form'),
  'category' => t('Form'),
);


function panelizer_panelizer_form_default_content_type_render($subtype, $conf, $panel_args, &$context) {
  $block = new stdClass();
  $block->module = t('node_form');

  $block->title = t('Panelizer defaults');
  $block->delta = 'panelizer-default';

  if (isset($context->form)) {
    if (isset($context->form['panelizer'])) {
      $block->content['panelizer'] = $context->form['panelizer'];
      unset($block->content['panelizer']['#pre_render']);
      unset($block->content['panelizer']['#theme_wrappers']);
      $block->content['panelizer']['#type'] = '';
      if (isset($block->content['panelizer']['path']['#size'])) {
        $block->content['panelizer']['path']['#size'] /= 2;
      }

      // Set access to false on the original rather than removing so that
      // vertical tabs doesn't clone it. I think this is due to references.
      $context->form['panelizer']['#access'] = FALSE;
    }
  }
  else {
    $block->content = t('Panelizer default options.');
  }
  return $block;
}

function ctools_panelizer_form_default_content_type_admin_title($subtype, $conf, $context) {
  return t('"@s" Panelizer form default options', array('@s' => $context->identifier));
}

function ctools_panelizer_form_default_content_type_edit_form($form, &$form_state) {
  // provide a blank form so we have a place to have context setting.
  return $form;
}
