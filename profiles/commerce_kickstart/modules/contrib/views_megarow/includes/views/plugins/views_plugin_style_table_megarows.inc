<?php
/**
 * @file
 * Contains the table style plugin.
 */

/**
 * Style plugin to render each item as a row in a table.
 *
 * @ingroup views_style_plugins
 */
class views_plugin_style_table_megarows extends views_plugin_style_table {
  /**
   * Exposes settings for the megarow rendering.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['megarow'] = array(
      '#type' => 'fieldset',
      '#title' => t('Megarow settings'),
      '#collapsible' => TRUE,
      '#tree' => FALSE,
    );
    $form['megarow']['autoclose'] = array(
      '#type' => 'checkbox',
      '#title' => t('Automatically close the megarow'),
      '#default_value' => $this->options['autoclose'],
      '#description' => t('Automatically close the megarow after submitting a form. (This is only working with nodes and megarow form wrappers.)'),
    );
    $form['megarow']['enable_scroll'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable scroll'),
      '#default_value' => $this->options['enable_scroll'],
      '#description' => t('Enable the scroll of the page to the row that has just been closed.'),
    );
    $form['megarow']['scroll_padding'] = array(
      '#type' => 'textfield',
      '#title' => t('Scroll padding'),
      '#default_value' => $this->options['scroll_padding'],
      '#field_suffix' => 'px',
      '#size' => 3,
      '#description' => t('Padding between the closed row and the top of the page, 120px is for instance the height of the admin + shortcurts bar.'),
      '#states' => array(
        'visible' => array(
          ':input[name="style_options[megarow][enable_scroll]"]' => array('checked' => TRUE),
        )
      ),
    );
    $form['megarow']['loading_text'] = array(
      '#type' => 'textfield',
      '#title' => t('Loading text'),
      '#default_value' => $this->options['loading_text'],
      '#description' => t('Text displayed while the megarow content is loaded.'),
    );
    $form['megarow']['close'] = array(
      '#type' => 'textfield',
      '#title' => t('Close text'),
      '#default_value' => $this->options['close'],
      '#required' => TRUE,
      '#size' => 8,
      '#description' => t("Text displayed to behave as the megarow's close button."),
    );
  }

  /**
   * Sets our own default options.
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['autoclose'] = array('default' => FALSE, 'bool' => TRUE);
    $options['enable_scroll'] = array('default' => TRUE, 'bool' => TRUE);
    $options['scroll_padding'] = array('default' => 120);
    $options['loading_text'] = array('default' => 'Loading...', 'translatable' => TRUE);
    $options['close'] = array('default' => 'x', 'translatable' => TRUE);

    return $options;
  }

  /**
   * Store variables in the right place due to weird views tree storage.
   */
  function options_submit(&$form, &$form_state) {
    parent::options_submit($form, $form_state);
    $form_state['values']['style_options']['autoclose'] = $form_state['values']['autoclose'];
    $form_state['values']['style_options']['enable_scroll'] = $form_state['values']['enable_scroll'];
    $form_state['values']['style_options']['scroll_padding'] = $form_state['values']['scroll_padding'];
    $form_state['values']['style_options']['loading_text'] = $form_state['values']['loading_text'];
    $form_state['values']['style_options']['close'] = $form_state['values']['close'];
  }
}
