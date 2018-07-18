<?php

/**
 * @file
 * Custom display extender plugin for Views.
 */

/**
 * Custom display extender plugin for Views.
 */
class metatag_views_plugin_display_extender_metatags extends views_plugin_display_extender {

  /**
   * {@inheritdoc}
   */
  function options_definition() {
    $options = parent::option_definition();
    $options['metatags'] = array('default' => '');
    $options['metatags_tokenize'] = array('bool' => TRUE, 'default' => FALSE);
    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function options_definition_alter(&$options) {
    $options['metatags'] = array('default' => array());
    $options['metatags_tokenize'] = array('bool' => TRUE, 'default' => FALSE);
  }

  /**
   * {@inheritdoc}
   */
  function options_summary(&$categories, &$options) {
    // Defines where within the Views admin UI the new settings will be visible.
    $categories['metatags'] = array(
      'title' => t('Meta tags'),
      'column' => 'second',
    );
    $options['metatags'] = array(
      'category' => 'metatags',
      'title' => t('Meta tags'),
      'value' => $this->has_metatags() ? t('Overridden') : t('Using defaults'),
    );
  }

  /**
   * {@inheritdoc}
   */
  function options_form(&$form, &$form_state) {
    // Defines the form.
    if ($form_state['section'] == 'metatags') {
      $form['#title'] .= t('The meta tags for this display');
      $metatags = $this->get_metatags();

      // Build/inject the Metatag form.
      $instance = 'view:' . $form_state['view']->name;
      $options['token types'] = array('view');
      $options['context'] = 'view';
      metatag_metatags_form($form, $instance, $metatags[LANGUAGE_NONE], $options);

      $form['metatags']['#type'] = 'container';

      // Code for replacement tokens from first row taken from
      // views_handler_area_text::options_form().
      $form['tokenize'] = array(
        '#type' => 'checkbox',
        '#title' => t('Use replacement tokens from the first row'),
        '#default_value' => $this->display->get_option('metatags_tokenize'),
        '#weight' => 50,
      );

      // Get a list of the available fields and arguments for token replacement.
      $options = array();
      foreach ($this->view->display_handler->get_handlers('field') as $field => $handler) {
        $options[t('Fields')]["[$field]"] = $handler->ui_name();
      }

      $count = 0;
      // This lets us prepare the key as we want it printed.
      foreach ($this->view->display_handler->get_handlers('argument') as $arg => $handler) {
        $options[t('Arguments')]['%' . ++$count] = t('@argument title', array('@argument' => $handler->ui_name()));
        $options[t('Arguments')]['!' . $count] = t('@argument input', array('@argument' => $handler->ui_name()));
      }

      if (!empty($options)) {
        $output = '<p>' . t('The following tokens are available. If you would like to have the characters \'[\' and \']\' please use the html entity codes \'%5B\' or  \'%5D\' or they will get replaced with empty space.' . '</p>');
        foreach (array_keys($options) as $type) {
          if (!empty($options[$type])) {
            $items = array();
            foreach ($options[$type] as $key => $value) {
              $items[] = $key . ' == ' . check_plain($value);
            }
            $output .= theme('item_list',
              array(
                'items' => $items,
                'type' => $type,
              ));
          }
        }

        $form['token_help'] = array(
          '#type' => 'fieldset',
          '#title' => t('Replacement patterns'),
          '#collapsible' => TRUE,
          '#collapsed' => TRUE,
          '#value' => $output,
          '#id' => 'edit-options-token-help',
          '#dependency' => array(
            'edit-options-tokenize' => array(1),
          ),
          '#prefix' => '<div>',
          '#suffix' => '</div>',
          '#weight' => 51,
        );
      }
      // Basic tags fieldset should not collapse to display in a larger popup.
      // @see https://www.drupal.org/node/1294478
      // @see https://www.drupal.org/node/2624020
      $form['metatags'][LANGUAGE_NONE]['basic']['#collapsible'] = FALSE;
    }
  }

  /**
   * {@inheritdoc}
   */
  function options_submit(&$form, &$form_state) {
    // Save the form values.
    if ($form_state['section'] == 'metatags') {
      $metatags = $form_state['values']['metatags'];

      // Leave some possibility for future versions to support translation.
      foreach ($metatags as $langcode => $values) {
        if (!empty($form['metatags'][$langcode]['#metatag_defaults'])) {
          metatag_filter_values_from_defaults($form_state['values']['metatags'][$langcode], $form['metatags'][$langcode]['#metatag_defaults']);
        }
      }

      $this->display->set_option('metatags', $metatags);
      $this->display->set_option('metatags_tokenize', $form_state['values']['tokenize']);

      // Update the i18n strings.
      if (!empty($metatags[LANGUAGE_NONE]) && $this->definition['enabled'] && module_exists('i18n_string') && !variable_get('metatag_i18n_disabled', FALSE)) {
        metatag_translations_update($metatags[LANGUAGE_NONE], 'metatag_views:' . $this->view->name . '_' . $this->display->plugin_name);
      }
    }
  }

  /**
   * Identify whether or not the current display has custom meta tags defined.
   *
   * @return bool
   *   Indicates whether the current display has custom meta tags defined.
   */
  protected function has_metatags() {
    $metatags = $this->get_metatags();
    return !empty($metatags[LANGUAGE_NONE]);
  }

  /**
   * Get the Metatag configuration for this display.
   *
   * @return array
   *   The meta tag values, keys by language (default LANGUAGE_NONE).
   */
  private function get_metatags() {
    $metatags = $this->display->get_option('metatags');

    // Leave some possibility for future versions to support translation.
    if (empty($metatags)) {
      $metatags = array(LANGUAGE_NONE => array());
    }
    if (!isset($metatags[LANGUAGE_NONE])) {
      $metatags = array(LANGUAGE_NONE => $metatags);
    }

    return $metatags;
  }

}
