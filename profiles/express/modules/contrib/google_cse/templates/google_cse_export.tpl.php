<?php
/**
 * @file
 * XML for Google CSE
 */

//to debug, comment out the text/xml header and enable the print_r.
// dsm() does NOT work at this level
header("Content-type: text/xml");
drupal_add_http_header('Content-Disposition', 'attachment; filename =' . $node['title'] . '.xml');
echo '<?xml version="1.0" encoding="UTF-8" ?>';
?>

<CustomSearchEngine id="<?php print $node['cse_id']?>" creator="<?php print $node['cse_creator']?>" language="en" encoding="UTF-8" enable_suggest="true">
    <Title><?php print $node['title']?></Title>
    <Context>
        <BackgroundLabels>
            <Label name="_cse_<?php print $node['cse_id']?>" mode="FILTER" />
            <Label name="_cse_exclude_<?php print $node['cse_id']?>" mode="ELIMINATE" />
        </BackgroundLabels>
    </Context>
    <LookAndFeel nonprofit="true" element_layout="7" theme="7" custom_theme="true" text_font="Arial, sans-serif" url_length="full" element_branding="show" enable_cse_thumbnail="false" promotion_url_length="full" ads_layout="1">
        <Logo />
        <Colors url="#008000" background="#FFFFFF" border="#FFFFFF" title="#0000CC" text="#000000" visited="#0000CC" title_hover="#0000CC" title_active="#0000CC" />
        <Promotions title_color="#0000CC" title_visited_color="#0000CC" url_color="#008000" background_color="#FFFFFF" border_color="#336699" snippet_color="#000000" title_hover_color="#0000CC" title_active_color="#0000CC" />
        <SearchControls input_border_color="#D9D9D9" button_border_color="#666666" button_background_color="#CECECE" tab_border_color="#E9E9E9" tab_background_color="#E9E9E9" tab_selected_border_color="#FF9900" tab_selected_background_color="#FFFFFF" />
        <Results border_color="#FFFFFF" border_hover_color="#FFFFFF" background_color="#FFFFFF" background_hover_color="#FFFFFF" ads_background_color="#FDF6E5" ads_border_color="#FDF6E5" />
        <Widget query_parameter_name="cse" />
    </LookAndFeel>
    <AdSense />
    <EnterpriseAccount />
    <ImageSearchSettings enable="false" />
    <autocomplete_settings />
    <sort_by_keys label="Relevance" key="" />
    <sort_by_keys label="Date" key="date" />
    <cse_advance_settings enable_speech="true" />
</CustomSearchEngine>
