<?php

/**
 * An interface for pluggable mail back-ends.
 */
class MaillogMailSystem implements MailSystemInterface {
  /**
   * Format a message composed by drupal_mail() prior sending.
   *
   * @param $message
   *   A message array, as described in hook_mail_alter().
   *
   * @return
   *   The formatted $message.
   */
  public function format(array $message) {
    $mailclass = variable_get('maillog_engine', 'DefaultMailSystem');
    $default = new $mailclass();
    return $default->format($message);
  }

  /**
   * Send a message composed by drupal_mail().
   *
   * @param $message
   *   Message array with at least the following elements:
   *   - id: A unique identifier of the e-mail type. Examples: 'contact_user_copy',
   *     'user_password_reset'.
   *   - to: The mail address or addresses where the message will be sent to.
   *     The formatting of this string must comply with RFC 2822. Some examples:
   *     - user@example.com
   *     - user@example.com, anotheruser@example.com
   *     - User <user@example.com>
   *     - User <user@example.com>, Another User <anotheruser@example.com>
   *    - subject: Subject of the e-mail to be sent. This must not contain any
   *      newline characters, or the mail may not be sent properly.
   *    - body: Message to be sent. Accepts both CRLF and LF line-endings.
   *      E-mail bodies must be wrapped. You can use drupal_wrap_mail() for
   *      smart plain text wrapping.
   *    - headers: Associative array containing all additional mail headers not
   *      defined by one of the other parameters.  PHP's mail() looks for Cc
   *      and Bcc headers and sends the mail to addresses in these headers too.
   *
   * @return
   *   TRUE if the mail was successfully accepted for delivery, otherwise FALSE.
   */
  public function mail(array $message) {
    // Log the e-mail
    if (variable_get('maillog_log', TRUE)) {
      $record = new stdClass;

      // In case the subject/from/to is already encoded, decode with mime_header_decode
      $record->header_message_id = isset($message['headers']['Message-ID']) ? $message['headers']['Message-ID'] : NULL;
      $record->subject = $message['subject'];
      $record->subject = mime_header_decode($record->subject);
      $record->body = $message['body'];
      $record->header_from = isset($message['from']) ? $message['from'] : NULL;
      $record->header_from = mime_header_decode($record->header_from);

      $header_to = array();
      if (isset($message['to'])) {
        if (is_array($message['to'])) {
          foreach($message['to'] as $value) {
            $header_to[] = mime_header_decode($value);
          }
        }
        else {
          $header_to[] = mime_header_decode($message['to']);
        }
      }
      $record->header_to = implode(', ', $header_to);

      $record->header_reply_to = isset($message['headers']['Reply-To']) ? $message['headers']['Reply-To'] : '';
      $record->header_all = serialize($message['headers']);
      $record->sent_date = REQUEST_TIME;

      drupal_write_record('maillog', $record);
    }

    // Display the e-mail using Devel module
    if (variable_get('maillog_devel', TRUE) && function_exists('dpm')) {
      $devel_msg = array();
      $devel_msg[t('Subject')] = $message['subject'];
      $devel_msg[t('From')] = $message['from'];
      $devel_msg[t('To')] = $message['to'];
      $devel_msg[t('Reply-To')] = isset($message['reply_to']) ? $message['reply_to'] : NULL;
      $devel_msg[t('Header')] = $message['headers'];
      $devel_msg[t('Body')] = $message['body'];

      dpm($devel_msg, 'maillog');
    }

    if (variable_get('maillog_send', TRUE)) {
      $mailclass = variable_get('maillog_engine', 'DefaultMailSystem');
      $default = new $mailclass();
      $result = $default->mail($message);
    }
    elseif (user_access('administer maillog')) {
      $message = t('Sending of e-mail messages is disabled by Maillog module. Go <a href="@href">here</a> to enable.', array('@href' => url('admin/reports/maillog')));

      drupal_set_message($message, 'warning', TRUE);
    }
    else {
      global $user;
      watchdog('maillog', 'Attempted to send an email, but sending emails is disabled.');
    }
    return isset($result) ? $result : TRUE;
  }
}
