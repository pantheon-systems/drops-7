<?php
/**
 * @file
 * Template for the color scale options for the visualization entity choropleth bundle admin.
 */
foreach ($colors as $color):
?>
<span style="width:10px;height:10px;display:inline-block;background-color:<?php print $color ?>;border:1px black solid;"></span>
<?php
endforeach;
?>
