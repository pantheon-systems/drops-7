<?php

/**
 * @file
 * Default template for feed displays that use the RSS style.
 *
 * @ingroup views_templates
 */

 // Replace feed title with saved feed entity name
 // Only if a feed entity at this url exists
 global $base_url;
 $current_url = $base_url . '/' . current_path();
 $query = db_select('rss_feed', 'rf');
 $query->fields('rf', array('id', 'name', 'url'));
 $query->condition('rf.url', $current_url, '=');
 $feeds = $query->execute()->fetchAssoc();
 $feed_title = isset($feeds['name']) ? $feeds['name'] : variable_get('site_name');

?>
<?php print "<?xml"; ?> version="1.0" encoding="utf-8" <?php print "?>"; ?>
<rss version="2.0" xml:base="<?php print $link; ?>"<?php print $namespaces; ?>>
  <channel>
    <title><?php print $feed_title; ?></title>
    <link><?php print $link; ?></link>
    <description><?php print $description; ?></description>
    <language><?php print $langcode; ?></language>
    <?php print $channel_elements; ?>
    <?php print $items; ?>
  </channel>
</rss>
