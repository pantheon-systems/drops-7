<?php

/**
 * @file
 * Hooks provided by the Elysia cron module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * You can extend cron functionality in you modules by using elysia_cron api.
 *
 * With it you can:
 * - have more than one cron job per module
 * - have a different schedule rule for each cron job defined
 * - set a description for each cron job.
 *
 * To do this you should add in you module a new hook. This is the syntax:
 *
 * - 'key' is the identifier for the task you are defining.
 *  You can define a timing for the standard cron hook of the module by using
 *  the "MODULENAME_cron" key. (See examples).
 *
 * - description:
 *  a textual description of the job, used in elysia cron's status
 *  page. Use the untranslated string, without the "t()" wrapper (elysia_cron
 *  will apply it)
 *
 * - rule:
 *  the crontab rule. For example: "0 * * * *" to execute the task every hour.
 *
 * - weight (optional):
 *  a numerical value to define order of execution. (Default:0)
 *
 * - callback (optional):
 *  you can define here a name of a PHP function that should
 *  by called to execute the task. This is not mandatory: if you don't specify
 *  it Elysia cron will search for a function called like the task KEY.
 *  If this function is not found, Elysia cron will call the "hook_cronapi"
 *  function with $op = 'execute' and $job = 'KEY' (the key of the task).
 * (See examples)
 *
 * - arguments (optional):
 *  an array of arguments passed to callback (only if callback is defined).
 *
 * - file/file path:
 *  the PHP file that contains the callback (hook_menu's syntax).
 *
 * @param string $op
 *   Operation: "list" or "execute".
 * @param string|null $job
 *   Name of current job or it is NULL if we define job list.
 *
 * @return array
 *   Job list.
 */
function hook_cronapi($op, $job = NULL) {
  // General example of all parameters.
  $items['key'] = array(
    'description' => 'string',
    'rule' => 'string',
    'weight' => 1234,
    'callback' => 'function_name',
    'arguments' => array('first', 'second', 3),
    // External file, like in hook_menu.
    'file' => 'string',
    'file path' => 'string',
  );

  // Run function example_sendmail_cron() every 2 hours.
  // Note: i don't need to define a callback, i'll use "example_sendmail_cron"
  // function.
  $items['example_sendmail_cron'] = array(
    'description' => 'Send mail with news',
    'rule' => '0 */2 * * *',
  );

  // Run example_news_fetch('all') every 5 minutes.
  // Note: this function has argument.
  $items['example_news_cron'] = array(
    'description' => 'Send mail with news',
    'rule' => '*/5 * * * *',
    'callback' => 'example_news_fetch',
    'arguments' => array('all'),
  );

  // Definition of rules list and embedded code.
  if ($op == 'list') {
    // Rules list.
    $items['job1'] = array(
      'description' => 'Send mail with news',
      'rule' => '0 */2 * * *',
    );

    $items['job2'] = array(
      'description' => 'Send mail with news',
      'rule' => '*/5 * * * *',
    );
  }
  elseif ($op == 'execute') {
    // Embedded code.
    switch ($job) {
      case 'job1':
        // ... job1 code.
        break;

      case 'job2':
        // ... job2 code.
        break;
    }
  }

  return $items;
}

/**
 * Altering hook_cron definition.
 *
 * You can use the "hook_cron_alter" function to edit cronapi data of other
 * modules.
 *
 * @param array $data
 *   Array of cron rules.
 */
function hook_cron_alter(&$data) {
  $data['key']['rule'] = '0 */6 * * *';
}

/**
 * @} End of "addtogroup hooks".
 */
