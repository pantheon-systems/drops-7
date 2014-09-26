// +--------------------------------------------------------------------+
// | CiviCRM version 4.2                                                |
// +--------------------------------------------------------------------+
// | Copyright CiviCRM LLC (c) 2004-2012                                |
// +--------------------------------------------------------------------+
// | This file is a part of CiviCRM.                                    |
// |                                                                    |
// | CiviCRM is free software; you can copy, modify, and distribute it  |
// | under the terms of the GNU Affero General Public License           |
// | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
// |                                                                    |
// | CiviCRM is distributed in the hope that it will be useful, but     |
// | WITHOUT ANY WARRANTY; without even the implied warranty of         |
// | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
// | See the GNU Affero General Public License for more details.        |
// |                                                                    |
// | You should have received a copy of the GNU Affero General Public   |
// | License and the CiviCRM Licensing Exception along                  |
// | with this program; if not, contact CiviCRM LLC                     |
// | at info[AT]civicrm[DOT]org. If you have questions about the        |
// | GNU Affero General Public License or the licensing of CiviCRM,     |
// | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
// +--------------------------------------------------------------------+
(function($){ $.fn.toolTip = function(){
  var clickedElement = null;
  return this.each(function() {
    var text = $(this).children().find('div.crm-help').html();
    if(text != undefined) {
      $(this).bind( 'click', function(e){
		$("#crm-toolTip").remove();
		if ( clickedElement == $(this).children().attr('id') ) { clickedElement = null; return; }
		 $("body").append('<div id="crm-toolTip" style="z-index: 100;"><div id="hide-tooltip" class="ui-icon ui-icon-close"></div>' + text + "</div>");
		  if ($.browser.msie && $.browser.version.substr(0,1)<7) {
		  	$("#crm-toolTip").css('position','absolute');
			$(window).bind('scroll', function() {
				var windowheight = $(window).height();
				var toolTipBottom = $(window).scrollTop() + 30;
				var posFromTop = windowheight+toolTipBottom;
				$("#crm-toolTip").css("top", toolTipBottom + "px");
				});
			};
		
		  $("#crm-toolTip").fadeIn("medium");
		  clickedElement = cj(this).children().attr('id');
	      })
	      .bind( 'mouseout', function() {
			$('#hide-tooltip').click( function() {
			  $("#crm-toolTip").hide();
			});
	     });
    	}
  	});
}})(jQuery);

