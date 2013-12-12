<?php

/**
 * Megarow handler that outputs the links which open the megarow.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_megarow_links extends views_handler_field {
  function query() {
    // Do nothing.
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['megarow']['links'] = array('default' => '');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['megarow'] = array(
      '#type' => 'fieldset',
      '#title' => t('Megarow Settings'),
    );
    $form['megarow']['links'] = array(
      '#type' => 'textarea',
      '#title' => t('Links'),
      '#description' => t('Enter one link per line, in the format label|url.'),
      '#default_value' => $this->options['megarow']['links'],
    );

    // Token item.
    if (module_exists('token')) {
      // Add token module replacements fields
        $form['megarow']['tokens'] = array(
          '#type' => 'fieldset',
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#title' => t('Placeholder tokens'),
          '#description' => t("The following placeholder tokens can be used in the edit, preview and revisions links. When used in a path they will be replaced with the appropriate values."),
        );
        $token_type = array(
          'theme' => 'token_tree',
          'token_types' => array($this->getTokenType()),
          'global_types' => TRUE,
          'click_insert' => TRUE,
          'dialog' => TRUE,
        );
        $form['megarow']['tokens']['help'] = array(
          '#type' => 'markup',
          '#markup' => theme('token_tree', $token_type),
        );
    }
  }

  /**
   * Returns the available token type based on the view default entity type.
   * @return string
   */
  protected function getTokenType() {
    static $token_type;

    if (empty($token_type)) {
      $table_data = views_fetch_data($this->table);
      $token_type = str_replace('_', '-', $table_data['table']['entity type']);
    }

    // Hardcode the pattern for taxonomy terms.
    if ($token_type == 'taxonomy-term') {
      $token_type = 'term';
    }
    return $token_type;
  }

  /**
   * Renders the field.
   * @param type $values
   * @return string
   */
  public function render($values) {
    // Get the entity matching this row.
    $results = array($values);
    $entities = $this->query->get_result_entities($results);
    $entity = reset($entities[1]);
    $table_data = views_fetch_data($this->table);
    $entity_type = $table_data['table']['entity type'];
    list($entity_id) = entity_extract_ids($entity_type, $entity);

    // Create an array of links.
    $provided_links = explode("\n", $this->options['megarow']['links']);
    $provided_links = array_map('trim', $provided_links);
    $provided_links = array_filter($provided_links, 'strlen');

    $links = array();
    $tokens = $this->get_render_tokens(array());
    foreach ($provided_links as $link) {
      $link_parts = explode('|', $link);

      // Replace tokens if necessary in the url.
      $url = 'display_megarow/' . $entity_id . '/' . $this->replaceTokens($link_parts[1], $entity);
      $url = $this->render_altered(array('text' => $url), $tokens);

      // Do the same for the label.
      $label = $this->replaceTokens($link_parts[0], $entity);
      $label = $this->render_altered(array('text' => $label), $tokens);

      // Add the link for rendering.
      $links[] = $this->getLink($label, $url, array('class' => 'views-megarow-open'));
    }

    $nb_links = count($links);
    if ($nb_links == 0) {
      $element = array();
    }
    else if ($nb_links > 1) {
      $element = array(
        '#prefix' => $this->getElementPrefix($values),
        '#markup' => theme('links__ctools_dropbutton', array('links' => $links, 'attributes' => array('class' => array('links', 'inline')))),
        '#suffix' => $this->getElementSuffix($values),
      );
    }
    else {
      $element = array(
        '#type' => 'link',
        '#title' => $links[0]['title'],
        '#href' => $links[0]['href'],
        '#options' => array(
          'attributes' => $links[0]['attributes'],
        ),
      );
    }
    return $element;
  }

  /**
   * Creates the url replacing the tokens.
   * @param type $raw_url
   * @param type $values
   * @return string
   */
  protected function replaceTokens($raw_url, $entity) {
    $data = array(
      $this->getTokenType() => $entity,
    );

    return token_replace($raw_url, $data);
  }

  /**
   * Returns a link as a renderable array.
   * @param type $title
   * @param type $url
   * @param type $attributes
   * @return array
   */
  protected function getLink($title, $url, $attributes = array()) {
    return array('title' => $title, 'href' => $url, 'attributes' => $attributes);
  }

  /**
   * Returns a prefix if we need for the operations element.
   * Returns empty string but this will allow us to rewrite the output more
   * easily when we extend the handler.
   * @param type $values
   * @return string
   */
  protected function getElementPrefix($values) {
    return '';
  }

  /**
   * Returns a suffix if we need for the operations element.
   * Returns empty string but this will allow us to rewrite the output more
   * easily when we extend the handler.
   * @param type $values
   * @return string
   */
  protected function getElementSuffix($values) {
    return '';
  }
}
