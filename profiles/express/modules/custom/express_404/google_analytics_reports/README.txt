DESCRIPTION
-----------
Google Analytics Reports module provides graphical reporting of your site's
tracking data. Graphical reports include small path-based report in blocks,
and a full path-based report.

Google Analytics Reports API module provide API for developers to access data
from Google Analytics using Google Analytics Core Reporting API
https://developers.google.com/analytics/devguides/reporting/core/v3/.

Google Analytics Reports module provide Views query plugin to create Google
Analytics reports using Views interface.


REQUIREMENTS
------------
* Google Analytics user account https://www.google.com/analytics


DEPENDENCIES
------------
* Google Analytics Reports API has no dependencies.
* Google Analytics Reports depends on Google Analytics Reports API and Views
  modules.


RECOMMENDED MODULES
-------------------
* Charts module https://www.drupal.org/project/charts. Enable Google Charts or
  Highcharts sub-module to see graphical reports.
* Ajax Blocks module https://www.drupal.org/project/ajaxblocks for better page
  loading with Google Analytics Reports blocks.


INSTALLATION
------------
1. Copy the 'google_analytics_reports' module directory in to your Drupal
   sites/all/modules directory as usual. See https://www.drupal.org/documentati
   on/install/modules-themes/modules-7 for details.

CONFIGURATION
-------------
Configuration of Google Analytics Reports API module:
1.  Open Google Developers Console https://console.developers.google.com.
2.  Press "Create Project" button, enter project name and press "Create".
3.  Open "APIs & auth" -> "APIs" page in created project, search for
    "Analytics API", open API page and press "Enable API".
4.  Open "APIs & auth" -> "Consent screen" page, enter "Product name" and press
    "Save" button.
5.  Open "APIs & auth" -> "Credentials" page and press "Create new Client ID"
    button.
6.  Select "Web application in Application type", leave empty "Authorized
    JavaScript origins", fill in "Authorized redirect URIs" with
    "http://YOURSITEDOMAIN/admin/config/system/google-analytics-reports-api"
    and press "Create Client ID" button.
7.  Go to "admin/config/system/google-analytics-reports-api" page.
8.  Copy "Client ID" and "Client secret" from opened Google Console page into
    your google-analytics-reports-api page.
9.  Press "Start setup and authorize" account and allow the project access
    to Google Analytics data.
10. Select reports profile for which you want to see the reports.

Configuration of Google Analytics Reports module:
1. Configure Google Analytics Reports API module first.
2. Enable Charts module and Google Charts or Highcharts sub-module to see
   graphical reports.
3. Go to "admin/reports/google-analytics-reports/summary" page to see
   Google Analytics Summary report.
4. Go to "admin/structure/block" page and enable "Google Analytics Reports
   Summary Block" and/or "Google Analytics Reports Page Block" blocks.


CACHING
-------
Note that Google has a moderately strict Quota Policy https://developers.google
.com/analytics/devguides/reporting/core/v3/limits-quotas#core_reporting. To aid
with this limitation, this module caches query results for a time that you
specify in the admin settings. Our recommendation is at least three days.

CREDITS
-------
* Joel Kitching (jkitching)
* Tony Rasmussen (raspberryman)
* Dylan Tack (grendzy)
* Nickolay Leshchev (Plazik)
