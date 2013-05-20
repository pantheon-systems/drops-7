<?php
/**
 * @file
 * Developer Utilities
 */

function lingotek_dev_page($node = NULL) {
  $nid = '';
  if (isset($node)) {
    $nid = $node->nid;
  }
  return drupal_get_form('lingotek_dev_form', $nid);
}

function lingotek_dev_form($ignore, $param) {
  $nid = $param['build_info']['args'][0];

  $form['dev'] = array(
    '#type' => 'fieldset',
    '#title' => t('LingoNode'),
    '#description' => t('Modify the contents of a LingoNode'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['dev']['nid'] = array(
    '#type' => 'textfield',
    '#title' => t('nid'),
    '#description' => t('Drupal Node Id'),
    '#default_value' => $nid,
  );
  $form['dev']['key'] = array(
    '#type' => 'textfield',
    '#title' => t('lingokey'),
    '#description' => t('Key for the LingoNode'),
    '#default_value' => '',
  );
  $form['dev']['value'] = array(
    '#type' => 'textfield',
    '#title' => t('lingovalue'),
    '#description' => t('Value for the LingoNode'),
    '#default_value' => '',
  );
  $form['actions']['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Save/Edit'),
    '#weight' => 15,
    '#submit' => array('lingotek_dev_submit'),
  );

  $form['data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Data'),
    '#description' => t('Contents of the LingoNodes'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $header = array('nid', t('Key'), t('Value'));
  $rows = array();
  if ($nid == '') {
    $rs = lingotek_lingonode('all');
    foreach ($rs as $nid => $entry) {
      foreach ($entry as $key => $value) {
        $row = array(
          $nid,
          $key,
          $value,
        );
        $rows[] = $row;
      }
    }
  }
  else {
    $rs = lingotek_lingonode($nid);
    $header = array(t('Key'), t('Value'));
    foreach ($rs as $key => $value) {
      $row = array(
        $key,
        $value,
      );
      $rows[] = $row;
    }
  }

  $form['data']['value'] = array
  (
    '#type' => 'item',
    '#markup' => theme('table', array('header' => $header, 'rows' => $rows)),
  );

  return $form;
}


function lingotek_dev_submit($form) {
  $nid = intval($form['dev']['nid']['#value']);
  $key = $form['dev']['key']['#value'];
  $value = $form['dev']['value']['#value'];
  drupal_set_message( t('Results: @dump', array( '@dump' => lingotek_dump(lingotek_lingonode($nid, $key, $value)) ) ) );
}
