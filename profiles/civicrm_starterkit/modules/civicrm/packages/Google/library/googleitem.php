<?php
/*
 * Copyright (C) 2007 Google Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Classes used to represent an item to be used for Google Checkout
 * @version $Id: googleitem.php 1234 2007-09-25 14:58:57Z ropu $
 */

 /**
  * Creates an item to be added to the shopping cart.
  * A new instance of the class must be created for each item to be added.
  * 
  * Required fields are the item name, description, quantity and price
  * The private-data and tax-selector for each item can be set in the 
  * constructor call or using individual Set functions
  */
  class GoogleItem {
     
    var $item_name; 
    var $item_description;
    var $unit_price;
    var $quantity;
    var $merchant_private_item_data;
    var $merchant_item_id;
    var $tax_table_selector;
    var $email_delivery;
    var $subscription;
    var $digital_content=false;
    var $digital_description;
    var $digital_key;
    var $digital_url;
    var $currency;
    var $item_weight;
    var $numeric_weight;

    /**
     * {@link http://code.google.com/apis/checkout/developer/index.html#tag_item <item>}
     * 
     * @param string $name the name of the item -- required
     * @param string $desc the description of the item -- required
     * @param integer $qty the number of units of this item the customer has 
     *                    in its shopping cart -- required
     * @param double $price the unit price of the item -- required
     * @param string $item_weight the weight unit used to specify the item's
     *                            weight,
     *                            one of 'LB' (pounds) or 'KG' (kilograms)
     * @param double $numeric_weight the weight of the item
     * 
     */
    function GoogleItem($name, $desc, $qty, $price, $item_weight='', $numeric_weight='') {
      $this->item_name = $name; 
      $this->item_description= $desc;
      $this->unit_price = $price;
      $this->quantity = $qty;

      if($item_weight != '' && $numeric_weight !== '') {
        switch(strtoupper($item_weight)){
          case 'KG':
            $this->item_weight = strtoupper($item_weight);
            break;
          case 'LB':
          default:
            $this->item_weight = 'LB';
        }
        $this->numeric_weight = (double)$numeric_weight;
      } 
    }
    
    function SetMerchantPrivateItemData($private_data) {
      $this->merchant_private_item_data = $private_data;  
    }

    /**
     * Set the merchant item id that the merchant uses to uniquely identify an
     * item. Google Checkout will include this value in the
     * merchant calculation callbacks
     * 
     * GC tag: {@link http://code.google.com/apis/checkout/developer/index.html#tag_merchant-item-id <merchant-item-id>}
     * 
     * @param mixed $item_id the value that identifies this item on the 
     *                                 merchant's side
     * 
     * @return void
     */
    function SetMerchantItemId($item_id) {
      $this->merchant_item_id = $item_id;  
    }
    
    /**
     * Sets the tax table selector which identifies an alternate tax table that
     * should be used to calculate tax for a particular item. 
     * 
     * GC tag: {@link http://code.google.com/apis/checkout/developer/index.html#tag_tax-table-selector <tax-table-selector>}
     * 
     * @param string $tax_selector this value should correspond to the name 
     *                             of an alternate-tax-table.
     * 
     * @return void
     */
    function SetTaxTableSelector($tax_selector) {
      $this->tax_table_selector = $tax_selector;  
    }

    /**
     * Used when the item's content is digital, sets whether the merchant will
     * send an email to the buyer explaining how to access the digital content.
     * Email delivery allows the merchant to charge the buyer for an order
     * before allowing the buyer to access the digital content.
     * 
     * GC tag: {@link http://code.google.com/apis/checkout/developer/index.html#tag_email-delivery <email-delivery>}
     * 
     * @param bool $email_delivery true if email_delivery applies, defaults to
     *                             false
     * 
     * @return void
     */
    function SetEmailDigitalDelivery($email_delivery='false') {
      $this->digital_url = '';
      $this->digital_key = '';
      $this->digital_description = '';
      $this->email_delivery = $email_delivery;  
      $this->digital_content=true;
    }
    
    /**
     * Sets the information related to the digital delivery of the item.
     * 
     * GC tag: {@link http://code.google.com/apis/checkout/developer/index.html#tag_digital-content <digital-content>}
     * 
     * @param string $digital_url the url the customer must go to download the
     *                            item. --optional
     * @param string $digital_key the key which allows to download or unlock the
     *                            digital content item -- optional
     * @param string $digital_description instructions for downloading adigital 
     *                                    content item, 1024 characters max, can
     *                                    contain xml-escaped HTML -- optional
     * 
     * @return void
     */
    function SetURLDigitalContent($digital_url, $digital_key, $digital_description) {
      $this->digital_url = $digital_url;
      $this->digital_key = $digital_key;
      $this->digital_description = $digital_description;
      $this->email_delivery = 'false';  
      $this->digital_content = true;
    }
    
  /**
    *  Sets the subscription item for the cart
    *
    *   @param googlesubscription $sub the subscription item 
    *   @return void
    */
    
    function SetSubscription($sub) {
      $this->subscription = $sub;
    }
  
  /**
    * Sets the currency for the item
    * 
    * @param string $currency currency USD or GBP
    * @return void
    */
    function SetCurrency($currency){
      $this->currency = $currency;
    }
      
  /**
      * Returns the generated XMl from item specifications
      * To use this function, you need to specify the item currency
      * @param void
      * @return string XML cart
      */  
    function GetXML(){
      require_once('xml-processing/gc_xmlbuilder.php');
      
      $xml_data = new gc_XmlBuilder();
      $xml_data->Push('item');
      $xml_data->Element('item-name', $this->item_name);
      $xml_data->Element('item-description', $this->item_description);
      $xml_data->Element('unit-price', $this->unit_price,
          array('currency' => $this->currency));
      $xml_data->Element('quantity', $this->quantity);
      if($this->merchant_private_item_data != '') {
      //          echo get_class($item->merchant_private_item_data);
        if(is_a($this->merchant_private_item_data, 
                                            'merchantprivate')) {
          $this->merchant_private_item_data->AddMerchantPrivateToXML($xml_data);
        }
        else {
          $xml_data->Element('merchant-private-item-data', 
                                           $this->merchant_private_item_data);
        }
      }
      if($this->merchant_item_id != '')
        $xml_data->Element('merchant-item-id', $this->merchant_item_id);
      if($this->tax_table_selector != '')
        $xml_data->Element('tax-table-selector', $this->tax_table_selector);
      //     recurring Carrier calculation
      if($this->item_weight != '' && $this->numeric_weight !== '') {
        $xml_data->EmptyElement('item-weight', array( 'unit' => $this->item_weight,
                                              'value' => $this->numeric_weight
                                             ));
      }
      //     recurring Digital Delivery Tags
      if($this->digital_content) {
        $xml_data->Push('digital-content');
        if(!empty($this->digital_url)) {
          $xml_data->Element('description', substr($this->digital_description,
                                                        0, MAX_DIGITAL_DESC));
          $xml_data->Element('url', $this->digital_url);
      //            To avoid NULL key message in GC confirmation Page
          if(!empty($this->digital_key)) {
            $xml_data->Element('key', $this->digital_key);
          }
        }
        else if(!empty($item->digital_description)) {
          $xml_data->element('description', substr($item->digital_description, 0,MAX_DIGITAL_DESC));
        }
        else {
          $xml_data->Element('email-delivery', 
                    $this->_GetBooleanValue($this->email_delivery, "true"));
        }
        $xml_data->pop('digital-content');          
      }
      $xml_data->Pop('item'); 
      return $xml_data->GetXML();
    }
  }
?>
