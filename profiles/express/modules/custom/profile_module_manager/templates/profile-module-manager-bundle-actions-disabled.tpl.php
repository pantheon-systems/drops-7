<?php
/* Variables:
 *
 * $enable_url : url to enable bundle
 * $demo_url : url for demo/more info of bundle
 */

?>

<?php
  $links = array();
  if (isset($enable_url)) {
    $links[] =  l(t('Enable'), $enable_url, array('attributes' => array('class' => array('bundle-enable-link')), 'html' => TRUE));
  }
  if (isset($demo_url)) {
    $links[] = l(t('More Information'), $demo_url, array('attributes' => array('class' => array('bundle-demo-link'), 'target' => '_blank'), 'html' => TRUE));
  }
  print join(' | ', $links);
?>
