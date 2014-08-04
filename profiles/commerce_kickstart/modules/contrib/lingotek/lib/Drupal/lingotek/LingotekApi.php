<?php

/**
 * @file
 * Defines Drupal\lingotek\LingotekApi
 */
class LingotekApi {
  /**
   * The status string representing a successful API call.
   */

  const RESPONSE_STATUS_SUCCESS = 'success';

  /**
   * The faux Lingotek user ID to use for anonymous user operations.
   */
  const ANONYMOUS_LINGOTEK_ID = 'anonymous';

  /**
   * The endpoint for API version 4
   */
  const API_ENDPOINT_V4 = '/lingopoint/api/4';

  /**
   * Holds the static instance of the singleton object.
   *
   * @var LingotekApi
   */
  private static $instance;

  /**
   * Debug status for extra logging of API calls.
   *
   * @var bool
   */
  private $debug;

  /**
   * The endpoint for API calls.
   *
   * @var string
   */
  private $api_url;

  /**
   * Gets the singleton instance of the API class.
   *
   * @return LingotekApi
   *   An instantiated LingotekApi object.
   */
  public static function instance() {
    if (!isset(self::$instance)) {
      $class_name = __CLASS__;
      self::$instance = new $class_name();
    }

    return self::$instance;
  }

  /**
   * Add a document to the Lingotek platform.
   *
   * Uploads the translatable object's content in the selected language.
   *
   * @param object $translatable_object
   *   A Drupal node object or lingotek ConfigChunk object
   */
  public function addContentDocument(LingotekTranslatableEntity $translatable_object, $with_targets = FALSE) {
    $success = FALSE;

    $project_id = $translatable_object->getProjectId();

    $source_lingotek_locale = $translatable_object->getSourceLocale();
    $source_language = isset($source_lingotek_locale) && !empty($source_lingotek_locale) ? $source_lingotek_locale : Lingotek::convertDrupal2Lingotek(lingotek_get_source_language());

    if ($project_id) {
      $parameters = array(
        'projectId' => $project_id,
        'format' => $this->xmlFormat(),
        'sourceLanguage' => $source_language,
        'tmVaultId' => $translatable_object->getVaultId(),
      );
      $parameters['documentName'] = $translatable_object->getDocumentName();
      $parameters['documentDesc'] = $translatable_object->getDescription();
      $parameters['content'] = $translatable_object->documentLingotekXML();
      $parameters['url'] = $translatable_object->getUrl();
      $parameters['workflowId'] = $translatable_object->getWorkflowId();

      $this->addAdvancedParameters($parameters, $translatable_object);

      if ($with_targets) {
        $parameters['targetAsJSON'] = Lingotek::getLanguagesWithoutSourceAsJSON($source_language);

        $parameters['applyWorkflow'] = 'true'; // API expects a 'true' string
        $result = $this->request('addContentDocumentWithTargetsAsync', $parameters);
      }
      else {
        $result = $this->request('addContentDocumentAsync', $parameters);
      }

      if ($result) {
        if (isset($result->errors) && $result->errors) {
          LingotekLog::error(t('Request to send document to Lingotek failed: ') . print_r($result->errors, TRUE), array());
          $translatable_object->setStatus(LingotekSync::STATUS_FAILED);
          $translatable_object->setLastError(is_array($result->errors) ? array_shift($result->errors) : $result->errors);
          return FALSE;
        }
        if (get_class($translatable_object) == 'LingotekConfigChunk') {
          $translatable_object->setDocumentId($result->id);
          $translatable_object->setProjectId($project_id);
          $translatable_object->setStatus(LingotekSync::STATUS_CURRENT);
          $translatable_object->setTargetsStatus(LingotekSync::STATUS_PENDING);

          // WTD: there is a race condition here where a user could modify a locales-
          // source entry between the time the dirty segments are pulled and the time
          // they are set to current at this point.  This same race condition exists
          // for nodes as well; however, the odds may be lower due to number of entries.
          LingotekConfigChunk::setSegmentStatusToCurrentById($translatable_object->getId());
        }
        else {
          // node assumed (based on two functions below...
          $entity_type = $translatable_object->getEntityType();
          lingotek_keystore($entity_type, $translatable_object->getId(), 'document_id', $result->id);
          lingotek_keystore($entity_type, $translatable_object->getId(), 'last_uploaded', time());
        }
          
        $success = TRUE;
      }
    }
    return $success;
  }
  
