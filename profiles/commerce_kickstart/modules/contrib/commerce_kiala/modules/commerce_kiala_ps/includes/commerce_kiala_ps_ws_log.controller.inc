<?php

/**
 * @file
 * The controller for the Web service log entity type.
 */

class CommerceKialaPSWSLogEntityController extends EntityAPIController {
  /**
   * Implements EntityAPIControllerInterface::create().
   */
  public function create(array $values = array()) {
    // merge in defaults
    $values += array(
      'timestamp' => REQUEST_TIME,
      'is_new' => empty($values[$this->idKey]),
      'status' => '',
    );
    return parent::create($values);
  }

  /**
   * Implements EntityAPIControllerInterface::save()
   *
   * @param $entity
   *   The full entity object to save.
   * @param $transaction
   *   Optionally a DatabaseTransaction object to use. Allows overrides to pass
   *   in their transaction object.
   *
   * @return
   *   The saved entity object.
   */
  public function save($entity, DatabaseTransaction $transaction = NULL) {
    $transaction = isset($transaction) ? $transaction : db_transaction();

    // determine is_new
    $is_new = TRUE;
    if (!empty($entity->{$this->idKey})) {
      // if have an id, always update
      unset($entity->is_new);
      $is_new = FALSE;
    }
    elseif (isset($entity->is_new)) {
      // let entity tell us if we're new
      $is_new = $entity->is_new;
    }

    // set status
    if (!empty($entity->response->status)) {
      $entity->status = $entity->response->status;
    }

    try {
      // set properties for new entities
      if ($is_new) {
        $entity->timestamp = REQUEST_TIME;
      }

      // clone for save so that we dont alter entity object to the serialized arrays
      $clone = clone($entity);
      $return = parent::save($clone, $transaction);

      // alter actual entity after successful save
      if ($return) {
        unset($entity->is_new);
        unset($entity->original);

        // add id
        if ($is_new) {
          $entity->{$this->idKey} = $clone->{$this->idKey};
        }
      }

      return $return;
    }
    catch (Exception $e) {
      $transaction->rollback();
      watchdog_exception($this->entityType, $e);
      throw $e;
    }
  }

  /**
   * Access callback
   */
  public function access($op = 'view', $entity = NULL, $account = NULL) {
    return entity_access($op, $this->entityType, $entity, $account);
  }


  /**
   * Builds a structured array representing the entity's content.
   *
   * The content built for the entity will vary depending on the $view_mode
   * parameter.
   *
   * @param $entity
   *   An entity object.
   * @param $view_mode
   *   View mode, e.g. 'administrator'
   * @param $langcode
   *   (optional) A language code to use for rendering. Defaults to the global
   *   content language of the current request.
   * @return
   *   The renderable array.
   */
  public function buildContent($entity, $view_mode = 'administrator', $langcode = NULL, $content = array()) {
    // Load the order this transaction is attached to.
    $order = commerce_order_load($entity->order_id);

    $rows = array(
      array(t('Log ID'), $entity->internalIdentifier()),
      array(t('Order', array(), array('context' => 'a drupal commerce order')), l(check_plain($order->order_number), 'admin/commerce/orders/' . $order->order_id)),
      array(t('Tracking Number'), !empty($entity->tracking_number) ? check_plain($entity->tracking_number) : ''),
      array(t('Method'), check_plain($entity->method)),
      array(t('Status'), check_plain($entity->status)),
      array(t('Created'), format_date($entity->timestamp)),
    );

    if (user_access('administer kiala packship ws')) {
      if (!empty($entity->response)) {
        $rows[] = array(t('Response'), '<pre>' . check_plain(print_r($entity->response, TRUE)) . '</pre>');
      }
    }

    $content['log_table'] = array(
      /*'#attached' => array(
        'css' => array(
          drupal_get_path('module', 'commerce_payment') . '/theme/commerce_payment.admin.css',
        ),
      ),*/
      '#markup' => theme('table', array('rows' => $rows, 'attributes' => array('class' => array('kiala-ps-ws-log')))),
    );

    return parent::buildContent($entity, $view_mode, $langcode, $content);
  }
}
