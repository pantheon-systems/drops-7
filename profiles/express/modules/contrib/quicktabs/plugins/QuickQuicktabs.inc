<?php

/**
 * Renders the content using the original Quicktabs mechanism of previous versions.
 * Includes support for ajax rendered content.
 */
class QuickQuicktabs extends QuickRenderer {

  public function render() {
    $quickset = $this->quickset;

    $render_array = array();

    $active_tab = $quickset->getActiveTab();
    if ($tabs = $this->build_tablinks($active_tab)) {
      $render_array['#attached'] = $this->add_attached();

      $qt_name = $quickset->getName();
      $settings = $quickset->getSettings();
      $contents = $quickset->getContents();

      $render_array['content'] = array(
        '#theme' => 'qt_quicktabs',
        '#options' => array('attributes' => array(
          'id' => 'quicktabs-' . $qt_name,
          'class' => 'quicktabs-wrapper quicktabs-style-' . drupal_html_class($settings['style']),
        )),
        'tabs' => array('#theme' => 'qt_quicktabs_tabset', '#options' => array('active' => $active_tab, 'style' => drupal_html_class($settings['style'])), 'tablinks' => $tabs),
        // The main content area, each quicktab container needs a unique id.
        'container' => array(
          '#prefix' => '<div id="quicktabs-container-' . $qt_name .'" class="quicktabs_main quicktabs-style-' . drupal_html_class($settings['style']) .'">',
          '#suffix' => '</div>',
          'divs' => array(),
        ),
      );
      

    
      // If in ajax mode, we'll only be rendering one tab, otherwise all of them.
      if ($settings['ajax']){
        // check in case of $active_tab=QUICKTABS_DELTA_NONE
        $tabs_to_render = isset ($contents[$active_tab]) ? array($active_tab => $contents[$active_tab]) : array ();
      }else{
        $tabs_to_render = $contents;
      }
    
      foreach ($tabs_to_render as $key => $tab) {
        if (!empty($tab)) {
          $attribs = array(
            'id' => 'quicktabs-tabpage-'. $qt_name . '-'. $key,
            'class' => array('quicktabs-tabpage', ($active_tab == $key ? '' : 'quicktabs-hide')),
          );
          $render_array['content']['container']['divs'][] = array(
            '#prefix' => '<div '. drupal_attributes($attribs) .'>',
            '#suffix' => '</div>',
            'content' => $tab->render(),
          );
        }
      }
    }
    return $render_array;
  }

  /**
   * Build the actual tab links, with appropriate href, title and attributes.
   *
   * @param $active_tab The index of the active tab.
   */
  protected function build_tablinks($active_tab) {
    $quickset = $this->quickset;
    $settings = $quickset->getSettings();
    $tabs = array();
    foreach ($quickset->getContents() as $i => $tab) {
      if (!empty($tab)) {
        $tablink = array(
          '#type' => 'link',
          '#title' => $quickset->translateString($tab->getTitle(), 'tab', $i),
          '#href' =>  $_GET['q'],
          '#options' => $this->construct_link_options($i),
        );
        if ($settings['ajax']) {
          $tab_settings = $tab->getSettings();
          $ajax_keys = $tab->getAjaxKeys();
          $ajax_args = array();
          foreach ($ajax_keys as $key) {
            $ajax_args[] = $tab_settings[$key];
          }
          $ajax_path = $quickset->getAjaxPath($i, $tab->getType());
          $ajax_href = $ajax_path . '/'. implode('/', $ajax_args);
          $tablink['#ajax'] = array(
            'progress' => array('message' => '', 'type' => 'throbber'),
            'path' => $ajax_href,
          );
        }
        $tabs[$i] = $tablink;
      }
    }
    return $tabs;
  }

  /**
   * Add any necessary js, css and libraries for the render array.
   */
  protected function add_attached() {
    $attached = array(
      'css' => array(
        array('data' => drupal_get_path('module', 'quicktabs') .'/css/quicktabs.css'),
      ),
      'js' => array(
        array('data' => drupal_get_path('module', 'quicktabs') . '/js/quicktabs.js'),
        array('data' => 'misc/progress.js', 'weight' => JS_LIBRARY),
      ),
    );
    $settings = $this->quickset->getSettings();
    // Add the custom style css if a custom style has been set.
    $style_css = quicktabs_get_css($settings['style']);
    if (!empty($style_css)) {
      $attached['css'][] = $style_css;
    }
    // Prepare a tab_settings array for passing the tab info to our JavaScript.
    $tab_settings = array();
    foreach ($this->quickset->getContents() as $i => $content) {
      if (!empty($content)) {
        $tab_settings[$i] = $content->getSettings();
      }
    }
    // Add our JS settings
    $javascript = drupal_add_js();
    if (isset($javascript['settings']['data'])) {
      foreach ($javascript['settings']['data'] as $key => $settings) {
        if (key($settings) == 'quicktabs') {
          $qtkey = $key;
          break;
        }
      }
    }
    $name = $this->quickset->getName();
    if (!isset($qtkey) || (isset($javascript['settings']['data'][$qtkey]['quicktabs'])
      && !array_key_exists('qt_' . $name, $javascript['settings']['data'][$qtkey]['quicktabs']))) {
      $quicktabs_array = array_merge(array('name' => $name, 'tabs' => $tab_settings), $settings);
      $attached['js'][] = array('data' => array('quicktabs' => array('qt_' . $name => $quicktabs_array)), 'type' => 'setting');
    }
    return $attached;
  }

  /**
   * Helper function to construct link options for tab links.
   */
  protected function construct_link_options($tabkey) {
    $qt_name = $this->quickset->getName();
    $settings = $this->quickset->getSettings();
    $contents = $this->quickset->getContents();
    $tab_type = $contents[$tabkey]->getType();
    $tab_unique_keys = $contents[$tabkey]->getUniqueKeys();
    $tab_settings = $contents[$tabkey]->getSettings();

    $id = 'quicktabs-tab-' . implode('-', array($qt_name, $tabkey));
    $classes = array(
      'quicktabs-tab',
      'quicktabs-tab-' . $tab_type,
    );
    // Generate unique class for the quicktab.
    $key_contents = array();
    foreach ($tab_unique_keys as $key) {
      if (isset($tab_settings[$key])) {
        $key_content = $tab_settings[$key];
        $key_content = str_replace(array( '_', ' ' ), '-', $key_content);
        $key_content = preg_replace('/[^A-Za-z0-9\-]/', '', $key_content);
        $key_contents[] = $key_content;
      }
    }
    $suffix = implode('-', $key_contents);
    // Encode class suffixes longer than 50 as hash using md5.
    if (strlen($suffix) > 50) {
      $suffix = md5($suffix);
    }
    if (strlen($suffix) > 0) {
      $classes[] = 'quicktabs-tab-' . $tab_type . '-' . $suffix;
    }

    // Need to construct the correct querystring for the tab links.
    $query = drupal_get_query_parameters(NULL, array("qt-$qt_name", 'q', 'page'));
    $query["qt-{$qt_name}"] = $tabkey;

    $link_options = array(
      'attributes' => array(
        'id' => $id,
        'class' => $classes,
      ),
      'query' => $query,
      'fragment' => 'qt-' . $qt_name,
      'html' => isset($settings['html']) ? $settings['html'] : FALSE,
    );
    return $link_options;
  }
}
