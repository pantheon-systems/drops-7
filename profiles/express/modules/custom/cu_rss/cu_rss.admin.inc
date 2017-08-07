<?php

/** Implements hook_form().
 *
 * RSS Feed Entity Add/Edit Form.
 */
function cu_rss_rss_form($form, &$form_state, $rss_feed = NULL, $node = NULL) {
  $form = array();

  // Add for existing entities
  if ($rss_feed) {
    $form['#entity']->id = $rss_feed->id;
  }

  if (isset($rss_feed)) {
    $title = $rss_feed->name;
  }
  elseif (isset($node)) {
    $title = $node->title;
  }
  else {
    $title = '';
  }
  $form['feed_builder'] = array(
    '#type' => 'fieldset',
    '#title' => t('RSS Feed Builder'),
    '#description' => t('Build custom RSS feeds by selecting from the categories and tags below. Selecting terms will update the feed url automatically.'),
  );
  $form['feed_builder']['rss_title'] = array(
    '#title' => 'RSS Feed Title',
    '#type' => 'textfield',
    '#prefix' => '<div id="article-rss-title">',
    '#suffix' => '</div>',
    '#default_value' => $title,
    '#required' => TRUE,
  );

  // Build original/no configurations rss feed url
  global $base_url;
  $parts = array();
  $parts[] = $base_url;
  $parts[] = 'feed';

  if ($node) {
    foreach ($node->field_article_page_category[LANGUAGE_NONE] as $category_tid) {
      $default_categories[] = $category_tid['tid'];
    }
    foreach ($node->field_article_page_filter[LANGUAGE_NONE] as $tag_tid) {
      $default_tags[] = $tag_tid['tid'];
    }

    if (!empty($default_categories) || !empty($default_tags)) {
      $parts[] = !empty($default_categories) ? join('+', $default_categories) : 'all';
      $parts[] = !empty($default_tags) ? join('+', $default_tags) : 'all';
    }
  }

  $parts[] = 'rss.xml';
  $url = isset($rss_feed->url) ? $rss_feed->url : join('/', $parts);

  $form['feed_builder']['rss_url'] = array(
    '#title' => 'RSS Feed URL',
    '#type' => 'textfield',
    '#description' => t('Use this url to link to the rss feed.'),
    '#prefix' => '<div id="article-rss-url">',
    '#suffix' => '</div>',
    '#default_value' => $url,
  );

  $form['feed_builder']['categories'] = array(
    '#prefix' => '<div class="admin-sub-panel">',
    '#suffix' => '</div>',
  );
  // Get category terms
  $categories = array();
  $v = taxonomy_vocabulary_machine_name_load('category');
  $terms = taxonomy_get_tree($v->vid);
  foreach ($terms as $tid => $term) {
    $categories[$term->tid] = $term->name;
  }

  // Default/saved category values
  $default_categories = array();
  if ($rss_feed) {
    $default_categories = unserialize($rss_feed->categories);
  }
  elseif ($node) {
    foreach ($node->field_article_page_category[LANGUAGE_NONE] as $category_tid) {
      $default_categories[] = $category_tid['tid'];
    }
  } else {
    $default_categories = array();
  }

  // Add form element for categories
  $form['feed_builder']['categories']['rss_categories'] = array(
    '#title' => t('Categories'),
    '#type' => 'checkboxes',
    '#options' => $categories,
    '#default_value' => $default_categories,
    '#description' => t('Filter your feed by selecting from the terms above.'),
  );
  // Add ajax for categories
  $form['feed_builder']['categories']['rss_categories']['#ajax'] = array(
    'callback' => 'cu_rss_rss_url_callback',
    'wrapper' => 'article-rss-url',
    'method' => 'replace',
    'effect' => 'fade',
  );

  $form['feed_builder']['tags'] = array(
    '#prefix' => '<div class="admin-sub-panel">',
    '#suffix' => '</div>',
  );
  // Get tag terms
  $tags = array();
  //$tags['all'] = 'All';
  $v = taxonomy_vocabulary_machine_name_load('tags');
  $terms = taxonomy_get_tree($v->vid);
  foreach ($terms as $tid => $term) {
    $tags[$term->tid] = $term->name;
  }

  // Default/saved tag values
  $default_tags = array();
  if ($rss_feed) {
    $default_tags = unserialize($rss_feed->tags);
  }
  elseif ($node) {
    foreach ($node->field_article_page_filter[LANGUAGE_NONE] as $tag_tid) {
      $default_tags[] = $tag_tid['tid'];
    }
  } else {
    $default_tags = array();
  }

  // Add form element for tags
  $form['feed_builder']['tags']['rss_tags'] = array(
    '#title' => t('Tags'),
    '#type' => 'checkboxes',
    '#options' => $tags,
    '#default_value' => $default_tags,
    '#description' => t('Filter your feed by selecting from the terms above.'),
  );
  // Add ajax for categories
  $form['feed_builder']['tags']['rss_tags']['#ajax'] = array(
    'callback' => 'cu_rss_rss_url_callback',
    'wrapper' => 'article-rss-url',
    'method' => 'replace',
    'effect' => 'fade',
  );

  // Add submit/cancel/delete actions
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Feed'),
  );
  $form['actions']['cancel'] = array(
    '#type'   => 'submit',
    '#value'  => t('Cancel'),
    '#submit' => array('cu_rss_form_cancel'),
    '#limit_validation_errors' => array(),
  );
  // Only add delete if it exists already
  if (empty($rss_feed->is_new)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#submit' => array('cu_rss_delete_submit'),
      '#value' => t('Delete'),
    );
  }
  // Add validation and submit functions
  $form['#validate'][] = 'cu_rss_feed_url_validate';
  $form['#submit'][] = 'cu_rss_feed_url_submit';
  return $form;
}

