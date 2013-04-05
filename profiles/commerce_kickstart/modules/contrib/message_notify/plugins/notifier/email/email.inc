<?php

$plugin = array(
  'title' => t('Email'),
  'description' => t('Send Message via email.'),
  'class' => 'MessageNotifierEmail',
  'options' => array(
    // Override mail. Don't use the email that is assigned to the user.
    'mail' => FALSE,
  ),
   // A notifier must define its own view modes.
   // Those view modes are later going to be rendered and sent.
  'view_modes' => array(
    'message_notify_email_subject' => array('label' => t('Notify - Email subject')),
    'message_notify_email_body' => array('label' => t('Notify - Email body')),
  ),
);
