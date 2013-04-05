<?php
/**
 * @file
 * commerce_kickstart_social.features.menu_links.inc
 */

/**
 * Implements hook_menu_default_menu_links().
 */
function commerce_kickstart_social_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: menu-social-connection_follow-us-on-twitter:https://twitter.com/commerceguys
  $menu_links['menu-social-connection_follow-us-on-twitter:https://twitter.com/commerceguys'] = array(
    'menu_name' => 'menu-social-connection',
    'link_path' => 'https://twitter.com/commerceguys',
    'router_path' => '',
    'link_title' => 'Follow Us on Twitter',
    'options' => array(
      'toolbar_expanded' => 0,
      'toolbar_break' => 0,
      'identifier' => 'menu-social-connection_follow-us-on-twitter:https://twitter.com/commerceguys',
      'attributes' => array(
        'class' => array(
          0 => 'twitter',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '1',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-49',
  );
  // Exported menu link: menu-social-connection_like-us-on-facebook:https://www.facebook.com/commerceguys
  $menu_links['menu-social-connection_like-us-on-facebook:https://www.facebook.com/commerceguys'] = array(
    'menu_name' => 'menu-social-connection',
    'link_path' => 'https://www.facebook.com/commerceguys',
    'router_path' => '',
    'link_title' => 'Like us on Facebook',
    'options' => array(
      'toolbar_expanded' => 0,
      'toolbar_break' => 0,
      'identifier' => 'menu-social-connection_like-us-on-facebook:https://www.facebook.com/commerceguys',
      'attributes' => array(
        'class' => array(
          0 => 'facebook',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '1',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-50',
  );
  // Exported menu link: menu-social-connection_what-we-like-on-pinterest:http://pinterest.com/
  $menu_links['menu-social-connection_what-we-like-on-pinterest:http://pinterest.com/'] = array(
    'menu_name' => 'menu-social-connection',
    'link_path' => 'http://pinterest.com/',
    'router_path' => '',
    'link_title' => 'What We Like on Pinterest',
    'options' => array(
      'toolbar_expanded' => 0,
      'toolbar_break' => 0,
      'identifier' => 'menu-social-connection_what-we-like-on-pinterest:http://pinterest.com/',
      'attributes' => array(
        'class' => array(
          0 => 'pinterest',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '1',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-48',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Follow Us on Twitter');
  t('Like us on Facebook');
  t('What We Like on Pinterest');


  return $menu_links;
}
