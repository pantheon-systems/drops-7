This module provides a user interface to change the internal path/URI of entities.
This is done by replacing path patterns with token values of the given entity.
Technically this is achieved by replacing the entity's 'uri callback'.

Features
========

* Path patterns are configurable per entity type and bundle.
* Token replacement for path patterns.
* CTools integration to make settings importable and exportable, so configurations
are exportable via Features.

All functionality that relies on calling entity_uri() for building the the
internal path for the given entity, will work with the new pattern. For example
this is implemented in the pathauto module.

Token Types
-----------
Some entity types (like taxonomy terms) have a different token type than their
entity type (entity type: 'taxonomy_term', token type: 'term'). For this
entity_token (packaged in http://drupal.org/project/entity) is used to determine
the correct token type for an entity.
Without entity_token enabled, only vocabulary and term will work, else the
entity type will be stated as token type.

Example
=======

One use case is, to provide different views for terms of different vocabularies, e.g.:

* Vocabulary Tags: tags/[term:tid]
* Vocabulary Genre: genre/[term:tid]

Configuration
=============

The configuration for the module is located on admin/config/content/entity_path.
There entity_path settings for each entity type and bundle can be set.

An entity path for a specific bundle overrides the entity path
for the general entity type.

Development
===========

Please state issues for any problem, question or feature request you have on
http://drupal.org/project/issues/entity_path

Idea
====

The module originally was a proof of concept for Taxonomy Redirect and named
"Taxonomy Path" in sandbox mode. The original idea was from the
Taxonomy Redirect D7 Port Issue. As we now deal with Entities in Drupal 7 all
over the place, the concept could get generalized and now should work for any
entity properly implementing the API function entity_uri() for its path.