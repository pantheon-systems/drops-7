<?php

/**
 * Implements hook_rules_event_info().
 */
function mailjet_event_rules_event_info() {
  $event = array();

  $event['mailjet_open'] = array(
    'label' => t('Open'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Event'),
        'type' => 'mailjet_event'
      ),
    ),
  );

  $event['mailjet_click'] = array(
    'label' => t('Click'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Mailjet Event'),
        'type' => 'mailjet_event',
      ),
    ),
  );

  $event['mailjet_bounce'] = array(
    'label' => t('Bounce'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Mailjet Event'),
        'type' => 'mailjet_event',
      ),
    ),
  );

  $event['mailjet_spam'] = array(
    'label' => t('Spam'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Event'),
        'type' => 'mailjet_event'
      ),
    ),
  );

  $event['mailjet_blocked'] = array(
    'label' => t('Blocked'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Event'),
        'type' => 'mailjet_event'
      ),
    ),
  );

  $event['mailjet_unsub'] = array(
    'label' => t('Unsubscribe'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Event'),
        'type' => 'mailjet_event'
      ),
    ),
  );

  $event['mailjet_typofix'] = array(
    'label' => t('Typo'),
    'group' => 'Mailjet',
    'variables' => array(
      'mailjet_event' => array(
        'label' => t('Event'),
        'type' => 'mailjet_event'
      ),
    ),
  );

  return $event;
}
