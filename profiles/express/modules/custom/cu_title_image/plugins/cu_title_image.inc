<?php
/**
 * @file
 * Context plugin file to provide changing the active theme as a context
 * reaction.
 */

/**
 * Expose themes as context reactions.
 */
class context_reaction_title_image extends context_reaction {

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
      '#title' => t('Page Title Image'),

     
      'title_image_file' => array(
        '#title' => t('Title Image File'),
        '#description' => t('URL to image to be used with the page title.'),
        '#type' => 'textfield',
        '#default_value' => isset($settings['title_image_file']) ? $settings['title_image_file'] : '',
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
    $title_image = NULL;
    foreach ($this->get_contexts() as $context) {
      if (isset($context->reactions['title_image'])) {
        $title_image = $context->reactions['title_image'];
      }
    }
    return $title_image;
  }
  
}