<?php

/**
 * @file
 * TODO: add file doco.
 * More power!
 */

if (isset ($content['field_event_categories'])) :
    $categories = $content['field_event_categories'];
    $children = array_intersect_key($categories, element_children($categories));

    foreach($children as $child):
      $eventcategories[] = $child['#markup'];
    endforeach;
    $categories = implode('%2c', $eventcategories);
  if ($content['field_event_expire'][0]['#markup'] == 'yes'):
      $expire = 'Y';
    else:
      $expire = 'N';
    endif;
    $number = '&number=' . $content['field_event_number'][0]['#markup'];

    $date_range = '';

    if (!empty($content['field_event_date_range'])) :
      $start = $content['field_event_date_range']['#items'][0]['value'];
      $end = $content['field_event_date_range']['#items'][0]['value2'];

      $start = date('n/j/Y', strtotime($start));
      $end = date('n/j/Y', strtotime($end));

      $date_range = '&starting=' . $start . '&ending=' . $end;
      $number = '&number=1000';
  endif;

  $q = 'type=' . $content['field_event_type'][0]['#markup'] . $number . $date_range . '&category=' . $categories . '&expire=' . $expire . '&ics=' . $content['field_event_subcategories'][0]['#markup'];
  $t = $content['field_event_template'][0]['#markup'];
  $cal_id = 'adxevents' . $bean->bid; ?>

  <div id="<?php print $cal_id; ?>" class="events-calendar-template-<?php print $t; ?>">Loading Events...</div>
  <script type="text/javascript">if (typeof jQuery != 'undefined') {
    jQuery(document).ready(function () {
        var adx="Events are temporarily unavailable. Please check back later.";
        jQuery.ajax({ dataType: 'script', url: '//events.colorado.edu/EventListSyndicator.aspx?<?php print $q; ?>&adpid=<?php print $t; ?>&nem=No+events+are+available+that+match+your+request&sortorder=ASC&ver=2.0&target=<?php print $cal_id; ?>'
              });setTimeout(function () {if (typeof response=='undefined') {jQuery('#<?php print $cal_id; ?>').html(adx);}}, 5000);
      });} else { document.getElementById('<?php print $cal_id; ?>').innerHTML = 'Events are temporarily unavailable because the jQuery library cannot be located.'; }</script>
<?php
endif;
?>
<?php if (!empty($content['field_event_link'])): ?>
  <p><?php print render($content['field_event_link']); ?></p>
<?php endif;
?>
