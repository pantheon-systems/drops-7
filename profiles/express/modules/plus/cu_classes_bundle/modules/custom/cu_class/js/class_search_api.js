/**
 * 
 */

var start;
var end;

// DISPLAY OPTIONS FOR THE PROGRESS SPINNER
var opts = {
	lines : 13 // The number of lines to draw
	,
	length : 28 // The length of each line
	,
	width : 14 // The line thickness
	,
	radius : 42 // The radius of the inner circle
	,
	scale : 1 // Scales overall size of the spinner
	,
	corners : 1 // Corner roundness (0..1)
	,
	color : '#000' // #rgb or #rrggbb or array of colors
	,
	opacity : 0.25 // Opacity of the lines
	,
	rotate : 0 // The rotation offset
	,
	direction : 1 // 1: clockwise, -1: counterclockwise
	,
	speed : 1 // Rounds per second
	,
	trail : 60 // Afterglow percentage
	,
	fps : 20 // Frames per second when using setTimeout() as a fallback for
				// CSS
	,
	zIndex : 2e9 // The z-index (defaults to 2000000000)
	,
	className : 'spinner' // The CSS class to assign to the spinner
	,
	top : '50%' // Top position relative to parent
	,
	left : '50%' // Left position relative to parent
	,
	shadow : false // Whether to render a shadow
	,
	hwaccel : false // Whether to use hardware acceleration
	,
	position : 'absolute' // Element positioning
}

var spinner = new Spinner(opts);
var environmentURI = new Object();
environmentURI.DEV = "https://osbdev.dev.cu.edu";
environmentURI.TST = "https://osbtst.dev.cu.edu";
environmentURI.STG = "https://osbstg.qa.cu.edu";
environmentURI.PRD = "https://osbprd.prod.cu.edu";
environmentURI.ESB = "https://esbprd.prod.cu.edu";

var alertLevel = new Object();
alertLevel.success = "alert-success";
alertLevel.info = "alert-info";
alertLevel.warn = "alert-warning";
alertLevel.error = "alert-danger";

var connection = {
	getEnvURI : function(environment) {
		switch (environment) {
		case "DEV":
			return environmentURI.DEV;
			break;
		case "TST":
			return environmentURI.TST;
			break;
		case "STG":
			return environmentURI.STG;
			break;
		case "PRD":
			return environmentURI.PRD;
			break;
		case "ESB":
			return environmentURI.ESB;
			break;
		default:
			return "<environment_uri>"
		}
	}
}

var classList = new LinkedList();

$.fn.nextOrFirst = function(selector) {
	var next = this.next(selector);
	return (next.length) ? next : this.prevAll(selector).last();
};

$.fn.prevOrLast = function(selector) {
	var prev = this.prev(selector);
	return (prev.length) ? prev : this.nextAll(selector).last();
};

var classDetails="";

