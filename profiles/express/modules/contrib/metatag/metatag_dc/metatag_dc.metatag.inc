<?php

/**
 * @file
 * Metatag integration for the metatag_dc module.
 */

/**
 * Implements hook_metatag_bundled_config_alter().
 */
function metatag_dc_metatag_bundled_config_alter(array &$configs) {
  foreach ($configs as &$config) {
    switch ($config->instance) {
      case 'global':
        $config->config += array(
          'dcterms.format' => array('value' => 'text/html'),
          'dcterms.identifier' => array('value' => '[current-page:url:absolute]'),
          'dcterms.title' => array('value' => '[current-page:title]'),
          'dcterms.type' => array('value' => 'Text'),
        );
        break;

      case 'global:frontpage':
        $config->config += array(
          'dcterms.description' => array('value' => '[site:slogan]'),
          'dcterms.identifier' => array('value' => '[site:url]'),
          'dcterms.title' => array('value' => '[site:name]'),
        );
        break;

      // On error pages point everything to the homepage.
      case 'global:403':
      case 'global:404':
        $config->config += array(
          'dcterms.identifier' => array('value' => '[site:url]'),
          'dcterms.title' => array('value' => '[site:name]'),
        );
        break;

      case 'node':
        $config->config += array(
          'dcterms.creator' => array('value' => '[node:author]'),
          'dcterms.date' => array('value' => '[node:created:custom:Y-m-d\TH:iP]'),
          'dcterms.description' => array('value' => '[node:summary]'),
          'dcterms.language' => array('value' => '[node:language]'),
          'dcterms.title' => array('value' => '[node:title]'),
        );
        break;

      case 'taxonomy_term':
        $config->config += array(
          'dcterms.description' => array('value' => '[term:description]'),
          'dcterms.title' => array('value' => '[term:name]'),
        );
        break;

      case 'user':
        $config->config += array(
          'dcterms.creator' => array('value' => '[user:name]'),
          'dcterms.date' => array('value' => '[user:created:custom:Y-m-d\TH:iP]'),
          'dcterms.title' => array('value' => '[user:name]'),
        );
        break;
    }
  }
}

/**
 * Implements hook_metatag_info().
 * Dublin Core Elements taken from http://purl.org/dc/elements/1.1/.
 */
