<?php
/**
 * @file
 * cu_wysiwyg.features.wysiwyg.inc
 */

/**
 * Implements hook_wysiwyg_default_profiles().
 */
function cu_wysiwyg_wysiwyg_default_profiles() {
  $profiles = array();

  // Exported profile: wysiwyg
  $profiles['wysiwyg'] = array(
    'format' => 'wysiwyg',
    'editor' => 'ckeditor',
    'settings' => array(
      'default' => 1,
      'user_choose' => 0,
      'show_toggle' => 1,
      'theme' => 'advanced',
      'language' => 'en',
      'buttons' => array(
        'default' => array(
          'Bold' => 1,
          'Italic' => 1,
          'JustifyLeft' => 1,
          'JustifyCenter' => 1,
          'JustifyRight' => 1,
          'BulletedList' => 1,
          'NumberedList' => 1,
          'Outdent' => 1,
          'Indent' => 1,
          'Undo' => 1,
          'Redo' => 1,
          'Link' => 1,
          'Unlink' => 1,
          'Anchor' => 1,
          'Image' => 1,
          'Blockquote' => 1,
          'HorizontalRule' => 1,
          'Cut' => 1,
          'Copy' => 1,
          'Paste' => 1,
          'PasteText' => 1,
          'PasteFromWord' => 1,
          'ShowBlocks' => 1,
          'RemoveFormat' => 1,
          'Format' => 1,
          'Styles' => 1,
          'Table' => 1,
          'CreateDiv' => 1,
          'SpellChecker' => 1,
          'Scayt' => 1,
        ),
        'button_sc_generator' => array(
          'button_sc_generator_button' => 1,
        ),
        'caption_sc_generator' => array(
          'caption_sc_generator_button' => 1,
        ),
        'icon_sc_generator' => array(
          'icon_sc_generator_button' => 1,
        ),
        'give_sc_generator' => array(
          'give_sc_generator_button' => 1,
        ),
        'expand_sc_generator' => array(
          'expand_sc_generator_button' => 1,
        ),
        'map_sc_generator' => array(
          'map_sc_generator_button' => 1,
        ),
        'box_sc_generator' => array(
          'box_sc_generator_button' => 1,
        ),
        'video_sc_generator' => array(
          'video_sc_generator_button' => 1,
        ),
        'drupal_path' => array(
          'Link' => 1,
        ),
      ),
      'toolbarLocation' => 'top',
      'resize_enabled' => 1,
      'default_toolbar_grouping' => 1,
      'simple_source_formatting' => 0,
      'css_setting' => 'self',
      'css_path' => '%bprofiles/cu_fit/modules/features/cu_wysiwyg/ckeditor-wysiwyg.css',
      'stylesSet' => 'Small=small
Lead=p.lead
Hero=p.hero
Supersize=p.supersize
Underline=ul.list-style-underline
No Bullets=ul.list-style-nobullet
Border=ul.list-style-border
Icon List 1=ul.list-style-icon-1
Icon List 2=ul.list-style-icon-2
Icon List 3=ul.list-style-icon-3
Icon List 4=ul.list-style-icon-4
Icon List 5=ul.list-style-icon-5
Icon List 6=ul.list-style-icon-6
Alpha Uppercase=ol.list-style-alpha-upper
Alpha Lowercase=ol.list-style-alpha-lower
Roman Uppercase=ol.list-style-roman-upper
Roman Lowercase=ol.list-style-roman-lower
Zebra=table.table-zebra
Condensed=table.table-condensed
Small Table=table.table-small
Columns=table.table-vertical
Rows=table.table-horizontal',
      'block_formats' => 'p,h2,h3,h4,h5,h6,div,blockquote',
      'advanced__active_tab' => 'edit-css',
      'forcePasteAsPlainText' => 0,
    ),
  );

  return $profiles;
}