$(document).ready(
		function() {

			classList = new LinkedList();
			
	         $('#headerCarousel').carousel({
	             interval: 4000
	         })

			$("#myCarousel").carousel({
				interval : false
			});

			$("#class_details_modal").on('show.bs.modal', function(event) {
				// Get button that triggered the modal
				var button = $(event.relatedTarget);
				// Extract value from data-* attributes
				var classNbr = button.data('class_nbr');
				classDetails = '';
				console.log(classNbr);
				var classNode = classList.itemByValue(classNbr);
				$(this).find('.active_course_id').text(classNode.value);
				getClassDetails(classNode.value, $(this).find('.active_course_details'),$(this).find('.active_course_map')[0]);
			});

			// Start slide animation
			$("#myCarousel").on(
					'slide.bs.carousel',
					function(event) {
						console.log('slide');

						var active = $(event.target).find(
								'.carousel-inner > .item.active');
						var from = active.index();
						var next = $(event.relatedTarget);
						var to = next.index();
						var direction = event.direction;

						var classNode = classList.itemByValue(active.find(
								'.active_course_id').text());
						var next_class_number = null;
						if (direction == "left") {
							next_class_number = classNode.next.value;
							next.find('.active_course_id').text(
									classNode.next.value);
						} else if (direction == "right") {
							if (classNode.previous == null) {
								next_class_number = classNode.end.value;
							} else {
								next_class_number = classNode.previous.value;
							}
							next.find('.active_course_id').text(
									next_class_number);
						} else {
							console.log("error...no direction");
						}
						getClassDetails(classNode.value, next.find('.active_course_details'),next.find('.active_course_map')[0]);

						// console.log('prev', active.prevOrLast());
						// console.log('active', active, from);
						// console.log('next', next, to);
						// console.log('direction', direction);

					});

			// var warningpanel;
			$('#msgpanel').msgpanel({
				panelClass : 'msgpanel-warning-msg',
				speed : 'slow',
				panelOpacity : 0.95
			});

			$("#connection_uri").text(generateUri(this));

			$('input[type=radio][name=env]').change(function() {
				$("#connection_uri").text(generateUri(this));
			});

			$('input[type=radio][name=version]').change(function() {
				$("#connection_uri").text(generateUri(this));
			});

			$('input[type=radio][name=campus]').change(function() {
				$("#connection_uri").text(generateUri(this));
			});

			$('input[type=radio][name=ACAD_CAREER]').change(function() {
				filter('ACAD_CAREER', this.value);
				search("cu_master","accordion");
				$("#results_accordian").accordion("refresh");
			});

			$('input[type=radio][name=CAMPUS]').change(function() {
				filter('CAMPUS', this.value);
				search("cu_master","accordion");
				$("#results_accordian").accordion("refresh");
			});

			$('input[type=radio][name=SSR_OPEN_ONLY]').change(function() {
				filter('SSR_OPEN_ONLY', this.value);
				search("cu_master","accordion");
				$("#results_accordian").accordion("refresh");
			});

			$("#tabs").tabs();

			$("#connection_accordion").accordion({
				collapsible : true,
				active : false
			});

			$("#filters_accordion").accordion({
				collapsible : true,
				active : false,
				heightStyle : "content"
			});

			$("#results_accordian").accordion({
				collapsible : true,
				active : false,
				heightStyle : "content",
				autoActivate : false,
				beforeActivate : function(event, ui) {
					//if(ui.newHeader.attr('id')!=undefined){
					if(ui.newHeader.hasClass('ui-state-hover')){
						filter("CRSE_ID", ui.newHeader.attr('id'));
						search("cu_limited",ui.newPanel);
						$("#results_accordian").accordion("refresh");
					}
				}
			});

			$("#filters").tagsinput({
				itemValue : 'value',
				itemText : 'text'
			});

			/* BEFORE REMOVING AN ITEM, UPDATE THE FIELD VALUE */
			$("#filters").on(
					'itemRemoved',
					function(event) {
						console.log("Removing " + event.item.id);
						$('input:radio[name=' + event.item.id + ']').attr(
								'checked', false);
						event.item.field.val("");
						console.log($("#filters").val());
						search("cu_master","accordion");
					});

			/* BEFORE ADDING AN ITEM, REMOVE DUPLICATES */
			$("#filters").on('beforeItemAdd', function(event) {
				console.log("Preparing to add " + event.item.id);
				$($("#filters").tagsinput('items')).each(function() {
					if (this.id == event.item.id) {
						this.value = event.item.value;
						this.text = event.item.text;
						$('#filters').tagsinput('refresh');
						$("#" + this.id).val(event.item.value);
						event.cancel = true;
						// console.log("Found duplicate filter field id.
						// Removing it.");
						// $("#filters").tagsinput('remove',{ "id": this.id,
						// "value": this.value , "text": this.field_id + '=' +
						// this.value, "field":$("#" + this.field_id) })
						return false;
					}
				});
			});

			/* ON ADDING AN ITEM, UPDATE THE FIELD VALUE */
			$("#filters").on('itemAdded', function(event) {
				console.log("Adding " + event.item.id);
				event.item.field.val(event.item.value);
				$("#" + event.item.field_id).val(event.item.value);
			});

			// search("mobile");

			// Attach a submit handler to the form
			$("#submit").click(function(event) {
				// $('#results_accordian').empty();
				search("cu_master","accordion");
			});
			

		    $( document ).uitooltip({
		      position: {
		        my: "center bottom-20",
		        at: "center top",
		        using: function( position, feedback ) {
		          $( this ).css( position );
		          $( "<div>" )
		            .addClass( "arrow" )
		            .addClass( feedback.vertical )
		            .addClass( feedback.horizontal )
		            .appendTo( this );
		        }
		      }
		    }); 
		});

