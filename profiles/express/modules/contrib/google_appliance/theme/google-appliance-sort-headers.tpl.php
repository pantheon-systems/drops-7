<?php // $Id$ 
/**
* @file 
* 	default theme implementation for search results sort headers
*
* @see 
*		template_preprocess_google_appliance_sort_headers()
*/
//dsm($variables);
?>

<div class="container-inline google-appliance-sort-headers">
	<?php foreach ($sorters as $sorter): ?>
  	<div class="google-appliance-sorter"><?php print $sorter['display'] ?></div>
  <?php endforeach ?>
</div>



      
