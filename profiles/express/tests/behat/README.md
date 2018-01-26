
## Local Setup

The proper way to develop Behat tests locally is to run a certain setup that differs from express.local or the Express Starter kit. The reason the setup differs for creating tests is that connecting to a browser emulating server is a huge PITA when you are using VMs. 

Specifically, when a JS test opens a window and simulates how a user would navigate a site, you aren't able to see what is going on. Having the VM connect to a browser emulating server on your local machine could mitigate this issue; however, if you have to run any PHP, you won't be able to do that since the web application isn't connected to the emualtor server. 

Rather than fiddle with a brittle local setup using VMs connecting to your local machine, it is easiest and most straight-forward to setup your own environment. 

You will need:
- PHP 7 - [Homebrew works well for this step](https://github.com/Homebrew/homebrew-php).
- MySQL - [Homebrew can be used also here](https://gist.github.com/nrollr/3f57fc15ded7dddddcc4e82fe137b58e).
- Composer - Once PHP is installed, [you can install Composer](https://getcomposer.org/doc/00-intro.md#installation-linux-unix-osx) locally or globally.
- Drush - Once Composer is installed, `composer global require "drush/drush:8.*"` to install Drush globally. 

You can check the versions of what you have locally, but for this tutorial the versions are as follows:
- PHP `7.1.10`
- MySQL `Server version: 5.7.19 Homebrew`
- Composer `1.5.2`
- Drush `8.1.12`

## Running Tests

You can now install an Express site however you want to using your MySQL setup for the server. The [Express Starter](https://github.com/CuBoulder/express-starter) kit can build you out a skeleton site that you can run `express_profile_configure_form.express_core_version=cu_testing_core --yes` from after changing the database settings in "sites/default/settings.php". The testing core install all of the bundles, which is needed for running the test suite.

Your local site might be on a version of Express that installs the LDAP module which requires HTTPS to login. In order to disable this check if you have trouble you can `vset` a variable before you start your test runs. 

```bash
drush vset ldap_servers_require_ssl_for_credentials 0
```

You will need to install Behat's dependencies as well as export an environmental variable in order to run the Behat tests.

```bash
cd site-path/profiles/express/tests/behat
mv composer

composer install
```

Rather than having you change the behat.yml configuration file, it easier to change environmental variables and have Behat pickup on those. This way environmental variables can be used on the CI setup without changing the configuration files as well.

```bash

export BEHAT_PARAMS='{"extensions":{"Drupal\\DrupalExtension":{"drupal":{"drupal_root":"BUILD_TOP/drupal"}},"Behat\\MinkExtension":{"base_url":"http://127.0.0.1:8888/","files_path":"BUILD_TOP/drupal/profiles/express/tests/behat/assets/"}}}'

```

To run tests...

```bash
# Chrome is installed already...
alias chrome="/Applications/Google\ Chrome.app/Contents/MacOS/Google\ Chrome"
chrome --disable-gpu --headless --remote-debugging-address=0.0.0.0 --remote-debugging-port=9222 > /dev/null 2>&1 &

./bin/behat --config behat.local.yml --verbose --tags '~@exclude_all_bundles&&~broken'
```

## Fixing Broken Tests

The headless Chrome driver is used to run tests on Travis. Selenium was dropped due to its brittle nature of not playing nice with different combinations of Chrome/Firefox as Travis updated the default versions of Chrome and Firefox periodically. Many projects have switched to the method we now use on Travis; however, you will need to use Selenium if a JS test is breaking and you want to maunally inspect why. 

You will need to..
- [Download standalone server](http://docs.seleniumhq.org/download/)
- [Download latest Chrome webdriver](https://sites.google.com/a/chromium.org/chromedriver/downloads)

```bash
# Startup Selenium webserver with the proper version of "selenium-server-standalone-3.4.0.jar".
java -Dwebdriver.chrome.driver=chromedriver -jar selenium-server-standalone-3.4.0.jar > /dev/null 2>&1 &` 

# Run tests using selenium configuration. 
./bin/behat --config behat.selenium.yml --verbose --tags '~@exclude_all_bundles&&~broken'
```