function generateUri(source) {
	//console.log(source);
	source.checked = true;
	var uri = "";
	var baseUri = source.name == "env" ? connection.getEnvURI(source.value)
			: connection.getEnvURI($('input[type=radio][name=env]:checked')
					.val());
	var campus = source.name == "campus" ? source.value : $(
			'input[type=radio][name=campus]:checked').val();
	var version = source.name == "version" ? source.value : $(
			'input[type=radio][name=version]:checked').val();
	uri = baseUri + campus + "Cs_Any_ClassSearch" + version
			+ "/SSR_GET_CLASSES_R.v1/get/classes";
	//console.log(uri);
	return uri;
}

function search(type, display_target) {

	// LET'S CHECK FOR ACTIVE FILTERS AND SET THE SERVICE CALL TYPE ACCORDINGLY
	/*
	 * if($("#filters").val()==""){ type = "cu_master"; } else { type =
	 * "cu_detail"; }
	 */

	var target = document.getElementById('courses_panel');

	var self = this;
	self.serviceURI = $("#connection_uri").text() + "?type=" + type;
	console.log("URL: " + self.serviceURI);
	self.username = $("#uid").val();
	if (self.username == "") {
		displayMsg(alertLevel.warn, "Username is required");
		spinner.stop();
		return;
	}
	self.password = $("#pwd").val();
	if (self.password == "") {
		displayMsg(alertLevel.warn, "Password is required");
		spinner.stop();
		return;
	}

	var hash = btoa(self.username + ":" + self.password);
	var requestXml = getRequestXml();

	self.ajax = function(uri, method, request_data) {
		var request = {
			url : uri,
			type : method,
			contentType : "application/xml",
			accepts : "application/xml",
			cache : false,
			data : request_data,
			dataType : "text",
			xhrFields : {
				'withCredentials' : true
			},
			headers : {
				'Access-Control-Allow-Origin' : "*",
				'Access-Control-Allow-Methods' : "GET, POST, PUT, DELETE, OPTIONS",
				'Access-Control-Allow-Headers' : "Authorization",
				'Authorization' : "Basic " + hash
			},
			beforeSend : function(xhr) {
				start = new Date().getTime();
				// console.log("=====> URI: "+ uri);
				// console.log("Authorization: Basic " + hash);
				xhr.setRequestHeader("Authorization", "Basic " + hash);
				if(display_target=="accordion") { 
					$("#results_accordian").empty();
					spinner.spin(target)
				} else {
					display_target.html("<div><blink>Retrieving the most up-to-date information...</blink></div>");
					spinner.spin(target);
				}
			},
			error : function(jqXHR) {
				console.log("ajax error " + jqXHR.statusText);
				console.log("ajax error " + jqXHR.status);
				switch (jqXHR.status) {
				case 404:
					var msg = "<strong>404 "
							+ jqXHR.statusText
							+ " -</strong> The service address you are using may be incorrect."
					displayMsg(alertLevel.error, msg);
					break;
				case 500:
					var msg = "<strong>500 "
							+ jqXHR.statusText
							+ " -</strong> This can have multiple causes such as"
							+ " invalid credentials, improper search parameters"
							+ ", or a database error.  Contact UIS if you cannot resolve "
							+ "the issue.<p>" + jqXHR.responseText;
					displayMsg(alertLevel.error, msg);
					break;
				}
				spinner.stop();
			}
		};
		return $.ajax(request);
	}

	if(display_target=="accordion"){
		self.ajax(self.serviceURI, 'POST', requestXml).done(function(data){updateAccordion(data);});
	} else {
		self.ajax(self.serviceURI, 'POST', requestXml).done(function(data){display_target.html(updateCourse(data));});
//		console.log("no target defined");
	}

}

/**
 * Updates the Courses Accordion
 */
