<?php
/**
 * @file
 * Metatag integration for the metatag_dc module.
 */

/**
 * Implements hook_metatag_bundled_config_alter().
 */
function metatag_dc_advanced_metatag_bundled_config_alter(array &$configs) {
  foreach ($configs as &$config) {
    switch ($config->instance) {
      case 'node':
        $config->config += array(
          'dcterms.modified' => array('value' => '[node:changed:custom:Y-m-d\TH:iP]'),
        );
        break;
    }
  }
}

/**
 * Implements hook_metatag_info().
 * Dublin Core Elements taken from http://purl.org/dc/elements/1.1/.
 */
function metatag_dc_advanced_metatag_info() {
  $info['groups']['dublin-core-additional'] = array(
    'label' => t('Dublin Core Additional Tags'),
    'description' => t('These tags are not part of the Metadata Element Set but may be useful for certain scenarios.'),
    'form' => array(
      '#weight' => 71,
    ),
  );

  // Dublin Core meta tags stack after the Twitter Cards tags.
  $weight = 80;

  // Additional tags.
  $defaults = array(
    'class' => 'DrupalTextMetaTag',
    'group' => 'dublin-core-additional',
    'element' => array(
      '#type' => 'term',
      '#theme' => 'metatag_dc',
    ),
  );

  $info['tags']['dcterms.abstract'] = array(
    'label' => t('Abstract'),
    'description' => t('A summary of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.accessRights'] = array(
    'label' => t('Access rights'),
    'description' => t('Information about who can access the resource or an indication of its security status.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.accrualMethod'] = array(
    'label' => t('Accrual Method'),
    'description' => t('The method by which items are added to a collection.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.accrualPeriodicity'] = array(
    'label' => t('Accrual Periodicity'),
    'description' => t('The frequency with which items are added to a collection.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.accrualPolicy'] = array(
    'label' => t('Accrual Policy'),
    'description' => t('The policy governing the addition of items to a collection.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.alternative'] = array(
    'label' => t('Alternative Title'),
    'description' => t('An alternative name for the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.audience'] = array(
    'label' => t('Audience'),
    'description' => t('A class of entity for whom the resource is intended or useful.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.available'] = array(
    'label' => t('Date Available'),
    'description' => t('Date (often a range) that the resource became or will become available.'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.bibliographicCitation'] = array(
    'label' => t('Bibliographic Citation'),
    'description' => t('A bibliographic reference for the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.conformsTo'] = array(
    'label' => t('Conforms To'),
    'description' => t('An established standard to which the described resource conforms.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.created'] = array(
    'label' => t('Date Created'),
    'description' => t('Date of creation of the resource.'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.dateAccepted'] = array(
    'label' => t('Date Accepted'),
    'description' => t('Date of acceptance of the resource.'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.dateCopyrighted'] = array(
    'label' => t('Date Copyrighted'),
    'description' => t('Date of copyright.'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.dateSubmitted'] = array(
    'label' => t('Date Submitted'),
    'description' => t('Date of submission of the resource.'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.educationLevel'] = array(
    'label' => t('Audience Education Level'),
    'description' => t('A class of entity, defined in terms of progression through an educational or training context, for which the described resource is intended.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.extent'] = array(
    'label' => t('Extent'),
    'description' => t('The size or duration of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.hasFormat'] = array(
    'label' => t('Has Format'),
    'description' => t('A related resource that is substantially the same as the pre-existing described resource, but in another format.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.hasPart'] = array(
    'label' => t('Has Part'),
    'description' => t('A related resource that is included either physically or logically in the described resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.hasVersion'] = array(
    'label' => t('Has Version'),
    'description' => t('A related resource that is a version, edition, or adaptation of the described resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.instructionalMethod'] = array(
    'label' => t('Instructional Method'),
    'description' => t('A process, used to engender knowledge, attitudes and skills, that the described resource is designed to support.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.isFormatOf'] = array(
    'label' => t('Is Format Of'),
    'description' => t('A related resource that is substantially the same as the described resource, but in another format.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.isPartOf'] = array(
    'label' => t('Is Part Of'),
    'description' => t('A related resource in which the described resource is physically or logically included.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.isReferencedBy'] = array(
    'label' => t('Is Referenced By'),
    'description' => t('A related resource that references, cites, or otherwise points to the described resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.isReplacedBy'] = array(
    'label' => t('Is Replaced by'),
    'description' => t('A related resource that supplants, displaces, or supersedes the described resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.isRequiredBy'] = array(
    'label' => t('Is Required By'),
    'description' => t('A related resource that requires the described resource to support its function, delivery, or coherence.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.isVersionOf'] = array(
    'label' => t('Is Version Of'),
    'description' => t('A related resource of which the described resource is a version, edition, or adaptation.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.issued'] = array(
    'label' => t('Date Issued'),
    'description' => t('Date of formal issuance (e.g., publication) of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.license'] = array(
    'label' => t('License'),
    'description' => t('A legal document giving official permission to do something with the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.mediator'] = array(
    'label' => t('Mediator'),
    'description' => t('An entity that mediates access to the resource and for whom the resource is intended or useful.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.medium'] = array(
    'label' => t('Medium'),
    'description' => t('The material or physical carrier of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.modified'] = array(
    'label' => t('Modified Date'),
    'description' => t('Date on which the resource was changed.'),
    'weight' => ++$weight,
    'devel_generate' => array(
      'type' => 'date',
    ),
  ) + $defaults;
  $info['tags']['dcterms.provenance'] = array(
    'label' => t('Provenance'),
    'description' => t('A statement of any changes in ownership and custody of the resource since its creation that are significant for its authenticity, integrity, and interpretation.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.references'] = array(
    'label' => t('References'),
    'description' => t('A related resource that is referenced, cited, or otherwise pointed to by the described resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.replaces'] = array(
    'label' => t('Replaces'),
    'description' => t('A related resource that is supplanted, displaced, or superseded by the described resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.requires'] = array(
    'label' => t('Requires'),
    'description' => t('A related resource that is required by the described resource to support its function, delivery, or coherence.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.rightsHolder'] = array(
    'label' => t('Rights Holder'),
    'description' => t('A person or organization owning or managing rights over the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.spatial'] = array(
    'label' => t('Spatial'),
    'description' => t('Spatial characteristics of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.tableOfContents'] = array(
    'label' => t('Table Of Contents'),
    'description' => t('A list of subunits of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.temporal'] = array(
    'label' => t('Temporal'),
    'description' => t('Temporal characteristics of the resource.'),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['dcterms.valid'] = array(
    'label' => t('Valid'),
    'description' => t('Date (often a range) of validity of a resource.'),
    'weight' => ++$weight,
  ) + $defaults;

  return $info;
}
