<?php

/**
 * @file
 * Describe hooks provided by the Lingotek Translation module.
 */

/**
 * Alter the Lingotek data for a translatable object prior to sending it for translation
 *
 * This is called just before new documents are sent to the Lingotek
 * Translation Management System (TMS).
 *
 * @param $translatable_obj
 *  An instance of the LingotekTranslatableEntity class
 */
function hook_lingotek_entity_pre_upload(LingotekTranslatableEntity $translatable_obj) {
}