function updateAccordion(data)
{
	end = new Date().getTime();
	xml_neat = formatXml(data);
	console.log(xml_neat);

	// Check for errors
	if (typeof $(data).find("IS_FAULT").html() !== "undefined"
			&& $(data).find("IS_FAULT").text() == "Y") {
		console.log("Found fault response.");

		var msg = '<strong>ERROR - </strong> An error has occurred. Please review the following message(s):';

		$(data).find("SCC_FAULT_RESP > detail > MSGS > MSG").each(
				function() {
					msg += '<p>Code:&nbsp;&nbsp;'
							+ $(this).find('ID:first').text()
							+ '<br/>Message:&nbsp;&nbsp;'
							+ $(this).find('DESCR:first').text();
				});

		displayMsg(alertLevel.error, msg);
		spinner.stop();

		return;
	}

	var elapsed_time = end - start;

	var prefix = "ser\\:";

//	console.log("Without namespace: "
//			+ (typeof $(data).find("SUBJECTS").html()));
//	console
//			.log("With namespace: "
//					+ ($(data).find(prefix + "SUBJECTS")).html());

	if (typeof $(data).find("SUBJECTS").html() !== "undefined") {
//		console.log("Found response without namespace.");
		prefix = "";
	} else if (typeof $(data).find(prefix + "SUBJECTS").html() !== "undefined") {
//		console.log("Found response with namespace.");
		prefix = "ser\\:";
	} else {
		console.log("no subjects found. exiting");
		return;
	}
	classList = new LinkedList();
	var total_course_count = $(data).find(prefix + "SSR_COURSE_COUNT").text();
	var total_class_count = $(data).find(prefix + "SSR_CLASS_COUNT").text();
	$(data)
			.find(prefix + "SUBJECTS > " + prefix + "SUBJECT")
			.each(
					function() {
						var content = "";
						var count = 0;
						$(this)
								.find(
										prefix + 'CLASSES_SUMMARY > ' + prefix
												+ 'CLASS_SUMMARY')
								.each(
										function() {
											count++;
											classList.add($(this).find(
													prefix + 'CLASS_NBR:first')
													.text());
											content += '<tr>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'CLASS_NBR:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'CLASS_SECTION:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'CAMPUS_LOVDescr:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'INSTRUCTION_MODE_LOVDescr:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'STATUS_LOVDescr:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ '<button type="button" class="btn btn-xs btn-info" data-CLASS_NBR="'
													+ $(this)
															.find(
																	prefix
																			+ 'CLASS_NBR:first')
															.text()
													+ '" data-toggle="modal" data-target="#class_details_modal">Details</button>'
													+ '</td>' + '</tr>';
										});
						var countBadge = "";
						if (count >= 1)
							countBadge = '&nbsp;&nbsp;<span class="badge">'
									+ count + '</span>';

						var courseHeader = '' + '<h3 id="'
								+ $(this).find(prefix + 'CRSE_ID:first').text()
								+ '">'
								+ $(this).find(
										prefix + 'COURSE_TITLE_LONG:first')
										.text()
								+ countBadge
								+ '</h3>'
								+ '<div>'
								+ ' <table class="table table-striped">'
								+ '   <tr><th style="">Course ID</th><th>Catalog #</th><th>Career</th></tr>'
								+ '   <tr>'
								+ '     <td>'
								+ $(this).find(prefix + 'CRSE_ID:first').text()
								+ '&nbsp;<img src="img/16x16/Filter.png" style="cursor: pointer;cursor:hand;" onclick="filter(\'CRSE_ID\',\''
								+ $(this).find('CRSE_ID:first').text()
								+ '\');"/></td>'
								+ '     <td>'
								+ $(this).find(prefix + 'CATALOG_NBR:first')
										.text()
								+ '&nbsp;<img src="img/16x16/Filter.png" style="cursor: pointer;cursor:hand;" onclick="filter(\'CATALOG_NBR\',\''
								+ $(this).find('CATALOG_NBR:first').text()
								+ '\');"/></td>'
								+ '     <td>'
								+ $(this).find(prefix + 'ACAD_CAREER:first')
										.text()
								+ '&nbsp;<img src="img/16x16/Filter.png" style="cursor: pointer;cursor:hand;" onclick="filter(\'ACAD_CAREER\',\''
								+ $(this).find('ACAD_CAREER:first').text()
								+ '\');"/></td>' + '   </tr>' + ' </table>';

						var classesPanel = ''
								+ ' <div class="panel panel-info">'
								+ '   <div class="panel-heading">'
								+ '     <h3 class="panel-title">Classes</h3>'
								+ '   </div>'
								+ '   <div class="panel-body">'
								+ '     <table class="table table-condensed">'
								+ '       <tr><th>Class #</th><th>Section</th><th>Campus</th><th>Instruction Mode</th><th>Status</th><th>&nbsp;</th></tr>'
								+ content + '     </table>' + '   </div>'
								+ ' </div>';

						if (countBadge == "") {
							classesPanel = '';
						}
						// close
						$('#results_accordian').append(
								courseHeader + classesPanel + '</div>');
					});
	$("#results_accordian").accordion("refresh");
	$("#results_accordian").accordion( "option", "active", "false" );
	spinner.stop();
	displayMsg(alertLevel.success,
			"<strong>Success!</strong> - Your search returned "
					+ total_course_count + " courses with a combined total of "
					+ total_class_count + " classes in " + elapsed_time / 1000
					+ " seconds.");
}

