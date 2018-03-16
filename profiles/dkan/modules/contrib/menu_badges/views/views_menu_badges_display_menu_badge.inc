<?php
/**
 * @file
 * Menu Badges Views plugin.
 */

/**
 * Implementation of views_plugin_display().
 */
class views_menu_badges_display_menu_badge extends views_plugin_display_default {

  function execute() {
    return strip_tags($this->view->render($this->display->id));
  }
}