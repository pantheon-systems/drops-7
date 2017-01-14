<?php

/**
 * @file
 * Defines LingotekProfile
 */

/**
 * A class wrapper for Lingotek Profiles
 */
class LingotekProfile {

  protected static $profiles;
  protected static $global_profile;
  protected $profile;
  protected $profile_id;
  protected $inherit;

  /**
   * Constructor.
   *
   * This is private since we want consumers to instantiate via the factory methods.
   *
   * @param $profile_name
   *   The name of the profile to load
   */
  private function __construct($profile_id, $profile_attributes = array()) {
    $this->setId($profile_id);
    $this->setInherit(TRUE);
    $this->refresh();
    
    if (empty(self::$global_profile)) {
      self::$global_profile = lingotek_get_global_profile();
    }

    if ($profile_id === LingotekSync::PROFILE_DISABLED || $profile_id === LingotekSync::PROFILE_ENABLED) {
      $this->setName($profile_id);
      return $this;
    }
    if ($profile_id === LingotekSync::PROFILE_INHERIT) {
      $this->setName($profile_id);
      return LingotekSync::PROFILE_INHERIT;
    }
    if (empty(self::$profiles[$profile_id]) && !empty($profile_attributes)) {
      // create one on the fly
      $unique_attributes = array();
      foreach ($profile_attributes as $key => $value) {
        if (empty(self::$global_profile[$key]) || self::$global_profile[$key] !== $value) {
          $unique_attributes[$key] = $value;
        }
      }
      self::$profiles[$profile_id] = $unique_attributes;
      $this->save();
    }
    // A convenience reference to the current profile.
    $this->profile = &self::$profiles[$profile_id];
  }

  public static function create($profile_id, array $profile_attributes) {
    if (isset(self::$profiles[$profile_id])) {
      throw new LingotekException('Unable to create profile "' . $profile_id . '": profile already exists.');
    }
    return new LingotekProfile($profile_id, $profile_attributes);
  }

  public static function update($profile_id, array $profile_attributes) {
    if (!isset(self::$profiles[$profile_id])) {
      throw new LingotekException('Unable to update profile "' . $profile_id . '": profile does not exist.');
    }
    $profile = self::loadById($profile_id);
    foreach ($profile_attributes as $key => $value) {
      if (!empty(self::$global_profile[$key]) && self::$global_profile[$key] === $value) {
        // remove any attributes that are the same as the global ones
        $profile->deleteAttribute($key);
      }
      else {
        // keep any custom attributes
        $profile->setAttribute($key, $value);
      }
    }
  }

  public static function loadById($profile_id) {
    return new LingotekProfile($profile_id);
  }

  public static function loadByName($profile_name) {
    $this->refresh();
    foreach (self::$profiles as $profile_id => $profile) {
      if ($profile['name'] == $profile_name) {
        return new LingotekProfile($profile_id);
      }
    }
    throw new LingotekException('Unknown profile name: ' . $profile_name);
  }

  public static function loadByBundle($entity_type, $bundle, $source_locale = NULL) {
    $entity_profiles = variable_get('lingotek_entity_profiles', array());
    if (!empty($source_locale) && isset($entity_profiles[$entity_type][$bundle . '__' . $source_locale])) {
      try {
        $profile = new LingotekProfile($entity_profiles[$entity_type][$bundle . '__' . $source_locale]);
        if ($profile->getName() == LingotekSync::PROFILE_INHERIT) {
          $profile = new LingotekProfile($entity_profiles[$entity_type][$bundle]);
        }
        return $profile;
      }
      catch (Exception $e) {
        // TODO: a debug statement perhaps saying there are no customizations for the given source locale?
      }
    }
    if (isset($entity_profiles[$entity_type][$bundle])) {
      return new LingotekProfile($entity_profiles[$entity_type][$bundle]);
    }
    return self::loadById(LingotekSync::PROFILE_DISABLED);
  }

