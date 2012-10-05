-- SUMMARY --

The Apps module provides a much better user experience of extending your Drupal site by providing facilities to download modules and all of their dependencies in one easy step.

For a full description of the module, visit the project page:
  http://drupal.org/project/apps

To submit bug reports and feature suggestions, or to track changes:
    http://drupal.org/project/issues/apps

A video tutorial is available here:  http://youtu.be/SF7bZw9wCwg


-- REQUIREMENTS --

A connector module is needed to access an app's server.
 * An example of a connector module is Level Ten Apps http://drupal.org/project/levelten_apps

-- INSTALLATION --

* Install as usual, see http://drupal.org/node/70151 for further information.

* An Apps tab will open up in the Administration bar

* To install an app, click the Apps tab, which will bring up the apps available to you.

* Click a specific app's page and choose "install app"

* The necessary modules will then be downloaded to your Drupal site
     

-- CONFIGURATION --

* After an app is done downloading, a configuration page will appear.

* Configure user permissions in Administration » People » Permissions:

       
-- CUSTOMIZATION --




-- TROUBLESHOOTING --


-- FAQ --

*I am having trouble installing Apps that contain files from a GitHub download URL*

GitHub download URLs are typically of the style, http://github.com/user/repo/tarball/branch
Drupal's downloader and GitHub's download URLs have typically not played nice
together, so we recommend either appending a filename to the end of the GitHub
URL such as, http://github.com/user/repo/tarball/branch/filename.tar.gz or
using a utility such as http://githubredir.debian.net/ to generate those
URLs for you.


-- APP INSTALLATION --

Apps uses the same mechanism for installing modules as the update module in
core. This depends on certain php extensions to be installed on your server.
Below is the documentation for the various methods of installing.

*Install via FTP*

In order to install via ftp, you must have the ftp php extension enabled.
Most apache2/php installs have this by default which is by it probably shows
up on most installs. You may run into a server that doesn't have ftp so then
you will need to install it or use an alternative method. See
http://us2.php.net/manual/en/book.ftp.php for how to install the ftp php
extension. You will also need an ftp username and password that has rights
to write to your site directory on your server.

*Install via SSH*

This is the recommended method of install

In order to install via ssh, you must have the ssh2 php extension installed
and enabled. This does not come by default with many apache2/php installs
so it commonly needs to be added. See http://us2.php.net/manual/en/book.ssh2.php
for how to install the ssh2 php extension. You will also need a username and
password of a user that can ssh into the server and has write permissions to
your site directory on your server.

Install directly to sites directory

This is not the preferred method of install and should be a last resort.

In order to install directly the sites/all/modules directory it needs to be
writable. In order to do this go to the root of your drupal install and type

sudo chmod 777 sites/all/modules

Be aware that there are security issues with leaving your site in this state.

-- APP UNINSTALL --
When an app is uninstalled, it should remove or cleanup items that a normal
Drupal module would do via hook_uninstall. This includes variables that the
module has set and modifications to existing tables. If there is content
that the app has created that makes sense to remove, it should be deleted.
If contexts, views, etc. were created by code, then disabling or uninstalling
the app will remove them automatically. Features won't remove content types,
but will make them eligible to delete.

See also: hook_uninstall [1]

-- DEFAULT CONTENT --
If an app provides default content provided by the defaultcontent module, it
has to be listed as a dependency for the app. Any app's demo content module
that uses the defaultcontent module for its content should include it as a
dependency.

The dependency will be listed automatically in the .info file when creating
the feature to be used as the app's demo content module with the following:

dependencies[] = "defaultcontent"

The dependency should be added to the app manifest by specifying both the
dependency and download location. The following lines added to the app
manifest specifies a dependency on version 7.x-1.0-alpha6:

dependencies[defaultcontent] = defaultcontent 1.0-alpha6
downloadables[defaultcontent 1.0-alpha4] = http://ftp.drupal.org/files/projects/defaultcontent-7.x-1.0-alpha6.tar.gz

See also: Default Content Module [2]

-- CONTACT --

Current maintainers:
* randallknutson - http://drupal.org/user/183932
* febbraro - http://drupal.org/user/43670
* jec006 - http://drupal.org/user/855980
* tirdadc - http://drupal.org/user/383630


[1]: http://api.drupal.org/api/drupal/modules%21system%21system.api.php/function/hook_uninstall/7
[2]: http://drupal.org/project/defaultcontent
