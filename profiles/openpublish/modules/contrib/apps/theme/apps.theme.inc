<?PHP

/**
 * implement hook_preprocess_apps_list
 */
function theme_apps_list($vars) {
  $items = array();
  foreach($vars['apps'] as $id=> $app) {
    if (!preg_match("/^#/", $id)) {
      $items[] = drupal_render($app);
    }
  }
  return theme('item_list', array('items'=>$items));
}


/**
 * implement hook_preprocess_apps_app_teaser
 */
function template_preprocess_apps_app_teaser(&$vars) {
  
  apps_include('pages');
  $link = apps_app_page_path($vars['app']);
  $vars['name'] = l($vars['app']['name'], $link);
  if(!empty($vars['app']['logo'])) {
    $vars['app']['logo']['style_name']= 'apps_logo';
    $vars['logo'] = l(theme('image_style', $vars['app']['logo']), $link, array('html'=>TRUE));
  }
  else {
    $vars['logo'] = FALSE;
  }
  $status = $vars['app']['status'];
  $vars['status'] = $status === ENABLED ?
    'enabled' :
    (($status === DISABLED) ?
      'disabled' :
      (($status === INCOMPATABLE) ?
        'incompatable' :
        'available'
      )
    );
    
  //these will need values at some point  - rating system
  $vars['numratings'] = '(' . t('1 ratings') . ')';
  $vars['rating'] = 'five';
  //action link
  $vars['action'] = l('Details', $link);
}

/**
 * implement hook_preprocess_apps_app_page
 */
function template_preprocess_apps_app_page(&$vars) {
  //first do everything we do in the teaser
  drupal_add_css(drupal_get_path('module', 'apps').'/theme/css/apps.css');
  template_preprocess_apps_app_teaser($vars);
  
  //we need some special stuff for the logo here
  if(!empty($vars['app']['logo']) && $vars['app']['logo']) {
    $vars['app']['logo']['style_name']= 'apps_logo_small';
    $vars['logo'] = theme('image_style', $vars['app']['logo']);
  }
   
    
  if(isset($vars['app']['screenshots'][0])) {
    $vars['app']['screenshots'][0]['style_name'] = 'apps_screenshot';
    $vars['screenshot'] = theme('image_style', $vars['app']['screenshots'][0]);
  }
  else {
    $vars['screenshot'] = FALSE;
  }
  
  $vars['description'] = $vars['app']['description'];
  $vars['name'] = $vars['app']['name'];
  $vars['author'] = l($vars['app']['author'], $vars['app']['author_url']);
  $vars['version'] = t('Version') . ' ' . $vars['app']['version'];
  if ($vars['app']['status'] === INCOMPATABLE) {
    $incompatable_deps = array();
    $incompatable_message = '';
    foreach($vars['app']['dependencies'] as $dep) {
      if ($dep['status'] === INCOMPATABLE) {
        $incompatable_deps[] = "{$dep['version']['name']} {$dep['version']['original_version']}";
      }
    }
    if (!empty($incompatable_deps)) {
      $incompatable_message = "Incompatable - requires " . implode(", ", $incompatable_deps);
    }
    else {
      //this should never run
      $incompatable_message = "Incompatable with current install";
    }
    //drupal_set_message($incompatable_message, 'error');
    $vars['status'] = $incompatable_message;
  }
  
  //Title texts for App Detail Page
  $vars['author_title'] = t('Author');
  $vars['version_title'] = t('Version');
  $vars['rating_title'] = t('Customer Rating');
  $vars['description_title'] = t('Description');
  //$vars['rating_widget_caption'] = t('Rate this application:');
  $vars['rating_caption'] = t('Average rating for this version:');
}


/**
 * implement hook_preprocess_apps_app_featured
 */
function template_preprocess_apps_app_featured(&$vars) {
  //first do everything we do in the teaser
  template_preprocess_apps_app_teaser($vars);
  if($vars['app']['screenshots'][0]) {
    $vars['app']['screenshots'][0]['style_name'] = 'apps_featured_screenshot';
    $vars['screenshot'] = theme('image_style', $vars['app']['screenshots'][0]);
  }
  $vars['description'] = $vars['app']['description'];
  
  //these will need values at some point  - rating system
  $vars['numratings'] = '(' . t('0 ratings') . ')'; //the number of times its been rated
  $vars['rating'] = 'five'; //should be the number of stars spelled out (five, four, three, two or one)
  //action link
  $link = apps_app_page_path($vars['app']);
  $vars['action'] = l('free', $link);  //The text and link for the action button
}

/**
 * implement hook_preprocess_apps_install_page
 */
function template_preprocess_apps_install_page(&$vars) {
  drupal_add_css(drupal_get_path('module', 'apps').'/theme/css/apps.css');
  if (isset($vars['content']['featured_app'])) {
    $vars['content']['featured_app']['#theme'] = 'apps_app_featured';
  }
}

/**
 * implement hook_preprocess_apps_manage_page
 */
function template_preprocess_apps_manage_page(&$vars) {
  drupal_add_css(drupal_get_path('module', 'apps').'/theme/css/apps.css');
}

function template_preprocess_apps_update_page(&$vars) {
  drupal_add_css(drupal_get_path('module', 'apps').'/theme/css/apps.css');
}