  /**
   * Updates the content of an existing Lingotek document with the current object contents.
   *
   * @param stdClass $translatable_object
   *   A Drupal node object or another object, such as a config chunk, etc.
   *
   * @return bool
   *   TRUE on success, FALSE on failure.
   */
  public function updateContentDocument($translatable_object) {

    $parameters['documentId'] = $translatable_object->getMetadataValue('document_id');
    $parameters['documentName'] = $translatable_object->getDocumentName();
    $parameters['documentDesc'] = $translatable_object->getDescription();
    $parameters['content'] = $translatable_object->documentLingotekXML();
    $parameters['url'] = $translatable_object->getUrl();
    $parameters['format'] = $this->xmlFormat();

    $this->addAdvancedParameters($parameters, $translatable_object);

    $result = $this->request('updateContentDocumentAsync', $parameters);

    if ($result) {
      if (get_class($translatable_object) == 'LingotekConfigChunk') {
        $translatable_object->setStatus(LingotekSync::STATUS_CURRENT);
        $translatable_object->setTargetsStatus(LingotekSync::STATUS_PENDING);

        // WTD: there is a race condition here where a user could modify a locales-
        // source entry between the time the dirty segments are pulled and the time
        // they are set to current at this point.  This same race condition exists
        // for nodes as well; however, the odds may be lower due to number of entries.
        LingotekConfigChunk::setSegmentStatusToCurrentById($translatable_object->getId());
      }
    }

    return ( $result ) ? TRUE : FALSE;
  }

  public function removeDocument($document_id) {
    $success = FALSE;
    if ($document_id) {
      $result = $this->request('removeDocument', array('documentId' => $document_id));
      if ($result) {
        $success = TRUE;
      }
    }
    return $success;
  }

