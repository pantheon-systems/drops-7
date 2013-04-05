<?php


/**
 * Implements hook_crumbs_plugins()
 */
function taxonomy_crumbs_plugins($api) {

  $api->multiPlugin('termParent');

  foreach (field_info_fields() as $key => $info) {
    if ($info['type'] == 'taxonomy_term_reference') {
      foreach ($info['bundles'] as $entity_type => $bundles) {
        $class = '_taxonomy_CrumbsMultiPlugin_termReference_' . $entity_type;
        if (class_exists($class)) {
          $plugin = new $class($key, $bundles);
          $api->multiPlugin("termReference.$key.$entity_type", $plugin);
        }
      }
    }
  }

  $api->disabledByDefault('termReference.*');
}


// -----------------------------------------------------------------------------


class _taxonomy_CrumbsMultiPlugin_termReference implements crumbs_MultiPlugin {

  protected $fieldKey;
  protected $bundles;

  // To be defined by the child class
  protected $entityType;

  function __construct($field_key, $bundles) {
    $this->fieldKey = $field_key;
    $this->bundles = $bundles;
  }

  function describe($api) {
    foreach ($this->bundles as $bundle) {
      $api->addRule($bundle, $bundle);
    }
  }

  protected function _findParentPath($entity) {
    $terms = array();
    $items = field_get_items($this->entityType, $entity, $this->fieldKey);
    if ($items) {
      foreach ($items as $item) {
        $terms[$item['tid']] = TRUE;
      }
    }

    if (count($terms) > 1) {
      $walk = $terms;
      $visited = array();
      while (!empty($walk)) {
        $visited += $walk;
        foreach ($walk as $tid => $true) {
          $parents = taxonomy_get_parents($tid);
          unset($walk[$tid]);
          foreach ($parents as $tid => $parent) {
            unset($terms[$tid]);
            if (!isset($visited[$tid])) {
              $walk[$tid] = $parent;
            }
          }
        }
      }
    }

    // Return the path of the first found term, if any.
    foreach ($terms as $tid => $term_info) {
      $term = taxonomy_term_load($tid);
      if (!empty($term)) {
        $uri = entity_uri('taxonomy_term', $term);
        if (!empty($uri)) {
          return $uri['path'];
        }
      }
    }
  }

  protected function _getPath_node($nid) {
    return 'node/' . $nid;
  }

  protected function _getPath_user($uid) {
    return 'user/' . $uid;
  }
}


class _taxonomy_CrumbsMultiPlugin_termReference_node extends _taxonomy_CrumbsMultiPlugin_termReference {

  protected $entityType = 'node';

  /**
   * Match "node/%" router path
   */
  function findParent__node_x($path, $item) {
    $node = $item['map'][1];
    // Load the node if it hasn't been loaded due to a missing wildcard loader.
    $node = is_numeric($node) ? node_load($node) : $node;

    $parent_path = $this->_findParentPath($node);
    if ($parent_path) {
      return array($node->type => $parent_path);
    }
  }
}


class _taxonomy_CrumbsMultiPlugin_termReference_user extends _taxonomy_CrumbsMultiPlugin_termReference {

  protected $entityType = 'user';

  /**
   * Match "user/%" router path
   */
  function findParent__user_x($path, $item) {
    $user = $item['map'][1];
    // Load the user if it hasn't been loaded due to a missing wildcard loader.
    $user = is_numeric($user) ? user_load($user) : $user;

    $parent_path = $this->_findParentPath($user);
    if ($parent_path) {
      // TODO: Is 'user' the correct bundle name?
      return array('user' => $parent_path);
    }
  }
}


class _taxonomy_CrumbsMultiPlugin_termReference_commerce_product extends _taxonomy_CrumbsMultiPlugin_termReference {

  protected $entityType = 'commerce_product';

  /**
   * Match "product/%" router path.
   * This works only in combination with commerce_product_page module.
   */
  function findParent__product_x($path, $item) {
    $product = $item['map'][1];
    // Load the product if it hasn't been loaded due to a missing wildcard loader.
    $product = is_numeric($product) ? commerce_product_load($product) : $product;

    $parent_path = $this->_findParentPath($product);
    if ($parent_path) {
      // TODO: Is 'user' the correct bundle name?
      return array($product->type => $parent_path);
    }
  }
}


// -----------------------------------------------------------------------------


class taxonomy_CrumbsMultiPlugin_termParent implements crumbs_MultiPlugin {

  function describe($api) {
    foreach (taxonomy_get_vocabularies() as $voc_id => $voc) {
      $api->addRule($voc->machine_name, 'Vocabulary: '. $voc->name);
    }
    // Now set a generic title for the entire plugin.
    $api->addRule('*', t('Set taxonomy/term/123 as the parent for taxonomy/term/456, if 123 is the parent term of 456.'));
  }

  /**
   * Terms get their parent terms as breadcrumb parent.
   * The method name matches the router path "taxonomy/term/%".
   */
  function findParent__taxonomy_term_x($path, $item) {
    $term = $item['map'][2];
    // Load the term if it hasn't been loaded due to a missing wildcard loader.
    $term = is_numeric($term) ? taxonomy_term_load($term) : $term;

    $parents = taxonomy_get_parents($term->tid);
    $result = array();
    foreach ($parents as $parent_tid => $parent_term) {
      if ($parent_term->vocabulary_machine_name == $term->vocabulary_machine_name) {
        $uri = entity_uri('taxonomy_term', $parent_term);
        if (!empty($uri)) {
          return array($term->vocabulary_machine_name => $uri['path']);
        }
      }
    }
  }
}