function metatag_dc_metatag_info() {
  $info['groups']['dublin-core'] = array(
    'label' => t('Dublin Core Basic Tags'),
    'description' => t('The Dublin Core Metadata Element Set, aka "Dublin Core meta tags", are a set of internationally standardized metadata tags used to describe content to make identification and classification of content easier; the standards are controlled by the <a href="@url">Dublin Core Metadata Initiative (DCMI)</a>.', array('@url' => 'http://dublincore.org/')),
    'form' => array(
      '#weight' => 70,
    ),
  );

  // Dublin Core meta tags stack after the Twitter Cards tags.
  $weight = 70;

  // Basic tags.
  $defaults = array(
    'class' => 'DrupalTextMetaTag',
    'group' => 'dublin-core',
    'element' => array(
      '#type' => 'term',
      '#theme' => 'metatag_dc',
    ),
  );

  $info['tags']['dcterms.title'] = array(
    'label' => t('Title'),
    'description' => t('The name given to the resource.'),
    'element' => array(
      '#type' => 'term',
      '#theme' => 'metatag_dc',
    ),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.creator'] = array(
    'label' => t('Creator'),
    'description' => t('An entity primarily responsible for making the resource. Examples of a Creator include a person, an organization, or a service. Typically, the name of a Creator should be used to indicate the entity.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.subject'] = array(
    'label' => t('Subject'),
    'description' => t('The topic of the resource. Typically, the subject will be represented using keywords, key phrases, or classification codes. Recommended best practice is to use a controlled vocabulary. To describe the spatial or temporal topic of the resource, use the Coverage element.'),
    'maxlength' => 0,
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.description'] = array(
    'label' => t('Description'),
    'description' => t('An account of the resource. Description may include but is not limited to: an abstract, a table of contents, a graphical representation, or a free-text account of the resource.'),
    'maxlength' => 0,
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.publisher'] = array(
    'label' => t('Publisher'),
    'description' => t('An entity responsible for making the resource available. Examples of a Publisher include a person, an organization, or a service. Typically, the name of a Publisher should be used to indicate the entity.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.contributor'] = array(
    'label' => t('Contributor'),
    'description' => t('An entity responsible for making contributions to the resource. Examples of a Contributor include a person, an organization, or a service. Typically, the name of a Contributor should be used to indicate the entity.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.date'] = array(
    'label' => t('Date'),
    'description' => t('A point or period of time associated with an event in the lifecycle of the resource. Date may be used to express temporal information at any level of granularity.  Recommended best practice is to use an encoding scheme, such as the W3CDTF profile of ISO 8601 [W3CDTF].'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.type'] = array(
    'label' => t('Type'),
    'description' => t('The nature or genre of the resource. Recommended best practice is to use a controlled vocabulary such as the DCMI Type Vocabulary [DCMITYPE]. To describe the file format, physical medium, or dimensions of the resource, use the Format element.'),
    'form' => array(
      '#type' => 'select',
      '#options' => _metatag_dc_dcmi_type_vocabulary_options(),
      '#empty_option' => t('- None -'),
    ),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.format'] = array(
    'label' => t('Format'),
    'description' => t('The file format, physical medium, or dimensions of the resource. Examples of dimensions include size and duration. Recommended best practice is to use a controlled vocabulary such as the list of Internet Media Types [MIME].'),
    'devel_generate' => array(
      'maxlength' => 1,
    ),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.identifier'] = array(
    'label' => t('Identifier'),
    'description' => t('An unambiguous reference to the resource within a given context. Recommended best practice is to identify the resource by means of a string conforming to a formal identification system.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.source'] = array(
    'label' => t('Source'),
    'description' => t('A related resource from which the described resource is derived. The described resource may be derived from the related resource in whole or in part. Recommended best practice is to identify the related resource by means of a string conforming to a formal identification system.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.language'] = array(
    'label' => t('Language'),
    'description' => t('A language of the resource. Recommended best practice is to use a controlled vocabulary such as RFC 4646 [RFC4646].'),
    'is_language' => TRUE,
    'devel_generate' => array(
      'maxlength' => 1,
    ),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.relation'] = array(
    'label' => t('Relation'),
    'description' => t('A related resource. Recommended best practice is to identify the related resource by means of a string conforming to a formal identification system.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.coverage'] = array(
    'label' => t('Coverage'),
    'description' => t('The spatial or temporal topic of the resource, the spatial applicability of the resource, or the jurisdiction under which the resource is relevant. Spatial topic and spatial applicability may be a named place or a location specified by its geographic coordinates. Temporal topic may be a named period, date, or date range. A jurisdiction may be a named administrative entity or a geographic place to which the resource applies. Recommended best practice is to use a controlled vocabulary such as the Thesaurus of Geographic Names [TGN]. Where appropriate, named places or time periods can be used in preference to numeric identifiers such as sets of coordinates or date ranges.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.rights'] = array(
    'label' => t('Rights'),
    'description' => t('Information about rights held in and over the resource. Typically, rights information includes a statement about various property rights associated with the resource, including intellectual property rights.'),
    'weight' => ++$weight,
  ) + $defaults;

  return $info;
}

/**
 * List the DCMI 'type' options.
 *
 * Types taken from http://dublincore.org/documents/dcmi-type-vocabulary/.
 */
function _metatag_dc_dcmi_type_vocabulary_options() {
  $options = array(
    'Collection',
    'Dataset',
    'Event',
    'Image',
    'InteractiveResource',
    'MovingImage',
    'PhysicalObject',
    'Service',
    'Software',
    'Sound',
    'StillImage',
    'Text',
  );
  return drupal_map_assoc($options);
}
