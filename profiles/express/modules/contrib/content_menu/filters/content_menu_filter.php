<?php
/**
 * Created by dreizwo.de.
 * User: jakobs
 * Date: 16.08.2012
 * Time: 14:00:29
 * @author markus jakobs <jakobs@dreizwo.de>
 */

interface content_menu_filter {

  /**
   * @abstract
   * @param  $el the menuitem
   * @return true if a element should be hidden
   */
  public function hideElement($el);

  /**
   * @abstract
   * @param  $form
   * @param  $form_state
   * @param  $form_id
   * @return add the filter widget,
   * to display the filter on top its recommended to add
   * $form['#content_menu_filter_widget'][] = 'name_of_filter_widget';
   * otherwise the filter will be displayed as child after the table
   */
  public function addFilterWidget(&$form, &$form_state, $form_id);
}
