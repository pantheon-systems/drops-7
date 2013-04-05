<?php

/**
 * @file
 * Definition of commerce_backoffice_handler_field_term_entity_tid.
 *
 * This handler is temporarily in Backoffice until it gets committed to Views:
 * http://drupal.org/node/1826068.
 */

/**
 * Field handler to display all taxonomy terms of an entity.
 *
 * @ingroup views_field_handlers
 */
class commerce_backoffice_handler_field_term_entity_tid extends views_handler_field_prerender_list {

  function option_definition() {
    $options = parent::option_definition();

    $options['link_to_taxonomy'] = array('default' => TRUE, 'bool' => TRUE);
    $options['limit'] = array('default' => FALSE, 'bool' => TRUE);
    $options['vocabularies'] = array('default' => array());

    return $options;
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_taxonomy'] = array(
      '#title' => t('Link this field to its term page'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_taxonomy']),
    );

    $form['limit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Limit terms by vocabulary'),
      '#default_value'=> $this->options['limit'],
    );

    $options = array();
    $vocabularies = taxonomy_get_vocabularies();
    foreach ($vocabularies as $voc) {
      $options[$voc->machine_name] = check_plain($voc->name);
    }

    $form['vocabularies'] = array(
      '#prefix' => '<div><div id="edit-options-vocabularies">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Vocabularies'),
      '#options' => $options,
      '#default_value' => $this->options['vocabularies'],
      '#dependency' => array('edit-options-limit' => array(TRUE)),
    );

    parent::options_form($form, $form_state);
  }

  /**
   * The query method shouldn't do anything, since all information is taken
   * from entities loaded later in the process.
   */
  function query($use_groupby = FALSE) {}

  /**
   * Load the entities for all rows that are about to be displayed.
   */
  function post_execute(&$values) {
    if (!empty($values)) {
      // Load the entities.
      list($this->entity_type, $this->entities) = $this->query->get_result_entities($values);
      $vocabs = array_filter($this->options['vocabularies']);
      $vocabularies_to_fetch = $this->definition['vocabularies_to_fetch'];
      $tids = array();
      foreach ($this->entities as $row_id => $entity) {
        // Retrieve the taxonomy terms associated to this entity,
        // we're also taking in account the limit terms by vocabulary option.
        foreach ($vocabularies_to_fetch as $vocabulary => $field_name) {
          if (!empty($vocabs) && !in_array($vocabulary, $vocabs)) {
            continue;
          }
          $items = field_get_items($this->entity_type, $entity, $field_name);
          if ($items) {
            foreach ($items as $item) {
              $tids[$item['tid']][$row_id] = $row_id;
            }
          }
        }
      }
      if (!empty($tids)) {
        // Load all the associated taxonomy terms.
        $terms = entity_load('taxonomy_term', array_keys($tids));
        $vocabularies = taxonomy_vocabulary_load_multiple(NULL, array('machine_name' => array_keys($vocabularies_to_fetch)));
        // Sort by taxonomy weight.
        uasort($terms, array('commerce_backoffice_handler_field_term_entity_tid', 'sort_taxonomy_weight'));
        foreach ($terms as $tid => $term) {
          foreach ($tids[$tid] as $row_id) {
            $this->items[$row_id][$term->tid]['name'] = check_plain($term->name);
            $this->items[$row_id][$term->tid]['tid'] = $term->tid;
            $this->items[$row_id][$term->tid]['vocabulary_machine_name'] = check_plain($term->vocabulary_machine_name);
            $this->items[$row_id][$term->tid]['vocabulary'] = check_plain($vocabularies[$term->vid]->name);

            if (!empty($this->options['link_to_taxonomy'])) {
              $this->items[$row_id][$term->tid]['make_link'] = TRUE;
              $this->items[$row_id][$term->tid]['path'] = 'taxonomy/term/' . $term->tid;
            }
            $values[$row_id]->{'taxonomy_term_' . $this->options['id']} = $this->items[$row_id];
          }
        }
      }
    }
  }

  /**
   * Sort the taxonomy terms by weight.
   */
  private static function sort_taxonomy_weight($a, $b) {
    if ($a->weight == $b->weight) {
        return 0;
    }
    return ($a->weight < $b->weight) ? -1 : 1;
  }

  function get_items($values) {
    return isset($values->{'taxonomy_term_' . $this->options['id']}) ? $values->{'taxonomy_term_' . $this->options['id']} : array();
  }

  function render_item($count, $item) {
    return $item['name'];
  }

  function document_self_tokens(&$tokens) {
    $tokens['[' . $this->options['id'] . '-tid' . ']'] = t('The taxonomy term ID for the term.');
    $tokens['[' . $this->options['id'] . '-name' . ']'] = t('The taxonomy term name for the term.');
    $tokens['[' . $this->options['id'] . '-vocabulary-machine-name' . ']'] = t('The machine name for the vocabulary the term belongs to.');
    $tokens['[' . $this->options['id'] . '-vocabulary' . ']'] = t('The name for the vocabulary the term belongs to.');
  }

  function add_self_tokens(&$tokens, $item) {
    foreach(array('tid', 'name', 'vocabulary_machine_name', 'vocabulary') as $token) {
      // Replace _ with - for the vocabulary machine name.
      $tokens['[' . $this->options['id'] . '-' . str_replace('_', '-', $token). ']'] = isset($item[$token]) ? $item[$token] : '';
    }
  }

}
