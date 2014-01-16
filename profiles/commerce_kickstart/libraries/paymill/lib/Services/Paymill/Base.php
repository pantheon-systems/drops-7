<?php

require_once 'Apiclient/Curl.php';
/**
 * Paymill API wrapper super class
 */
abstract class Services_Paymill_Base
{
    /**
     * Resource relative url path name ending with '/', set or override in subclass
     *
     * @var string
     */
    protected $_serviceResource = null;

    /**
     * Paymill HTTP client class
     *
     * @var Services_Paymill_Apiclient_Interface
     */
    protected $_httpClient;

    /**
     * Base constructor
     * sets _httpClient property
     *
     * @param string $apiKey merchant api key
     * @param string $apiEndpoint endpoint URL for the api
     * @param array $extraData extra data for the client, e.g., proxy settings.
     */
    public function __construct($apiKey, $apiEndpoint, $extraData = array())
    {
        $this->_httpClient = new Services_Paymill_Apiclient_Curl($apiKey, $apiEndpoint, $extraData);
    }

    /**
     * General REST GET verb
     *
     * @param array  $filters    e.g. count,offest
     * @param string $identifier resource id
     *
     * @return array of resource items
     */
    public function get($filters = array(), $identifier = '')
    {
        $response = $this->_httpClient->request(
            $this->_serviceResource . $identifier,
            $filters,
            Services_Paymill_Apiclient_Interface::HTTP_GET
        );

        return $response['data'];
    }

    /**
     * General REST GET verb
     * returns one item or null
     *
     * @param string $identifier resource id
     *
     * @return array resource item | null
     */
    public function getOne($identifier = null)
    {
        if (!$identifier) {
            return null;
        }

        $filters = array("count" => 1, 'offset' => 0);

        return $this->get($filters, $identifier);
    }

    /**
     * General REST DELETE verb
     * Delete or inactivate/cancel resource item
     *
     * @param string $clientId
     *
     * @return array item deleted
     */
    public function delete($clientId = null)
    {
        $response =  $this->_httpClient->request(
            $this->_serviceResource . $clientId,
            array(),
            Services_Paymill_Apiclient_Interface::HTTP_DELETE
        );

        return $response['data'];
    }

    /**
     * General REST POST verb
     * create resource item
     *
     * @param array $itemData
     *
     * @return array created item
     */
    public function create($itemData = array())
    {
        $response = $this->_httpClient->request(
            $this->_serviceResource,
            $itemData,
            Services_Paymill_Apiclient_Interface::HTTP_POST
        );

        return $response['data'];
    }

    /**
     * General REST PUT verb
     * Update resource item
     *
     * @param array $itemData
     *
     * @return array item updated or null
     */
    public function update(array $itemData = array())
    {
        if (!isset($itemData['id']) ) {
            return null;
        }

        $itemId = $itemData['id'];
        unset ($itemData['id']);

        $response = $this->_httpClient->request(
            $this->_serviceResource . $itemId,
            $itemData,
            Services_Paymill_Apiclient_Interface::HTTP_PUT
        );

        return $response['data'];
    }

    /**
     * Returns the response of the last request as an array
     * @return mixed Response
     * @todo Add Unit test
     */
    public function getResponse(){
        return $this->_httpClient->getResponse();
    }
}