<?php

/**
 * Test notifier.
 */
class MessageNotifierTest extends MessageNotifierBase {

  public function deliver(array $output = array()) {
    $this->message->output = $output;
    // Return TRUE or FALSE as it was set on the Message.
    return empty($this->fail);
  }

}