  public static function loadByEntity($entity_type, $entity) {
    list($id, $vid, $bundle) = lingotek_entity_extract_ids($entity_type, $entity);
    $result = db_select('lingotek_entity_metadata', 'l')
      ->fields('l', array('value'))
      ->condition('l.entity_id', $id)
      ->condition('l.entity_type', $entity_type)
      ->condition('l.entity_key', 'profile')
      ->execute();
    if ($result) {
      $profile_id = $result->fetchfield();
      if ($profile_id !== FALSE) {
        return self::loadById($profile_id);
      }
    }
    $source_locale = lingotek_entity_locale($entity_type, $entity);
    return self::loadByBundle($entity_type, $bundle, $source_locale);
  }

  // Return the profile ID for a given entity.
  public static function getIdByEntity($entity_type, $entity) {
    $profile = self::loadByEntity($entity_type, $entity);
    return $profile->getId();
  }

  // @params TRUE or FALSE, depending on whether the profile should look for inherited attributes
  //     from the global profile
  public function setInherit($inherit) {
    $this->inherit = (bool) $inherit;
  }

  public function lookForInherited() {
    return $this->inherit;
  }

  public function getId() {
    return $this->profile_id;
  }

  // IDs should either be auto-generated or special-case IDs, not configurable by the user
  protected function setId($profile_id) {
    $this->profile_id = $profile_id;
  }

  // replaces lingotek_admin_profile_usage() in lingotek.admin.inc
  // replaces lingotek_admin_profile_usage_by_types() in lingotek.admin.inc
  public function getUsage($by_bundle = FALSE) {
    if ($by_bundle) {
      $bundles_using_profile = lingotek_get_bundles_by_profile_id($this->getId());
      $count_types = 0;
      foreach ($bundles_using_profile as $bup) {
        $count_types += count($bup);
      }
      return $count_types;
    }
    else {
      /**
       *This is a representation of the query and subquery we are building to get
       *the usage for each profile.
       *@author t.murphy, smithworx, jbhovik, clarticus
       *
       *
       *SELECT count(*) as COUNT, entity_type as ENTITY_TYPE
       *FROM lingotek_entity_metadata
       *WHERE entity_key = 'profile'
       *AND value = '<profile_id>'
       *AND entity_id NOT IN
       *           (SELECT entity_id
       *           FROM lingotek_entity_metadata
       *           WHERE entity_key = 'upload_status'
       *           AND value = 'TARGET')
       *GROUP BY entity_type;
       *
       */


      $subquery = db_select('lingotek_entity_metadata', 'lem')
          ->fields('lem', array('entity_id'))
          ->condition('lem.entity_key', 'upload_status')
          ->condition('lem.value', 'TARGET');
      $entity_ids = $subquery->execute()->fetchCol();

      $query = db_select('lingotek_entity_metadata', 'lem')
          ->fields('lem', array('entity_type'))
          ->condition('lem.entity_key', 'profile')
          ->condition('lem.value', $this->getId());

      if (!empty($entity_ids)) {
        $query->condition('lem.entity_id', $entity_ids, 'NOT IN');
      }


      $query->groupBy('lem.entity_type');
      $query->addExpression('count(lem.entity_id)', 'COUNT');
      $entities = $query->execute()->fetchAll();

      $entity_counts = array();
      foreach ($entities as $e) {
        $entity_counts[$e->entity_type] = $e->COUNT;
      }
      return $entity_counts;
    }
  }

  public function getDocumentIds() {
    $metadata_table = $this->getId() === LingotekSync::PROFILE_CONFIG ? 'lingotek_config_metadata' : 'lingotek_entity_metadata';
    $metadata_key_col = $this->getId() === LingotekSync::PROFILE_CONFIG ? 'config_key' : 'entity_key';
    $query = db_select($metadata_table, 't')
      ->fields('t', array('value'))
      ->condition('t.' . $metadata_key_col, 'document_id');
    return $query->execute()->fetchcol();
  }

  public function getBundles() {
    $entities = entity_get_info();
    $lentities = variable_get('lingotek_entity_profiles');
    $bundles = array();
    foreach ($entities as $entity_name => $entity) {
      if (!isset($lentities[$entity_name])) {
        unset($entities[$entity_name]);
      }
      foreach ($entity['bundles'] as $bundle_name => $bundle) {
        if (isset($lentities[$entity_name][$bundle_name]) && $lentities[$entity_name][$bundle_name] === (string)$this->getId()) {
          if (!isset($bundles[$entity_name])) {
            $bundles[$entity_name] = array();
          }
          $bundles[$entity_name][$bundle_name] = TRUE;
        }
      }
    }
    return $bundles;
  }

