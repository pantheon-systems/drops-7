<?php
/**
 * @file
 * Defines a context condition for the current environment state.
 */

/**
 * Defines a context condition for the current environment state.
 */
class environment_context_condition extends context_condition {
  /**
   * Override for context_condition::condition_values().
   */
  function condition_values() {
    return _environment_state_options_all();
  }

  /**
   * Override for context_condition::execute().
   */
  function execute($environment) {
    if ($this->condition_used()) {
      // Workflow should be a unique key, $env is unique but not it's title.
      foreach ($environment as $workflow => $env) {
        foreach ($this->get_contexts($env) as $context) {
          $this->condition_met($context, $env);
        }
        foreach ($this->get_contexts($workflow) as $context) {
          $this->condition_met($context, $workflow);
        }
      }
    }
  }
}
