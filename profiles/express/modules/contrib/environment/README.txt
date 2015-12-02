
ENVIRONMENT

Creates an API for specifying an environment for a site instance.  Other modules
may then change their logic depending upon the current active environment. When
switching environments a hook is invoked to allow any module to run additional
required actions.

The environment states are defined for use in hook_environment() and so 
may be unique to the current project. Predefined environments are 'development' 
and 'production'.

This module provides a UI to facilitate basic use, but is at heart a 
developer's module, providing the ability to perform site operations as 
part of a state machine transition.

Environment module now support now supports multiple environments per site. The
"default" workflow environment is the same as previous module behavior, and
includes the sample environment states of "development" and "production".
Creating new workflows is a developer task, much as creating new environment
states.

USING DRUSH

Environment provides two drush commands, 'environment' and 
'environment-switch'. Environment tells you the current environment, 
environment-switch toggles the current environment state as you declare. 
For usage instructions, turn on the Environment module and type `drush 
help environment` and `drush help environment-switch`.

FOR DEVELOPERS

'environment.api.php' describes the use of hook_environment() to define 
new environment states, hook_environment_alter() to tweak those 
definitions, and hook_environment_switch() to trigger any environment 
change operations defined in any module of your site.

To create new workflows, implement hook_environment_workflow() and at least
define a label element. To categorize an environment state as part of a given
workflow, just add that element to the environment definition.

The `environment_allowed()` function is a method for asserting behaviors 
predefined as permitted or not permitted in the environment definition.

FOR ADMINISTRATORS

Once installed, you can toggle the current environment amongst any 
defined by visiting 'admin/settings/environment'. This will do nothing 
unless the site has been built to do something depending on the current 
environment.

Sites which make heavy use of the Environment module may choke if you 
try to use the UI to toggle the environment state. In that case, you may 
want to use `hook_form_alter()` to hide that option.

If you prefer to manage environment states entirely in code, you can 
check on 'Require environment override', this will help you enforce that 
an `environment_override` variable defined in your settings.php file is 
correctly used as your environment. You may also simply define the 
`environment` variable, but you will then lose some of the extra UI help 
that explains why the UI no longer updates the environment state.

When you define the environment state in settings.php, you will still 
need to run `drush env-switch` to update the environment to that state, 
possibly using the --force flag if the system thinks it is already in 
that state.

Check 'admin/reports/status' for any problems with your Environment 
configuration.

FOR SITE BUILDERS

The Environment module provides Token and Context integration to 
facilitate building actual functionality around your current 
environment. This is primarily envisioned as mechanisms to help inform 
the current site user of the current environment. If you do anything 
inventive, be sure to let us know!

There are 'environment' and 'environment-label' tokens that will show 
the machine name or human title for the current environment.

There is also an Environment condition for the Context module that will 
allow you to trigger block placement or other context reactions based on 
the site environment.
