<?php


function crumbs_debug_page() {
  drupal_set_title('Crumbs debug');
  $path = isset($_GET['path_to_test']) ? $_GET['path_to_test'] : '';
  $path_checked = check_plain($path);
  $form_action = url('admin/structure/crumbs/debug');

  $input_html = <<<EOT
<input id="path" size="40" name="path_to_test" value="$path_checked"/>
EOT;
  $input_html = t('Path: !text_field', array('!text_field' => $input_html));
  $text = <<<EOT
<p>This page allows to have a look "behind the scenes", so you can analyse
which crumbs plugins and rules are responsible for the "crumbs parent" to a
given system path.</p>
<p>Crumbs will "ask" different plugins, until one of them responds with a parent
path candidate. The candidate with the highest weight wins.</p>
<p>Crumbs will stop invoking any more plugins, once it is clear that further
candidates will only have smaller weights.</p>
EOT;
  $text = t($text);

  $html = <<<EOT
    <form method="get" action="$form_action">
      $text
      <label for="path">$input_html</label>
    </form>
EOT;
  if (strlen($path)) {
    crumbs_benchmark();
    $log = array();

    $path = drupal_get_normal_path($path);

    $log[] = crumbs_benchmark('drupal_get_normal_path()');

    $item = crumbs_get_router_item($path);

    $log[] = crumbs_benchmark('crumbs_get_router_item("' . $path . '")');

    $parent_finder = crumbs_get_parent_finder();

    $log[] = crumbs_benchmark('crumbs_get_parent_finder()');

    $parent_path = $parent_finder->getParentPath($path, $item);

    $log[] = crumbs_benchmark('$finder->getParentPath()');
    $log_html = implode('<br/>', $log);

    $log = $parent_finder->getLoggedCandidates($path);
    $rows = array();
    $sort = array();
    $rule_found = FALSE;
    foreach ($log as $key => $x) {
      list($candidate, $weight) = $x;
      $key_html = $key;
      if ($key === @$item['crumbs_candidate_key']) {
        $key_html = '<strong>'. $key_html .'</strong>';
        $candidate = '<strong>'. $candidate .'</strong>';
        $rule_found = TRUE;
      }
      if ($weight === FALSE) {
        $key_html = '<strike>'. $key_html .'</strike>';
        $candidate = '<strike>'. $candidate .'</strike>';
        $weight = 'disabled';
      }
      else if (is_null($weight)) {
        $candidate = '-';
        $key_html = '<i>'. $key_html .'</i>';
        $weight = '-';
      }
      $rows[$key] = <<<EOT
<tr><td>$weight</td><td>$key_html</td><td>$candidate</td></tr>
EOT;
      $sort[$key] = $weight;
    }

    // array_multisort($sort, $rows);
    $rows_html = implode('', $rows);
    $parent_path_checked = check_plain($parent_path);
    if (!$rule_found) {
      $default_text = t('Default: Chop off last path fragment.');
      $rows_html .= <<<EOT
<tr><td></td><td>$default_text</td><td>$parent_path_checked</td></tr>
EOT;
    }
    $text = t('Parent found: !parent_path', array(
      '!parent_path' => l($parent_path, 'admin/structure/crumbs/debug', array(
        'query' => array('path_to_test' => $parent_path)
      )),
    ));
    $html .= <<<EOT
      <p>$text</p>
      <p>$log_html</p>
      <table>
        <thead><tr><th>Weight</th><th>Key</th><th>Parent path candidate</th></tr></thead>
        <tbody>$rows_html</tbody>
      </table>
EOT;
  }
  return $html;
}
