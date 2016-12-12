<?php

function cuemail_theme(&$existing, $type, $theme, $path) {
  $registry = array();
  $template_dir = drupal_get_path('theme', 'cuemail') . '/templates';
  $registry['newsletter_section_blocks'] = array(
    'template' => 'newsletter-section-blocks',
    'path' => $template_dir,
  );
  $registry['newsletter_section_ads'] = array(
    'template' => 'newsletter-section-ads',
    'path' => $template_dir,
  );
  $registry['newsletter_intro'] = array(
    'template' => 'newsletter-intro',
    'path' => $template_dir,
  );
  $registry['newsletter_list'] = array(
    'template' => 'newsletter-list',
    'path' => $template_dir,
  );
  return $registry;
}

/**
 * Implements theme_preprocess_html().
 */
function cuemail_preprocess_html(&$vars) {
  $data = array(
    '#tag' => 'meta',
    '#attributes' => array(
       'http-equiv' => 'Content-Type',
       'content' => 'text/html; charset=utf-8',
    ),
  );
  drupal_add_html_head($data, 'utf');
}

/**
 * Implements theme_preprocess_node().
 */
function cuemail_preprocess_node(&$vars) {

  $vars['theme_hook_suggestions'][] = 'node__' . $vars['type'] . '__' . $vars['view_mode'];
  $url = url('node/' . $vars['nid'], array('absolute' => TRUE, 'alias' => TRUE, 'https' => FALSE));
  $vars['node_url'] = $url;
  if ($vars['type'] == 'newsletter') {
    if (!empty($vars['content']['field_newsletter_intro_image'])) {
      $vars['content']['field_newsletter_intro_image'][0]['#image_style'] = 'email_medium';
    }
    $list = array();
    foreach ($vars['content']['field_newsletter_section']['#items'] as $key => $item) {
      $key_2 = key($vars['content']['field_newsletter_section'][$key]['entity']['field_collection_item']);
      if (!empty($vars['content']['field_newsletter_section'][$key]['entity']['field_collection_item'][$key_2]['field_newsletter_articles'])) {
        $articles = $vars['content']['field_newsletter_section'][$key]['entity']['field_collection_item'][$key_2]['field_newsletter_articles']['#items'];
        foreach ($articles as $article) {
          $node = node_load($article['target_id']);
          $list[] = $node->title;
        }
      }
    }
    $newsletter_logo_image_style_uri = image_style_path('medium', $vars['newsletter_logo_uri']);
    if (!file_exists($newsletter_logo_image_style_uri)) {
      image_style_create_derivative(image_style_load('medium'), $vars['newsletter_logo_uri'], $newsletter_logo_image_style_uri);
    }
    $image_info = image_get_info($newsletter_logo_image_style_uri);
    $vars['newsletter_logo_width'] = round($image_info['width'] * .46333);
    $vars['newsletter_logo_height'] = round($image_info['height'] * .46333);
    $vars['content']['list'] = theme('item_list', array(
      'items' => $list,
      'type' => 'ul',
      'attributes' => array(
        'class' => array(
          'bullet-list',
        ),
      ),
    ));
  }
  if ($vars['type'] == 'article') {
    if (!empty($vars['content']['field_article_thumbnail'][0])) {
      $vars['content']['field_article_thumbnail'][0]['#path']['options']['absolute'] = TRUE;
    }
    if ($vars['view_mode'] == 'email_feature') {     $vars['content']['field_article_thumbnail'][0]['#image_style'] = 'email_feature_thumbnail';
    }
    if (isset($vars['field_article_categories'])) {
      foreach ($vars['field_article_categories'] as $tid) {
        if (isset($tid['tid'])) {
          $tids[] = $tid['tid'];
        }
      }
    }
    if (isset ($tids)) {
      $terms = taxonomy_term_load_multiple($tids);
      foreach ($terms as $term) {
        if (isset($term->name)) {
          $tag = $term->name;
          if ($term->field_category_display[LANGUAGE_NONE][0]['value'] == 'show') {
            if (!empty($term->field_category_term_page_link)) {
              $new_tags[] = l($tag, $term->field_category_term_page_link[LANGUAGE_NONE][0]['url'], array('absolute' => TRUE, 'alias' => TRUE, 'https' => FALSE));
            }
            else {
              $new_tags[] = $tag;
            }
          }
        }
      }
      $markup = implode(' ', $new_tags);
      unset($vars['content']['field_article_categories']);
      $vars['content']['field_article_categories'][0]['#markup'] = '<p>' . $markup . '</p>';
    }
  }
}

/**
 * Implements theme_image_style().
 */
function cuemail_image_style(&$vars) {
  // Determine the dimensions of the styled image.
  $dimensions = array(
    'width' => $vars['width'],
    'height' => $vars['height'],
  );

  image_style_transform_dimensions($vars['style_name'], $dimensions);

  $vars['width'] = $dimensions['width'];
  $vars['height'] = $dimensions['height'];

  if ($vars['style_name'] == 'email_medium') {
    $vars['width'] = 560;
    $vars['height'] = 280;
  }

  if ($vars['style_name'] == 'email_ad') {
    $vars['width'] = 560;
    $vars['height'] = 280;
  }

  if ($vars['style_name'] == 'email_feature_thumbnail') {
    $vars['width'] = 560;
    $vars['height'] = 261;
  }

  // Determine the url for the styled image.
  $vars['path'] = image_style_url($vars['style_name'], $vars['path']);
  $vars['attributes']['class'] = array('image-' . $vars['style_name']);
  return theme('image', $vars);
}

function cuemail_logo_color($design = 'design-01') {
  $logos = array();
  $logos['design-01'] = 'logo-black-2x.gif';
  $logos['design-02'] = 'logo-white-2x.gif';
  $logos['design-03'] = 'logo-white-2x.gif';
  $logos['design-04'] = 'logo-black-2x.gif';
  $logos['design-05'] = 'logo-black-2x.gif';
  return $logos[$design];
}

function cuemail_html_compress($email){
  return str_replace(array("\n","\r","\t"),'',$email);
}
