<?php

/**
 * @file
 * Contains MetatagSearchAlterCallback.
 */

/**
 * Implements hook_search_api_alter_callback_info().
 */
function metatag_search_api_alter_callback_info() {
  return array(
    'search_api_metatag_alter_callback' => array(
      'name' => t('Meta tags'),
      'description' => t("Adds the item's meta tags to the indexed data."),
      'class' => 'MetatagSearchAlterCallback',
    ),
  );
}

/**
 * Only add the class if Search API is installed.
 */
if (class_exists('SearchApiAbstractAlterCallback')) {

  /**
   * Adds meta tag values to the indexed items.
   */
  class MetatagSearchAlterCallback extends SearchApiAbstractAlterCallback {

    /**
     * {@inheritdoc}
     */
    public function supportsIndex(SearchApiIndex $index) {
      // Only works on entities.
      return (bool) $index->getEntityType();
    }

    /**
     * {@inheritdoc}
     */
    public function alterItems(array &$items) {
      $entity_type = $this->index->getEntityType();
      $tags = metatag_get_info('tags');
      foreach ($items as $id => $item) {
        foreach (array_keys($tags) as $tag) {
          $items[$id]->{'metatag_' . $tag} = NULL;
          if (isset($item->language) && isset($item->metatags[$item->language][$tag])) {
            $instance = metatag_get_instance($tag, $item->metatags[$item->language][$tag]);
            $items[$id]->{'metatag_' . $tag} = $instance->getValue(array('token data' => array($entity_type => $item)));
          }
        }
      }
    }

    /**
     * {@inheritdoc}
     */
    public function propertyInfo() {
      $properties = array();

      // Get available meta tags.
      $tags = metatag_get_info('tags');
      foreach ($tags as $id => $tag) {
        switch ($tag['class']) {
          case 'DrupalLinkMetaTag':
            $type = 'uri';
            break;

          default:
            $type = 'text';
            break;
        }
        $properties['metatag_' . $id] = array(
          'label' => t('Meta tag: @label', array('@label' => $tag['label'])),
          'description' => t('@label meta tag attached to an item.', array('@label' => $tag['label'])),
          'type' => $type,
        );
      }

      return $properties;
    }

  }
}
