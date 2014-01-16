<?php

/**
 * @author Vladislav Shub <vlad@yotpo.com>
 */

namespace Yotpo\Test;

require(dirname(dirname(dirname(__FILE__))) . '/bootstrap.php');
\Yotpo\Bootstrap::init();

class YotpoTest extends \PHPUnit_Framework_TestCase {

    const TEST_APP_KEY = 'c2cThXB8foo9O63Xj4hx2L4SJFiioCJPsIOP83dr';
    const TEST_SECRET = 'DgGS4b7dBEAUbx5hYE29S0TKIpypDGpShVMjYFlz';

    private $utoken = null;
    private $yotpo = null;
    public function setUp(){
        $this->yotpo = new \Yotpo\Yotpo(self::TEST_APP_KEY, self::TEST_SECRET);
        $this->utoken = $this->yotpo->get_oauth_token()->access_token;
    }
    
    public function testInit() {
        $this->assertEquals('Yotpo\Yotpo', get_class($this->yotpo));
    }

    public function test_get_oauth_token() {
        $yotpo = new \Yotpo\Yotpo(self::TEST_APP_KEY, self::TEST_SECRET);
        $credentials = $yotpo->get_oauth_token();
        $this->assertObjectHasAttribute('access_token', $credentials);
        $this->assertObjectHasAttribute('token_type', $credentials);
    }

    public function test_create_user() {
        $user = $this->yotpo->create_user(array('email' => 'moshe1@ynet.co.il',
            'display_name' => 'Moshe The PHP Tester',
            'first_name' => 'Moshe',
            'last_name' => 'PHP Tester',
            'website_name' => 'http://www.ynet1.co.il',
            'password' => 'vladopen',
            'support_url' => 'http://www.ynet1.co.il/support',
            'callback_url'=> 'http://www.ynet1.co.il/callback',
            'url' => 'http://www.ynet1.co.il/url'));
        $this->assertObjectHasAttribute('code', $user->status);
        $this->assertEquals(200, $user->status->code, 'Code was not 200');
    }

    public function test_create_account_platform() {
        $account_platform_hash = array(
            'utoken' => $this->utoken,
            'shop_token' => $this->utoken,
            'shop_domain' => 'http://www.ynet.co.il',
            'plan_name' => 'free',
            'platform_type_id' => 2
        );
        $account_platform = $this->yotpo->create_account_platform($account_platform_hash);
        $this->assertObjectHasAttribute('code', $account_platform->status);
        $this->assertEquals(200, $account_platform->status->code, 'Code was not 200');
    }

    public function test_get_login_url() {
        $login_url = $this->yotpo->get_login_url();
        $this->assertObjectHasAttribute('code', $login_url->status);
        $this->assertEquals(200, $login_url->status->code, 'Code was not 200');
        $this->assertNotEmpty($login_url->response->signin_url);
    }

    public function test_check_subdomain() {
        $subdomain_hash = array(
            'subdomain' => 'wwwgooglecom',
            'utoken' => $this->utoken
        );
        $response = $this->yotpo->check_subdomain($subdomain_hash);
        $this->assertObjectHasAttribute('code', $response->status);
        $this->assertEquals(200, $response->status->code, 'Code was not 200');
    }

    public function test_update_account() {
        $account_hash = array(
            'utoken' => $this->utoken,
            'minisite_website_name' => 'Moshe!',
            'minisite_website' => 'http://www.ynet.co.il',
            'minisite_subdomain' => 'wwwgooglecom',
            'minisite_cname' => 'vlad.ynet.co.il',
            'minisite_subdomain_active' => true
        );
        $response = $this->yotpo->update_account($account_hash);
        $this->assertObjectHasAttribute('code', $response->status);
        $this->assertEquals(200, $response->status->code, 'Code was not 200');
    }

    public function test_create_purchase() {
        $purchase_hash = array(
            'utoken' => $this->utoken,
            'email' => 'customer@the.com',
            'customer_name' => 'The Customer',
            'order_date' => '2013-03-02',
            'currency_iso' => 'USD',
            'order_id' => '1233123',
            'platform' => 'general',
            'products' => array(
                'p1' => array(
                  'url' => 'http://example_product_url1.com',
                  'name' => 'product1',
                  'image' => 'http://example_product_image_url1.com',
                  'description' => 'this is the description of a product'
              )
            )
        );
        $response = $this->yotpo->create_purchase($purchase_hash);
        $this->assertObjectHasAttribute('code', $response->status);
        $this->assertEquals(200, $response->status->code, 'Code was not 200');
    }

    public function test_create_purchases() {
        $purchases_hash = array(
            'utoken' => $this->utoken,
            'platform' => 'general',
            'orders' => array(
                'email' => 'customer@the.com',
                'customer_name' => 'The Customer',
                'order_date' => '2013-04-04',
                'currency_iso' => 'USD',
                'order_id' => '1233123',
                'products' => array(
                    'p1' => array(
                      'url' => 'http://example_product_url1.com',
                      'name' => 'product1',
                      'image' => 'http://example_product_image_url1.com',
                      'description' => 'this is the description of a product'
                  )
                )
            )
        );
        $response = $this->yotpo->create_purchases($purchases_hash);
        $this->assertObjectHasAttribute('code', $response->status);
        $this->assertEquals(200, $response->status->code, 'Code was not 200');
    }

    public function test_get_purchases() {
        $this->fail('Not Yet Implemented Test');
    }

    public function test_send_test_reminder() {
        $this->fail('Not Yet Implemented Test');
    }

    public function test_get_all_bottom_lines() {
        $this->fail('Not Yet Implemented Test');
    }

    public function test_create_review() {
        $this->fail('Not Yet Implemented Test');
    }

    public function test_get_product_reviews() {
        $this->fail('Not Yet Implemented Test');
    }

    public function test_get_product_bottom_line() {
        $this->fail('Not Yet Implemented Test');
    }

    public function test_build_request() {
        $this->fail('Not Yet Implemented Test');
    }

}

?>
