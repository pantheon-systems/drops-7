<?php

/**
 * @file
 * Entity definitions for MailUpList and MailUpSubscription
 */

class MailUpList extends Entity {

  public $id;
  public $guid;
  public $name;
  public $description;
  public $company;
  public $groups;
  public $created;
  public $changed;

  public function __construct($values = array()) {
    parent::__construct($values, 'mailup_list');
  }
  
}

class MailUpSubscription extends Entity {

  public $id;
  public $label;
  public $created;
  public $changed;
  public $language;

  public function __construct($values = array()) {
    parent::__construct($values, 'mailup_subscription');
  }
}

/**
 * The controller class for MailUp List contains methods for the
 * mailup_list CRUD operations. The load method is inherited from the default
 * controller.
 */
class MailUpListEntityController extends EntityAPIController {

  public function save($list) {

    if (empty($list->created)) {
      // Allow EntityAPIController to insert instead of update
      $list->is_new = TRUE;
      $list->created = REQUEST_TIME;
    }
    $list->changed = REQUEST_TIME;
    $list->groups = serialize($list->groups);

    return parent::save($list);
  }

  public function attachLoad(&$queried_entities, $revision_id = FALSE) {
    foreach ($queried_entities as $id => &$entity) {
      $entity->groups = unserialize($entity->groups);
    }

    parent::attachLoad($queried_entities, $revision_id);
  }

}


/* ============== Subscriptions =============== */


/**
 * The controller class for MailUp List contains methods for mailup_subscription
 * CRUD operations. The load method is inherited from the default controller.
 */
class MailUpSubscriptionEntityController extends EntityAPIController {

  public function save($mailup_subscription, DatabaseTransaction $transaction = NULL) {
    
    $transaction = isset($transaction) ? $transaction : db_transaction();

    if (empty($mailup_subscription->created)) {
      $mailup_subscription->created = REQUEST_TIME;
    }
    $mailup_subscription->changed = REQUEST_TIME;

    if (empty($mailup_subscription->language)) {
      $mailup_subscription->language = LANGUAGE_NONE;
    }

    return parent::save($mailup_subscription, $transaction);
  }

}

