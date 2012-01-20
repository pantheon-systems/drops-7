<?php
// $Id: nodeconnect.form.inc,v 1.1.2.1 2011/01/21 18:21:32 e2thex Exp $
/**
 * @file
 * handles all form alters and submit functions for nodeconnect
 */

/*
 * Implements of hook_field_attache_form()
 *
 * here we attach a submit button for each node ref field
 */
function _nodeconnect_field_attach_form($entity_type, $entity, &$form, &$form_state, $langcode='und', $field_name, $field) {
  $language = $langcode;
  foreach ($form[$field_name][$language] as $key => $field) {
    if (is_numeric($key)) {
      // we have to add some extra js for single value fields so we add a class to let the
      // js know
      $extra = $form[$field_name][$language]['#cardinality'] == 1 ? 'single-value' : '';
      $form[$field_name][$language][$key]["add_nodeconnect__{$field_name}_$key"] = array(
        '#type' => 'submit',
        '#value' => "New Content",
        '#name' => "add_nodeconnect__{$field_name}_$key",
        '#prefix' => "<div class = 'nodeconnect-add $extra'>",
        '#suffix' => '</div>',
        '#key' => $key,
        '#field' => $field_name,
        '#add_child' => TRUE,
        '#language' => 'und',
        '#submit' => array('nodeconnect_include_form', 'nodeconnect_add_edit_button_submit'),
        '#weight' => -2,
        '#attached' => array(
          'js' => array(
            drupal_get_path('module', 'nodeconnect') . "/nodeconnect.js",
          ),
          'css' => array(
            drupal_get_path('module', 'nodeconnect') . "/nodeconnect.css",
          ),
        ),
      );
      $form[$field_name][$language][$key]["edit_nodeconnect__{$field_name}_$key"] = array(
        '#type' => 'submit',
        '#value' => "Edit Content",
        '#name' => "edit_nodeconnect__{$field_name}_$key",
        '#prefix' => "<div class = 'nodeconnect-edit $extra'>",
        '#suffix' => '</div>',
        '#key' => $key,
        '#field' => $field_name,
        '#add_child' => FALSE,
        '#language' => 'und',
        '#submit' => array('nodeconnect_include_form', 'nodeconnect_add_edit_button_submit'),
        '#weight' => -2,
      );
    }
  }
}

/*
 * nodeconnect_add_edit_button_submit() is called when a new node is to be added
 *
 * we cache the current state and form
 * and redirect to the nodeconnect_add page with an append build_cached_id
 *
 * TODO: we should capture the current destination so that we can add that back
 * when we return
 */
function nodeconnect_add_edit_button_submit(&$form, &$form_state) {
  $cache_id = "nodeconnect-" . $form['#build_id'];
  $field = $form_state['clicked_button']['#field'];
  $key   = $form_state['clicked_button']['#key'];
  $language = $form[$field]['#language'];
  $data = array(
    'form'       => $form, 
    'form_state' => $form_state,
    'dest'       => $_GET['q'],
    'field'      => $field,
    'key'        => $key,
    'add_child'  => $form_state['clicked_button']['#add_child'],
    'nid'        => $form_state['values'][$field][$language][$key]['nid'],
  );
  nodeconnect_cache_set($cache_id, $data);

  unset($_GET['destination']);
  if ($data['add_child']) {
    $form_state['redirect']  =  "admin/nodeconnect/add/$cache_id";
  }
  else {
    if ($data['nid']) {
      $form_state['redirect']  =  "admin/nodeconnect/edit/$cache_id";
    }
  }
}

/*
 * nodeconnect_child_node_form_alter() alters are child create form
 *   we add a value field to hold the parent build_cache_id
 * and then we add a cancel button that run nodeconnect_child_form_cancel
 * and we add a new submit nodeconect_child_form_submit
 */
function nodeconnect_child_node_form_alter(&$form, &$form_state, $form_id, $cache_id, $cache) {
  $form['parent_build_cache_id'] = array(
    '#type' => 'value',
    '#value' => $cache_id,
  );
  $form['actions']['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('nodeconnect_include_form', 'nodeconnect_child_form_cancel'),
    '#parent_build_cache_id' => $cache_id,
    '#limit_validation_errors' => array(),

  );
  $form['actions']['submit']['#submit'][] = 'nodeconnect_include_form';
  $form['actions']['submit']['#submit'][] = 'nodeconnect_child_form_submit';
}

/*
 * nodeconnect_child_form_submit() is for submiting of are child create form
 * on submiting of a child form we set 
 * set the nid in the cache entry
 * the rediect to our redirect page
 */
function nodeconnect_child_form_submit(&$form, &$form_state) {
  $cache_id =  $form_state['values']['parent_build_cache_id'];
  if ($cache_id && ($cache = cache_get($cache_id))) {
    $data = $cache->data;
    $data['nid'] = $form_state['values']['nid'];
    nodeconnect_cache_set($cache_id, $data);
    $form_state['redirect']  = "admin/nodeconnect/return/$cache_id";
  }
}

/*
 * nodeconnect_child_form_cancel() simple 
 * sets the redirect to a admin/nodeconnect/redirect page
 */
function nodeconnect_child_form_cancel(&$form, &$form_state) {
  $cache_id = $form_state['clicked_button']['#parent_build_cache_id'];
  if ($cache_id && ($cache = cache_get($cache_id))) {
    $form_state['redirect']  = "admin/nodeconnect/return/$cache_id";
  }
  
}

/* this is for when we return to the parent page
 * we find the cached form and form_state clean up the form_state a bit
 * and mark it to be rebuilt
 *
 * if the cache as an nid we set that in the input 
 */
function nodeconnect_return_node_form_alter(&$form, &$form_state, $form_id, $cache_id, $cache) {
  if (!isset($form_state['#nodeconnect_processed'])) {
    $form_state = $cache->data['form_state'];
    $form = $cache->data['form'];
    $form_state['submitted'] = FALSE;
    $form_state['rebuild'] = TRUE;
    unset($form_state['clicked_button']);
    unset($form_state['submit_handlers']);

    $data = $cache->data;
    $language = $form[$data['field']]['#language'];
    //if ($data['add_child']  && isset($data['nid']) && $form_state['rebuild']=== TRUE && ($node = node_load($data['nid'])) ) {
    if (isset($data['nid']) && $form_state['rebuild']=== TRUE && ($node = node_load($data['nid'])) ) {
      $element['#default_value'] = $node->nid;
      //use the autocompleate value so we are sure we validate
      $value =  node_reference_autocomplete_value($element, FALSE, $form_state);
      $form_state['input'][$data['field']][$language][$data['key']]['nid'] = $value;
    }
      
    $form_state['#nodeconnect_processed'] = TRUE;
    $form = drupal_rebuild_form($form_id, $form_state, $form);
  }

}

/*
 * nodeconnect_cache_set wraps cache set so we can 
 * set the expire easily
 */
function nodeconnect_cache_set($cid, $data) {
  return cache_set($cid, $data, 'cache', CACHE_PERMANENT);
}