  public function getEntities($entity_type = NULL) {
    if (!empty($entity_type)) {
      // get all bundles that belong to the given profile
      $all_bundles = $this->getBundles();
      $bundles = array();
      $entities = array();

      if (isset($all_bundles[$entity_type])) {
        $bundles = array($entity_type => $all_bundles[$entity_type]);
      }

      // get all entities that belond to those bundles
      foreach ($bundles as $entity_type => $entity_bundles) {
        if ($entity_type == 'comment') {
          $ref_tables = array();
          foreach (array_keys($entity_bundles) as $key) {
            $tmp_array = explode('_', $key);
            $key = implode('_', array_slice($tmp_array, 2));
            $ref_tables[] = $key;
          }
          $query = db_select('' . $entity_type . '', 'e')
              ->fields('e', array('cid'));
          $query->join('node', 'n', "n.nid = e.nid AND n.type IN ('" . implode("','", $ref_tables) . "')");
          $results = $query->execute()->fetchCol();
          foreach ($results as $id) {
            $entities[] = array('id' => $id, 'type' => $entity_type);
          }
        }
        else {
          $query = new EntityFieldQuery();
          $query->entityCondition('entity_type', $entity_type)
              ->entityCondition('bundle', array_keys($entity_bundles), 'IN');
          $result = $query->execute();
          unset($query);
          if (isset($result[$entity_type])) {
            foreach ($result[$entity_type] as $id => $entity_data) {
              $entities[] = array('id' => $id, 'type' => $entity_type);
            }
          }
        }
        // END OPTIMIZED WAY
      }

      // subtract all entities specifically *not* set to the given profile
      $query = db_select('lingotek_entity_metadata', 'lem')
          ->fields('lem', array('entity_id', 'entity_type'))
          ->condition('lem.entity_key', 'profile')
          ->condition('lem.value', $this->getId(), '!=')
          ->condition('lem.entity_type', $entity_type);
      $result = $query->execute();
      $subtract_entity_ids = $result->fetchAll();

      $doc_ids = lingotek_get_document_id_tree();
      $subtractions = array();
      foreach ($subtract_entity_ids as $sei) {
        if (!isset($subtractions[$sei->entity_type])) {
          $subtractions[$sei->entity_type] = array();
        }
        $subtractions[$sei->entity_type][$sei->entity_id] = TRUE;
      }
      $filtered_entities = array();
      foreach ($entities as $e) {
        if (!isset($subtractions[$e['type']][$e['id']])) {
          if (isset($doc_ids[$e['type']][$e['id']])) {
            $e['document_id'] = $doc_ids[$e['type']][$e['id']];
          }
          $filtered_entities[$e['id']] = $e;
        }
      }

      // add all entities specifically set to the given profile
      $query = db_select('lingotek_entity_metadata', 'lem')
          ->fields('lem', array('entity_id', 'entity_type'))
          ->condition('lem.entity_key', 'profile')
          ->condition('lem.value', $this->getId());
      if ($entity_type != 'all') {
        $query->condition('lem.entity_type', $entity_type);
      }
      $result = $query->execute();
      $add_entity_ids = $result->fetchAll();
      foreach ($add_entity_ids as $aei) {
        $addition = array('id' => $aei->entity_id, 'type' => $aei->entity_type);
        if (isset($doc_ids[$aei->entity_type][$aei->entity_id])) {
          $addition['document_id'] = $doc_ids[$aei->entity_type][$aei->entity_id];
        }
        $filtered_entities[$aei->entity_id] = $addition;
      }
      return $filtered_entities;
    }
    else { // GATHER LIST OF ENTITIES AND RECURSE

      // gather all bundles for searching, as some entities may be one-offs
      // even though Lingotek is not enabled for the entire bundle.
      self::$profiles[LingotekSync::PROFILE_DISABLED] = TRUE;
      // TODO: CREATE SOME KIND OF FOREACH LOOP
      $all_bundles = lingotek_get_bundles_by_profile_id(array_keys(self::$profiles));
      $all_entities = array();
      // aggregate all entity-type-specific results into a single numbered array
      foreach (array_keys($all_bundles) as $entity_type) {
        $entities = $this->getEntities($entity_type);
        foreach ($entities as $e) {
          $all_entities[] = $e;
        }
      }
      return $all_entities;
    }
  }