/**
 * Ajax callback
 *
 * Builds rss feed url
 */
function cu_rss_rss_url_callback($form, $form_state) {
  global $base_url;
  // Build url parts
  $parts = array();
  $parts[] = $base_url;
  $parts[] = 'feed';
  // If both filters are set to all, use simple url
  if (($form['feed_builder']['categories']['rss_categories']['#value'] != 'all') || ($form['feed_builder']['tags']['rss_tags']['#value'] != 'all')) {
    //$parts[] = $form['feed_builder']['categories']['rss_categories']['#value'];
    //$parts[] = $form['feed_builder']['tags']['rss_tags']['#value'];
  }
  $has_categories = FALSE;
  $categories = array();
  $has_tags = FALSE;
  $tags = array();
  foreach ($form_state['values']['rss_categories'] as $term) {
    if ($term) {
      $has_categories = TRUE;
      $categories[$term] = $term;
    }
  }
  foreach ($form_state['values']['rss_tags'] as $term) {
    if ($term) {
      $has_tags = TRUE;
      $tags[$term] = $term;
    }
  }
  if ($has_categories || $has_tags) {
    $parts[] = $has_categories ? join('+', $categories) : 'all';
    $parts[] = $has_tags ? join('+', $tags) : 'all';
  }

  $parts[] = 'rss.xml';
  $form['feed_builder']['rss_url']['#value'] = join('/', $parts);
  return $form['feed_builder']['rss_url'];
}

/**
 * Validation function for cu_rss_rss_form()
 */
function cu_rss_feed_url_validate($form, &$form_state) {
  $query = db_select('rss_feed', 'rf');
  $query->fields('rf', array('id', 'name', 'url'));
  $query->condition('rf.url', $form_state['values']['rss_url'], '=');
  if (!empty($form['#entity']) && isset($form['#entity']->id)) {
    $query->condition('rf.id', $form['#entity']->id, '!=');
  }
  $feeds = $query->execute()->fetchAssoc();
  if (!empty($feeds)) {
    form_set_error('NULL', t('You have already created a feed with these settings.'));
  }
}

/**
 * Submit function for cu_rss_rss_form().
 */
function cu_rss_feed_url_submit($form, $form_state) {
  $values = array(
    'name' => $form_state['values']['rss_title'],
    'url' => $form_state['values']['rss_url'],
    'categories' => serialize($form_state['values']['rss_categories']),
    'tags' => serialize($form_state['values']['rss_tags']),
  );

  // See if the entity exists
  if ($feed = entity_load_single('rss_feed', $form['#entity']->id)) {
    // Entity exists, add values just in case they have changed
    foreach ($values as $key => $value) {
      $feed->$key = $value;
    }
  }
  else {
    // create new entity with values
    $feed = entity_create('rss_feed', $values);
  }
  $feed->save();
  drupal_set_message('RSS Feed @name has been saved',array('@name' => $feed->name));
  drupal_goto('admin/settings/feeds/rss');
}

/**
 * Entity delete function for cu_rss_rss_form().
 */
function cu_rss_delete_submit($form) {
  $id = $form['#entity']->id;
  entity_delete('rss_feed', $id);
  drupal_set_message('The RSS Feed has been deleted.');
  drupal_goto('admin/settings/feeds/rss');
}

/**
 * Callback function for admin/settings/feeds/rss/manage/%cu_rss_feed/default
 *
 * Sets default feed url
 */
function cu_rss_feed_set_default($rss_feed) {
  variable_set('express_rss_feed_default', $rss_feed->url);
  drupal_set_message('Default RSS Feed has been set.');
  drupal_goto('admin/settings/feeds/rss');
}

/**
 * Cancel function for cu_rss_rss_form().
 */
function cu_rss_form_cancel() {
  drupal_goto('admin/settings/feeds/rss');
}
