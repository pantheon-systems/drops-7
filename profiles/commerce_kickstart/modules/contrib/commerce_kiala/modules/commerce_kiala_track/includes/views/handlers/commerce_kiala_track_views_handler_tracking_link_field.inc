<?php

/*
 * Display a external link to the kiala tracking page
 */
class commerce_kiala_track_views_handler_tracking_link_field extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);

    // The path is set by render_link function so don't allow to set it.
    $form['alter']['path'] = array('#access' => FALSE);
    $form['alter']['external'] = array('#access' => FALSE);
  }

  function render($values) {
    if ($tracking_number = $this->get_value($values)) {
      return $this->render_link($tracking_number, $values);
    }
  }

  function render_link($tracking_number, $values) {
    $this->options['alter']['make_link'] = TRUE;
    
    global $user;

    $language = !empty($user->language) ? $user->language : language_default()->language;    
    $handler = commerce_kiala_track_get_tracking_embeddable_handler($language, $tracking_number);
    $this->options['alter']['path'] = $handler->getUrl();
    
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    return $text;
  }
}