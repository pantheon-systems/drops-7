<?php

/**
 * Services_Paymill_Plan test case.
 */
class Services_Paymill_OffersTest extends PHPUnit_Framework_TestCase
{
    /**
     *
     * @var Services_Paymill_Offers
     */
    private $_offers;

    /**
     * Prepares the environment before running a test.
     */
    protected function setUp()
    {
        $this->_offers = new Services_Paymill_Offers(API_TEST_KEY,  API_HOST);
    }

    /**
     * Cleans up the environment after running a test.
     */
    protected function tearDown()
    {
        $this->_offers = null;
    }

    /**
     * Tests Services_Paymill_Offers->create()
     */
    public function testCreate()
    {

        $name = 'TestOffers';
        $params = array(
                'name' => $name,
                'amount' => '1111',
                'interval' => 'year',
                'trial_period_days' => '3',
                'currency' => 'eur',
                );

        $item = $this->_offers->create($params);
        $this->assertArrayHasKey('name', $item);
        $this->assertEquals($name, $item['name']);

        $itemId =  $item['id'];
        return $itemId;
    }

    /**
     * Tests Services_Paymill_Offes->get()
     * @depends testCreate
     */
    public function testGet()
    {
        $filters = array('count'=>10,'offset'=>0,);
        $items = $this->_offers->get($filters);
        
        $this->assertInternalType('array', $items);
        $this->assertGreaterThanOrEqual(1, count($items));
        // @todo deactivated by API #434
        // $this->assertArrayHasKey('id', $items[0]);
    }

    /**
     * Tests Services_Paymill_Offers->getOne()
     * @depends testCreate
     */
    public function testGetOne($itemId)
    {
        $item = $this->_offers->getOne($itemId);
        
        $this->assertNotNull($item);
        $this->assertArrayHasKey('id', $item);
        $this->assertEquals($itemId, $item['id']);

        return $item['id'];
    }

    /**
     * Tests Services_Paymill_Offers->update()
     * @depends testGetOne
     */
    public function testUpdate($itemId)
    {
        $name = 'My Updated Test Offers';
        $item = $this->_offers->update(array('id' => $itemId,  'name' => $name) );
        
        $this->assertInternalType('array', $item);
        $this->assertArrayHasKey('name', $item);
        $this->assertEquals($name, $item['name']);

        return $item['id'];
    }

    /**
     * Tests Services_Paymill_Offers->delete()
     * @depends testUpdate
     */
    public function testDelete($offerId)
    {
        $offer = $this->_offers->delete($offerId);
        
        $this->assertInternalType('array', $offer);
        $this->assertCount(0,$offer);
    }
}
