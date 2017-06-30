<?php
/**
 * @file
 * Drush commands for Radix theme.
 */

// BOOTSWATCH_URL
define('BOOTSWATCH_URL', 'https://github.com/arshad/radix-bootswatch/archive/master.zip');

/**
 * Implements hook_drush_command().
 */
function radix_drush_command() {
  $items = array();

  $items['radix'] = array(
    'description' => 'Create a Radix subtheme.',
    'arguments' => array(
      'name'         => 'The name of your subtheme.',
    ),
    'options' => array(
      'machine_name' => 'The machine-readable name of your subtheme. This will be auto-generated from the human-readable name if ommited.',
      'description' => 'The description of your subtheme',
      'destination' => 'The destination of your subtheme. Defaults to "all" (sites/all/themes).',
      'kit' => 'The name or url of the starter kit to use. Defaults to "default".',
      'bootswatch' => 'The Bootswatch theme to use. See https://github.com/arshad/radix-bootswatch.',
    ),
    'examples' => array(
      'drush radix "My Theme"' => 'Creates a Radix subtheme called "My Theme", using the default options.',
      'drush radix "My Theme" --machine_name=my_theme' => 'Creates a Radix subtheme called "My Theme" with a specific machine name.',
      'drush radix "My Theme" --bootswatch=cerulean' => 'Creates a Radix subtheme called "My Theme" using Cerulean Bootswatch theme.',
      'drush radix "My Theme" --kit="https://github.com/radix-kit-KITNAME/archive/BRANCH.zip"' => 'Creates a Radix subtheme called "My Theme" using a remote kit source. See http://docs.radixtheme.org/en/latest/drupal/#custom-kit.',
    ),
  );

  // Upgrade to the Gulp version.
  $items['radix-upgrade-gulp'] = array(
    'description' => 'Upgrade subtheme to use Gulp instead of Compass.',
    'examples' => array(
      'drush radix-upgrade-gulp my_theme' => 'Upgrades my_theme to use Gulp',
    ),
  );

  // Upgrade to the latest radix.
  $items['radix-upgrade-7034'] = array(
    'description' => 'Upgrade your Radix subtheme to the latest version.',
    'arguments' => array(
      'name' => 'The name of your subtheme.',
    ),
    'examples' => array(
      'drush radix-upgrade my_theme' => 'Upgrades my_theme to the latest version',
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function radix_drush_help($section) {
  switch ($section) {
    case 'drush:radix':
      return dt('This command will create a Radix subtheme. See examples to get started.');
    case 'drush:radix-upgrade-gulp':
      return dt('This command will upgrade a Radix subtheme to the Gulp version.');
  }
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_radix($name = NULL, $machine_name = NULL) {

  // If no $name provided, abort.
  if (!$name) {
    drush_print(dt('Theme name missing. See help using drush radix --help.'));
    return;
  }

  // Determine the theme name.
  if (!isset($name)) {
    $name = drush_get_option('name');
  }

  // Determine the machine name.
  if (!isset($machine_name)) {
    $machine_name = drush_get_option('machine_name');
  }
  if (!$machine_name) {
    $machine_name = $name;
  }
  $machine_name = str_replace(' ', '_', strtolower($machine_name));
  $search = array(
    '/[^a-z0-9_]/', // Remove characters not valid in function names.
    '/^[^a-z]+/',   // Functions must begin with an alpha character.
  );
  $machine_name = preg_replace($search, '', $machine_name);

  // Description of subtheme.
  $description = (drush_get_option('description')) ? trim(drush_get_option('description')) : 'A theme based on Radix.';

  // Determine the path to the new subtheme.
  $subtheme_path = 'sites/all/themes';
  if ($destination = drush_get_option('destination')) {
    $subtheme_path = drush_trim_path($destination);
  }
  $subtheme_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . $subtheme_path . '/' . $machine_name);

  // Determine the kit to use.
  $kit = (drush_get_option('kit')) ? drush_trim_path(drush_get_option('kit')) : 'default';

  // Make a fresh copy of the kit.
  $kit_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drupal_get_path('theme', 'radix') . '/kits/' . $kit);

  // Allow kits to be pulled from external urls.
  if (valid_url($kit, TRUE)) {
    $kit_url = $kit;
    $kit_name = 'kit';

    // Get kit name from kit url.
    if (preg_match("/\/radix\-kit\-([a-z0-9\_]*)\//", $kit_url, $matches)) {
      $kit_name = $kit = $matches[1];
    }

    // Switch to a temp directory.
    $current_dir = getcwd();
    chdir(drush_tempdir());

    drush_print(dt('Downloading @kit_name from @kit_url...', array(
      '@kit_name' => (!empty($kit_name)) ? $kit_name . ' kit' : $kit_name,
      '@kit_url' => $kit_url,
    )));
    if ($filepath = drush_download_file($kit_url)) {
      $filename = basename($filepath);

      // Decompress the zip archive.
      $files = drush_tarball_extract($filename, getcwd(), TRUE);

      // Re-index array.
      // This fixes an issue where a .tag.gz tarball returns a non-zero array.
      $files = array_values($files);
      $kit_path = getcwd() . '/' . $files[0];

      // Set working directory back to the previous working directory.
      chdir($current_dir);
    }
  }

  if (!is_dir(dirname($subtheme_path))) {
    drush_die(dt('The directory "!directory" was not found.', array('!directory' => dirname($subtheme_path))));
  }
  drush_op('drush_copy_dir', $kit_path, $subtheme_path);

  // Copy Bootswatch theme to bootswatch folder.
  if ($bootswatch = drush_get_option('bootswatch')) {
    // Switch to a temp directory.
    $current_dir = getcwd();
    chdir(drush_tempdir());

    if ($filepath = drush_download_file(BOOTSWATCH_URL)) {
      $filename = basename($filepath);
      $dirname =  basename($filepath, '.zip');

      // Decompress the zip archive.
      drush_tarball_extract($filename, $dirname, TRUE);

      // Copy bootswatch theme to subtheme.
      $bootswatch_path = $subtheme_path . '/scss/bootswatch/';
      drush_print($bootswatch_path);
      drush_move_dir($dirname . '/radix-bootswatch-master/' . $bootswatch, $bootswatch_path, TRUE);
    }

    // Set working directory back to the previous working directory.
    chdir($current_dir);
  }

  // Alter the contents of the .info file based on the command options.
  $alterations = array(
    '{{Name}}' => $name,
    '{{Description}}' => $description,
    '{{machine_name}}' => $machine_name,
    'hidden = true' => '',
  );
//  drush_op('radix_file_str_replace', $subtheme_info_file, array_keys($alterations), $alterations);

  // Replace all occurrences of '{{machine_name}}' with the machine name of our sub theme.
  $files_to_replace = array(
    $kit . '.info',
    'template.php',
    'theme-settings.php',
    'config.json',
    'bower.json',
    'package.json',
    'scss/' . $kit . '.style.scss',
  );

  // Also make alterations in the include files.
  $include_files = array_diff(scandir($subtheme_path . '/includes'), array('..', '.'));
  foreach ($include_files as $include) {
    $files_to_replace[] = 'includes/' . $include;
  }

  foreach ($files_to_replace as $file_to_replace) {
    drush_op('radix_file_str_replace', $subtheme_path . '/' . $file_to_replace, array_keys($alterations), $alterations);
  }

  // Rename files.
  $files_to_rename = array(
    '{{kit}}.info',
    'scss/{{kit}}.style.scss',
    'assets/js/{{kit}}.script.js',
  );
  foreach ($files_to_rename as $file_to_rename_path) {
    $file_original_path = $subtheme_path . '/' . str_replace('{{kit}}', $kit, $file_to_rename_path);
    $file_new_path = $subtheme_path . '/' . str_replace('{{kit}}', $machine_name, $file_to_rename_path);
    drush_op('rename', drush_normalize_path($file_original_path), drush_normalize_path($file_new_path));
  }

  // Notify user of the newly created theme.
  $message = 'Successfully created the Radix subtheme "!name" created in: !path using the "!kit" kit';

  // Add bootstwatch to message.
  if ($bootswatch != '') {
    $message .= ' and the "!bootswatch" bootswatch theme';
  }

  $message = dt($message . '.', array(
    '!name' => $name,
    '!path' => $subtheme_path,
    '!kit' => $kit,
    '!bootswatch' => $bootswatch,
  ));
  drush_print($message);
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_radix_upgrade_gulp() {
  global $base_url;

  // Prepare options.
  $all_themes = list_themes();

  // Remove default option.
  unset($all_themes['default']);

  $options = array_filter($all_themes, function($n) {
    return ($n->base_theme == 'radix');
  });

  array_walk($options, function(&$n, $key) {
    $n = $n->info['name'];
  });

  // Set configuration values.
  $machine_name = drush_choice($options, dt('Which theme would you like to update?'));
  $name = $all_themes[$machine_name]->info['name'];
  $description = $all_themes[$machine_name]->info['description'];
  $theme_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drupal_get_path('theme', 'radix')) . '/';
  $subtheme_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drupal_get_path('theme', $machine_name)) . '/';

  // Remove files that are no longer necessary.
  $remove_files = array(
    'config.rb',
    'Gemfile',
    'Gemfile.lock',
    'Guardfile',
    'assets/sass/screen.scss',
  );

  foreach ($remove_files as $file) {
    if (drush_op('unlink', $subtheme_path . $file)) {
      print "Deleted file: $file.\n";
    } else {
      print "Failed to delete file: $file.\n";
    }
  }

  // Copy files from kit.
  $copy_files = array(
    'bower.json' => 'bower.json',
    'config.json' => 'config.json',
    'package.json' => 'package.json',
    'gulpfile.js' => 'gulpfile.js',
    'scss/default.style.scss' => "assets/sass/$machine_name.style.scss",
  );

  foreach ($copy_files as $from => $to) {
    if (drush_op('copy', $theme_path . 'kits/default/' . $from, $subtheme_path . $to)) {
      print "Copied file: $from to $to.\n";
    } else {
      print "Failed to copy file: $from to $to.\n";
    }
  }

  // Update configuration files.
  $update_files = array(
    'bower.json',
    'config.json',
    'package.json',
  );

  foreach ($update_files as $file) {
    $file_path = $subtheme_path . $file;
    radix_file_str_replace($file_path, 'http://panopoly.dev', $base_url);
    radix_file_str_replace($file_path, '{{Name}}', $machine_name);
    radix_file_str_replace($file_path, '{{Description}}', $description);
    radix_file_str_replace($file_path, '{{machine_name}}', $machine_name);
  }

  // Copy radix scss.
  if (drush_op('drush_copy_dir', $theme_path . 'kits/default/scss/radix', $subtheme_path . 'assets/sass/radix')) {
    print "Copied directory: radix/scss.\n";
  } else {
    print "Failed to copy directory: radix/scss.\n";
  }

  // Move files.
  $rename_files = array(
    'assets/sass' => 'scss',
    'scss/global' => 'scss/base',
    'scss/partials' => 'scss/components',
    'scss/base/_helper.scss' => 'scss/base/_helpers.scss',
    'scss/base/_mixin.scss' => 'scss/base/_mixins.scss',
    'scss/base/_variable.scss' => 'scss/base/_variables.scss',
    'assets/stylesheets' => 'css',
    'assets/javascripts' => 'js',
    'js/script.js' => "js/$machine_name.script.js",
  );

  foreach ($rename_files as $from => $to) {
    if (drush_op('rename', $subtheme_path . $from, $subtheme_path . $to)) {
      print "Renamed file: $from to $to.\n";
    }
    else {
      print "Failed to rename file: $from.\n";
    }
  }

  // Update info file.
  $info_file_path = "$subtheme_path$machine_name.info";
  radix_file_str_replace($info_file_path, 'assets/stylesheets', 'assets/css');
  radix_file_str_replace($info_file_path, 'assets/javascripts', 'assets/js');
  radix_file_str_replace($info_file_path, 'assets/css/screen.css', "assets/css/$machine_name.style.css");
  radix_file_str_replace($info_file_path, 'assets/js/script.js', "assets/js/$machine_name.script.js");
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_radix_upgrade_7034($subtheme = NULL) {
  // Check if the subtheme exists.
  $subtheme_path = drupal_get_path('theme', $subtheme);
  if (!$subtheme_path) {
    drush_set_error('SUBTHEME_NOT_FOUND', dt('Radix subtheme \'@subtheme\' not found.', array(
      '@subtheme' => $subtheme
    )));
    return;
  }

  // Ask for confirmation
  drush_print('--- --- ---');
  drush_print(dt('This command will upgrade subtheme \'@subtheme\' to Radix 7.x-3.4.', array(
    '@subtheme' => $subtheme
  )));
  drush_print(dt('Please make sure you have a backup before proceeding.'));

  if (!drush_confirm('Are you sure you want to proceed?')) {
    drush_user_abort();
    return;
  }

  // Set paths.
  $radix_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . drupal_get_path('theme', 'radix'));
  $subtheme_path = drush_normalize_path(drush_get_context('DRUSH_DRUPAL_ROOT') . '/' . $subtheme_path);

  // Check if subtheme radix scss files is there.
  $subtheme_radix_scss_path = $subtheme_path . '/scss/radix';
  if (!file_exists($subtheme_radix_scss_path)) {
    drush_set_error('SUBTHEME_RADIX_SCSS_NOT_FOUND', dt('Radix scss files for subtheme \'@subtheme\' not found.', array(
      '@subtheme' => $subtheme
    )));
    return;
  }

  // Start the upgrade.
  drush_print(dt('Starting subtheme \'@subtheme\' upgrade to Radix 7.x-3.4.', array(
    '@subtheme' => $subtheme
  )));
  drush_print('--- --- ---');

  // Step 1: Delete subtheme radix scss files.
  if (drush_delete_dir($subtheme_radix_scss_path)) {
    drush_print(dt('1. Successfully removed old scss/radix files for subtheme \'@subtheme\'.', array(
      '@subtheme' => $subtheme
    )));
  }

  // Step 2: Copy Radix scss.
  if (drush_op('drush_copy_dir', $radix_path . '/kits/default/scss/radix', $subtheme_path . '/scss/radix')) {
    drush_print(dt('2. Successfully copied new scss/radix files to subtheme \'@subtheme\'.', array(
      '@subtheme' => $subtheme
    )));
  } else {
    drush_set_error('SCSS_NOT_COPIED', dt('2. Failed to copy Radix scss files to subtheme \'@subtheme\'.', array(
      '@subtheme' => $subtheme
    )));
  }

  // Step 3: Move js files.
  if (file_exists($subtheme_path . '/js')) {
    if (drush_copy_dir($subtheme_path . '/js', $subtheme_path . '/assets/js', FILE_EXISTS_OVERWRITE)) {
      drush_print(dt('3. Successfully copied js files to assets/js.'));

      // Step 4: Delete js files.
      if (drush_delete_dir($subtheme_path . '/js')) {
        drush_print(dt('4. Successfully removed old js.'));
      }
    }
  }
  else {
    drush_set_error('JS_NOT_COPIED', dt('3. Failed to copy js files to assets/js.'));
    drush_set_error('JS_NOT_COPIED', dt('4. Failed to remove old js files.'));
  }

  // Step 5: Update gulpfile.
  $subtheme_gulpfile_path = $subtheme_path . '/gulpfile.js';
  if (drush_op('copy', $radix_path . '/kits/default/gulpfile.js', $subtheme_gulpfile_path)) {
    drush_print(dt('5. Successfully copied new gulpfile.js to subtheme \'@subtheme\'.', array(
      '@subtheme' => $subtheme
    )));
  }
  else {
    drush_set_error('GULPFILE_NOT_COPIED', dt('5. Failed to copy new gulpfile.js to subtheme \'@subtheme\'.', array(
      '@subtheme' => $subtheme
    )));
  }

  // Step 6: Add settings to info file.
  $subtheme_info_path = $subtheme_path . '/' . $subtheme . '.info';
  $preprocess_css_setting = "\n; Settings\n; Set this to FALSE for development only.\nsettings[preprocess_css] = FALSE";
  if (radix_file_str_replace($subtheme_info_path, 'base theme  = radix', 'base theme  = radix' . $preprocess_css_setting)) {
    drush_print(dt('6. Successfully added preprocess_css setting to \'@subtheme\'.info.', array(
      '@subtheme' => $subtheme
    )));
  }
  else {
    drush_set_error('GULPFILE_NOT_COPIED', dt('6. Failed to add preprocess_css setting to \'@subtheme\'.info.', array(
      '@subtheme' => $subtheme
    )));
  }

  // Print messages.
  drush_print('--- --- ---');
  drush_print(dt('Radix subtheme \'@subtheme\' has been upgraded to the Radix 7.x-3.4. Please verify your theme files.', array(
    '@subtheme' => $subtheme
  )));
  drush_print('--- --- ---');

  drush_print(dt('IMPORTANT: Run the following commands to install new npm modules: '));
  drush_print(dt('  npm install --save gulp-jshint gulp-scss-lint'));
  drush_print(dt('  gulp'));
  drush_print('--- --- ---');
}

/**
 * Replace strings in a file.
 */
function radix_file_str_replace($file_path, $find, $replace) {
  $file_path = drush_normalize_path($file_path);
  $file_contents = file_get_contents($file_path);
  $file_contents = str_replace($find, $replace, $file_contents);
  return file_put_contents($file_path, $file_contents);
}
