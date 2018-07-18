<?php

/**
 * @file
 * Contains code for et_page_top_bottom context reaction.
 */

/**
 * Class et_page_top_bottom_context_reaction.
 */
class et_page_top_bottom_context_reaction extends context_reaction {

  /**
   * Provide form to enter embeds.
   *
   * @param array $context
   *   Current context array.
   *
   * @return array
   *   Added form options displayed to user on reaction screen.
   */
  public function options_form($context) {
    return array(
      'et_page_top_bottom' => array(
        '#type' => 'select',
        '#options' => $this->getEmbeds(),
        '#title' => t('Page Top or Bottom Embed'),
        '#description' => t('Select the embeds you wish to appear on selected pages.'),
      ),
    );
  }

  /**
   * Capture selected embeds to send to execute function.
   *
   * @param array $values
   *   Values passed to the main Context form submit function.
   *
   * @return array
   *   The values you want passed with a certain key.
   */
  public function options_form_submit($values) {
    return array('et_page_top_bottom' => $values['et_page_top_bottom']);
  }

  /**
   * Add embed code to page.
   *
   * @param array $page
   *   The page array before entering context reaction.
   *
   * @return array
   *   The page array after it has been passed through context reaction.
   */
  public function execute(&$page) {
    foreach ($this->get_contexts() as $context) {
      if (!empty($context->reactions[$this->plugin])) {

        $id = $context->reactions[$this->plugin]['et_page_top_bottom'];
        $embed = entity_load_single('embed_templates', $id);

        // Don't view embed if it is unpublished.
        if ($embed->status === 'unpublished') {
          return null;
        }

        $data = unserialize($embed->data);
        $type = $embed->type;

        $js = _et_page_top_bottom_get_js($data, $type);

        // Add weight based on position.
        // Set default to show at the bottom of the page.
        $weight = 9999;
        $position = 'bottom';
        if (isset($data['position']) && $data['position'] === 'top') {
          $weight = -9999;
          $position = 'top';
        }

        // Add JS to proper section of $page array.
        $page['page_' . $position]['et_page_top_bottom'][] = array(
          '#markup' => $js,
          '#weight' => $weight,
        );
      }
    }
    return $page;
  }

  /**
   * Get all published embeds and return as select list options.
   *
   * @return array
   *   Options of embeds user can select. Might be empty.
   */
  public function getEmbeds() {
    $options = array();

    // Get published embed names.
    $query_options = array(
      'renderer' => array('page_top_bottom'),
      'status' => array('published'),
    );
    $names = embed_templates_get_embeds($query_options);

    // Return names for select list.
    if ($names) {
      foreach ($names as $name) {
        $options[$name->id] = $name->name;
      }
      return $options;
    }

    // Return no options if no published embeds.
    return $options;
  }

}
