var lingotek = lingotek || {};
lingotek.pm = {
  updateTimeout: 2000
};

lingotek.pm.node = {};

(function ($) {

lingotek.pm.init = function() {
  lingotek.pm.node.nid = $("#lingotek_nid").val();
  $('#lingotek-update-button').click(function() {
    if ($("[tag='lingotek_pm_row']:checked").length) {
      $(this).val(Drupal.t('Updating...')).attr('disabled', 'true');
      lingotek.pm.checked(lingotek.pm.updateCallback);
      lingotek.pm.updateButtonTarget = $('#lingotek-update-button');      
      setTimeout(lingotek.pm.statusUpdate, lingotek.pm.updateTimeout);
    }
  });
  
  // Set up mark complete button on Lingotek node tabs.
  $('#lingotek-complete-button').click(function() {
    if ($("[tag='lingotek_pm_row']:checked").length) {
      $(this).val(Drupal.t('Completing phases...')).attr('disabled', 'true');
      lingotek.pm.checked(lingotek.pm.markCompleteCallback);
      lingotek.pm.updateButtonTarget = $('#lingotek-complete-button');
    }
  });
  
}

// Updates loading status on a submit button.
// TODO: Refactor me to use a throbber or spinner.
lingotek.pm.statusUpdate = function() {
  var target = (lingotek.pm.updateButtonTarget) ? lingotek.pm.updateButtonTarget : null;
  if (target) {
    target.val(target.val() + '.');
    setTimeout(lingotek.pm.statusUpdate, lingotek.pm.updateTimeout);
  }
}

lingotek.pm.toggle_checkboxes = function(obj) {
  var checkboxes = $("[tag='lingotek_pm_row']");
  if(obj.checked) {
    checkboxes.attr("checked", "checked");
  }
  else {
    checkboxes.removeAttr("checked");
  }
}

lingotek.pm.checked = function(callback) {
  lingotek.pm.checker = {};
  lingotek.pm.checker.target = new Array();
  lingotek.pm.checker.check = $("[tag='lingotek_pm_row']:checked");
  lingotek.pm.checker.counter = 0;
  lingotek.pm.checker.callback = callback;

  lingotek.pm.checker.check.each(function(i, input) {
    lingotek.pm.checker.counter++;
    var target = $(input);
    lingotek.pm.checker.target.push(target.attr("language"));
    if (lingotek.pm.checker.check.length == lingotek.pm.checker.counter) {
      lingotek.pm.checker.callback(lingotek.pm.checker.target);
      lingotek.pm.checker = {};
    }
  });
}

lingotek.pm.markCompleteCallback = function(targets) {
  var form_data = {
    'targets[]' : targets,
    'token': $('input#submit-token').val()
  }  
  $.post(Drupal.settings.basePath + 'lingotek/mark-phases-complete/' + lingotek.pm.node.nid, form_data, function(json) { location.reload(true); });
}


lingotek.pm.updateCallback = function(targets) {
  $.post("?q=lingotek/update/" + lingotek.pm.node.nid, {'targets[]' : targets}, function(json) { location.reload(true); });
}

lingotek.pm.mt = function() {
  lingotek.pm.checked(lingotek.pm.mtCallback);
}

lingotek.pm.mtCallback = function(targets) {
  $.post("?q=lingotek/mt/" + lingotek.pm.node.nid, {'targets[]' : targets, 'engine' : $("#lingotek-mt-engine").val()}, function(json) { location.reload(true); });
}

})(jQuery);

Drupal.behaviors.lingotekSetupStatus = {
  attach: lingotek.pm.init
}
