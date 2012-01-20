<?php

/**
 * @file
 */

class views_node_taxonomy_filter_handler_filter_tid extends views_handler_filter {

  /**
   * Determine if a filter can be exposed.
   */
  function can_expose() { return FALSE; }

  function option_definition() {
    $options = parent::option_definition();
    $options['operator'] = array('default' => 'is');

    return $options;
  }

  function operator_options() {
    if ($this->options['multiple']) {
      return array(
        'is' => t('Is one of'),
        'is not' => t('Is not one of'),
      );
    }
    else {
      return array(
        'is' => t('Is'),
        'is not' => t('Is not'),
      );
    }
  }

  /**
   * Supply an edit form for the Filter.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $vocabs = array();
    $result = db_query("SELECT v.vid, v.machine_name, v.name FROM {taxonomy_vocabulary} v ");
    foreach ($result as $vocab) {
      $vocabs[$vocab->machine_name] = $vocab->name;
    }
    $default_vocab = $this->options['value'][0];
    $default_child = $this->options['value'][1];
    $default_require = $this->options['value'][2];
    
    $form['value'][] = array(
      '#type' => 'select',
      '#title' => t('Vocabulary'),
      '#options' => $vocabs,
      '#default_value' => $default_vocab,
      '#description' => t('Select the Vocabulary.'),
      '#multiple' => TRUE,
    );
    $form['value'][] = array(
      '#type' => 'checkbox',
      '#title' => t('Include child Terms?'),
      '#default_value' => $default_child,
      '#multiple' => FALSE,
    );

    $form['value'][] = array(
      '#type' => 'checkbox',
      '#title' => t('Require matching value'),
      '#description'=> t('If the node does not have a value for this field, no results will be shown'),
      '#default_value' => $default_require,
    );
  }

  /**
   * Display the value of the Filter selection in the Views edit screens.
   */
  function admin_summary() {
    $vocabularies =  taxonomy_get_vocabularies();
    $vocab_name = '';
    foreach ($vocabularies as $vocabulary) {
      if (is_array($this->value[0])) {
        foreach ($this->value[0] as $voc) {
          if ($vocabulary->machine_name == $voc) {
            $vocab = $vocabularies[$vocabulary->vid];
            $vocab_name .= $vocab->name . ', ';
          }
        }
      }
      else {
        if ($vocabulary->machine_name == $this->value[0]) {
          $vocab = $vocabularies[$vocabulary->vid];
          $vocab_name = $vocab->name;
        }
      }
    }
    $vocab_name = rtrim($vocab_name, ', ');
    return t("for Vocabulary: @vocab_name", array('@vocab_name' => $vocab_name));
  }


  /**
   * Modify the query appropriately.
   */
  function query() {

    if (empty($this->value[0])) {
      return;
    }
    $allowed_vocabs = $this->value[0];
    if (arg(0) != 'node') {
      return;
    }

    $nid = arg(1);
    if (!is_numeric($nid)) {
      return;
    }
    $node = node_load($nid);
    $langcode = empty($node->language) ? LANGUAGE_NONE : $node->language;
    $tids = array();
    $list_vocabs = array();

    $fields = field_info_fields();
    //This is going to give us all taxonomy reference fields
    foreach ($fields as $name => $field) {
      if ($field['type'] == 'taxonomy_term_reference') {
        $vocabs[$field['settings']['allowed_values'][0]['vocabulary']][] = $field['field_name'];
      }
    }   

    //find which field is the field we're looking for
    foreach ($allowed_vocabs as $allowed) {
      if (in_array($allowed, array_keys($vocabs))) {
        $term_fields = $vocabs[$allowed];
      }
    }
    //we have all the fields using the given vocab, grab the terms
    foreach ($term_fields as $term_field) {
      if (isset($node->$term_field) && !empty($node->$term_field)) {
        $node_term_set = $node->$term_field;
        if ($langcode) {
          foreach ($node_term_set[$langcode] as $term) {
            //if selected, get all children for the term
            if ($this->value[1] == 1) {
              $tids[] = $term['tid'];
              $children = taxonomy_get_children($term['tid']);
              foreach ($children as $child) {
                $tids[] = $child->tid;  
              }
            }
            else {
              $tids[] = $term['tid'];
            }
          }
        }
        else {
          foreach ($node_term_set as $term) {
            //if selected, get all children for the term
            if ($this->value[1] == 1) {
              $tids[] = $term['tid'];
              $children = taxonomy_get_children($term['tid']);
              foreach ($children as $child) {
                $tids[] = $child->tid;  
              }
            }
            else {
              $tids[] = $term['tid'];
            }
          }
        }
      }
    }
    if (sizeof($tids) < 1) {
      //if we want to require this relationship, make tids 0 - there will never be terms with tid = 0 (autoincrement starts at 1)
      if(isset($this->value[2]) && $this->value[2]){
        $tids = 0;
      } else {
        return;
      }
    } else {
      $tids = implode(',', $tids);
    }
     
    $alias = $this->query->ensure_table('taxonomy_index');
    $this->query->add_where_expression($this->options['group'], "$alias.tid IN ($tids)");
  }
}