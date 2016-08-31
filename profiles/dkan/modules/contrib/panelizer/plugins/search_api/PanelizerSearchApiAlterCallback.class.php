<?php
/**
 * @file
 * Contains the PanelizerSearchApiAlterCallback class.
 */

/**
 * Search API data alteration callback that adds Panelizer content to items.
 */
class PanelizerSearchApiAlterCallback extends SearchApiAbstractAlterCallback {
  /**
   * Only support indexes with Panelize-able entities.
   */
  public function supportsIndex(SearchApiIndex $index) {
    $panelizer_plugins = panelizer_get_entity_plugins();
    if (isset($panelizer_plugins[$index->getEntityType()])) {
      $plugin = $panelizer_plugins[$index->getEntityType()];
      return !empty($plugin['uses page manager']);
    }
  }

  public function alterItems(array &$items) {
    // Prevent session information from being saved while indexing.
    drupal_save_session(FALSE);

    // Force the current user to anonymous to prevent access bypass in search
    // indexes.
    $original_user = $GLOBALS['user'];
    $GLOBALS['user'] = drupal_anonymous_user();

    $entity_type = $this->index->getEntityType();
    $entity_handler = panelizer_entity_plugin_get_handler($entity_type);

    foreach ($items as &$item) {
      $entity_id = entity_id($entity_type, $item);

      $item->search_api_panelizer_content = NULL;
      $item->search_api_panelizer_title = NULL;

      // If Search API specifies a language to view the item in, force the
      // global language_content to be Search API item language. Fieldable
      // panel panes will render in the correct language.
      if (isset($item->search_api_language)) {
        global $language_content;
        $original_language_content = $language_content;
        $languages = language_list();
        if (isset($languages[$item->search_api_language])) {
          $language_content = $languages[$item->search_api_language];
        }
        else {
          $language_content = language_default();
        }
      }

      try {
        if ($render_info = $entity_handler->render_entity($item, 'page_manager')) {
          $item->search_api_panelizer_content = $render_info['content'];
          $item->search_api_panelizer_title = !empty($render_info['title']) ? $render_info['title'] : NULL;
        }
      }
      catch (Exception $e) {
        watchdog_exception('panelizer', $e, 'Error indexing Panelizer content for %entity_type with ID %entity_id', array('%entity_type' => $entity_type, '%entity_id' => $entity_id));
      }

      // Restore the language_content global if it was overridden.
      if (isset($original_language_content)) {
        $language_content = $original_language_content;
      }
    }

    // Restore the user.
    $GLOBALS['user'] = $original_user;
    drupal_save_session(TRUE);
  }

  public function propertyInfo() {
    return array(
      'search_api_panelizer_content' => array(
        'label' => t('Panelizer "Full page override" HTML output'),
        'description' => t('The whole HTML content of the entity when viewed with Panelizer\'s "Full page override".'),
        'type' => 'text',
      ),
      'search_api_panelizer_title' => array(
        'label' => t('Panelizer "Full page override" page title'),
        'description' => t('The page title of the entity when viewed with Panelizer\'s "Full page override".'),
        'type' => 'text',
      ),
    );
  }
}
