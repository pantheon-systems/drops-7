<?php

/**
 * @file
 * API hooks for the express_add_content module.
 */

/**
 * Example hook_express_node_list_groups().
 *
 * Group content types on 'node/add' page.
 *
 * @param $bundles
 *   An array of grouped content types.
 *     - 'group': A name you want to group content by.
 *                This can be any arbitrary name.
 *       - 'title': The title you want displayed above the grouping.
 *       - 'types': The human-readable name of the content type you want to add
 *                  to the group.
 */
function hook_express_node_list_groups($bundles) {
  $bundles['group']['title'] = 'News and Articles';
  $bundles['group']['types'][] = 'Article';
  return $bundles;
}

/**
 * Example hook_express_bean_list_groups().
 *
 * Group content types on 'block/add' page.
 *
 * @param $bundles
 *   An array of grouped content types.
 *     - 'group': A name you want to group beans by.
 *                This can be any arbitrary name.
 *       - 'title': The title you want displayed above the grouping.
 *       - 'types': The human-readable name of the bean type you want to add
 *                  to the group.
 */
function hook_express_bean_list_groups($bundles) {
  $bundles['group']['title'] = 'News and Articles';
  $bundles['group']['types'][] = 'Article List';
  return $bundles;
}
