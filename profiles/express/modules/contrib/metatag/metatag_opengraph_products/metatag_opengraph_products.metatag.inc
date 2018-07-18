<?php

/**
 * @file
 * Metatag integration for the metatag_opengraph_product module.
 */

/**
 * Implements hook_metatag_info().
 */
function metatag_opengraph_products_metatag_info() {
  // Open Graph products.
  $info['groups']['open-graph-products'] = array(
    'label' => t('Open Graph - Products'),
    'description' => t("These Open Graph meta tags for describing products."),
    'form' => array(
      '#weight' => 51,
    ),
  );

  // Default values for each meta tag.
  $defaults = array(
    'description' => '',
    'class' => 'DrupalTextMetaTag',
    'group' => 'open-graph-products',
    'element' => array(
      '#theme' => 'metatag_property',
    ),
  );

  $weight = 50;

  $info['tags']['product:price:amount'] = array(
    'label' => t('Price'),
    'description' => t("The numeric price with decimal point, without currency indicator. Values below 0.01 may not be supported by clients."),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:price:currency'] = array(
    'label' => t('Currency'),
    'description' => t("The currency for the price (if any)."),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:availability'] = array(
    'label' => t('Availability'),
    'description' => t('Case-insensitive string, possible values: "instock", "pending", "oos"; per <a href="@url">Facebook\' documentation</a>.', array('@url' => 'https://developers.facebook.com/docs/reference/opengraph/object-type/product')),
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:brand'] = array(
    'label' => t('Brand'),
    'description' => '',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:upc'] = array(
    'label' => t('UPC'),
    'description' => '',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:ean'] = array(
    'label' => t('EAN'),
    'description' => '',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:isbn'] = array(
    'label' => t('ISBN'),
    'description' => '',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:plural_title'] = array(
    'label' => t('Plural Title'),
    'description' => 'A title to be used to describe multiple items of this product',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:retailer'] = array(
    'label' => t('Retailer ID'),
    'description' => 'A Facebook ID (or reference to the profile) of the retailer.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:retailer_title'] = array(
    'label' => t('Retailer Name'),
    'description' => 'The name of the retailer.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:retailer_part_no'] = array(
    'label' => t('Retailer SKU/Product Number'),
    'description' => 'A retailer part number.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:mfr_part_no'] = array(
    'label' => t('Manufacturer SKU/Part Number'),
    'description' => 'A manufacturer part number.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:size'] = array(
    'label' => t('Size'),
    'description' => 'A size describing the product, such as S, M, L.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:product_link'] = array(
    'label' => t('Product Link'),
    'description' => 'A link to find out more about the product',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:category'] = array(
    'label' => t('Category'),
    // 'description' => 'A category',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:color'] = array(
    'label' => t('Color'),
    // 'description' => 'The product\'s color.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:material'] = array(
    'label' => t('Material'),
    'description' => 'A description of the material used to make the product.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:pattern'] = array(
    'label' => t('Pattern'),
    'description' => 'A description of the pattern used.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:shipping_cost:amount'] = array(
    'label' => t('Shipping Cost Amount'),
    'description' => 'The shipping costs.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:shipping_cost:currency'] = array(
    'label' => t('Shipping Cost Currency'),
    'description' => 'The shipping cost currency.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:weight:value'] = array(
    'label' => t('Product Weight'),
    'description' => 'The weight, without shipping materials.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:weight:units'] = array(
    'label' => t('Product Weight Units'),
    'description' => 'The unit of weight.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:shipping_weight:value'] = array(
    'label' => t('Shipping Weight'),
    'description' => 'The shipping weight.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:shipping_weight:units'] = array(
    'label' => t('Shipping Weight Units'),
    'description' => 'The unit of shipping weight.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:expiration_time'] = array(
    'label' => t('Expiration'),
    'description' => 'A time representing when the product expired, or will expire.',
    'weight' => ++$weight,
  ) + $defaults;
  $info['tags']['product:condition'] = array(
    'label' => t('Condition'),
    'description' => 'The condition of the product.',
    'weight' => ++$weight,
  ) + $defaults;

  return $info;
}
