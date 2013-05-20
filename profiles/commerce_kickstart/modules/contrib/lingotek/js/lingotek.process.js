/**
 * @file
 * Custom process javascript.
 */
var lingotek;
if(!lingotek) lingotek = {};

lingotek.ProcessManager = function() {
    var processes = new Array();
    var count = 0;
    var active = 0;

    this.addProcess = function(nid) {
        count++;
        active++;
        processes[nid] = {};

        var table = jQuery("#lingotek-process-table");
        table.append('<tr><td>' + this.getProcessTitle(nid) + '</td><td><div id="lingotek-process-row-' + nid + '" class="lingotekProgress"></div></td></tr>');
        //Only show individual processes if there is more than one.
        if(count > 1) {
            table.show();
        }
    };

    this.removeProcess = function(nid) {
        //delete processes[nid];
        active--;

        if(active == 0) { //All processes complete
            delete processes;
            this.unmask();
            location.reload(TRUE);
        }
        else {
            lingotek.mt.dequeue();
        }
    };

    this.setSegmentCount = function(nid, segmentCount) {
        processes[nid].count = segmentCount;
    };

    this.isDone = function(nid) {
        if(processes[nid].current >= processes[nid].count) {
            return TRUE;
        }
    };

    this.updateProcess = function(nid, segments, targetLanguages) {
        processes[nid].current = segments;

        var percent = this.getClientPercentComplete();
        jQuery("#lingotek-process")
        .html(percent + "%")
        .attr("style", 'background-position: ' + percent + 'px 0px;');
        percent = this.getProcessPercentComplete(nid);
        jQuery("#lingotek-process-row-" + nid)
        .html(percent + "%")
        .attr("style", 'background-position: ' + percent + 'px 0px;');

        if(this.isDone(nid)) {
            lingotek.mt.finalizeTranslation(nid, targetLanguages);
        }
        else {
            lingotek.mt.dequeue();
        }
    };

    this.getProcessPercentComplete = function(nid) {
        return Math.floor(processes[nid].current / processes[nid].count * 100);
    };

    this.getClientPercentComplete = function() {
        var totalCurrent = 0;
        var totalCount = 0;
        for (var key in processes) {
            var process = processes[key];
            if (!process.current) {
                process.current = 0;
            }
            totalCurrent += process.current;
            totalCount += process.count;
        }
        return Math.floor(totalCurrent / totalCount * 100);
    }

    this.getProcessTitle = function(nid) {
        return jQuery("textarea[tag='lingotek-mt-xliff'][nid='" + nid + "']").attr("node");
    };

    this.mask = function() {
        jQuery("body").append(
            '<div id="lingotek-mask" class="lingotekMask"></div>' +
            '<div id="lingotek-container" class="lingotekContainer">' +
            '<div id="lingotek-contents" id="lingotek-contents" class="lingotekContents">' +
            '<div id="lingotek-process" class="lingotekProgress"></div>' +
            '<table id="lingotek-process-table" style="display: none;"></table>' +
            '<input type="button" value="' + Drupal.t('Stop') + '" onclick="location.reload(TRUE);" />' +
            '</div>' +
            '</div>'
            ).scrollTop(0);
    };

    this.unmask = function() {
        jQuery("#lingotek-mask").remove();
        jQuery("#lingotek-container").remove();
    };

    //Initialize:
    this.mask();

};
