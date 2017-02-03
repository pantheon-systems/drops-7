<?php
/**
 * @file
 * Template for Radix Pond.
 *
 * Variables:
 * - $css_id: An optional CSS id to use for the layout.
 * - $content: An array of content, each item in the array is keyed to one
 * panel of the layout. This layout supports the following sections:
 */
?>

<div class="panel-display pond clearfix <?php if (!empty($classes)) { print $classes; } ?><?php if (!empty($class)) { print $class; } ?>" <?php if (!empty($css_id)) { print "id=\"$css_id\""; } ?>>

  <div class="container-fluid">
    <div class="row">
      <div class="col-md-12 radix-layouts-header panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['header']; ?>
        </div>
      </div>
    </div>

    <div class="row">
      <div class="col-md-4 radix-layouts-column1 panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['column1']; ?>
        </div>
      </div>
      <div class="col-md-4 radix-layouts-column2 panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['column2']; ?>
        </div>
      </div>
      <div class="col-md-4 radix-layouts-column3 panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['column3']; ?>
        </div>
      </div>
    </div>

    <div class="row">
      <div class="col-md-4 radix-layouts-secondarycolumn1 panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['secondarycolumn1']; ?>
        </div>
      </div>
      <div class="col-md-4 radix-layouts-secondarycolumn2 panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['secondarycolumn2']; ?>
        </div>
      </div>
      <div class="col-md-4 radix-layouts-secondarycolumn3 panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['secondarycolumn3']; ?>
        </div>
      </div>
    </div>

    <div class="row">
      <div class="col-md-12 radix-layouts-footer panel-panel">
        <div class="panel-panel-inner">
          <?php print $content['footer']; ?>
        </div>
      </div>
    </div>
  </div>

</div><!-- /.radix-pond -->
