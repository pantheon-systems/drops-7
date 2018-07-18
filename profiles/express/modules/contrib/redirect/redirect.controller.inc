<?php


/**
 * Controller class for redirects.
 *
 * This extends the DrupalDefaultEntityController class, adding required
 * special handling for redirect objects.
 */
class RedirectController extends DrupalDefaultEntityController {

  protected function attachLoad(&$redirects, $revision_id = FALSE) {
    // Unserialize the URL option fields.
    foreach ($redirects as $key => $redirect) {
      $redirects[$key]->source_options = unserialize($redirect->source_options);
      $redirects[$key]->redirect_options = unserialize($redirect->redirect_options);
    }
    parent::attachLoad($redirects, $revision_id);
  }
}
