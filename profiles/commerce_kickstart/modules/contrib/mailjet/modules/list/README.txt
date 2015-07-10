Instructions for Creating a MailJet List

The MailJet module for Drupal leverages the power of views enabling you to create customised lists of email address filterable by any number of criteria.
 Providing the list includes afield that contains an email address any other fields or filters are admissible. 

To use the MailJet List module you first need to configure the MailJet module with your API 
credentials here admin/config/system/mailjet once you have entered both your API key and secret key you are able to synchronise a MailJet list. 

To create a MailJet list you start by creating a view as you would any standard view. 
Once you have decided on your criteria and have build the filters you want to include you need to change the view format/ view style to be that of ‘MailJet List’ this will ensure it is available in the list of views when adding a mail jet list. When you select this style plugin you are presented with an options form asking you to select the field which contains the email address which is what we listed as a requirement. Select which field contains the email address field you wish to be synced with MailJet. 
You are then able to save the view and this will be available when you create a new MailJet list in the modules UI.

Now you can visit admin/mailjet/list/add to create a MailJet list on MailJet using your new view. 
The form has some standard fields that control the name of your list on MailJet. 
The ID field is disabled and is completed by the module when the list synchronises with MailJet.

In the List view select box you can select the view that we created previously followed by any contextual arguments that this view needs, if it doesn’t need any leave this field blank.

Now you can save the list and it will synchronise with MailJet.

Your list will be created on MailJet and now if you view your MailJet list you have a tab called ‘Sync’  from here you can update the contacts on MailJet and also removed any expired contacts. 