/**
 * Generate Course Data
 */
function updateCourse(data)
{
	var response = "";
	end = new Date().getTime();
	xml_neat = formatXml(data);
	console.log(xml_neat);

	// Check for errors
	if (typeof $(data).find("IS_FAULT").html() !== "undefined"
			&& $(data).find("IS_FAULT").text() == "Y") {
		console.log("Found fault response.");

		var msg = '<strong>ERROR - </strong> An error has occurred. Please review the following message(s):';

		$(data).find("SCC_FAULT_RESP > detail > MSGS > MSG").each(
				function() {
					msg += '<p>Code:&nbsp;&nbsp;'
							+ $(this).find('ID:first').text()
							+ '<br/>Message:&nbsp;&nbsp;'
							+ $(this).find('DESCR:first').text();
				});

		displayMsg(alertLevel.error, msg);
		spinner.stop();
		return;
	}

	var elapsed_time = end - start;

	var prefix = "ser\\:";
	if (typeof $(data).find("SUBJECTS").html() !== "undefined") {
//		console.log("Found response without namespace.");
		prefix = "";
	} else if (typeof $(data).find(prefix + "SUBJECTS").html() !== "undefined") {
//		console.log("Found response with namespace.");
		prefix = "ser\\:";
	} else {
		console.log("no subjects found. exiting");
		return;
	}
	classList = new LinkedList();
	var total_course_count = $(data).find(prefix + "SSR_COURSE_COUNT").text();
	var total_class_count = $(data).find(prefix + "SSR_CLASS_COUNT").text();
	$(data)
			.find(prefix + "SUBJECTS > " + prefix + "SUBJECT")
			.each(
					function() {
						var content = "";
						var count = 0;
						$(this)
								.find(
										prefix + 'CLASSES_SUMMARY > ' + prefix
												+ 'CLASS_SUMMARY')
								.each(
										function() {
											count++;
											classList.add($(this).find(
													prefix + 'CLASS_NBR:first')
													.text());
											content += '<tr title="' + $(this).find(prefix+'SSR_DESCRLONG:first').text() + '">'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'CLASS_NBR:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'CLASS_SECTION:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'CAMPUS_LOVDescr:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'INSTRUCTION_MODE_LOVDescr:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ $(this)
															.find(
																	prefix
																			+ 'STATUS_LOVDescr:first')
															.text()
													+ '</td>'
													+ '<td>'
													+ '<button type="button" class="btn btn-xs btn-info" data-CLASS_NBR="'
													+ $(this)
															.find(
																	prefix
																			+ 'CLASS_NBR:first')
															.text()
													+ '" data-toggle="modal" data-target="#class_details_modal">Details</button>'
													+ '</td>' + '</tr>';
										});
						var countBadge = "";
						if (count >= 1)
							countBadge = '&nbsp;&nbsp;<span class="badge">'
									+ count + '</span>';

						var courseHeader = '' 
//								+ '<h3 id="'
//								+ $(this).find(prefix + 'CRSE_ID:first').text()
//								+ '">'
//								+ $(this).find(
//										prefix + 'COURSE_TITLE_LONG:first')
//										.text()
//								+ countBadge
//								+ '</h3>'
								+ '<div>'
								+ ' <table class="table table-striped">'
								+ '   <tr><th style="">Course ID</th><th>Catalog #</th><th>Career</th></tr>'
								+ '   <tr>'
								+ '     <td>'
								+ $(this).find(prefix + 'CRSE_ID:first').text()
								+ '&nbsp;<img src="img/16x16/Filter.png" style="cursor: pointer;cursor:hand;" onclick="filter(\'CRSE_ID\',\''
								+ $(this).find('CRSE_ID:first').text()
								+ '\');"/></td>'
								+ '     <td>'
								+ $(this).find(prefix + 'CATALOG_NBR:first')
										.text()
								+ '&nbsp;<img src="img/16x16/Filter.png" style="cursor: pointer;cursor:hand;" onclick="filter(\'CATALOG_NBR\',\''
								+ $(this).find('CATALOG_NBR:first').text()
								+ '\');"/></td>'
								+ '     <td>'
								+ $(this).find(prefix + 'ACAD_CAREER:first')
										.text()
								+ '&nbsp;<img src="img/16x16/Filter.png" style="cursor: pointer;cursor:hand;" onclick="filter(\'ACAD_CAREER\',\''
								+ $(this).find('ACAD_CAREER:first').text()
								+ '\');"/></td>' + '   </tr>' + ' </table>';

						var classesPanel = ''
								+ ' <div class="panel panel-info">'
								+ '   <div class="panel-heading">'
								+ '     <h3 class="panel-title">Classes</h3>'
								+ '   </div>'
								+ '   <div class="panel-body">'
								+ '     <table class="table table-condensed">'
								+ '       <tr><th>Class #</th><th>Section</th><th>Campus</th><th>Instruction Mode</th><th>Status</th><th>&nbsp;</th></tr>'
								+ content + '     </table>' + '   </div>'
								+ ' </div>';

						if (countBadge == "") {
							classesPanel = '';
						}

						response = courseHeader + classesPanel;
					});
	spinner.stop();
	displayMsg(alertLevel.success,
			"<strong>Success!</strong> - Your search returned "
					+ total_course_count + " courses with a combined total of "
					+ total_class_count + " classes in " + elapsed_time / 1000
					+ " seconds.");
	return response;
}


