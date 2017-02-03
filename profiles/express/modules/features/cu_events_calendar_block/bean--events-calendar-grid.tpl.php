<?php
  $grid_id = 'events-grid-' . rand();

  $categories = $content['field_event_categories'];
  $children = array_intersect_key($categories, element_children($categories));
  foreach ($children as $child) {
    $eventcategories[] = $child['#markup'];
  }
  $categories = join('%2c', $eventcategories);
?>
<div class="events-calendar-grid" id="<?php print $grid_id; ?>">
<?php
  if (($content['field_event_months'][0]['#markup'] > 1) && ($content['field_event_show_all_grids'][0]['#markup'] == 0)):
?>
  <div class="events-grid-nav">
    <a href="#<?php print $grid_id; ?>" class="grid-prev">&laquo; <span>Previous</span></a>  <a href="<?php print $grid_id; ?>" class="grid-next"><span>Next</span> &raquo;</a>
  </div>
<?php endif; ?>

<script language="javascript" src="//events.colorado.edu/minicalsyndicator.aspx?type=<?php print $content['field_event_type'][0]['#markup']; ?>&numberofmonths=<?php print $content['field_event_months'][0]['#markup']; ?>&winmode=S&category=<?php print $categories; ?>&view=Grid"></script>

<?php
  if (($content['field_event_months'][0]['#markup'] > 1) && ($content['field_event_show_all_grids'][0]['#markup'] == 0)):
?>

<script language="javascript">
  var current = 0;
  var total = jQuery("#<?php print $grid_id; ?> table").length;
  jQuery("#<?php print $grid_id; ?> table:gt(0)").hide();
  jQuery(".events-grid-nav a.grid-prev").addClass("events-nav-disabled");
  jQuery(".events-grid-nav a.grid-next").click(function () {
    current++;
    if (current < total) {
      jQuery(".events-grid-nav a.grid-prev").removeClass("events-nav-disabled");
      jQuery("#<?php print $grid_id; ?> table").hide();
      jQuery("#<?php print $grid_id; ?> table:eq(" + current + ")").show();
    }
    if (current == (total-1)) {
      jQuery(this).addClass("events-nav-disabled");
    }

    return false;
  });
  jQuery(".events-grid-nav a.grid-prev").click(function () {
    current--;
    if (current > -1) {
      jQuery(".events-grid-nav a.grid-next").removeClass("events-nav-disabled");
      jQuery("#<?php print $grid_id; ?> table").hide();
      jQuery("#<?php print $grid_id; ?> table:eq(" + current + ")").show();
    }
    if (current == 0) {
      jQuery(this).addClass("events-nav-disabled");
    }

    return false;
  });
</script>

<?php endif; ?>

</div>
