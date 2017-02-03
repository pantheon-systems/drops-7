<?php 
// $Id$ 
/**
* @file 
* 	defualt theme implementation for the search stats
*
* @see 
*		template_preprocess_google_appliance_search_stats()
*/
//dsm($variables);
?>

<div class="container-inline google-appliance-search-stats">
	<?php print t('Showing results @first - @last for %query', $stat_entries); ?>
</div>