/**
 * Add filter
 * 
 * @param field_id
 * @param value
 */
function filter(field_id, value) {

	console.log("Field: " + field_id + ", Value: " + value);
	$("#filters").tagsinput('add', {
		"id" : field_id,
		"value" : value,
		"text" : field_id + '=' + value,
		"field" : $("#" + field_id)
	});
	// search("cu_master");
	// $("#results_accordian").accordion("refresh");

}

/**
 * Display a message
 * 
 * @param level
 * @param msg
 */
function displayMsg(level, msg) {
	$('#msgpanel').removeClass(
			"alert-danger alert-warning alert-success alert-info");
	infopanel = $('#msgpanel').data('msgpanel');
	$('#msgpanel').addClass(level);
	infopanel.showPanel(msg);
}

/**
 * Helper function to return a request xml string
 * 
 * @returns {String}
 */
function getRequestXml() {
	var xml = "<CLASS_SEARCH_REQUEST>" + "<INSTITUTION>"
			+ $("#INSTITUTION").val() + "</INSTITUTION>" + "<STRM>"
			+ $("#STRM").val() + "</STRM>" + "<CLASS_NBR>"
			+ $("#CLASS_NBR").val() + "</CLASS_NBR>" + "<CRSE_ID>"
			+ $("#CRSE_ID").val() + "</CRSE_ID>" + "<CRSE_OFFER_NBR>"
			+ $("#CRSE_OFFER_NBR").val() + "</CRSE_OFFER_NBR>"
			+ "<SESSION_CODE>" + $("#SESSION_CODE").val() + "</SESSION_CODE>"
			+ "<CLASS_SECTION>" + $("#CLASS_SECTION").val()
			+ "</CLASS_SECTION>" + "<SUBJECT>" + $("#SUBJECT").val()
			+ "</SUBJECT>" + "<CATALOG_NBR>" + $("#CATALOG_NBR").val()
			+ "</CATALOG_NBR>" + "<SSR_EXACT_MATCH1>"
			+ $("#SSR_EXACT_MATCH1").val() + "</SSR_EXACT_MATCH1>"
			+ "<SSR_OPEN_ONLY>" + $("#SSR_OPEN_ONLY").val()
			+ "</SSR_OPEN_ONLY>" + "<OEE_IND>" + $("#OEE_IND").val()
			+ "</OEE_IND>" + "<DESCR>" + $("#DESCR").val() + "</DESCR>"
			+ "<ACAD_CAREER>" + $("#ACAD_CAREER").val() + "</ACAD_CAREER>"
			+ "<SSR_COMPONENT>" + $("#SSR_COMPONENT").val()
			+ "</SSR_COMPONENT>" + "<INSTRUCTION_MODE>"
			+ $("#INSTRUCTION_MODE").val() + "</INSTRUCTION_MODE>" + "<CAMPUS>"
			+ $("#CAMPUS").val() + "</CAMPUS>" + "<LOCATION>"
			+ $("#LOCATION").val() + "</LOCATION>" + "<MEETING_TIME_START>"
			+ $("#MEETING_TIME_START").val() + "</MEETING_TIME_START>"
			+ "<SSR_MTGTIME_START2>" + $("#SSR_MTGTIME_START2").val()
			+ "</SSR_MTGTIME_START2>" + "<MEETING_TIME_END>"
			+ $("#MEETING_TIME_END").val() + "</MEETING_TIME_END>" + "<MON>"
			+ $("#MON").val() + "</MON>" + "<TUES>" + $("#TUES").val()
			+ "</TUES>" + "<WED>" + $("#WED").val() + "</WED>" + "<THURS>"
			+ $("#THURS").val() + "</THURS>" + "<FRI>" + $("#FRI").val()
			+ "</FRI>" + "<SAT>" + $("#SAT").val() + "</SAT>" + "<SUN>"
			+ $("#SUN").val() + "</SUN>" + "<INCLUDE_CLASS_DAYS>"
			+ $("#INCLUDE_CLASS_DAYS").val() + "</INCLUDE_CLASS_DAYS>"
			+ "<LAST_NAME>" + $("#LAST_NAME").val() + "</LAST_NAME>"
			+ "<SSR_EXACT_MATCH2>" + $("#SSR_EXACT_MATCH2").val()
			+ "</SSR_EXACT_MATCH2>" + "<UNITS_MINIMUM>"
			+ $("#UNITS_MINIMUM").val() + "</UNITS_MINIMUM>"
			+ "<UNITS_MAXIMUM>" + $("#UNITS_MAXIMUM").val()
			+ "</UNITS_MAXIMUM>" + "<SCC_ENTITY_INST_ID>"
			+ $("#SCC_ENTITY_INST_ID").val() + "</SCC_ENTITY_INST_ID>"
			+ "<OBEY_WARNING_LIMIT>" + $("#OBEY_WARNING_LIMIT").val()
			+ "</OBEY_WARNING_LIMIT>" + "<SSR_START_TIME_OPR>"
			+ $("#SSR_START_TIME_OPR").val() + "</SSR_START_TIME_OPR>"
			+ "<SSR_END_TIME_OPR>" + $("#SSR_END_TIME_OPR").val()
			+ "</SSR_END_TIME_OPR>" + "<SSR_UNITS_MIN_OPR>"
			+ $("#SSR_UNITS_MIN_OPR").val() + "</SSR_UNITS_MIN_OPR>"
			+ "<SSR_UNITS_MAX_OPR>" + $("#SSR_UNITS_MAX_OPR").val()
			+ "</SSR_UNITS_MAX_OPR>" + "<START_DT>" + $("#START_DT").val()
			+ "</START_DT>" + "<END_DT>" + $("#END_DT").val() + "</END_DT>"
			+ "</CLASS_SEARCH_REQUEST>";
	return xml;
}

