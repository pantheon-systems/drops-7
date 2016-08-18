<?php

/**
 * Contains \MessageArgumentsBase.
 */

abstract class MessageArgumentsBase implements MessageArgumentInterface {

  /**
   * @var Message
   *
   * The message object.
   */
  protected $message;

  /**
   * {@inheritdoc}
   */
  public function getMessage() {
    return $this->message;
  }

  /**
   * {@inheritdoc}
   */
  public function setMessage(Message $message) {
    $this->message = $message;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function getArguments() {
    $arguments = array();
    $callbacks = $this->getNameArgument();

    foreach ($callbacks as $argument => $callback) {
      $arguments[$argument] = call_user_func($callback);
    }

    return $arguments;
  }

}
