<?php

/**
 * @file
* Defines default rule examples for MailJet events
*/

/**
 * Implements hook_default_rules_configuration().
*/
function mailjet_event_default_rules_configuration() {
  $rules = array();

  $rule = rules_reaction_rule();
  $rule->label = t('Mailjet Bounce Event');
  $rule->tags = array('Mailjet');
  $rule->active = TRUE;

  $rule->event('mailjet_bounce');

  $rules['mailjet_event_bounce'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Mailjet Bounce Blocked');
  $rule->tags = array('Mailjet');
  $rule->active = TRUE;

  $rule->event('mailjet_blocked');

  $rules['mailjet_event_blocked'] = $rule;

  $rule = rules_reaction_rule();
  $rule->label = t('Mailjet Bounce Spam');
  $rule->tags = array('Mailjet');
  $rule->active = TRUE;

  $rule->event('mailjet_spam');

  $rules['mailjet_event_spam'] = $rule;

  return $rules;
}