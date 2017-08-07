<?php

/**
 * @file
 * Defines a default ldap_view
 */

/**
 * Implements hook_views_default_views().
 */
function ldap_views_views_default_views() {
  /*
   * View 'ad_users'
   */
  $view = new view;
  $view->name = 'ad_users_example';
  $view->description = 'Users from AD. You must define a ldap data source whith ad_users identifier, and mail, name and samaccountname attributes';
  $view->tag = 'default';
  $view->base_table = 'ldap';
  $view->human_name = 'AD Users example';
  $view->core = 7;
  $view->api_version = '3.0-alpha1';
  $view->disabled = TRUE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'AD Users';
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['qid'] = 'ad_users';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'attribute' => 'attribute',
    'attribute_1' => 'attribute_1',
    'attribute_2' => 'attribute_2',
    'attribute_3' => 'attribute_3',
    'attribute_4' => 'attribute_4',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'attribute' => array(
      'align' => '',
      'separator' => '',
    ),
    'attribute_1' => array(
      'align' => '',
      'separator' => '',
    ),
    'attribute_2' => array(
      'align' => '',
      'separator' => '',
    ),
    'attribute_3' => array(
      'align' => '',
      'separator' => '',
    ),
    'attribute_4' => array(
      'align' => '',
      'separator' => '',
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['empty_table'] = 0;
  /* Field: cn */
  $handler->display->display_options['fields']['cn']['id'] = 'cn';
  $handler->display->display_options['fields']['cn']['table'] = 'ldap';
  $handler->display->display_options['fields']['cn']['field'] = 'cn';
  $handler->display->display_options['fields']['cn']['ui_name'] = 'cn';
  $handler->display->display_options['fields']['cn']['label'] = 'Common Name';
  $handler->display->display_options['fields']['cn']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['external'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['cn']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['cn']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['cn']['alter']['html'] = 0;
  $handler->display->display_options['fields']['cn']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['cn']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['cn']['hide_empty'] = 0;
  $handler->display->display_options['fields']['cn']['empty_zero'] = 0;
  $handler->display->display_options['fields']['cn']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['cn']['multivalue'] = 'v-index';
  $handler->display->display_options['fields']['cn']['value_separator'] = ' ';
  $handler->display->display_options['fields']['cn']['index_value'] = '0';
  /* Field: dn */
  $handler->display->display_options['fields']['dn']['id'] = 'dn';
  $handler->display->display_options['fields']['dn']['table'] = 'ldap';
  $handler->display->display_options['fields']['dn']['field'] = 'dn';
  $handler->display->display_options['fields']['dn']['ui_name'] = 'dn';
  $handler->display->display_options['fields']['dn']['label'] = 'Distinguished Name';
  $handler->display->display_options['fields']['dn']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['external'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['dn']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['dn']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['dn']['alter']['html'] = 0;
  $handler->display->display_options['fields']['dn']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['dn']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['dn']['hide_empty'] = 0;
  $handler->display->display_options['fields']['dn']['empty_zero'] = 0;
  $handler->display->display_options['fields']['dn']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['dn']['index_value'] = '';
  /* Field: username */
  $handler->display->display_options['fields']['attribute']['id'] = 'attribute';
  $handler->display->display_options['fields']['attribute']['table'] = 'ldap';
  $handler->display->display_options['fields']['attribute']['field'] = 'attribute';
  $handler->display->display_options['fields']['attribute']['ui_name'] = 'username';
  $handler->display->display_options['fields']['attribute']['label'] = 'User name';
  $handler->display->display_options['fields']['attribute']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['external'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['attribute']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['attribute']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['attribute']['alter']['html'] = 0;
  $handler->display->display_options['fields']['attribute']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['attribute']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['attribute']['hide_empty'] = 0;
  $handler->display->display_options['fields']['attribute']['empty_zero'] = 0;
  $handler->display->display_options['fields']['attribute']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['attribute']['multivalue'] = 'v-index';
  $handler->display->display_options['fields']['attribute']['index_value'] = '0';
  $handler->display->display_options['fields']['attribute']['attribute_name'] = 'samaccountname';
  /* Field: name */
  $handler->display->display_options['fields']['attribute_1']['id'] = 'attribute_1';
  $handler->display->display_options['fields']['attribute_1']['table'] = 'ldap';
  $handler->display->display_options['fields']['attribute_1']['field'] = 'attribute';
  $handler->display->display_options['fields']['attribute_1']['ui_name'] = 'name';
  $handler->display->display_options['fields']['attribute_1']['label'] = 'Name';
  $handler->display->display_options['fields']['attribute_1']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['external'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['attribute_1']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['attribute_1']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['attribute_1']['alter']['html'] = 0;
  $handler->display->display_options['fields']['attribute_1']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['attribute_1']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['attribute_1']['hide_empty'] = 0;
  $handler->display->display_options['fields']['attribute_1']['empty_zero'] = 0;
  $handler->display->display_options['fields']['attribute_1']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['attribute_1']['multivalue'] = 'v-index';
  $handler->display->display_options['fields']['attribute_1']['index_value'] = '0';
  $handler->display->display_options['fields']['attribute_1']['attribute_name'] = 'name';
  /* Field: mail */
  $handler->display->display_options['fields']['attribute_2']['id'] = 'attribute_2';
  $handler->display->display_options['fields']['attribute_2']['table'] = 'ldap';
  $handler->display->display_options['fields']['attribute_2']['field'] = 'attribute';
  $handler->display->display_options['fields']['attribute_2']['ui_name'] = 'mail';
  $handler->display->display_options['fields']['attribute_2']['label'] = 'Mail';
  $handler->display->display_options['fields']['attribute_2']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['external'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['attribute_2']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['attribute_2']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['attribute_2']['alter']['html'] = 0;
  $handler->display->display_options['fields']['attribute_2']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['attribute_2']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['attribute_2']['hide_empty'] = 0;
  $handler->display->display_options['fields']['attribute_2']['empty_zero'] = 0;
  $handler->display->display_options['fields']['attribute_2']['hide_alter_empty'] = 0;
  $handler->display->display_options['fields']['attribute_2']['index_value'] = '0';
  $handler->display->display_options['fields']['attribute_2']['attribute_name'] = 'mail';
  /* Sort criterion: LDAP Server: LDAP Attribute */
  $handler->display->display_options['sorts']['attribute']['id'] = 'attribute';
  $handler->display->display_options['sorts']['attribute']['table'] = 'ldap';
  $handler->display->display_options['sorts']['attribute']['field'] = 'attribute';
  $handler->display->display_options['sorts']['attribute']['attribute_name'] = 'name';

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'ad-users';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'AD Users';
  $handler->display->display_options['menu']['weight'] = '0';
  $views[$view->name] = $view;

  return $views;
}


