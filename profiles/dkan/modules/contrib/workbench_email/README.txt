/**
 * @file
 * README file for Workbench Email.
 */

Workbench Email

CONTENTS
--------

1.  Introduction
2.  Installation
2.1  Requirements
3.  Configuration
4.  Using the module
5.  Troubleshooting
6.  Developer notes
6.1  Database schema
7.  Feature roadmap

----
1.  Introduction

Workbench Email

----
1.1  Concepts

Extends Workbench Moderation by adding the ability to add emails to specific
transitions. Based on those email transitions, the admin can configure
each email's subject / message. Then when the content moves through the
specific transition, if an email transition is already set, the current
content editor has the ability to send email to those specific role based
user(s).

----
2.  Installation

Install the module and enable it according to Drupal standards.

The module's configuration pages reside at:
- admin/config/workbench/moderation/email-transitions
- admin/config/workbench/moderation/emails

The module depends on workbench moderation and will display help messages if
that module has not been setup correctly.

----
2.1  Requirements

Workbench Email requires:
- Workbench Moderation (and dependencies)
- Token

----
3.  Configuration

Workbench Moderation's configuration section is located at:

- Admin > Configuration > Workbench > Workbench Moderation -> Email Transitions

This section allows the admin to configure email transitions based on
transition states and user roles.

- Admin > Configuration > Workbench > Workbench Moderation -> Emails

Depending on what email transtions have been set, the admin can configure each
transitions subject / message.


----
3.3  Checking permissions

In order to use moderate the emails and email transitions, the user must be
given the appropriate role. Navigate to admin/people/permissions and
select Administer Workbench Moderation Emails under Workbench Email for the
appropriate role.

----
4.  Using the module

Once the module is installed and moderation is enabled for one or more node
types, users with permission may:

* Select the appropriate users that you wish to send an email to when moderated
content is moving through configured email transition.

----
5.  Troubleshooting

* If users do not see the node form select list that allows them to select the
user(s) they wish to send an email to, check the email transitions and emails
administration pages. If no email transition is defined, no form option will
display. If no email subject / message is defined, the system will display the
following message:
- No email template is set, so no email was sent. Contact your system admin
to resolve this issue.
* If no email templates are available within the administration area
(admin/config/workbench/moderation/emails), then check that you have email
transitions set (admin/config/workbench/moderation/email-transitions).
* If no email transitions are available (transitions show up but no roles
can be selected), then no roles have been associated to the moderation of
content. Check Workbench Moderation readme.txt to figure out the correct
permissions for this.

----
6.  Developer notes

This is my first drupal contributed module, so I know there is room for
improvements. Emails being sent when content moves through a transition is
something that clients always seem to request but no system has existed that
is easy to configure and can be exported (featured). I know rules, actions,
triggers etc exist but they are cumbersome to configure and export (IMO).
So, I'm curious to see what other people think about this, feel free to
comment :)

----
6.1  Database schema

Workbench Email uses two tables to store emails and email transitions.

* workbench_email_transitions
  Stores administrator-configured email transitions.

* workbench_emails
  Stores administrator-configured subject / message for each email transition

----
7.  Feature roadmap
