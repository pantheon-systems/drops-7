drupal-mailjet-module-apiv3
===========================

Mailjet APIv3 module for Drupal

This module for Drupal 7.x. provides complete control of Drupal Email settings with Mailjet and also adds specific Drupal Commerce email marketing functionality such as triggered marketing emails and marketing campaign revenue statistics.

The Mailjet Module for Drupal 7.x configures your default Drupal SMTP settings to use Mailjet's SMTP relay with enhanced deliverability and tracking.  The module also provides the ability to synchronise your Drupal opt-in contacts and send bulk and targeted emails to them with real time statistics including opens, clicks, geography, average time to click, unsubs, etc. 

Mailjet is a powerful all-in-one email service provider used to get maximum insight and deliverability results from both  marketing and transactional emails. Our analytics tools and intelligent APIs give senders the best understanding of how to maximize benefits for each individual contact and campaign email after email. 

Requirements
------------
  * Views (https://www.drupal.org/project/views)
  * Views Bulk Operations (https://www.drupal.org/project/views_bulk_operations)
  * Entity (https://www.drupal.org/project/entity)

Recommended modules
-------------------
  The following modules are not strictly required but it is nice to install them to get 
  the full capability of the Mailjet features.
  * commerce (https://www.drupal.org/project/commerce)
  For the stats sub module to enable the ROI feature install:
  * views_date_format_sql (https://www.drupal.org/project/views_date_format_sql)
  * charts (https://www.drupal.org/project/charts)
  For the list module you need to install:
  * viewfield (https://www.drupal.org/project/viewfield)

Installation
------------

1. Download a release from https://www.drupal.org/project/mailjet.
2. Upload the module in your Drupal sites/all/modules/ directory.
3. Log in as administrator in Drupal.
4. Enable the Mailjet settings module on the Administer > Site building > Modules page.
5. Fill in required settings on the Administer > Site configuration > Mailjet settings page.
6. You will be required to enter API key and Secret Key, if you do not have any, 
    you should go to https://www.mailjet.com/
    And signup for your credential data. You should enter those credentials under your 
    API tab (your_site/admin/config/system/mailjet/api). 

Configuration
-------------

1. The site can be set up to use the mailjet module as an email gateway, this can be easily configured, by clicking on the Settings tab => your_site/admin/config/system/mailjet , and then selecting the checkbox on the top, "Send emails through Mailjet", click "Save Settings" button on the bottom of the page. 
You can test that feature by sending a test email, just click the button on the top of the page Send test email in Settings tab.
2. If you want to enable the Campaign feature, you should enable the mailjet_campaign module, you can do that from Administer > Site building > Modules page (your_site/admin/modules)
3.  Enabling the campaign sub module will create additional menu item in your administration menu, 
    the new menu is called "Campaign" (your_site/admin/mailjet/campaign). 
4. Clicking this menu item will display all the campaigns created by the administrator, 
    from this point you will be able to create new campaigns as well, 
    the same way you do that on mailjet.com.
5. If you want to create a campaign simply go to the campaign page => your_site/admin/mailjet/campaign
    On the top right side of the page that will be presented there is a button “Create a campaign”, 
    clicking that button will lead you to a new page presenting a form that needs to be full fill, 
    this is the first out of three steps of creating a new campaign. The following fields are requiered - 
    title of the campaign, language, and contact list that you already created, 
    select your edition mode and click “Save and continue”.
    In the next step you should enter the “Sender name”, choose template of the email and write your email, 
    if you want you can add links inside the email body(the "TEXT" text area) 
    leading to your site and if a customer click on that link and purchase any product 
    from your site this order will be recorded in the ROI stats feature 
    where you can see how good is your conversion rate, click the “Done” button on the bottom of 
    the email text area, and click “Continue”, in the next step which is the last one you can choose 
    to send the email now or schedule it for later, click “Save and send”.
6. If you enable the stats module 2 menu items will appear Dashboard where you can see the results 
    of the mail campaigns and the Mailjet ROI stats, clicking the ROI stats you can see the actual 
    conversion of your campaigns, this feature will display a view which will present the campaign name, 
    number of orders made by users who clicked on the link of your site in your email campaign.
7. My account menu item will redirect you to the mailjet logging page.
8. Upgrade menu link will redirect you to the pricing list of mailjet where you can pick up a plan 
    and upgrade your account.
9. The contacts menu item allows you to create lists that can be used for your campaigns.
    If you click on Contacts the list of all contact lists will be displayed on the top right 
    side of the screen a button for creating a new contact list is available => Create a contact list. 
    If you click on the button a short notification from mailjet with some terms will appear 
    if you click the OK button you will be redirected to the creation form of your contacts list. 
    Here you need to enter your list name, choose an import method => 
    Upload from CSV, Copy/Paste from Excel, Copy/Paste from TXT, CSV, RTF. 
    Upload your file and click Create. 
    On the next step you should choose the import type email or mailjet_list_view click create.
10. If you want to enable the trigger_examples sub-module you need to enable the views_bulk_operations
    module and apply the following patch to it:  
    https://www.drupal.org/files/issues/views-vbo-patch-anon-users.patch
      
Author
------
Mailjet SAS
plugins@mailjet.com
