<?php

/**
 * @file
 * Contains SearchApiAlterAddUserContent.
 */

/**
 * Adds the nodes created by the indexed user for indexing.
 */
class SearchApiAlterAddUserContent extends SearchApiAbstractAlterCallback {

  /**
   * {@inheritdoc}
   */
  public function supportsIndex(SearchApiIndex $index) {
    return $index->getEntityType() === 'user';
  }

  /**
   * {@inheritdoc}
   */
  public function propertyInfo() {
    return array(
      'search_api_user_content' => array(
        'label' => t('User content'),
        'description' => t('The nodes created by this user'),
        'type' => 'list<node>',
      ),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function alterItems(array &$items) {
    $uids = array();
    foreach ($items as $item) {
      $uids[] = $item->uid;
    }

    $sql = 'SELECT nid, uid FROM {node} WHERE uid IN (:uids)';
    $nids = db_query($sql, array(':uids' => $uids));
    $user_nodes = array();
    foreach ($nids as $row) {
      $user_nodes[$row->uid][] = $row->nid;
    }

    foreach ($items as $item) {
      $item->search_api_user_content = array();
      if (!empty($user_nodes[$item->uid])) {
        $item->search_api_user_content = $user_nodes[$item->uid];
      }
    }
  }

}
