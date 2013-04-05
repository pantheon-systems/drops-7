<?php

/**
 * @file
 * Provides support for the Views module.
 */
/**
 * "Message" entity Views definition.
 */
class MessageViewsController extends EntityDefaultViewsController {

  /**
   * Override views_data().
   */
  public function views_data() {
    $data = parent::views_data();

    $data['message']['message_render']['field'] = array(
      'title' => 'Render message (Get text)',
      'real field' => 'mid',
      'help' => t('Show the message by replacing any tokens with their real value.'),
      'handler' => 'message_handler_field_message_render',
    );

    $data['message']['type']['filter']['handler'] = 'message_handler_filter_message_type';
    $data['message']['type_category'] = array(
      'title' => 'Message category',
      'help' => 'Filter messages by message type category.',
      'filter' => array(
        'handler' => 'message_handler_filter_message_type_category',
        'real field' => 'type',
      )
    );

    return $data;
  }
}
