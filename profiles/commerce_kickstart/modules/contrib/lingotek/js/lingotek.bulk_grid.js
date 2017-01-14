/**
 * @file
 * Custom javascript.
 */
function lingotek_perform_action(nid, action) {
  jQuery('#edit-grid-container .form-checkbox').removeAttr('checked');
  jQuery('#edit-the-grid-' + nid).attr('checked', 'checked');
  jQuery('#edit-select-actions').val(action);
  jQuery('#edit-select-actions').trigger('change');
}

(function ($) {
  function lingotek_trigger_modal(self) {
    var $self = $(self);
    url = $self.attr('href');
    var entity_ids = [];
    $('#edit-grid-container .form-checkbox').each(function () {
      if ($(this).attr('checked')) {
        val = $(this).val();
        if (val != 'on') {
          entity_ids.push(val);
        }
      }
    });
    if (entity_ids.length > 0) {
      $('#edit-select-actions').val('select');
      ob = Drupal.ajax[url];
      ob.element_settings.url = ob.options.url = ob.url = url + '/' + entity_ids.join(',');
      $self.trigger('click');
      $self.attr('href', url);
      $('.modal-header .close').click(function () {
        location.reload();
      });
    } else {
      var $console = $('#console').length ? $('#console') : $("#lingotek-console");
      $console.html(Drupal.t('<div class="messages warning"><h2 class="element-invisible">Warning message</h2>You must select at least one entity to perform this action.</div>'));
    }
  }

  var message_already_shown = false;
//causes all config content in a matching set to be selected together
  Drupal.behaviors.lingotekBulkGrid = {
    attach: function (context) {
      $('.form-checkbox').change(function () {
        var cells_of_selected_row = $(this).parents("tr").children();

        var selected_set_name = cells_of_selected_row.children('.set_name').text();

        var rows_in_same_set = $("tr").children().children('.set_name:contains("' + selected_set_name + '")').parent().parent();

        var rows_with_incompletes = rows_in_same_set.children().children('.target-pending, .target-ready, .target-edited').parent().parent();
        var boxes_checked = rows_in_same_set.children().children().children("input:checkbox:checked").length;
        if ($(this).is(':checked')) {
          rows_with_incompletes.addClass('selected');
        }
        else if (boxes_checked <= 0) {
          rows_in_same_set.removeClass('selected');
        }
        else {
          // only uncheck the box that was clicked
        }
        var this_row_incomplete = $.inArray($(this).parents('tr')[0], rows_with_incompletes) !== -1;
        var other_rows_with_incompletes = rows_with_incompletes.length - this_row_incomplete;

        if (!message_already_shown && other_rows_with_incompletes > 0) {
          $('#edit-grid-container').prepend('<div class="messages warning">All items in the same config set will be updated simultaneously, therefore some items are automatically highlighted. Disassociation will occur on an individual basis and only checked items will be affected.</div>');
          message_already_shown = true;
        }
      });

      $('input#edit-submit-actions.form-submit').hide();
      $('#edit-select-actions').once('lingotek_once_id', function () {
        $('#edit-select-actions').change(function () {
          val = $(this).val();

          if (val == 'reset' || val == 'delete') {
            lingotek_trigger_modal($('#' + val + '-link'));
          } else if (val == 'edit') {
            lingotek_trigger_modal($('#edit-settings-link'));
          } else if (val == 'workflow') {
            lingotek_trigger_modal($('#change-workflow-link'));
          } else if (val == 'delete_translations') {
            lingotek_trigger_modal($('#delete-translations-link'));
          } else {
            $('input#edit-submit-actions.form-submit').trigger('click');
          }
        });
      });

      $('#edit-limit-select').change(function () {
        $('#edit-search-submit.form-submit').trigger('click');
      });
    }
  };

  function addClickToDownloadReady() {
    original_download_ready_URL = $('#download-ready').attr('href');
    $('#download-ready').click(function () {
      modifyActionButtonURL('#download-ready', original_download_ready_URL);
    });
  }

  function addClickToUploadButton() {
    original_upload_edited_URL = $('#upload-edited').attr('href');
    $('#upload-edited').click(function () {
      modifyActionButtonURL('#upload-edited', original_upload_edited_URL);
    });
  }

  this.check_box_count = 0;
  function addClickToCheckboxes() {
    $('#edit-grid-container .form-checkbox').each(function () {
      $(this).change(function (event) {
        clarifyButtonsForCheckboxes(event);
      });
    });
  }

  //changes the href associated with the download/upload buttons after they are clicked
  //but before the links are actually followed. Also checks to see if the results are
  //filtered.
  function modifyActionButtonURL(element_id, original_URL) {
    var new_URL = original_URL.valueOf();//clones the original
    var entity_ids = getIDArray();
    var id_string = entity_ids.join(",");
    new_URL += entity_ids.length !== 0 ? "/" + entity_ids.join(",") : "";
    new_URL = entity_ids.length === 0 ? original_URL : new_URL;
    $(element_id).attr('href', new_URL);
  }

  //looks at every currently displayed row and pushes the entity_id of each
  //row with a checked checkbox into the return variable
  function getIDArray(visible_check) {
    var entity_ids = [];
    var visible = visible_check === true;
    $('#edit-grid-container .form-checkbox').each(function () {
      var val = $(this).val();
      if ($(this).attr('checked') || visible) {
        if (val !== 'on') {//'on' represents the 'select all' checkbox
          entity_ids.push(val);
        }
      }
    });
    return entity_ids;
  }

  function clarifyButtonsForFilter() {
    $('.notify-checked-action').hide();
    $('#upload-edited').attr('title', 'Re-upload all edited source content');
    $('#download-ready').attr('title', 'Download Ready translations');
    var text = $('#clear-filters').text();

    if (text === undefined || text === "") {
      $('.notify-filtered-action').hide();
    }
    else {
      $('.notify-filtered-action').show();
      $('#upload-edited').attr('title', 'Upload filtered results');
      $('#download-ready').attr('title', 'Download filtered results');
    }
  }

  function clarifyButtonsForCheckboxes(event) {
    var box_checked = $(event.target).attr('checked');
    //accounts for the select all box
    if ($(event.target).val() === 'on' && box_checked) {
      this.check_box_count = $('#edit-grid-container .form-checkbox').length - 2;
    }
    else if ($(event.target).val() === 'on' && !box_checked) {
      this.check_box_count = 0;
    }
    else if (box_checked === true) {
      this.check_box_count++;
    }
    else {
      this.check_box_count--;
    }
    if (this.check_box_count > 0) {
      $('.notify-filtered-action').hide();
      $('.notify-checked-action').show();
      $('#upload-edited').attr('title', 'Upload selected results');
      $('#download-ready').attr('title', 'Download selected results');
      return false;
    }
    else {
      clarifyButtonsForFilter();
    }
  }

  //guarantees that search and actions fields will match in width. Looks nicer,
  //can't do this simply with css, because the actions dropdown's width may change
  //based on its content
  function alignFields() {
    var common_width = $('#edit-select-actions').width();
    var padding_top = $('#edit-select-actions').css('padding-top');
    var padding_bottom = $('#edit-select-actions').css('padding-bottom');
    var height = $('#edit-select-actions').height();
    $('#edit-search').width(common_width);
    $('#edit-search').css('paddingBottom', padding_bottom);
    $('#edit-search').css('paddingTop', padding_top);
    $('#edit-search').css('min-height', height);
  }

  function setupToggleMarked() {
    $('.ltk-marked-checkbox').bind('click',function(){
      var $self = $(this);
      var url = $self.attr('href');
      var marked = url.substring(url.length - 1, url.length);
      var elements = $self.attr('id').split("-");
      var entityType = elements[1];
      var entityId = elements[2];
      var newMarkedValue = marked == 1 ? 0 : 1;
      var title = newMarkedValue == 1 ? 'Unmark content' : 'Mark content';
      var markedClass0 = 'fa-square-o';
      var markedClass1 = 'fa-check-square';
      var newMarkedClass = newMarkedValue ? markedClass1 : markedClass0;
      var newUrl = url.substring(0, url.length - 1) + newMarkedValue;
      $.ajax({
          url: url,
          method: 'GET',
          success: function (data) {
            $self.attr('href',newUrl);
            $self.attr('title',title);
            $self.removeClass(markedClass0 + ' ' + markedClass1);
            $self.addClass(newMarkedClass);
          }
        });
    });
  }

  //update_empty_cells allows cells with no translations statuses to display them
  //when they are available
  function update_empty_cells(data, parent, entity_id) {
    if(data[entity_id].length !== undefined) {
      return;
    }
    var used_keys = {};
    var entity_type = document.getElementById('entity-type').getAttribute('value');
    if (entity_type === 'config') {
      return;
    }
      for(var key in data[entity_id]){
        if(entity_type !== 'config' && !data[entity_id][key].hasOwnProperty('status')){
          continue;
        }
        var lang_code = key.valueOf();
        //this keeps the displayed language code consistent with what is retrieved
        //on page load
        lang_code = lang_code.toLowerCase();
        lang_code = lang_code.replace('_','-');
        var url = window.location.href;
        url = url.substr(0,url.indexOf('admin'));
        var href = url + 'lingotek/workbench/' + data.entity_type + '/' + entity_id + '/' + key;
        var link_text = key.substring(0,2);
        //accounts for multiple dialects, current format is to shorten the first language
        //and give the full language for all subsequent dialects of that language
        if(used_keys.hasOwnProperty(link_text)){
          link_text = lang_code;
        }
        else {
          used_keys[link_text] = link_text;
        }
        //Create the appropriate title
        var title;
        var status = entity_type !== 'config' ? data[entity_id][key].status : data[entity_id][key].toUpperCase();
        switch(status) {
          case "READY":
            title = 'Ready to download';
            break;
          case "CURRENT":
            title = 'Current';
            break;
          case "READY_INTERIM":
            title = 'Ready to Download Interim Translations';
            break;
          case "INTERIM":
            title = 'Interim translation downloaded';
          case "EDITED":
            title = 'Needs to be Uploaded';
            break;
          case "PENDING":
            title = 'In progress';
            break;
          case "ERROR":
            title = 'Error';
            break;
          case "DELETED":
            continue;
        }
        //create the link
        var status_link = $('<a></a>');
        status_link.attr('href', href);
        status_link.attr('target','_blank');
        status_link.attr('title',title);
        status_link.addClass('language-icon target-' + status.toLowerCase());
        status_link.text(link_text);

        $('.emptyTD', parent).each(function(){
          var index = $('td',parent).index($(this));
          var translation_header = $('th').eq(index);
          if($('a',translation_header).text().toLowerCase() === 'translations'){
            $(this).append(status_link);
          }
        });
      }
      //remove the identifying class
      $('.emptyTD',parent).removeClass();
  }

  function updateRowStatus(data, row, entity_id) {
    //if the row does not yet have status indicators
    if($('.emptyTD',row).length > 0){
      update_empty_cells(data, row, entity_id);
      return;
    }
    //content is disabled and should not be updated
    if($(row).find('.fa-minus-square').length > 0){
      return;
    }
    if(data[entity_id].hasOwnProperty('last_modified')){
      var main_table = document.getElementsByClassName('table-select-processed');
      var table_headers = main_table[0].getElementsByTagName('th');
      var last_modified_index = null;
      for(var i = 0; i < table_headers.length; i++){
        if(table_headers[i].textContent.toLowerCase().indexOf('modified') !== -1) {
          last_modified_index = i;
          break;
        }
      }
      if(last_modified_index !== null) {
        var tds = row[0].getElementsByTagName('td');
        tds[last_modified_index].textContent = data[entity_id]['last_modified'];
      }
    }
    var entity_type = document.getElementById('entity-type').getAttribute('value');
    if(data[entity_id].length !== undefined && entity_type === 'config') {
      $('.language-icon', row).parent().empty().addClass('emptyTD');
      $('.fa-check-square', row).removeClass().addClass('fa fa-square-o').attr('title', 'Needs to be Uploaded');
      return;
    }

    // Find and update the source icon
    var source_status = data[entity_id]['source_status'];
    var source_icon = $(row).find('.ltk-source-icon');
    var entity_profile = data[entity_id]['profile'];
    switch (source_status) {
      case "NONE" :
        source_icon.removeClass().addClass('ltk-source-icon source-none');
        source_icon.removeAttr('title').attr('title', 'Upload');
        break;
      case "EDITED":
        source_icon.removeClass().addClass('ltk-source-icon source-edited');
        source_icon.removeAttr('target');
        source_icon.removeAttr('title').attr('title', 'Re-upload (content has changed since last upload');
        source_icon.removeAttr('href').attr('href', '#');
        source_icon.click(function(){
          lingotek_perform_action(entity_id,'upload');
        });
        break;
      case "CURRENT":
        source_icon.removeClass().addClass('ltk-source-icon source-current');
        source_icon.removeAttr('title').attr('title', 'Source Uploaded');
        break;
      case "ERROR":
        source_icon.removeClass().addClass('ltk-source-icon source-error');
        error_title = data[entity_id]['last_upload_error'];
        source_icon.removeAttr('title').attr('title', error_title);
        break;
    }
    if (entity_profile === 'DISABLED') {
      source_icon.removeClass().addClass('ltk-source-icon source-disabled');
      source_icon.attr('title', 'Disabled, cannot request translation');
    }

    //iterate through each target icon and update them
    $(row).find('a.language-icon').each(function () {
      var icon_href = $(this).attr('href');
      //retrieve the language code from the href
      icon_href = icon_href.split("#")[0];

      var language_code = icon_href.substring(icon_href.length - 'xx_XX'.length);//normal locale code
      if(data[entity_id][language_code] === undefined){
        var language_code = icon_href.substring(icon_href.length - 'xx'.length);//language code case
      }
      var title = $(this).attr('title');
      var cutoff = title.indexOf('-');
      title = title.substring(0, cutoff + 1);
      var target_status = entity_type !== 'config' ? data[entity_id][language_code]
        : data[entity_id][language_code].toUpperCase();
      switch (target_status) {
        case "NONE":
          var attrs = {
                        class:'ltk-target-none',
                        title:'No Translation',
                      };
          $(this).replaceWith(function () {
            var new_element = $("<span></span>", attrs).append($(this).contents());
            return new_element;
          });
        case "READY":
          $(this).removeClass().addClass('language-icon target-ready');
          $(this).attr('title', 'Ready for Download');
          break;
        case "CURRENT":
          $(this).removeClass().addClass('language-icon target-current');
          $(this).attr('title', 'Current');
          break;
        case "READY_INTERIM":
          $(this).removeClass().addClass('language-icon target-ready_interim');
          $(this).attr('title', 'Ready for Interim Download');
          break;
        case "INTERIM":
          $(this).removeClass().addClass('language-icon target-interim');
          $(this).attr('title', 'In-progress (interim translation downloaded)');
          break;
        case "EDITED":
          $(this).removeClass().addClass('language-icon target-edited');
          $(this).attr('title', 'Not Current');
          break;
        case "PENDING":
          $(this).removeClass().addClass('language-icon target-pending');
          $(this).attr('title', 'In-Progress');
          break;
        case "UNTRACKED":
          $(this).removeClass().addClass('language-icon target-untracked')
          $(this).attr('title', 'Translation exists, but it is not being tracked by Lingotek');
          break;
        case "ERROR":
          $(this).removeClass().addClass('language-icon target-error');
          $(this).attr('title', 'Error');
          break;
      }
      if (entity_profile === 'DISABLED') {
        var attrs = {
                      class:'ltk-target-disabled',
                      title:'Disabled, cannot request translation',
                    };
        $(this).replaceWith(function () {
          var new_element = $("<span></span>", attrs).append($(this).contents());
            return new_element;
          });
      }
    });
  }

  function updateStatusIndicators(data) {
    //the checkboxes always have the row's entity id
    $('#edit-grid-container .form-checkbox').each(function () {
      var entity_id = $(this).val();
      if (data.hasOwnProperty(entity_id)) {
        var parent = $(this).closest('tr');
        //this creates the random fill in effect, not sure if its a keeper
        var i = Math.floor((Math.random() * 7) + 1);
        setTimeout(updateRowStatus,300 * i,data,parent,entity_id);
      }
    });
  }

  function pollTranslationStatus(){
    // Prevent jumping to top of page when source icons are clicked.
    $('.ltk-source-icon.source-none').click(function(e) {
      e.preventDefault();
    });
    $('.ltk-source-icon.source-edited').click(function(e){
      e.preventDefault();
    });
    //makes it easy to find empty cells, the only empty ones will be in the status
    //column if the row hasn't been uploaded yet.
    $('td:empty').addClass('emptyTD');
    var ids_to_poll = '';
    //get all the entity_ids currently displayed
    $('#edit-grid-container .form-checkbox').each(function () {
      var entity_id = $(this).val();
      if(entity_id !== 'on') {
        ids_to_poll += $(this).val() + ',';
      }
    });
    //start the poller on 30 sec interval (30000)
    setInterval(function () {
      $.ajax({
          url: $('#async-update').attr('href') + '/' + ids_to_poll.substr(0,ids_to_poll.length-1),
          dataType: 'json',
          success: function (data) {
            if (data !== null) {
              updateStatusIndicators(data);
            }
          }
        });
      }, 10000);
  }
  function pollAutomaticDownloads(){
    //config section does not have profiles, so automatic downloads should not
    //happen
    if($('#entity-type').val() === 'config'){
      return;
    }
    setInterval(function () {
      $.ajax({
          url: $('#auto-download').attr('href'),
          dataType: 'json'
        });
      }, 30000);
  }
  function configShowMoreOptions(){
    $('#more-options').toggleClass('more-options-flip');
    $('#force-down').toggle();
  }
  function setupConfigMoreOptions() {
    $('#force-down').hide();
    $('#more-options').click(configShowMoreOptions);
  }
  $(document).ready(function () {
    setupConfigMoreOptions();
    alignFields();
    setupToggleMarked();
    pollTranslationStatus();
//    pollAutomaticDownloads();
    addClickToDownloadReady();
    addClickToUploadButton();
    addClickToCheckboxes();
    clarifyButtonsForFilter();
  });
})(jQuery);
