<?php

/**
 * @file
 * Custom classes used with the Metatag:Views module.
 */

if (class_exists('i18n_string_object_wrapper')) {

  /**
   * I18n object overrides.
   */
  class metatag_views_i18n_metatag extends i18n_string_object_wrapper {

    /**
     * Get base keys for translating this object.
     */
    public function get_string_context() {
      // This will result in a context like
      // 'metatag_views:view_name__page:description' for the 'description' tag
      // on the 'page' display of the 'view_name' view.
      return array('metatag_views', $this->object->view_id . METATAG_VIEWS_CONTEXT_SEPARATOR . $this->object->id);
    }

  }
}