function getClassDetails(clssNbr, target, mapDiv) {

	type = "cu_detail";
	var originalClassNumber = $("#CLASS_NBR").val();
	$("#CLASS_NBR").val(clssNbr);

	var self = this;
	self.serviceURI = $("#connection_uri").text() + "?type=" + type;
	console.log("URL: " + self.serviceURI);
	self.username = $("#uid").val();
	if (self.username == "") {
		displayMsg(alertLevel.warn, "Username is required");
		spinner.stop();
		return;
	}
	self.password = $("#pwd").val();
	if (self.password == "") {
		displayMsg(alertLevel.warn, "Password is required");
		spinner.stop();
		return;
	}

	var hash = btoa(self.username + ":" + self.password);
	var requestXml = getRequestXml();

	self.ajax = function(uri, method, request_data) {
		var request = {
			url : uri,
			type : method,
			contentType : "application/xml",
			accepts : "application/xml",
			cache : false,
			data : request_data,
			dataType : "text",
			xhrFields : {
				'withCredentials' : true
			},
			headers : {
				'Access-Control-Allow-Origin' : "*",
				'Access-Control-Allow-Methods' : "GET, POST, PUT, DELETE, OPTIONS",
				'Access-Control-Allow-Headers' : "Authorization",
				'Authorization' : "Basic " + hash
			},
			beforeSend : function(xhr) {
				console.log("=====> URI:  " + uri);
				console.log("Authorization: Basic " + hash);
				xhr.setRequestHeader("Authorization", "Basic " + hash);
			},
			error : function(jqXHR) {
				console.log("ajax error " + jqXHR.statusText);
				console.log("ajax error " + jqXHR.status);
				switch (jqXHR.status) {
				case 404:
					var msg = "<strong>404 "
							+ jqXHR.statusText
							+ " -</strong> The service address you are using may be incorrect."
					displayMsg(alertLevel.error, msg);
					break;
				case 500:
					var msg = "<strong>500 "
							+ jqXHR.statusText
							+ " -</strong> This can have multiple causes such as"
							+ " invalid credentials, improper search parameters"
							+ ", or a database error.  Contact UIS if you cannot resolve "
							+ "the issue.<p>" + jqXHR.responseText;
					displayMsg(alertLevel.error, msg);
					break;
				}
				spinner.stop();
			}
		};
		return $.ajax(request);
	}

	var content = "";

	self
			.ajax(self.serviceURI, 'POST', requestXml)
			.done(
					function(data) {
						xml_neat = formatXml(data);
						console.log(xml_neat);
						var prefix = "ser\\:";

						// set namespace
						if (typeof $(data).find("SUBJECTS").html() !== "undefined") {
							console.log("Found response without namespace.");
							prefix = "";
						} else if (typeof $(data).find(prefix + "SUBJECTS")
								.html() !== "undefined") {
							console.log("Found response with namespace.");
							prefix = "ser\\:";
						} else {
							console.log("no subjects found. exiting");
							return;
						}
						console.log("getting data...");
						var lat;
						var long;
						var locationName;
						$(data)
								.find(prefix + "SUBJECTS > " + prefix + "SUBJECT")
								.each(
										function() {
											console.log("reading content");
											var count = 0;
											$(this).find(prefix + 'CLASSES_SUMMARY > ' + prefix + 'CLASS_SUMMARY')
													.each(
															function() {
																count++;
																console.log("found summary");
																lat=$(this).find(prefix+ 'SCC_LATITUDE:first').text();
																long=$(this).find(prefix+ 'SCC_LONGITUDE:first').text();
																locationName=$(this).find(prefix+ 'SSR_MTG_LOC_LONG:first').text();
																content += '<p>'+ $(this).find(prefix+ 'CRSE_ID_LOVDescr:first').text();
																content += '<p>'+ $(this).find(prefix+ 'SSR_DESCRLONG:first').text();
																content += '<p>Enrollment Capacity: '+ $(this).find(prefix+ 'ENRL_CAP:first').text();
																content += '<p>Teacher: '+ $(this).find(prefix+ 'NAME_DISPLAY:first').text();
																console.log(content);
																classDetails=content;
															});
										});
					
						console.log(classDetails);					
						console.log("reseting class number");
						target.html(classDetails);
						var location = new google.maps.LatLng(lat, long);
						var map = new google.maps.Map(mapDiv, {
						    center: location,
						    scrollwheel: false,
						    zoom: 17
						  });


						  var marker = new google.maps.Marker({
						    position: location,
						    map: map,
						    title: locationName
						  });
						  var infowindow = new google.maps.InfoWindow({
					           content: locationName
					         });
						  infowindow.open(map,marker);
						// reset form
						$("#CLASS_NBR").val(originalClassNumber);
					});

}
