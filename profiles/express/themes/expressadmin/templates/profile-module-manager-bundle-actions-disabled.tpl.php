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
    $links[] =  l('<i class="fa fa-plus-circle"></i> ' . t('Enable'), $enable_url, array('attributes' => array('class' => array('bundle-enable-link btn btn-primary')), 'html' => TRUE));
  }
  if (isset($demo_url)) {
    $links[] = l('<i class="fa fa-info-circle"></i> ' . t('More Information'), $demo_url, array('attributes' => array('class' => array('bundle-demo-link btn btn-default'), 'target' => '_blank'), 'html' => TRUE));
  }
  print join(' ', $links);
?>
