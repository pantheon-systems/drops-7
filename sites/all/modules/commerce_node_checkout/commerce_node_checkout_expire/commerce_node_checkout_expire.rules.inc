<?php
/**
 * @file
 *   Provides rules intergration.
 */

/**
 * Implements hook_rules_action_info().
 */
function commerce_node_checkout_expire_rules_action_info() {
  $actions = array();
  $actions['commerce_node_checkout_expire_rules_load_expiring_nodes'] = array(
    'label' => t('Load expiring nodes'),
    'parameter' => array(
      'timestamp' => array(
        'type' => 'date',
        'label' => t('Expiration time'),
        'description' => t('Enter the time the node will expire. Enter date and time in format: <em>dd-mm-yyyy hh:mm:ss</em> or use relative time i.e. now, 1 day, 2 months, 1 year, 3 years.'),
      ),
    ),
    'provides' => array(
      'nodes' => array(
        'type' => 'list<node>',
        'label' => t('Expiring Nodes'),
        'description' => t('The expiring nodes'),
      ),
    ),
    'group' => t('Commerce Node Checkout'),
  );
  $actions['commerce_node_checkout_expire_rules_filter_notification_nodes'] = array(
    'label' => t('Filter nodes for notifications'),
    'parameter' => array(
      'nodes' => array(
        'type' => 'list<node>',
        'label' => t('Expiring Nodes'),
        'description' => t('The expiring nodes'),
      ),
    ),
    'provides' => array(
      'nodes' => array(
        'type' => 'list<node>',
        'label' => t('Nodes for notifications'),
        'description' => t('The nodes that need expiration notifications sent for.'),
      ),
    ),
    'group' => t('Commerce Node Checkout'),
  );
  $actions['commerce_node_checkout_expire_rules_node_set_notification'] = array(
    'label' => t('Set node as notified'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => TRUE
      ),
    ),
    'group' => t('Commerce Node Checkout'),
  );
  $actions['commerce_node_checkout_expire_rules_node_unset_notification'] = array(
    'label' => t('Set node as not notified'),
    'parameter' => array(
      'node' => array(
        'type' => 'node',
        'label' => t('Content'),
        'save' => TRUE
      ),
    ),
    'group' => t('Commerce Node Checkout'),
  );
  $actions['commerce_node_checkout_expire_rules_set_line_item_expiration'] = array(
    'label' => t('Set line item expiration date'),
    'parameter' => array(
      'line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line item'),
      ),
    ),
    'group' => t('Commerce Node Checkout'),
  );
  return $actions;
}

/**
 * Rules action callback to load nodes that are expiring at a certain
 * time.
 */
function commerce_node_checkout_expire_rules_load_expiring_nodes($time_string) {
  return array('nodes' => commerce_node_checkout_expire_load_expiring_nodes($time_string));
}

/**
 * Rules action callback to filter a list of expiring nodes for those that
 * require notifications to be sent out for.
 */
function commerce_node_checkout_expire_rules_filter_notification_nodes($nodes) {
  // Iterate the nodes
  foreach ($nodes as $nid => $node) {
    // Create a wraper
    $wrapper = entity_metadata_wrapper('node', $node);

    // Extract the notifications field value
    $value = $wrapper->{COMMERCE_NODE_CHECKOUT_NOTIFICATION_FIELD}->value();

    // Skip this node if notifications are disabled or if they have
    // already been sent.
    if (!$value['enabled'] || $value['sent']) {
      unset($nodes[$nid]);
    }
  }

  return array('nodes' => $nodes);
}

/**
 * Rules action callback to set a node as having the expiration notification
 * message sent out.
 *
 * We do this so it will not keep getting sent out over and over again.
 */
function commerce_node_checkout_expire_rules_node_set_notification($node) {
  $node->{COMMERCE_NODE_CHECKOUT_NOTIFICATION_FIELD}[LANGUAGE_NONE][0]['sent'] = 1;
}

/**
 * Rules action callback to set a node as having the expiration notification
 * message not yet sent out.
 *
 * We do this so when the node is renewed, we know that we have to
 * send out the notification again.
 */
function commerce_node_checkout_expire_rules_node_unset_notification($node) {
  $node->{COMMERCE_NODE_CHECKOUT_NOTIFICATION_FIELD}[LANGUAGE_NONE][0]['sent'] = 0;
}

/**
 * Rules action callback to set the expiration date for a line item.
 */
function commerce_node_checkout_expire_rules_set_line_item_expiration($line_item) {
  // Create a wrapper for the line item
  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);

  // Extract the product
  $product = $line_item_wrapper->commerce_product->value();

  // Extract the node
  $node = $line_item_wrapper->commerce_node_checkout_node->value();

  // Load the interval field from the product.
  $interval = field_get_items('commerce_product', $product, 'commerce_node_checkout_expire');

  // See if an interval is set for this products
  if ($interval && !empty($interval[0])) {
    // Default the expiration time to be based off of the current time
    $expiration_start = 'now';

    // Load the last expiration time for this node
    if ($last_expiration = commerce_node_checkout_expire_get_node_expiration($node)) {
      // See if the last expiration time is still in the future
      if (REQUEST_TIME < $last_expiration) {
        // Use the last expiration time to base the new expiration time
        // of off. This allows a user to purchase additional time for a node
        // and have the time append to the remaining time.
        $expiration_start = $last_expiration;
      }
    }

    // Create the expiration date object
    $due_date_obj = new DateObject($expiration_start);

    // Apply the date interval
    if (interval_apply_interval($due_date_obj, $interval[0])) {
      // Update the expiry field on the line item.
      $line_item_wrapper->commerce_node_checkout_expires->set($due_date_obj->format('U'));
      $line_item_wrapper->save();
    }
  }
}
