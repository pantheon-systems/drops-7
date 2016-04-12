## CONTENTS OF THIS FILE

 * Description


## DESCRIPTION

It is common for a rule to be generated based on an entity (discounts, shipping
rates, etc).

This module allows conditions to be defined on the entity add / edit form, and
those conditions are later mapped to rules conditions when the rule is
generated.

Inline Conditions are specially defined (hook_inline_condition_info()) and
consist of a configure callback (provides a user-facing form) and a build
callback (adds the actual condition to the rule). Integration consists of
creating a field of the "inline_conditions" type on the entity, and later
calling inline_conditions_build_rule() from the implementation of
hook_default_rules_configuration(). See inline_conditions.api.php for more
information.
