<?php
/**
 * @file
 * Template for the choropleth visualization wrapper.
 */

?>

<style type="text/css">
    #admin-menu{
      display: none;
    }
    body.admin-menu {
      margin-top: 0px !important;
    }
    .region-content .block {
      box-shadow: none;
      -webkit-box-shadow: none;
    }
    #iframe-shell .entity-visualization .content {
    	display: none;
    }
</style>

<div id="iframe-shell">
    <?php print render($page['content']); ?>
</div>
