/*

jquery.msgpanel.js - Slide-down message panel plugin

Methods:

$('selector').msgpanel(options) - configure a div element to act as a message panel
$('selector').data('msgpanel').showPanel('text') - show the panel
$('selector').data('msgpanel').hidePanel() - hide the panel

*/

;(function($) {
    var opts,
        zindex,
        zindexDefault = 2000,
        nshowing = 0;

    zindex = zindexDefault;

    var defaults = {
        speed : 'normal',
        panelClass : 'msgpanel-info-msg',
        widthPercent : '95%',
        panelOpacity : 1
    };

    $.fn.msgpanel = function (options) {
        opts = $.extend({}, defaults, options);
        return this.each(function(){
            $this = $(this);
            $this.addClass('ui-corner-bottom msgpanel ' + opts.panelClass);
            $this.css({
                top: '20px',
                width : opts.widthPercent
            });
            if (opts.marginLeftPercent) {
                $this.css({
                    'margin-left' : opts.marginLeftPercent
                });
            }
            var m = $this.find('.msgpanelmsg');
            if (0 === m.length) {
                $this.attr('title', 'Click X to close');
                $this.append('<div class="msgpanelmsg"></div><div class="msgpanelclsbtn ui-corner-all" style="background: white; float: right; margin: .3em"><span class="ui-icon ui-icon-closethick" style="" /></div>');
                $this.data('msgpanel', new MsgPanel(this, opts));
                var mpanel = $this.data('msgpanel');
                var btn = $this.find('.msgpanelclsbtn');
                btn.bind('click', function(){mpanel.hidePanel();});
            }
        });
    };

    function calcHeight (el) {
        var $el = $(el);
        return $el.height() +
                parseInt($el.css('paddingTop')) +
                parseInt($el.css('paddingBottom'));
    }

    function MsgPanel (el, options) {
        var $self = this, showing = false;

        $self.showPanel = function (text, callopts) {
            var $el = $(el);
            $el.find('.msgpanelmsg').html(text);
            if (showing) return;
            var opts = $.extend({}, options, callopts);
            $el.css( 'marginTop', -( calcHeight(el) ) );
            // ensure that this panel stacks above other panels
            $el.css('z-index', ++zindex);
            $el.animate({ marginTop : 0, opacity : opts.panelOpacity }, opts.speed);
            showing = true;
            ++nshowing;
            window.setTimeout($self.hidePanel,3000);
        };

        $self.hidePanel = function () {
            if (!showing) return;
            var h, $el = $(el);
            h = calcHeight(el);
            $el.animate({ marginTop : -h, opacity : 0 }, options.speed);
            showing = false;
            --nshowing;
            if (0 === nshowing) zindex = zindexDefault;
        };
    }
})(jQuery);
