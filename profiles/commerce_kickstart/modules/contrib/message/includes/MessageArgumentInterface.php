<?php

/**
 * Contains MessageArgumentInterface.
 */

interface MessageArgumentInterface {

  /**
   * @return Message
   */
  public function getMessage();

  /**
   * @param Message $message
   *   The message object.
   *
   * @return MessageArgumentsBase
   */
  public function setMessage(Message $message);

  /**
   * Retrieve the arguments info.
   *
   * @return array
   *   The arguments as and their values.
   */
  public function getArguments();

  /**
   * The method return information about the arguments for the message and the
   * callbacks which responsible to compute the argument value.
   *
   * @return array
   *   Associate array with the name of the argument and the callback
   *   responsible to compute the argument value.
   *
   * @code
   * return array(
   *  '@name' => array($this, 'processName'),
   *  '%time' => array($this, 'processTime'),
   *  '!link' => array($this, 'processLink'),
   * );
   * @endcode
   *
   * The callback will return the value for the argument. The message object can
   * be access via the getMessage() method.
   */
  public function getNameArgument();
}