  public function save() {
    if ($this->getId() !== LingotekSync::PROFILE_DISABLED && $this->getId() !== LingotekSync::PROFILE_ENABLED) {
      variable_set('lingotek_profiles', self::$profiles);
    }
  }

  public function refresh() {
    self::$profiles = variable_get('lingotek_profiles', array());
  }

  public function delete() {
    if (!$this->isProtected()) {
      if ($this->getEntities()) {
        throw new LingotekException('Unable to delete profile "@name": profile not empty.', array('@name' => $this->getName()));
      }
      unset(self::$profiles[$this->getId()]);
      variable_set('lingotek_profiles', self::$profiles);
    }
  }

  public function getName() {
    return $this->getAttribute('name');
  }

  public function setName($profile_name) {
    $this->setAttribute('name', $profile_name);
  }

  public function isAutoDownload($target_locale = NULL) {
    return $this->getAttribute('auto_download', $target_locale);
  }

  public function setAutoDownload($active, $target_locale = NULL) {
    // Make sure only TRUE or FALSE is saved.
    $active = (bool) $active;
    $this->setAttribute('auto_download', $active, $target_locale);
  }

  public function isNodeBased() {
    $node_based = $this->getAttribute('lingotek_nodes_translation_method');
    if ($node_based == 'node') {
      return TRUE;
    }
    return FALSE;
  }

  public function setNodeBased($active) {
    if ($active) {
      $this->setAttribute('lingotek_nodes_translation_method', 'node');
    }
    else {
      $this->deleteAttribute('lingotek_nodes_translation_method');
    }
  }

  public function getWorkflow($target_locale = NULL) {
    return $this->getAttribute('workflow_id', $target_locale);
  }

  public function setWorkflow($workflow_id, $target_locale = NULL) {
    return $this->setAttribute('workflow_id', $workflow_id, $target_locale);
  }

  public function getProjectId() {
    return $this->getAttribute('project_id');
  }

  public function setProjectId($project_id) {
    $this->setAttribute('project_id', $project_id);
  }

  public function disableTargetLocale($target_locale) {
    $this->deleteTargetLocaleOverrides($target_locale);
    $this->setAttribute('disabled', TRUE, $target_locale);
  }

  public function isTargetLocaleDisabled($target_locale) {
    // don't check for disabled attributes in the parent profiles
    $this->setInherit(FALSE);
    $disabled = FALSE;
    if ($this->getAttribute('disabled', $target_locale)) {
      $disabled = TRUE;
    }
    $this->setInherit(TRUE);
    return $disabled;
  }

  public function isTargetLocaleCustom($target_locale) {
    return !$this->isTargetLocaleDisabled($target_locale) && $this->getTargetLocaleOverrides($target_locale);
  }

  public function toArray() {
    return array_merge(self::$global_profile, $this->profile);
  }

  protected function initTargetLocaleOverride($target_locale) {
    if (!isset($this->profile['target_language_overrides'][$target_locale])) {
      $this->profile['target_language_overrides'][$target_locale] = array();
    }
  }

  public function getAttribute($attrib_name, $target_locale = NULL) {
    if (!empty($target_locale) && isset($this->profile['target_language_overrides'][$target_locale][$attrib_name])) {
      return $this->profile['target_language_overrides'][$target_locale][$attrib_name];
    }
    elseif ($this->lookForInherited()) {
      if (!empty($this->profile[$attrib_name])) {
        return $this->profile[$attrib_name];
      }
      elseif (!empty(self::$global_profile[$attrib_name])) {
        return self::$global_profile[$attrib_name];
      }
    }
    return NULL;
  }

