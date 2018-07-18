# Embed Templates Module

## Background:
Clients often have a need for taking code from a third-party service and injecting 
that into their site through embed codes. Tracking pixels, third-party forms, and 
media assets are good examples of embeds that your clients might request for their 
web application.

With most sites giving users a text filter that strips out JavaScript and other 
HTML tags, it can be difficult to manage the injection of these embeds at scale. 
The purpose of the Embed Templates module is to present users with text fields 
for the unique information that is inserted in template files containing JavaScript 
or other restricted HTML tags. This way users with only basic permissions can manage 
embeds all by themselves without requiring an administrator to approve each one.

The concept of Embed Templates is split into two parts: definitions and renderers.

## Definitions
An embed template entity can have an associated "type" that tells the module how 
to define the data structure of the embed that is then serialized for storage and 
rendered in the template file later. 

Every embed type will share some common fields in order to allow users the ability 
to manage embeds from an administration screen. A label, path, and status will be 
attached to every embed entity, and your template variables will be serialized and
stored in a table with the other information. That's all the embed entity is: simple.

## Renderers
Each embed type then needs a way to render the serialized data that a user has 
entered. Since the key/value pairs are arbitrary, you could have the same forms 
and submission callbacks tied to multiple renderers. Each renderer will live in 
its own module so you can easily only add what you need for your specific use case.

## Installing The Module
As every other Drupal module says, install this module as you normally would. 
There are no external libraries to install and dependencies are kept as simple as
possible. 

## Administation 
Several permissions are created for each embed type that you will have to assign 
to the proper user roles before you allow any users to create an embed entity. 
You can find those permissions under "Embed Templates" on the user permissions 
admin screen.

The overall administration screen for embeds is located at "admin/content/embeds"
on your Drupal site. From that screen you can add, edit, search for, and perform 
bulk operations on embeds. 

## Development and Extension
The main embed_templates module tries to provide as many helper functions as possible 
for you to add embed types and renderers with as little code as possible. 

You should look through the functions in that module for help on how to query for 
information you might need. There are several getter functions included. 

As far as adding embed types to this module, only embed renderers will be considered 
for future additions. We don't want to have a million different services and templates 
included as sub-modules, but we do want to allow other developers to contribute 
shared functionality via common functions and additional renderers. 

If you come up with an embed template for a popular service, please create a project
starting with the "embed_templates_" namespace and file an issue to be added to 
this readme as an extension. Once enough embed types are created, we will split 
them into categories, but for now, they will all be lumped together. 
