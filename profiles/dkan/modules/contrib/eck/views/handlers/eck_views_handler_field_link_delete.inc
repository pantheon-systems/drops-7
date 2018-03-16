<?php
/**
 * @file
 * Field handler to present a link to delete the entity content.
 */

// @codingStandardsIgnoreStart
class eck_views_handler_field_link_delete extends eck_views_handler_field_link {

  function render_link($entity, $values) {
    $entity_type = $entity->entityType();
    $entity_id = entity_id($entity_type, $entity);
    $bundle = $entity->bundle();
    $action = 'delete';

    if (eck__multiple_access_check(
      array(
        "eck administer entities",
        "eck {$action} entities",
        "eck administer {$entity_type} {$bundle} entities",
        "eck {$action} {$entity_type} {$bundle} entities"))) {

      $crud_info = get_bundle_crud_info($entity_type, $bundle);
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = str_replace('%eckentity', $entity_id, $crud_info['delete']['path']);
      $this->options['alter']['query'] = drupal_get_destination();

      $offset_is_over_limit = FALSE;
      if (isset($this->view->query->offset) && isset($this->view->query->limit)) {
        $offset = $this->view->query->offset;
        $limit = $this->view->query->limit;
        $offset_is_over_limit = $offset >= $limit;
      }

      if ($offset_is_over_limit && count($this->view->result) == 1) {
        $destination = drupal_get_destination();
        $url_query = parse_url($destination['destination']);
        parse_str($url_query['query'], $url_data);
        $url_data['page'] = $url_data['page'] - 1;
        $destination['destination'] = $url_query['path'] . '?' . http_build_query($url_data);
        $this->options['alter']['query'] = $destination;
      }

      $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');

      return $text;
    }
  }
}
// @codingStandardsIgnoreEnd