  /**
   * Adds a target language to an existing Lingotek Document or Project.
   *
   * @param int $lingotek_document_id
   *   The document to which the new translation target should be added.  Or null if the target will be added to the project.
   * @param int $lingotek_project_id
   *   The project to which the new translation target should be added.  Or null if the target will be added to a document instead.
   * @param string $lingotek_locale
   *   The two letter code representing the language which should be added as a translation target.
   * @param string $workflow_id
   *   The optional workflow to associate with this target. If omitted, the project's default
   *   workflow will be applied.
   *
   * @return mixed
   *  The ID of the new translation target in the Lingotek system, or FALSE on error.
   */
  public function addTranslationTarget($lingotek_document_id, $lingotek_project_id, $lingotek_locale, $workflow_id = '') {
    global $_lingotek_client, $_lingotek_locale;

    $parameters = array(
      'applyWorkflow' => 'true', // Ensure that as translation targets are added, the associated project's Workflow template is applied.
      'targetLanguage' => $lingotek_locale
    );

    if (isset($lingotek_document_id) && !isset($lingotek_project_id)) {
      $parameters['documentId'] = $lingotek_document_id;
    }
    elseif (isset($lingotek_project_id) && !isset($lingotek_document_id)) {
      $parameters['projectId'] = $lingotek_project_id;
    }

    if ($workflow_id) {
      $parameters['workflowId'] = $workflow_id;
    }

    if ($new_translation_target = $this->request('addTranslationTarget', $parameters)) {
      // If the request went through, there was no OAuth error and we should enable.
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Removes a target language to an existing Lingotek Document or Project.
   *
   * @param int $lingotek_document_id
   *   The document to which the new translation target should be added.  Or null if the target will be added to the project.
   * @param int $lingotek_project_id
   *   The project to which the new translation target should be added.  Or null if the target will be added to a document instead.
   * @param string $lingotek_locale
   *   The two letter code representing the language which should be added as a translation target.
   * @param string $workflow_id
   *   The optional workflow to associate with this target. If omitted, the project's default
   *   workflow will be applied.
   *
   * @return bool
   *  TRUE on success, or FALSE on error.
   */
  public function removeTranslationTarget($lingotek_document_id, $lingotek_project_id, $lingotek_locale, $workflow_id = '') {

    $parameters = array(
      'applyWorkflow' => 'true', // Ensure that as translation targets are added, the associated project's Workflow template is applied.
      'targetLanguage' => $lingotek_locale
    );

    if (isset($lingotek_document_id) && !isset($lingotek_project_id)) {
      $parameters['documentId'] = $lingotek_document_id;
    }
    elseif (isset($lingotek_project_id) && !isset($lingotek_document_id)) {
      $parameters['projectId'] = $lingotek_project_id;
    }

    if ($workflow_id) {
      $parameters['workflowId'] = $workflow_id;
    }

    if ($old_translation_target = $this->request('removeTranslationTarget', $parameters)) {
      return ( $old_translation_target->results == 'success' ) ? TRUE : FALSE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Gets the phase data for the active phase of the specified translation target.
   *
   * @param int $translation_target_id
   *   The ID of a translation target on the Lingotek system.
   *
   * @return mixed
   *   An object representing data for the current translation phase, or FALSE on error.
   */
  public function currentPhase($translation_target_id) {
    if ($target = $this->getTranslationTarget($translation_target_id)) {
      if (!empty($target->phases)) {
        $current_phase = FALSE;
        foreach ($target->phases as $phase) {

          if (!$phase->isMarkedComplete) {
            $current_phase = $phase;
            break;
          }
        }

        // Return either the first uncompleted phase, or the last phase if all phases are complete.
        return ($current_phase) ? $current_phase : end($target->phases);
      }
      else {
        return FALSE;
      }
    }
    else {
      return FALSE;
    }
  }

  /**
   * Downloads the translated document for the specified document and language.
   *
   * @param int $document_id
   *   The Lingotek document ID that should be downloaded.
   * @param string $lingotek_locale
   *   A Lingotek language/locale code.
   *
   * @return mixed
   *   On success, a SimpleXMLElement object representing the translated document. FALSE on failure.
   *
   */
  public function downloadDocument($document_id, $lingotek_locale) {
    $document = FALSE;

    $params = array(
      'documentId' => $document_id,
      'targetLanguage' => $lingotek_locale,
    );

    $results = $this->request('downloadDocument', $params);

    if ($results) {
      try {
        // TODO: This is borrowed from the now-deprecated LingotekSession::download()
        // and could use refactoring.
        $tmpFile = tempnam(file_directory_temp(), 'lingotek');
        $fp = fopen($tmpFile, 'w');
        fwrite($fp, $results);
        fclose($fp);

        $text = '';
        $file = FALSE;

        // downloadDocument returns zip-encoded data.
        $zip = new ZipArchive;
        $zip->open($tmpFile);
        $name = $zip->getNameIndex(0);
        $file = $zip->getStream($name);

        if ($file) {
          while (!feof($file)) {
            $text .= fread($file, 2);
          }

          fclose($file);
        }

        unlink($tmpFile);

        $document = new SimpleXMLElement($text);
      } catch (Exception $e) {
        LingotekLog::error('Unable to parse downloaded document. Error: @error. Text: !xml.', array('!xml' => $text, '@error' => $e->getMessage()));
      }
    }

    return $document;
  }

  /**
   * Gets Lingotek Document data for the specified document.
   *
   * @param int $document_id
   *   The ID of the Lingotek Document to retrieve.
   *
   * @return mixed
   *  The API response object with Lingotek Document data, or FALSE on error.
   */
  public function getDocument($document_id) {
    $documents = &drupal_static(__FUNCTION__);

    if (!empty($documents[$document_id])) {
      $document = $documents[$document_id];
    }
    else {
      $params = array('documentId' => $document_id);

      if ($document = $this->request('getDocument', $params)) {
        $documents[$document_id] = $document;
      }
    }

    return $document;
  }

  /**
   * Gets the workflow progress of the specified document.
   *
   * @param int $document_id
   *   The ID of the Lingotek Document to retrieve.
   *
   * @return mixed
   *  The API response object with Lingotek Document data, or FALSE on error.
   */
  public function getDocumentProgress($document_id) {
    $documents = &drupal_static(__FUNCTION__);

    if (!empty($documents[$document_id])) {
      $document = $documents[$document_id];
    }
    else {
      $params = array('documentId' => $document_id);

      if ($document = $this->request('getDocumentProgress', $params)) {
        $documents[$document_id] = $document;
      }
    }

    return $document;
  }

  /**  
   * Gets the workflow progress of the specified documents.
   *
   * @param int $document_id
   *   The IDs of the Lingotek Documents to retrieve.
   *
   * @return mixed
   *  The API response object with Lingotek Document data, or FALSE on error.
   */
  public function listDocumentProgress($document_ids) {
    $params = array();
    foreach ($document_ids as $document_id) {
      $params['documentId'][] = $document_id;
    }
    $documents = $this->request('listDocumentProgress', $params);
    return $documents;
  }

  /**
   * Gets the workflow progress of the specified project (or list of document ids).
   *
   * @param int $project_id
   * 
   * @param array<int> $document_ids
   *   An array of document IDs of the Lingotek Document to retrieve.
   *
   * @return mixed
   *  The API response object with Lingotek Document data, or FALSE on error.
   */
  public function getProgressReport($project_id = NULL, $document_ids = NULL) {
    $params = array();
    if (is_array($document_ids)) {
      $params['documentId'] = $document_ids;
    }
    elseif (!is_null($project_id)) {
      $params['projectId'] = $project_id;
    }
    $report = $this->request('getProgressReport', $params);
    return $report;
  }

  /**
   * Gets data for a specific Workflow Phase.
   *
   * @param int $phase_id
   *   The ID of the phase to retrieve.
   *
   * @return mixed
   *   The API response object, or FALSE on error.
   */
  public function getPhase($phase_id) {
    $params = array(
      'phaseId' => $phase_id
    );

    return $this->request('getPhase', $params);
  }

  /**
   * Gets a translation target.
   *
   * This fetches an target language object for a specific document.
   *
   * @param int $translation_target_id
   *   ID for the target language object.
   * @return
   *   Object representing a target language for a specific document in the Lingotek platform, or FALSE on error.
   */
  public function getTranslationTarget($translation_target_id) {
    $targets = &drupal_static(__FUNCTION__);

    $params = array(
      'translationTargetId' => $translation_target_id
    );

    if (isset($targets[$translation_target_id])) {
      return $targets[$translation_target_id];
    }
    elseif ($output = $this->request('getTranslationTarget', $params)) {
      $targets[$translation_target_id] = $output;
      return $output;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Get a User Profile Attributes
   *
   * Note:  the Request() method will switch the ExternalID to whatever is passed in, instead of the regular ExternalID.
   */
  public function getProfileAttributes($externalId = NULL) {
    $result = FALSE;

    $parameters = array();
    if (isset($externalId)) {
      $parameters['externalId'] = $externalId;
    }

    if ($output = $this->request('getProfileAttributes', $parameters)) {
      if ($output->results == 'success' && is_object($output->attributes)) {
        $result = $output->attributes;
      }
    }

    /*
      stdClass::__set_state(array(
      'results' => 'success',
      'attributes' =>
      stdClass::__set_state(array(
      'id' => 26,
      'name' => 'Community Admin',
      'login_id' => 'community_admin@S8NFUBG8',
      'on_leaderboard' => false,
      'language_skills' =>
      array (
      ),
      )),
      ))
     */

    return $result;
  }

  /**
   * Uses getProfileAttributes to Get the User Profile Attributes, and return the ID.
   */
  public function getProfileId($externalId = NULL) {
    $result = FALSE;
    $profile = $this->getProfileAttributes($externalId);
    if ($profile) {
      $result = $profile->id;
    }
    return $result;
  }

  /**
   * Assigns a role to a user.  (Must be done by an community admin)
   * Returns TRUE or FALSE
   */
  public function addRoleAssignment($role, $userId) {
    $result = FALSE;
    if (isset($role) && isset($userId)) {
      $parameters = array(
        'role' => $role,
        'clientId' => $userId
      );
      if ($output = $this->request('addRoleAssignment', $parameters)) {
        if ($output->results == 'success') {
          $result = TRUE;
        }
      }
    }
    return $result;
  }

  /**
   * Assigns a user to a project.  (Must be done by an community admin)
   * Returns TRUE or FALSE
   * This will only return TRUE the first time.
   */
  public function assignProjectManager($projectId, $userId) {
    $result = FALSE;
    if (isset($projectId) && isset($userId)) {
      $parameters = array(
        'projectId' => $projectId,
        'managerUserId' => $userId
      );
      $output = $this->request('assignProjectManager', $parameters);
      if ($output) {
        if ($output->results == 'success') {
          $result = TRUE;
        }
      }
    }
    return $result;
  }

  /**
   * Configures the Lingotek community to allow the local Drupal user to use the Workbench.
   *
   * @param str $username
   *   A Drupal username.
   */
  private function checkUserWorkbenchLinkPermissions($username = self::ANONYMOUS_LINGOTEK_ID) {

    // Don't do anything with the community_admin user.
    if ($username == 'community_admin') {
      return true;
    }

    // To use the workbench, users need to be tagged.  Check to see if the current user has already been tagged.
    $workbench_list = variable_get('lingotek_workbench_tagged_users', array());
    $found = array_search($username, $workbench_list);

    // If not, update his account to allow  him to use the workbench.
    if ($found === false) {
      $profileId = $this->getProfileId($username);
      if ($profileId) {
        $projectId = variable_get('lingotek_project', NULL);
        $role = $this->addRoleAssignment("project_manager", $profileId);
        if ($role && isset($projectId)) {
          $manager = $this->assignProjectManager($projectId, $profileId); // This call will only return true the FIRST time.
          $workbench_list[] = $username;
          variable_set('lingotek_workbench_tagged_users', $workbench_list);
        }
      }
    }
  }

  /**
   * Gets a workbench URL for the specified document ID and phase.
   *
   * @param int $document_id
   *   A Lingotek Document ID.
   * @param int $phase_id
   *   A Lingotek workflow phase ID.
   *
   * @return mixed
   *   A workbench URL string on success, or FALSE on failure.
   */
  public function getWorkbenchLink($document_id, $phase_id) {
    $links = &drupal_static(__FUNCTION__);

    global $user;
    $externalId = isset($user->name) ? $user->name : ''; // send a blank string for anonymous users (community translation)
    self::checkUserWorkbenchLinkPermissions($externalId);

    $static_id = $document_id . '-' . $phase_id;
    if (empty($links[$static_id])) {
      $params = array(
        'documentId' => $document_id,
        'phaseId' => $phase_id,
        'externalId' => $externalId
      );

      $output = $this->request('getWorkbenchLink', $params);
      if ($output && isset($output->url)) {
        $links[$static_id] = $url = $output->url;
      }
      else {
        $url = FALSE;
      }
    }
    else {
      $url = $links[$static_id];
    }
    return $url;
  }

  /**
   * Gets available Lingotek projects.
   * 
   * @param $reset
   *   A boolean value to determin whether we need to query the API
   * 
   * @return array
   *   An array of available projects with project IDs as keys, project labels as values.
   */
  public function listProjects($reset = FALSE) {
    $projects = variable_get('lingotek_project_defaults', array());

    if (!empty($projects) && $reset == FALSE) {
      return $projects;
    }
    
    if ($projects_raw = $this->request('listProjects')) {
      $projects = array();
      foreach ($projects_raw->projects as $project) {
        $projects[$project->id] = $project->name;
      }
      variable_set('lingotek_project_defaults', $projects);
    }

    return $projects;
  }

  /**
   * Gets available Lingotek Workflows.
   * 
   * @param $reset
   *   A boolean value to determine whether we need to query the API
   * @param $include_public
   *   A boolean value to determine whether to show public workflows
   * 
   * @return array
   *   An array of available Workflows with workflow IDs as keys, workflow labels as values.
   */
  public function listWorkflows($reset = FALSE, $include_public = FALSE) {
    $workflows = variable_get('lingotek_workflow_defaults', array());
    if (!empty($workflows) && $reset == FALSE) {
      return $workflows;
    }

    if ($workflows_raw = $this->request('listWorkflows')) {
      $workflows = array();
      foreach ($workflows_raw->workflows as $workflow) {
        if ($include_public || (!$workflow->is_public 
            && $workflow->owner != LINGOTEK_DEFAULT_WORKFLOW_TEMPLATE))
        $workflows[$workflow->id] = $workflow->name;
      }
      variable_set('lingotek_workflow_defaults', $workflows);
    }

    return $workflows;
  }

  /**
   * Gets available Lingotek Translation Memory Vaults.
   * 
   * @param $reset
   *   A boolean value to determin whether we need to query the API
   * 
   * @return array
   *   An array of available vaults.
   */
  public function listVaults($reset = FALSE, $show_public_vaults = FALSE) {
    $vaults = variable_get('lingotek_vaults_defaults', array());

    if (!empty($vaults) && $reset == FALSE) {
      return $vaults;
    }

    if ($vaults_raw = $this->request('listTMVaults')) {
      $vaults = array();
      if (!empty($vaults_raw->personalVaults)) {
        foreach ($vaults_raw->personalVaults as $vault) {
          $vaults['Personal Vaults'][$vault->id] = $vault->name;
        }
      }

      if (!empty($vaults_raw->communityVaults)) {
        foreach ($vaults_raw->communityVaults as $vault) {
          $vaults['Community Vaults'][$vault->id] = $vault->name;
        }
      }

      if ($show_public_vaults && !empty($vaults_raw->publicVaults)) {
        foreach ($vaults_raw->publicVaults as $vault) {
          $vaults['Public Vaults'][$vault->id] = $vault->name;
        }
      }
      variable_set('lingotek_vaults_defaults', $vaults);
    }

    return $vaults;
  }

  /**
   * Updates one or more nids to belong to a given workflow
   * 
   * @param array $document_ids
   *   An array of document IDs
   * @param string $workflow_id
   *   A string containing the desired workflow_id
   * @param string $prefillPhase
   *   An optional parameter specifying the prefill phase
   * 
   * @return bool
   *   TRUE on success, FALSE on failure.
   */
  public function changeWorkflow($document_ids, $workflow_id, $prefillPhase=NULL) {
    $parameters = array(
      'documentId' => $document_ids,
      'workflowId' => $workflow_id,
      'preserveTargets' => 'true',
    );
    if ($prefillPhase) {
      $parameters['prefillPhase'] = $prefillPhase;
    }

    return ($this->request('resetDocument', $parameters) ? TRUE : FALSE);
  }

  /**
   * Marks a phase as complete.
   *
   * @param int $phase_id
   *   The phase ID to be marked as complete.
   *
   * @return bool
   *   TRUE on success, FALSE on failure.
   */
  public function markPhaseComplete($phase_id) {
    $parameters = array(
      'phaseId' => $phase_id,
    );

    return ($this->request('markPhaseComplete', $parameters)) ? TRUE : FALSE;
  }

  /**
   * Gets the appropriate format code for the current system state.
   *
   * @return string
   *   A XML format code.
   */
  public function xmlFormat() {
    return (variable_get('lingotek_advanced_parsing', TRUE)) ? 'XML_OKAPI' : 'XML';
  }

  /**
   * Tests the current configuration to ensure that API calls can be made.
   *
   * @return bool
   *   TRUE if the configuration is correct, FALSE otherwise.
   */
  public function testAuthentication($force = FALSE) {
    $valid_connection = &drupal_static(__FUNCTION__);

    if ($force || !isset($valid_connection)) {
      // Only test the connection if the oauth keys have been setup.
      $consumer_key = variable_get('lingotek_oauth_consumer_id', '');
      $consumer_secret = variable_get('lingotek_oauth_consumer_secret', '');
      if (!empty($consumer_key) && !empty($consumer_secret)) {
        $valid_connection = ($this->request('validateApiKeys')) ? TRUE : FALSE;
      }
      else {
        $valid_connection = FALSE;
      }
    }

    return $valid_connection;
  }

  /**
   * Calls a Lingotek API method.
   *
   * @return mixed
   *   On success, a stdClass object of the returned response data, FALSE on error.
   */
  public function request($method, $parameters = array(), $request_method = 'POST', $credentials = NULL) {
    global $user;
    LingotekLog::trace('<h2>@method</h2> (trace)', array('@method' => $method));
    $response_data = FALSE;
    // Every v4 API request needs to have the externalID parameter present.
    // Defaults the externalId to the lingotek_login_id, unless externalId is passed as a parameter
    if (!isset($parameters['externalId'])) {
      $parameters['externalId'] = variable_get('lingotek_login_id', '');
    }
    module_load_include('php', 'lingotek', 'lib/oauth-php/library/OAuthStore');
    module_load_include('php', 'lingotek', 'lib/oauth-php/library/LingotekOAuthRequester');

    $credentials = is_null($credentials) ? array(
      'consumer_key' => variable_get('lingotek_oauth_consumer_id', ''),
      'consumer_secret' => variable_get('lingotek_oauth_consumer_secret', '')
        ) : $credentials;

    $timer_name = $method . '-' . microtime(TRUE);
    timer_start($timer_name);

    $result = NULL;
    $response = NULL;
    $api_url = $this->api_url . '/' . $method;
    try {
      OAuthStore::instance('2Leg', $credentials);
      $request = @new LingotekOAuthRequester($api_url, $request_method, $parameters);
      // There is an error right here.  The new LingotekOAuthRequester throws it, because it barfs on $parameters
      // The error:  Warning: rawurlencode() expects parameter 1 to be string, array given in LingotekOAuthRequest->urlencode() (line 619 of .../modules/lingotek/lib/oauth-php/library/LingotekOAuthRequest.php).
      // The thing is, if you encode the params, they just get translated back to an array by the object.  They have some type of error internal to the object code that is handling things wrong.
      // I couldn't find a way to get around this without changing the library.  For now, I am just supressing the warning w/ and @ sign.
      $result = $request->doRequest(0, array(CURLOPT_SSL_VERIFYPEER => FALSE));
      $response = json_decode($result['body']);
    } catch (OAuthException2 $e) {
      LingotekLog::error('Failed OAuth request. <br />Method: @method <br />Message: @message 
      <br />API URL: @url
      <br />Parameters: !params.
      <br />Response: !response', array(
        '@method' => $method,
        '@message' => $e->getMessage(),
        '@url' => $api_url,
        '!params' => ($parameters),
        '!response' => ($response)), 'api');
    }

    $timer_results = timer_stop($timer_name);
    
    // cleanup parameters so that the logs aren't too long
    if(isset($parameters['fprmFileContents'])) {
      $parameters['fprmFileContents'] = 'removed for brevity';
    }
    if(isset($parameters['secondaryFprmFileContents'])) {
      $parameters['secondaryFprmFileContents'] = 'removed for brevity';
    }
    $message_params = array(
      '@url' => $api_url,
      '@method' => $method,
      '!params' => $parameters,
      '!request' => $request,
      '!response' => ($method == 'downloadDocument' && !isset($response->results)) ? "Zipped document" : $response,
      '@response_time' => number_format($timer_results['time']) . ' ms',
    );

    /*
      Exceptions:
      downloadDocument - Returns misc data (no $response->results), and should always be sent back.
      assignProjectManager - Returns fails/falses if the person is already a community manager (which should be ignored)
     */
    if ($method == 'downloadDocument') { // Exception downloadDocument
      LingotekLog::api('<h1>@method</h1> <strong>API URL:</strong> @url
        <br /><strong>Response Time:</strong> @response_time<br /><strong>Request Params</strong>: !params<br /><strong>Response:</strong> !response<br/><strong>Full Request:</strong> !request', $message_params);
      $response_data = !empty($result) ? $result['body'] : "";
    }
    elseif ((!is_null($response) && $response->results == self::RESPONSE_STATUS_SUCCESS) || $method == 'assignProjectManager') { // SUCCESS
      LingotekLog::api('<h1>@method</h1> <strong>API URL:</strong> @url
        <br /><strong>Response Time:</strong> @response_time<br /><strong>Request Params</strong>: !params<br /><strong>Response:</strong> !response<br/><strong>Full Request:</strong> !request', $message_params);
      $response_data = $response;
    }
    else { // ERROR
      LingotekLog::error('<h1>@method (Failed)</h1> <strong>API URL:</strong> @url
        <br /><strong>Response Time:</strong> @response_time<br /><strong>Request Params</strong>: !params<br /><strong>Response:</strong> !response<br/><strong>Full Request:</strong> !request', $message_params, 'api');
      $response_data = $response;
    }
    return $response_data;
  }

  /**
   * Calls a Lingotek API to provision a new Community (account).
   * Modified version of the request() method.
   *
   * @return mixed
   *   On success, a stdClass object of the returned response data, FALSE on error.
   */
  public function createCommunity($parameters = array(), $callback_url = NULL) {
    $credentials = array('consumer_key' => LINGOTEK_AP_OAUTH_KEY, 'consumer_secret' => LINGOTEK_AP_OAUTH_SECRET);
    if (isset($callback_url)) {
      $parameters['projectName'] = lingotek_get_site_name(); 
      $parameters['callbackUrl'] = $callback_url;
    }
    $response = $this->request('autoProvisionCommunity', $parameters, 'POST', $credentials);
    return $response;
  }

  /**
   * Adds advanced parameters for use with addContentDocument and updateContentDocument.
   *
   * @param array $parameters
   *   An array of API request parameters.
   * @param object $entity
   *   A Drupal entity object.
   */
  private function addAdvancedParameters(&$parameters, LingotekTranslatableEntity $entity) {
    // Extra parameters when using advanced XML configuration.
    $advanced_parsing_enabled = variable_get('lingotek_advanced_parsing', FALSE);
    $use_advanced_parsing = ($advanced_parsing_enabled ||
        (!$advanced_parsing_enabled && lingotek_keystore($entity->getEntityType(), $entity->getId(), 'use_advanced_parsing')));

    if ($use_advanced_parsing) {

      $fprmFileContents = variable_get('lingotek_advanced_xml_config1', '');
      $secondaryFprmFileContents = variable_get('lingotek_advanced_xml_config2', '');

      if (!strlen($fprmFileContents) || !strlen($secondaryFprmFileContents)) {
        lingotek_set_default_advanced_xml();
        $fprmFileContents = variable_get('lingotek_advanced_xml_config1', '');
        $secondaryFprmFileContents = variable_get('lingotek_advanced_xml_config2', '');
      }

      $advanced_parameters = array(
        'fprmFileContents' => $fprmFileContents,
        'secondaryFprmFileContents' => $secondaryFprmFileContents,
        'secondaryFilter' => 'okf_html',
      );

      $parameters = array_merge($parameters, $advanced_parameters);
    }
  }

  /**
   * Private constructor.
   */
  private function __construct() {
    $this->debug = variable_get('lingotek_api_debug', FALSE);
    $host = LINGOTEK_API_SERVER;
    // Trim trailing slash from user-entered server name, if it exists.
    if (substr($host, -1) == '/') {
      $host = substr($host, 0, -1);
    }
    $this->api_url = $host . self::API_ENDPOINT_V4;
  }

  /**
   * Private clone implementation.
   */
  private function __clone() {
    
  }

}
