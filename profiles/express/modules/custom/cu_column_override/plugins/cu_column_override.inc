<?php
/**
 * @file
 * Context plugin file to provide changing the active theme as a context
 * reaction.
 */

/**
 * Expose themes as context reactions.
 */
class context_reaction_column_override extends context_reaction {

  /**
   * Grab the available themes and provide them as a reaction for context.
   *
   * @param $context
   *   The context as passed from context module.
   *
   * @return array
   *   The FAPI array as read by context module.
   */
  function options_form($context) {
    
    $settings = $this->fetch_from_context($context);
    
    $form = array(
      '#tree' => TRUE,
      '#title' => t('Column Override'),

     
      'column_override_after_content_columns' => array(
        '#title' => t('After Content Columns'),
        '#type' => 'select',
        '#options' => array(
          'no-change' => 'No Change',
          '1' => '1',
          '2' => '2',
          '3' => '3',
          '4' => '4',
          '6' => '6',
        ),
        '#default_value' => isset($settings['column_override_after_content_columns']) ? $settings['column_override_after_content_columns'] : 'no-change',
      ),
      'column_override_after_content_2_columns' => array(
        '#title' => t('After Content 2 Columns'),
        '#type' => 'select',
        '#options' => array(
          'no-change' => 'No Change',
          '1' => '1',
          '2' => '2',
          '3' => '3',
          '4' => '4',
          '6' => '6',
        ),
        '#default_value' => isset($settings['column_override_after_content_2_columns']) ? $settings['column_override_after_content_2_columns'] : 'no-change',
      ),
      'column_override_footer_columns' => array(
        '#title' => t('Footer Columns'),
        '#type' => 'select',
        '#options' => array(
          'no-change' => 'No Change',
          '1' => '1',
          '2' => '2',
          '3' => '3',
          '4' => '4',
          '6' => '6',
        ),
        '#default_value' => isset($settings['column_override_footer_columns']) ? $settings['column_override_footer_columns'] : 'no-change',
      ),
    );
    return $form;
  }

  /**
   * Return the active theme based on the context
   *
   * @return string | null
   *   String of the theme name, or NULL if not to be altered.
   */
  function execute() {
    $column_override = NULL;
    foreach ($this->get_contexts() as $context) {
      if (isset($context->reactions['column_override'])) {
        $column_override = $context->reactions['column_override'];
      }
    }
    return $column_override;
  }
  
}