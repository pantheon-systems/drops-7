var moment = require('../../moment');


    /**************************************************
      Belarusian
     *************************************************/

exports['locale:be'] = {
    setUp : function (cb) {
        moment.locale('be');
        moment.createFromInputFallback = function () {
            throw new Error('input not handled by moment');
        };
        cb();
    },

    tearDown : function (cb) {
        moment.locale('en');
        cb();
    },

    'parse' : function (test) {
        var tests = 'студзень студ_люты лют_сакавік сак_красавік крас_травень трав_чэрвень чэрв_ліпень ліп_жнівень жнів_верасень вер_кастрычнік каст_лістапад ліст_снежань снеж'.split('_'), i;
        function equalTest(input, mmm, i) {
            test.equal(moment(input, mmm).month(), i, input + ' should be month ' + (i + 1));
        }
        for (i = 0; i < 12; i++) {
            tests[i] = tests[i].split(' ');
            equalTest(tests[i][0], 'MMM', i);
            equalTest(tests[i][1], 'MMM', i);
            equalTest(tests[i][0], 'MMMM', i);
            equalTest(tests[i][1], 'MMMM', i);
            equalTest(tests[i][0].toLocaleLowerCase(), 'MMMM', i);
            equalTest(tests[i][1].toLocaleLowerCase(), 'MMMM', i);
            equalTest(tests[i][0].toLocaleUpperCase(), 'MMMM', i);
            equalTest(tests[i][1].toLocaleUpperCase(), 'MMMM', i);
        }
        test.done();
    },

    'format' : function (test) {
        var a = [
                ['dddd, Do MMMM YYYY, HH:mm:ss',       'нядзеля, 14-га лютага 2010, 15:25:50'],
                ['ddd, h A',                           'нд, 3 дня'],
                ['M Mo MM MMMM MMM',                   '2 2-і 02 люты лют'],
                ['YYYY YY',                            '2010 10'],
                ['D Do DD',                            '14 14-га 14'],
                ['d do dddd ddd dd',                   '0 0-ы нядзеля нд нд'],
                ['DDD DDDo DDDD',                      '45 45-ы 045'],
                ['w wo ww',                            '7 7-ы 07'],
                ['h hh',                               '3 03'],
                ['H HH',                               '15 15'],
                ['m mm',                               '25 25'],
                ['s ss',                               '50 50'],
                ['a A',                                'дня дня'],
                ['DDDo [дзень года]',                   '45-ы дзень года'],
                ['LT',                                 '15:25'],
                ['LTS',                                '15:25:50'],
                ['L',                                  '14.02.2010'],
                ['LL',                                 '14 лютага 2010 г.'],
                ['LLL',                                '14 лютага 2010 г., 15:25'],
                ['LLLL',                               'нядзеля, 14 лютага 2010 г., 15:25'],
                ['l',                                  '14.2.2010'],
                ['ll',                                 '14 лют 2010 г.'],
                ['lll',                                '14 лют 2010 г., 15:25'],
                ['llll',                               'нд, 14 лют 2010 г., 15:25']
            ],
            b = moment(new Date(2010, 1, 14, 15, 25, 50, 125)),
            i;
        for (i = 0; i < a.length; i++) {
            test.equal(b.format(a[i][0]), a[i][1], a[i][0] + ' ---> ' + a[i][1]);
        }
        test.done();
    },

    'format meridiem' : function (test) {
        test.equal(moment([2012, 11, 28, 0, 0]).format('A'), 'ночы', 'night');
        test.equal(moment([2012, 11, 28, 3, 59]).format('A'), 'ночы', 'night');
        test.equal(moment([2012, 11, 28, 4, 0]).format('A'), 'раніцы', 'morning');
        test.equal(moment([2012, 11, 28, 11, 59]).format('A'), 'раніцы', 'morning');
        test.equal(moment([2012, 11, 28, 12, 0]).format('A'), 'дня', 'afternoon');
        test.equal(moment([2012, 11, 28, 16, 59]).format('A'), 'дня', 'afternoon');
        test.equal(moment([2012, 11, 28, 17, 0]).format('A'), 'вечара', 'evening');
        test.equal(moment([2012, 11, 28, 23, 59]).format('A'), 'вечара', 'evening');

        test.done();
    },

    'format ordinal' : function (test) {
        test.equal(moment([2011, 0, 1]).format('DDDo'), '1-ы', '1-ы');
        test.equal(moment([2011, 0, 2]).format('DDDo'), '2-і', '2-і');
        test.equal(moment([2011, 0, 3]).format('DDDo'), '3-і', '3-і');
        test.equal(moment([2011, 0, 4]).format('DDDo'), '4-ы', '4-ы');
        test.equal(moment([2011, 0, 5]).format('DDDo'), '5-ы', '5-ы');
        test.equal(moment([2011, 0, 6]).format('DDDo'), '6-ы', '6-ы');
        test.equal(moment([2011, 0, 7]).format('DDDo'), '7-ы', '7-ы');
        test.equal(moment([2011, 0, 8]).format('DDDo'), '8-ы', '8-ы');
        test.equal(moment([2011, 0, 9]).format('DDDo'), '9-ы', '9-ы');
        test.equal(moment([2011, 0, 10]).format('DDDo'), '10-ы', '10-ы');

        test.equal(moment([2011, 0, 11]).format('DDDo'), '11-ы', '11-ы');
        test.equal(moment([2011, 0, 12]).format('DDDo'), '12-ы', '12-ы');
        test.equal(moment([2011, 0, 13]).format('DDDo'), '13-ы', '13-ы');
        test.equal(moment([2011, 0, 14]).format('DDDo'), '14-ы', '14-ы');
        test.equal(moment([2011, 0, 15]).format('DDDo'), '15-ы', '15-ы');
        test.equal(moment([2011, 0, 16]).format('DDDo'), '16-ы', '16-ы');
        test.equal(moment([2011, 0, 17]).format('DDDo'), '17-ы', '17-ы');
        test.equal(moment([2011, 0, 18]).format('DDDo'), '18-ы', '18-ы');
        test.equal(moment([2011, 0, 19]).format('DDDo'), '19-ы', '19-ы');
        test.equal(moment([2011, 0, 20]).format('DDDo'), '20-ы', '20-ы');

        test.equal(moment([2011, 0, 21]).format('DDDo'), '21-ы', '21-ы');
        test.equal(moment([2011, 0, 22]).format('DDDo'), '22-і', '22-і');
        test.equal(moment([2011, 0, 23]).format('DDDo'), '23-і', '23-і');
        test.equal(moment([2011, 0, 24]).format('DDDo'), '24-ы', '24-ы');
        test.equal(moment([2011, 0, 25]).format('DDDo'), '25-ы', '25-ы');
        test.equal(moment([2011, 0, 26]).format('DDDo'), '26-ы', '26-ы');
        test.equal(moment([2011, 0, 27]).format('DDDo'), '27-ы', '27-ы');
        test.equal(moment([2011, 0, 28]).format('DDDo'), '28-ы', '28-ы');
        test.equal(moment([2011, 0, 29]).format('DDDo'), '29-ы', '29-ы');
        test.equal(moment([2011, 0, 30]).format('DDDo'), '30-ы', '30-ы');

        test.equal(moment([2011, 0, 31]).format('DDDo'), '31-ы', '31-ы');
        test.done();
    },

    'format month' : function (test) {
        var expected = 'студзень студ_люты лют_сакавік сак_красавік крас_травень трав_чэрвень чэрв_ліпень ліп_жнівень жнів_верасень вер_кастрычнік каст_лістапад ліст_снежань снеж'.split('_'), i;
        for (i = 0; i < expected.length; i++) {
            test.equal(moment([2011, i, 1]).format('MMMM MMM'), expected[i], expected[i]);
        }
        test.done();
    },

    'format month case' : function (test) {
        var months = {
            'nominative': 'студзень_люты_сакавік_красавік_травень_чэрвень_ліпень_жнівень_верасень_кастрычнік_лістапад_снежань'.split('_'),
            'accusative': 'студзеня_лютага_сакавіка_красавіка_траўня_чэрвеня_ліпеня_жніўня_верасня_кастрычніка_лістапада_снежня'.split('_')
        }, i;
        for (i = 0; i < 12; i++) {
            test.equal(moment([2011, i, 1]).format('D MMMM'), '1 ' + months.accusative[i], '1 ' + months.accusative[i]);
            test.equal(moment([2011, i, 1]).format('MMMM'), months.nominative[i], '1 ' + months.nominative[i]);
        }
        test.done();
    },

    'format month case with escaped symbols' : function (test) {
        var months = {
            'nominative': 'студзень_люты_сакавік_красавік_травень_чэрвень_ліпень_жнівень_верасень_кастрычнік_лістапад_снежань'.split('_'),
            'accusative': 'студзеня_лютага_сакавіка_красавіка_траўня_чэрвеня_ліпеня_жніўня_верасня_кастрычніка_лістапада_снежня'.split('_')
        }, i;
        for (i = 0; i < 12; i++) {
            test.equal(moment([2013, i, 1]).format('D[] MMMM'), '1 ' + months.accusative[i], '1 ' + months.accusative[i]);
            test.equal(moment([2013, i, 1]).format('[<i>]D[</i>] [<b>]MMMM[</b>]'), '<i>1</i> <b>' + months.accusative[i] + '</b>', '1 <b>' + months.accusative[i] + '</b>');
            test.equal(moment([2013, i, 1]).format('D[-ы дзень] MMMM'), '1-ы дзень ' + months.accusative[i], '1-ы дзень ' + months.accusative[i]);
            test.equal(moment([2013, i, 1]).format('D, MMMM'), '1, ' + months.nominative[i], '1, ' + months.nominative[i]);
        }
        test.done();
    },

    'format week' : function (test) {
        var expected = 'нядзеля нд нд_панядзелак пн пн_аўторак ат ат_серада ср ср_чацвер чц чц_пятніца пт пт_субота сб сб'.split('_'), i;
        for (i = 0; i < expected.length; i++) {
            test.equal(moment([2011, 0, 2 + i]).format('dddd ddd dd'), expected[i], expected[i]);
        }
        test.done();
    },

    'from' : function (test) {
        var start = moment([2007, 1, 28]);
        test.equal(start.from(moment([2007, 1, 28]).add({s: 44}), true),  'некалькі секунд',    '44 seconds = a few seconds');
        test.equal(start.from(moment([2007, 1, 28]).add({s: 45}), true),  'хвіліна',   '45 seconds = a minute');
        test.equal(start.from(moment([2007, 1, 28]).add({s: 89}), true),  'хвіліна',   '89 seconds = a minute');
        test.equal(start.from(moment([2007, 1, 28]).add({s: 90}), true),  '2 хвіліны',  '90 seconds = 2 minutes');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 31}), true),  '31 хвіліна',  '31 minutes = 31 minutes');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 44}), true),  '44 хвіліны', '44 minutes = 44 minutes');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 45}), true),  'гадзіна',    '45 minutes = an hour');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 89}), true),  'гадзіна',    '89 minutes = an hour');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 90}), true),  '2 гадзіны',    '90 minutes = 2 hours');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 5}), true),   '5 гадзін',    '5 hours = 5 hours');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 21}), true),  '21 гадзіна',   '21 hours = 21 hours');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 22}), true),  'дзень',      '22 hours = a day');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 35}), true),  'дзень',      '35 hours = a day');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 36}), true),  '2 дні',     '36 hours = 2 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 1}), true),   'дзень',      '1 day = a day');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 5}), true),   '5 дзён',     '5 days = 5 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 11}), true),  '11 дзён',     '11 days = 11 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 21}), true),  '21 дзень',     '21 days = 21 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 25}), true),  '25 дзён',    '25 days = 25 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 26}), true),  'месяц',    '26 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 30}), true),  'месяц',    '30 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 43}), true),  'месяц',    '43 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 46}), true),  '2 месяцы',   '46 days = 2 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 74}), true),  '2 месяцы',   '75 days = 2 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 76}), true),  '3 месяцы',   '76 days = 3 months');
        test.equal(start.from(moment([2007, 1, 28]).add({M: 1}), true),   'месяц',    '1 month = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({M: 5}), true),   '5 месяцаў',   '5 months = 5 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 345}), true), 'год',     '345 days = a year');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 548}), true), '2 гады',    '548 days = 2 years');
        test.equal(start.from(moment([2007, 1, 28]).add({y: 1}), true),   'год',     '1 year = a year');
        test.equal(start.from(moment([2007, 1, 28]).add({y: 5}), true),   '5 гадоў',    '5 years = 5 years');
        test.done();
    },

    'suffix' : function (test) {
        test.equal(moment(30000).from(0), 'праз некалькі секунд', 'prefix');
        test.equal(moment(0).from(30000), 'некалькі секунд таму', 'suffix');
        test.done();
    },

    'fromNow' : function (test) {
        test.equal(moment().add({s: 30}).fromNow(), 'праз некалькі секунд', 'in a few seconds');
        test.equal(moment().add({d: 5}).fromNow(), 'праз 5 дзён', 'in 5 days');
        test.equal(moment().add({m: 31}).fromNow(), 'праз 31 хвіліну', 'in 31 minutes = in 31 minutes');
        test.equal(moment().subtract({m: 31}).fromNow(), '31 хвіліну таму', '31 minutes ago = 31 minutes ago');
        test.done();
    },

    'calendar day' : function (test) {
        var a = moment().hours(2).minutes(0).seconds(0);

        test.equal(moment(a).calendar(),                     'Сёння ў 02:00',     'today at the same time');
        test.equal(moment(a).add({m: 25}).calendar(),      'Сёння ў 02:25',     'Now plus 25 min');
        test.equal(moment(a).add({h: 1}).calendar(),       'Сёння ў 03:00',     'Now plus 1 hour');
        test.equal(moment(a).add({d: 1}).calendar(),       'Заўтра ў 02:00',      'tomorrow at the same time');
        test.equal(moment(a).subtract({h: 1}).calendar(),  'Сёння ў 01:00',     'Now minus 1 hour');
        test.equal(moment(a).subtract({d: 1}).calendar(),  'Учора ў 02:00',       'yesterday at the same time');
        test.done();
    },

    'calendar next week' : function (test) {
        var i, m;
        function makeFormat(d) {
            return '[У] dddd [ў] LT';
        }

        for (i = 2; i < 7; i++) {
            m = moment().add({d: i});
            test.equal(m.calendar(),       m.format(makeFormat(m)),  'Today + ' + i + ' days current time');
            m.hours(0).minutes(0).seconds(0).milliseconds(0);
            test.equal(m.calendar(),       m.format(makeFormat(m)),  'Today + ' + i + ' days beginning of day');
            m.hours(23).minutes(59).seconds(59).milliseconds(999);
            test.equal(m.calendar(),       m.format(makeFormat(m)),  'Today + ' + i + ' days end of day');
        }
        test.done();
    },

    'calendar last week' : function (test) {
        var i, m;

        function makeFormat(d) {
            switch (d.day()) {
            case 0:
            case 3:
            case 5:
            case 6:
                return '[У мінулую] dddd [ў] LT';
            case 1:
            case 2:
            case 4:
                return '[У мінулы] dddd [ў] LT';
            }
        }

        for (i = 2; i < 7; i++) {
            m = moment().subtract({d: i});
            test.equal(m.calendar(),       m.format(makeFormat(m)),  'Today - ' + i + ' days current time');
            m.hours(0).minutes(0).seconds(0).milliseconds(0);
            test.equal(m.calendar(),       m.format(makeFormat(m)),  'Today - ' + i + ' days beginning of day');
            m.hours(23).minutes(59).seconds(59).milliseconds(999);
            test.equal(m.calendar(),       m.format(makeFormat(m)),  'Today - ' + i + ' days end of day');
        }
        test.done();
    },

    'calendar all else' : function (test) {
        var weeksAgo = moment().subtract({w: 1}),
            weeksFromNow = moment().add({w: 1});

        test.equal(weeksAgo.calendar(),       weeksAgo.format('L'),  '1 week ago');
        test.equal(weeksFromNow.calendar(),   weeksFromNow.format('L'),  'in 1 week');

        weeksAgo = moment().subtract({w: 2});
        weeksFromNow = moment().add({w: 2});

        test.equal(weeksAgo.calendar(),       weeksAgo.format('L'),  '2 weeks ago');
        test.equal(weeksFromNow.calendar(),   weeksFromNow.format('L'),  'in 2 weeks');

        test.done();
    },

    // Monday is the first day of the week.
    // The week that contains Jan 1st is the first week of the year.

    'weeks year starting sunday' : function (test) {
        test.equal(moment([2011, 11, 26]).week(), 1, 'Dec 26 2011 should be week 1');
        test.equal(moment([2012,  0,  1]).week(), 1, 'Jan  1 2012 should be week 1');
        test.equal(moment([2012,  0,  2]).week(), 2, 'Jan  2 2012 should be week 2');
        test.equal(moment([2012,  0,  8]).week(), 2, 'Jan  8 2012 should be week 2');
        test.equal(moment([2012,  0,  9]).week(), 3, 'Jan  9 2012 should be week 3');

        test.done();
    },

    'weeks year starting monday' : function (test) {
        test.equal(moment([2007, 0, 1]).week(),  1, 'Jan  1 2007 should be week 1');
        test.equal(moment([2007, 0, 7]).week(),  1, 'Jan  7 2007 should be week 1');
        test.equal(moment([2007, 0, 8]).week(),  2, 'Jan  8 2007 should be week 2');
        test.equal(moment([2007, 0, 14]).week(), 2, 'Jan 14 2007 should be week 2');
        test.equal(moment([2007, 0, 15]).week(), 3, 'Jan 15 2007 should be week 3');

        test.done();
    },

    'weeks year starting tuesday' : function (test) {
        test.equal(moment([2007, 11, 31]).week(), 1, 'Dec 31 2007 should be week 1');
        test.equal(moment([2008,  0,  1]).week(), 1, 'Jan  1 2008 should be week 1');
        test.equal(moment([2008,  0,  6]).week(), 1, 'Jan  6 2008 should be week 1');
        test.equal(moment([2008,  0,  7]).week(), 2, 'Jan  7 2008 should be week 2');
        test.equal(moment([2008,  0, 13]).week(), 2, 'Jan 13 2008 should be week 2');
        test.equal(moment([2008,  0, 14]).week(), 3, 'Jan 14 2008 should be week 3');

        test.done();
    },

    'weeks year starting wednesday' : function (test) {
        test.equal(moment([2002, 11, 30]).week(), 1, 'Dec 30 2002 should be week 1');
        test.equal(moment([2003,  0,  1]).week(), 1, 'Jan  1 2003 should be week 1');
        test.equal(moment([2003,  0,  5]).week(), 1, 'Jan  5 2003 should be week 1');
        test.equal(moment([2003,  0,  6]).week(), 2, 'Jan  6 2003 should be week 2');
        test.equal(moment([2003,  0, 12]).week(), 2, 'Jan 12 2003 should be week 2');
        test.equal(moment([2003,  0, 13]).week(), 3, 'Jan 13 2003 should be week 3');

        test.done();
    },

    'weeks year starting thursday' : function (test) {
        test.equal(moment([2008, 11, 29]).week(), 1, 'Dec 29 2008 should be week 1');
        test.equal(moment([2009,  0,  1]).week(), 1, 'Jan  1 2009 should be week 1');
        test.equal(moment([2009,  0,  4]).week(), 1, 'Jan  4 2009 should be week 1');
        test.equal(moment([2009,  0,  5]).week(), 2, 'Jan  5 2009 should be week 2');
        test.equal(moment([2009,  0, 11]).week(), 2, 'Jan 11 2009 should be week 2');
        test.equal(moment([2009,  0, 12]).week(), 3, 'Jan 12 2009 should be week 3');

        test.done();
    },

    'weeks year starting friday' : function (test) {
        test.equal(moment([2009, 11, 28]).week(), 1, 'Dec 28 2009 should be week 1');
        test.equal(moment([2010,  0,  1]).week(), 1, 'Jan  1 2010 should be week 1');
        test.equal(moment([2010,  0,  3]).week(), 1, 'Jan  3 2010 should be week 1');
        test.equal(moment([2010,  0,  4]).week(), 2, 'Jan  4 2010 should be week 2');
        test.equal(moment([2010,  0, 10]).week(), 2, 'Jan 10 2010 should be week 2');
        test.equal(moment([2010,  0, 11]).week(), 3, 'Jan 11 2010 should be week 3');

        test.done();
    },

    'weeks year starting saturday' : function (test) {
        test.equal(moment([2010, 11, 27]).week(), 1, 'Dec 27 2010 should be week 1');
        test.equal(moment([2011,  0,  1]).week(), 1, 'Jan  1 2011 should be week 1');
        test.equal(moment([2011,  0,  2]).week(), 1, 'Jan  2 2011 should be week 1');
        test.equal(moment([2011,  0,  3]).week(), 2, 'Jan  3 2011 should be week 2');
        test.equal(moment([2011,  0,  9]).week(), 2, 'Jan  9 2011 should be week 2');
        test.equal(moment([2011,  0, 10]).week(), 3, 'Jan 10 2011 should be week 3');

        test.done();
    },

    'weeks year starting sunday formatted' : function (test) {
        test.equal(moment([2011, 11, 26]).format('w ww wo'), '1 01 1-ы', 'Dec 26 2011 should be week 1');
        test.equal(moment([2012,  0,  1]).format('w ww wo'), '1 01 1-ы', 'Jan  1 2012 should be week 1');
        test.equal(moment([2012,  0,  2]).format('w ww wo'), '2 02 2-і', 'Jan  2 2012 should be week 2');
        test.equal(moment([2012,  0,  8]).format('w ww wo'), '2 02 2-і', 'Jan  8 2012 should be week 2');
        test.equal(moment([2012,  0,  9]).format('w ww wo'), '3 03 3-і', 'Jan  9 2012 should be week 3');

        test.done();
    },

    'lenient ordinal parsing' : function (test) {
        var i, ordinalStr, testMoment;
        for (i = 1; i <= 31; ++i) {
            ordinalStr = moment([2014, 0, i]).format('YYYY MM Do');
            testMoment = moment(ordinalStr, 'YYYY MM Do');
            test.equal(testMoment.year(), 2014,
                    'lenient ordinal parsing ' + i + ' year check');
            test.equal(testMoment.month(), 0,
                    'lenient ordinal parsing ' + i + ' month check');
            test.equal(testMoment.date(), i,
                    'lenient ordinal parsing ' + i + ' date check');
        }
        test.done();
    },

    'lenient ordinal parsing of number' : function (test) {
        var i, testMoment;
        for (i = 1; i <= 31; ++i) {
            testMoment = moment('2014 01 ' + i, 'YYYY MM Do');
            test.equal(testMoment.year(), 2014,
                    'lenient ordinal parsing of number ' + i + ' year check');
            test.equal(testMoment.month(), 0,
                    'lenient ordinal parsing of number ' + i + ' month check');
            test.equal(testMoment.date(), i,
                    'lenient ordinal parsing of number ' + i + ' date check');
        }
        test.done();
    },

    'strict ordinal parsing' : function (test) {
        var i, ordinalStr, testMoment;
        for (i = 1; i <= 31; ++i) {
            ordinalStr = moment([2014, 0, i]).format('YYYY MM Do');
            testMoment = moment(ordinalStr, 'YYYY MM Do', true);
            test.ok(testMoment.isValid(), 'strict ordinal parsing ' + i);
        }
        test.done();
    }

};
