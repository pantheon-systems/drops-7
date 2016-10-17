// Webform payment processing using CiviCRM's jQuery
cj(function($) {
  'use strict';
  var
    setting = Drupal.settings.webform_civicrm,
    $contributionAmount = $('[name*="[civicrm_1_contribution_1_contribution_total_amount]"]'),
    $processorFields = $('.civicrm-enabled[name$="civicrm_1_contribution_1_contribution_payment_processor_id]"]');

  function getPaymentProcessor() {
    if (!$processorFields.length) {
      return setting.paymentProcessor;
    }
    return $processorFields.filter('select, :checked').val();
  }

  function loadBillingBlock() {
    var type = getPaymentProcessor();
    if (type && type != '0') {
      $('#billing-payment-block').load(setting.contributionCallback + '&' + setting.processor_id_key + '=' + type, function() {
        $('#billing-payment-block').trigger('crmLoad').trigger('crmFormLoad');
        if (setting.billingSubmission) {
          $.each(setting.billingSubmission, function(key, val) {
            $('[name="' + key + '"]').val(val);
          });
        }
        // When an express payment button is clicked, skip the billing fields and submit the form with a placeholder
        var $expressButton = $('input[name$=_upload_express]', '#billing-payment-block');
        if ($expressButton.length) {
          $expressButton.removeClass('crm-form-submit').click(function(e) {
            e.preventDefault();
            $('input[name=credit_card_number]', '#billing-payment-block').val('express');
            $(this).closest('form').find('input.webform-submit.button-primary').click();
          })
        }
      });
    }
    else {
      $('#billing-payment-block').html('');
    }
  }
  $processorFields.on('change', function() {
    setting.billingSubmission || (setting.billingSubmission = {});
    $('input:visible, select', '#billing-payment-block').each(function() {
      var name = $(this).attr('name');
      name && (setting.billingSubmission[name] = $(this).val());
    });
    loadBillingBlock();
  });
  loadBillingBlock();

  function tally() {
    var total = 0;
    $('.line-item:visible', '#wf-crm-billing-items').each(function() {
      total += parseFloat($(this).data('amount'));
    });
    $('td+td', '#wf-crm-billing-total').html(CRM.formatMoney(total));
    if (total > 0) {
      $('#billing-payment-block').show();
    } else {
      $('#billing-payment-block').hide();
    }
  }

  function updateLineItem(item, amount, label) {
    var $lineItem = $('.line-item.' + item, '#wf-crm-billing-items');
    if (!$lineItem.length) {
      var oe = $('#wf-crm-billing-items tbody tr:first').hasClass('odd') ? ' even' : ' odd';
      $('#wf-crm-billing-items tbody').prepend('<tr class="line-item ' + item + oe + '" data-amount="' + amount + '">' +
        '<td>' + label + '</td>' +
        '<td>' + CRM.formatMoney(amount) + '</td>' +
      '</tr>');
    }
    else {
      var taxPara = 1;
      var tax = $lineItem.data('tax');
      if (tax && tax !== '0') {
        taxPara = 1 + (tax / 100);
      }
      $('td+td', $lineItem).html(CRM.formatMoney(amount * taxPara));
      $lineItem.data('amount', amount * taxPara);
    }
    tally();
  }

  function getFieldAmount(fid) {
    var amount, total = 0;
    $('input[name*="' + fid + '"], select.civicrm-enabled[name*="' + fid +'"] option')
      .filter('option:selected, [type=hidden], [type=number], [type=text], :checked')
      .each(function() {
        amount = parseFloat($(this).val());
        if ($(this).is('.webform-grid-option input')) {
          var mult = parseFloat(this.name.substring(this.name.lastIndexOf('[')+1, this.name.lastIndexOf(']')));
          !isNaN(mult) && (amount = amount * mult);
        }
        total += isNaN(amount) ? 0 : amount;
      });
    return total < 0 ? 0 : total;
  }

  function calculateContributionAmount() {
    var amount = getFieldAmount('civicrm_1_contribution_1_contribution_total_amount');
    var label = $contributionAmount.closest('div.webform-component').find('label').html() || Drupal.t('Contribution');
    updateLineItem('civicrm_1_contribution_1', amount, label);
  }

  if ($contributionAmount.length) {
    calculateContributionAmount();
    $contributionAmount.on('change keyup', calculateContributionAmount);
  }
  else {
    tally();
  }
});
