<?php

/**
 * Services_Paymill_Subscription test case.
 */
class Services_Paymill_SubscriptionsTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var Services_Paymill_Subscriptions
     */
    private $_subscriptions;

    /**
     * @var Services_Paymill_Clients
     */
    private $_clients;

    /**
     * @var Services_Paymill_Offers
     */
    private $_offers;    

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_apiTestKey = API_TEST_KEY;
        $this->_apiUrl = API_HOST;
        $this->_subscriptions = new Services_Paymill_Subscriptions($this->_apiTestKey,  $this->_apiUrl);
        $this->_clients = new Services_Paymill_Clients($this->_apiTestKey,  $this->_apiUrl);
        $this->_offers = new Services_Paymill_Offers($this->_apiTestKey,  $this->_apiUrl);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_subscriptions = null;
        $this->_offers = null;
        $this->_clients = null;
    }

    /**
     * Tests Services_Paymill_Subscriptions->create()
     */
    public function testCreateWithToken()
    {   
        $params = array(
                'name' => 'Test subscription',
                'amount' => '333',
                'interval' => 'year',
                'trial_period_days' => '3',
                'currency' => 'eur',
        );
        $offer = $this->_offers->create($params);
        $item = $this->_subscriptions->create(array('offer' => $offer['id'], 'token' => TOKEN) );

        $this->assertArrayHasKey('id', $item);
        
        return $item['id'];
    }

    /**
     * Tests Services_Paymill_Subscriptions->get()
     * @depends testCreateWithToken
     */
    public function testGet()
    {
        $filters = array('count'=>10,'offset'=>0,);
        $items = $this->_subscriptions->get($filters);
        
        $this->assertInternalType('array', $items);
        $this->assertGreaterThanOrEqual(1, count($items));
        $this->assertArrayHasKey('id', $items[0]);
    }

    /**
     * Tests Services_Paymill_Subscriptions->getOne()
     * @depends testCreateWithToken
     */
    public function testGetOne($itemId)
    {
        $item = $this->_subscriptions->getOne($itemId);
        
        $this->assertNotNull($item);
        $this->assertArrayHasKey('id', $item);
        $this->assertEquals($itemId, $item['id']);

        return $item['id'];
    }

    /**
     * Tests Services_Paymill_Subscriptions->update()
     * @depends testGetOne
     */
    public function testUpdate($itemId)
    {
        $item = $this->_subscriptions->update(array('id' => $itemId,  'cancel_at_period_end' => true) );
        
        $this->assertInternalType('array', $item);
        $this->assertArrayHasKey('cancel_at_period_end', $item);
        $this->assertEquals(true, $item['cancel_at_period_end']);

        return $item['id'];
    }

    /**
     * Tests Services_Paymill_Subscriptions->delete()
     * @depends testUpdate
     */
    public function testDelete($itemId)
    {
        $response = $this->_subscriptions->delete($itemId);
        
        $this->assertEquals($itemId, $response['id']);
    }
}