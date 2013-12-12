<?php


function crumbs_debug_page() {
  drupal_set_title('Crumbs debug');
  $path = isset($_GET['path_to_test']) ? $_GET['path_to_test'] : '';
  $path_checked = check_plain($path);
  $form_action = url('admin/structure/crumbs/debug');

  $input_html = <<<EOT
<input id="path" type="text" class="form-text" size="40" name="path_to_test" value="$path_checked"/>
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

    $engine = crumbs_get_plugin_engine();

    $log[] = crumbs_benchmark('crumbs_get_plugin_engine()');

    $op = new crumbs_PluginOperation_findTitle($path, $item, array());
    $engine->invokeAll_find($op);
    $title = $op->getValue();
    $title_candidates = $op->getLoggedCandidates();
    $title_candidate_key = $op->getCandidateKey();

    $log[] = crumbs_benchmark('find title');

    $op = new crumbs_PluginOperation_findParent($path, $item, array());
    $engine->invokeAll_find($op);
    $parent_path = $op->getValue();
    $parent_path_candidates = $op->getLoggedCandidates();
    $parent_path_candidate_key = $op->getCandidateKey();
    $parent_path = drupal_get_normal_path($parent_path);
    $parent_item = crumbs_get_router_item($parent_path);

    $log[] = crumbs_benchmark('find parent');

    if (!empty($parent_item)) {
      $op = new crumbs_PluginOperation_findTitle($parent_path, $parent_item, array());
      $engine->invokeAll_find($op);
      $parent_title = $op->getValue();
      $parent_title_candidates = $op->getLoggedCandidates();
      $parent_title_candidate_key = $op->getCandidateKey();
    }

    $log[] = crumbs_benchmark('find parent title');

    $log_html = implode('<br/>', $log);

    $default_row = array(
      '',
      '(default)',
      $item['title'],
      crumbs_reduce_path($path),
      $parent_item['title'],
    );

    $sort = array();
    $rule_found = FALSE;
    foreach (array('title', 'parent_path', 'parent_title') as $i => $k) {
      if (!isset(${$k . '_candidates'})) {
        continue;
      }
      if (!isset($$k)) {
        $default_row[$i + 2] = '<strong>' . $default_row[$i + 2] . '</strong>';
      }
      foreach (${$k . '_candidates'} as $key => $x) {
        list($candidate, $weight) = $x;
        if (FALSE === $weight) {
          // This one is disabled. Continue.
          $weight = 'FALSE';
        }
        elseif (is_null($weight)) {
          $weight = '';
        }
        elseif (!is_numeric($weight)) {
          $weight = '?';
        }
        if (!isset($sort[$weight][$key])) {
          $sort[$weight][$key] = array(
            $weight,
            $key,
            '',
            '',
            '',
          );
        }
        if (FALSE === $candidate) {
          $candidate = 'FALSE';
        }
        elseif (is_null($candidate)) {
          $candidate = '-';
        }
        if ($key === ${$k . '_candidate_key'}) {
          $candidate = '<strong>' . check_plain($candidate) . '</strong>';
        }
        $sort[$weight][$key][$i + 2] = $candidate;
      }
    }

    ksort($sort);
    $all = array();
    foreach ($sort as $weight => $rows) {
      foreach ($rows as $key => $row) {
        $all[] = $row;
      }
    }

    // Default row.
    $all[] = $default_row;

    $head = array(
      'Weight',
      'Key',
      'Title candidate',
      'Parent path candidate',
      'Parent title candidate',
    );
    $table = theme('table', array(
      'header' => $head,
      'rows' => $all,
    ));

    $html .= <<<EOT
      <p>$text</p>
      <p>$log_html</p>
      $table
EOT;
  }
  return $html;
}