  public function setAttribute($attrib_name, $value, $target_locale = NULL) {

    if ($this->isProtectedAttribute($attrib_name)) {
      return;
    }

    $original_value = $this->getAttribute($attrib_name, $value);

    if ($target_locale) {
      // Set the language-specific attribute if different from the base attribute
      if ($value !== $original_value) {
        $this->initTargetLocaleOverride($target_locale);
        $this->profile['target_language_overrides'][$target_locale][$attrib_name] = $value;
      }
      else {
        $this->deleteAttribute($attrib_name, $target_locale);
        // Clean up any empty language overrides
        if (empty($this->profile['target_language_overrides'][$target_locale])) {
          unset($this->profile['target_language_overrides'][$target_locale]);
        }
      }
    }
    else {
      // Set the base attribute if different from the global attribute
      if (isset(self::$global_profile[$attrib_name])) {
        $original_value = self::$global_profile[$attrib_name];
      }
      if ($value !== $original_value) {
        $this->profile[$attrib_name] = $value;
      }
      else {
        $this->deleteAttribute($attrib_name);
      }
    }
    // Clean up target language attribute if empty
    if (empty($this->profile['target_language_overrides'])) {
      unset($this->profile['target_language_overrides']);
    }
    $this->save();
  }

  public function deleteAttribute($attrib_name, $target_locale = NULL) {
    if ($target_locale) {
      if (isset($this->profile['target_language_overrides'][$target_locale][$attrib_name])) {
        unset($this->profile['target_language_overrides'][$target_locale][$attrib_name]);
        $this->save();
      }
    }
    else {
      if (isset($this->profile[$attrib_name])) {
        unset($this->profile[$attrib_name]);
        $this->save();
      }
    }
  }

  public function deleteTargetLocaleOverrides($target_locale) {
    unset($this->profile['target_language_overrides'][$target_locale]);
    $this->save();
  }

  public function getTargetLocaleOverrides($target_locale) {
    if (!empty($this->profile['target_language_overrides'][$target_locale])) {
      return $this->profile['target_language_overrides'][$target_locale];
    }
    return array();
  }

  public function getAttributes($target_locale = NULL) {
    if ($this->getId() == LingotekSync::PROFILE_DISABLED) {
      return array(
        'name' => LingotekSync::PROFILE_DISABLED,
        'profile' => LingotekSync::PROFILE_DISABLED,
      );
    }
    if ($this->getId() == LingotekSync::PROFILE_ENABLED) {
      return array(
        'name' => LingotekSync::PROFILE_ENABLED,
        'profile' => LingotekSync::PROFILE_ENABLED,
      );
    }
    if (empty(self::$profiles[$this->getId()])) {
      drupal_set_message(t('Lingotek profile ID @profile_id not found.', array('@profile_id' => $this->getId())), 'error', FALSE);

      //throw new LingotekException('Profile ID "' . $this->getId() . '" not found.');
    }
    if ($target_locale) {
      $attributes = $this->getTargetLocaleOverrides($target_locale);
      if ($this->lookForInherited()) {
        $attributes = array_merge(self::$profiles[$this->getId()], $attributes);
      }
    }
    elseif (!empty(self::$profiles[$this->getId()])) {
      $attributes = self::$profiles[$this->getId()];
    }
    else {
      $attributes = array();
    }
    return array_merge(self::$global_profile, $attributes);
  }

  public function filterTargetLocales($available_locales) {
    $filtered_locales = array();
    $default_workflow = $this->getWorkflow();
    // foreach locale, get the overrides
    foreach ($available_locales as $locale) {
      if ($this->isTargetLocaleDisabled($locale)) {
        // filter this out.
      }
      elseif ($this->isTargetLocaleCustom($locale)) {
        $filtered_locales[$locale] = $this->getTargetLocaleOverrides($locale);
      }
      else {
        $filtered_locales[$locale] = TRUE;
      }
    }
    return $filtered_locales;
  }

  protected function isProtected() {
    $locked_profiles = array(
      LingotekSync::PROFILE_DISABLED,
      LingotekSync::PROFILE_AUTOMATIC,
      LingotekSync::PROFILE_MANUAL
    );
    if (in_array($this->getId(), $locked_profiles)) {
      return TRUE;
    }
    return FALSE;
  }

  protected function isProtectedAttribute($attrib_name) {
    $locked_attribs = array(
      0 => array('auto_upload', 'auto_download'),
      1 => array('auto_upload', 'auto_download'),
    );
    if (array_key_exists($this->getId(), $locked_attribs) && in_array($attrib_name, $locked_attribs[$this->getId()])) {
      return TRUE;
    }
    return FALSE;
  }

}
