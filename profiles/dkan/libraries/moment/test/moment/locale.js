var moment = require('../../moment');

exports.locale = {
    setUp : function (done) {
        moment.createFromInputFallback = function () {
            throw new Error('input not handled by moment');
        };
        moment.locale('en');
        done();
    },

    'library getters and setters' : function (test) {
        var r = moment.locale('en');

        test.equal(r, 'en', 'locale should return en by default');
        test.equal(moment.locale(), 'en', 'locale should return en by default');

        moment.locale('fr');
        test.equal(moment.locale(), 'fr', 'locale should return the changed locale');

        moment.locale('en-gb');
        test.equal(moment.locale(), 'en-gb', 'locale should return the changed locale');

        moment.locale('en');
        test.equal(moment.locale(), 'en', 'locale should reset');

        moment.locale('does-not-exist');
        test.equal(moment.locale(), 'en', 'locale should reset');

        moment.locale('EN');
        test.equal(moment.locale(), 'en', 'Normalize locale key case');

        moment.locale('EN_gb');
        test.equal(moment.locale(), 'en-gb', 'Normalize locale key underscore');

        test.done();
    },

    'library setter array of locales' : function (test) {
        test.equal(moment.locale(['non-existent', 'fr', 'also-non-existent']), 'fr', 'passing an array uses the first valid locale');
        test.equal(moment.locale(['es', 'fr', 'also-non-existent']), 'es', 'passing an array uses the first valid locale');
        test.done();
    },

    'library setter locale substrings' : function (test) {
        test.equal(moment.locale('fr-crap'), 'fr', 'use substrings');
        test.equal(moment.locale('fr-does-not-exist'), 'fr', 'uses deep substrings');
        test.equal(moment.locale('fr-CA-does-not-exist'), 'fr-ca', 'uses deepest substring');
        test.done();
    },

    'library getter locale array and substrings' : function (test) {
        test.equals(moment.locale(['en-CH', 'fr']), 'en', 'prefer root locale to shallower ones');
        test.equals(moment.locale(['en-gb-leeds', 'en-CA']), 'en-gb', 'prefer root locale to shallower ones');
        test.equals(moment.locale(['en-fake', 'en-CA']), 'en-ca', 'prefer alternatives with shared roots');
        test.equals(moment.locale(['en-fake', 'en-fake2', 'en-ca']), 'en-ca', 'prefer alternatives with shared roots');
        test.equals(moment.locale(['fake-CA', 'fake-MX', 'fr']), 'fr', 'always find something if possible');
        test.equals(moment.locale(['fake-CA', 'fake-MX', 'fr']), 'fr', 'always find something if possible');
        test.equals(moment.locale(['fake-CA', 'fake-MX', 'fr-fake-fake-fake']), 'fr', 'always find something if possible');
        test.equals(moment.locale(['en', 'en-CA']), 'en', 'prefer earlier if it works');
        test.done();
    },

    'library ensure inheritance' : function (test) {
        moment.locale('made-up', {
            // I put them out of order
            months : 'February_March_April_May_June_July_August_September_October_November_December_January'.split('_')
            // the rest of the properties should be inherited.
        });

        test.equal(moment([2012, 5, 6]).format('MMMM'), 'July', 'Override some of the configs');
        test.equal(moment([2012, 5, 6]).format('MMM'), 'Jun', 'But not all of them');

        test.done();
    },

    'library ensure inheritance LT L LL LLL LLLL' : function (test) {
        var locale = 'test-inherit-lt';

        moment.defineLocale(locale, {
            longDateFormat : {
                LT : '-[LT]-',
                L : '-[L]-',
                LL : '-[LL]-',
                LLL : '-[LLL]-',
                LLLL : '-[LLLL]-'
            },
            calendar : {
                sameDay : '[sameDay] LT',
                nextDay : '[nextDay] L',
                nextWeek : '[nextWeek] LL',
                lastDay : '[lastDay] LLL',
                lastWeek : '[lastWeek] LLLL',
                sameElse : 'L'
            }
        });

        moment.locale('es');

        test.equal(moment().locale(locale).calendar(), 'sameDay -LT-', 'Should use instance locale in LT formatting');
        test.equal(moment().add(1, 'days').locale(locale).calendar(), 'nextDay -L-', 'Should use instance locale in L formatting');
        test.equal(moment().add(-1, 'days').locale(locale).calendar(), 'lastDay -LLL-', 'Should use instance locale in LL formatting');
        test.equal(moment().add(4, 'days').locale(locale).calendar(), 'nextWeek -LL-', 'Should use instance locale in LLL formatting');
        test.equal(moment().add(-4, 'days').locale(locale).calendar(), 'lastWeek -LLLL-', 'Should use instance locale in LLLL formatting');

        test.done();
    },

    'library localeData' : function (test) {
        test.expect(3);
        moment.locale('en');

        var jan = moment([2000, 0]);

        test.equal(moment.localeData().months(jan), 'January', 'no arguments returns global');
        test.equal(moment.localeData('zh-cn').months(jan), '一月', 'a string returns the locale based on key');
        test.equal(moment.localeData(moment().locale('es')).months(jan), 'enero', 'if you pass in a moment it uses the moment\'s locale');

        test.done();
    },

    'library deprecations' : function (test) {
        moment.lang('dude', {months: ['Movember']});
        test.equal(moment.locale(), 'dude', 'setting the lang sets the locale');
        test.equal(moment.lang(), moment.locale());
        test.equal(moment.langData(), moment.localeData(), 'langData is localeData');
        test.done();
    },

    'defineLocale' : function (test) {
        moment.locale('en');
        moment.defineLocale('dude', {months: ['Movember']});
        test.equal(moment().locale(), 'dude', 'defineLocale also sets it');
        test.equal(moment().locale('dude').locale(), 'dude', 'defineLocale defines a locale');
        test.done();
    },

    'library convenience' : function (test) {
        moment.locale('something', {week: {dow: 3}});
        moment.locale('something');
        test.equal(moment.locale(), 'something', 'locale can be used to create the locale too');
        test.done();
    },

    'firstDayOfWeek firstDayOfYear locale getters' : function (test) {
        moment.locale('something', {week: {dow: 3, doy: 4}});
        moment.locale('something');
        test.equal(moment.localeData().firstDayOfWeek(), 3, 'firstDayOfWeek');
        test.equal(moment.localeData().firstDayOfYear(), 4, 'firstDayOfYear');

        test.done();
    },

    'instance locale method' : function (test) {
        moment.locale('en');

        test.equal(moment([2012, 5, 6]).format('MMMM'), 'June', 'Normally default to global');
        test.equal(moment([2012, 5, 6]).locale('es').format('MMMM'), 'junio', 'Use the instance specific locale');
        test.equal(moment([2012, 5, 6]).format('MMMM'), 'June', 'Using an instance specific locale does not affect other moments');

        test.done();
    },

    'instance locale method with array' : function (test) {
        var m = moment().locale(['non-existent', 'fr', 'also-non-existent']);
        test.equal(m.locale(), 'fr', 'passing an array uses the first valid locale');
        m = moment().locale(['es', 'fr', 'also-non-existent']);
        test.equal(m.locale(), 'es', 'passing an array uses the first valid locale');
        test.done();
    },

    'instance getter locale substrings' : function (test) {
        var m = moment();

        m.locale('fr-crap');
        test.equal(m.locale(), 'fr', 'use substrings');

        m.locale('fr-does-not-exist');
        test.equal(m.locale(), 'fr', 'uses deep substrings');

        test.done();
    },

    'instance locale persists with manipulation' : function (test) {
        test.expect(3);
        moment.locale('en');

        test.equal(moment([2012, 5, 6]).locale('es').add({days: 1}).format('MMMM'), 'junio', 'With addition');
        test.equal(moment([2012, 5, 6]).locale('es').day(0).format('MMMM'), 'junio', 'With day getter');
        test.equal(moment([2012, 5, 6]).locale('es').endOf('day').format('MMMM'), 'junio', 'With endOf');

        test.done();
    },

    'instance locale persists with cloning' : function (test) {
        test.expect(2);
        moment.locale('en');

        var a = moment([2012, 5, 6]).locale('es'),
            b = a.clone(),
            c = moment(a);

        test.equal(b.format('MMMM'), 'junio', 'using moment.fn.clone()');
        test.equal(b.format('MMMM'), 'junio', 'using moment()');

        test.done();
    },

    'duration locale method' : function (test) {
        test.expect(3);
        moment.locale('en');

        test.equal(moment.duration({seconds:  44}).humanize(), 'a few seconds', 'Normally default to global');
        test.equal(moment.duration({seconds:  44}).locale('es').humanize(), 'unos segundos', 'Use the instance specific locale');
        test.equal(moment.duration({seconds:  44}).humanize(), 'a few seconds', 'Using an instance specific locale does not affect other durations');

        test.done();
    },

    'duration locale persists with cloning' : function (test) {
        test.expect(1);
        moment.locale('en');

        var a = moment.duration({seconds:  44}).locale('es'),
            b = moment.duration(a);

        test.equal(b.humanize(), 'unos segundos', 'using moment.duration()');
        test.done();
    },

    'changing the global locale doesn\'t affect existing duration instances' : function (test) {
        var mom = moment.duration();
        moment.locale('fr');
        test.equal('en', mom.locale());
        test.done();
    },

    'duration deprecations' : function (test) {
        test.equal(moment.duration().lang(), moment.duration().localeData(), 'duration.lang is the same as duration.localeData');
        test.done();
    },

    'from relative time future' : function (test) {
        var start = moment([2007, 1, 28]);

        test.equal(start.from(moment([2007, 1, 28]).subtract({s: 44})),  'in a few seconds', '44 seconds = a few seconds');
        test.equal(start.from(moment([2007, 1, 28]).subtract({s: 45})),  'in a minute',      '45 seconds = a minute');
        test.equal(start.from(moment([2007, 1, 28]).subtract({s: 89})),  'in a minute',      '89 seconds = a minute');
        test.equal(start.from(moment([2007, 1, 28]).subtract({s: 90})),  'in 2 minutes',     '90 seconds = 2 minutes');
        test.equal(start.from(moment([2007, 1, 28]).subtract({m: 44})),  'in 44 minutes',    '44 minutes = 44 minutes');
        test.equal(start.from(moment([2007, 1, 28]).subtract({m: 45})),  'in an hour',       '45 minutes = an hour');
        test.equal(start.from(moment([2007, 1, 28]).subtract({m: 89})),  'in an hour',       '89 minutes = an hour');
        test.equal(start.from(moment([2007, 1, 28]).subtract({m: 90})),  'in 2 hours',       '90 minutes = 2 hours');
        test.equal(start.from(moment([2007, 1, 28]).subtract({h: 5})),   'in 5 hours',       '5 hours = 5 hours');
        test.equal(start.from(moment([2007, 1, 28]).subtract({h: 21})),  'in 21 hours',      '21 hours = 21 hours');
        test.equal(start.from(moment([2007, 1, 28]).subtract({h: 22})),  'in a day',         '22 hours = a day');
        test.equal(start.from(moment([2007, 1, 28]).subtract({h: 35})),  'in a day',         '35 hours = a day');
        test.equal(start.from(moment([2007, 1, 28]).subtract({h: 36})),  'in 2 days',        '36 hours = 2 days');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 1})),   'in a day',         '1 day = a day');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 5})),   'in 5 days',        '5 days = 5 days');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 25})),  'in 25 days',       '25 days = 25 days');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 26})),  'in a month',       '26 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 30})),  'in a month',       '30 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 45})),  'in a month',       '45 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 47})),  'in 2 months',      '47 days = 2 months');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 74})),  'in 2 months',      '74 days = 2 months');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 78})),  'in 3 months',      '78 days = 3 months');
        test.equal(start.from(moment([2007, 1, 28]).subtract({M: 1})),   'in a month',       '1 month = a month');
        test.equal(start.from(moment([2007, 1, 28]).subtract({M: 5})),   'in 5 months',      '5 months = 5 months');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 315})), 'in 10 months',     '315 days = 10 months');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 344})), 'in a year',        '344 days = a year');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 345})), 'in a year',        '345 days = a year');
        test.equal(start.from(moment([2007, 1, 28]).subtract({d: 548})), 'in 2 years',       '548 days = in 2 years');
        test.equal(start.from(moment([2007, 1, 28]).subtract({y: 1})),   'in a year',        '1 year = a year');
        test.equal(start.from(moment([2007, 1, 28]).subtract({y: 5})),   'in 5 years',       '5 years = 5 years');

        test.done();
    },

    'from relative time past' : function (test) {
        var start = moment([2007, 1, 28]);

        test.equal(start.from(moment([2007, 1, 28]).add({s: 44})),  'a few seconds ago', '44 seconds = a few seconds');
        test.equal(start.from(moment([2007, 1, 28]).add({s: 45})),  'a minute ago',      '45 seconds = a minute');
        test.equal(start.from(moment([2007, 1, 28]).add({s: 89})),  'a minute ago',      '89 seconds = a minute');
        test.equal(start.from(moment([2007, 1, 28]).add({s: 90})),  '2 minutes ago',     '90 seconds = 2 minutes');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 44})),  '44 minutes ago',    '44 minutes = 44 minutes');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 45})),  'an hour ago',       '45 minutes = an hour');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 89})),  'an hour ago',       '89 minutes = an hour');
        test.equal(start.from(moment([2007, 1, 28]).add({m: 90})),  '2 hours ago',       '90 minutes = 2 hours');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 5})),   '5 hours ago',       '5 hours = 5 hours');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 21})),  '21 hours ago',      '21 hours = 21 hours');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 22})),  'a day ago',         '22 hours = a day');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 35})),  'a day ago',         '35 hours = a day');
        test.equal(start.from(moment([2007, 1, 28]).add({h: 36})),  '2 days ago',        '36 hours = 2 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 1})),   'a day ago',         '1 day = a day');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 5})),   '5 days ago',        '5 days = 5 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 25})),  '25 days ago',       '25 days = 25 days');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 26})),  'a month ago',       '26 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 30})),  'a month ago',       '30 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 43})),  'a month ago',       '43 days = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 46})),  '2 months ago',      '46 days = 2 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 74})),  '2 months ago',      '75 days = 2 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 76})),  '3 months ago',      '76 days = 3 months');
        test.equal(start.from(moment([2007, 1, 28]).add({M: 1})),   'a month ago',       '1 month = a month');
        test.equal(start.from(moment([2007, 1, 28]).add({M: 5})),   '5 months ago',      '5 months = 5 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 315})), '10 months ago',     '315 days = 10 months');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 344})), 'a year ago',        '344 days = a year');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 345})), 'a year ago',        '345 days = a year');
        test.equal(start.from(moment([2007, 1, 28]).add({d: 548})), '2 years ago',       '548 days = 2 years');
        test.equal(start.from(moment([2007, 1, 28]).add({y: 1})),   'a year ago',        '1 year = a year');
        test.equal(start.from(moment([2007, 1, 28]).add({y: 5})),   '5 years ago',       '5 years = 5 years');

        test.done();
    },

    'instance locale used with from' : function (test) {
        test.expect(2);
        moment.locale('en');

        var a = moment([2012, 5, 6]).locale('es'),
            b = moment([2012, 5, 7]);

        test.equal(a.from(b), 'hace un día', 'preserve locale of first moment');
        test.equal(b.from(a), 'in a day', 'do not preserve locale of second moment');

        test.done();
    },

    'instance localeData' : function (test) {
        moment.defineLocale('dude', {week: {dow: 3}});
        test.equal(moment().locale('dude').localeData()._week.dow, 3);
        test.done();
    },

    'month name callback function' : function (test) {
        function fakeReplace(m, format) {
            if (/test/.test(format)) {
                return 'test';
            }
            if (m.date() === 1) {
                return 'date';
            }
            return 'default';
        }

        moment.locale('made-up-2', {
            months : fakeReplace,
            monthsShort : fakeReplace,
            weekdays : fakeReplace,
            weekdaysShort : fakeReplace,
            weekdaysMin : fakeReplace
        });

        test.equal(moment().format('[test] dd ddd dddd MMM MMMM'), 'test test test test test test', 'format month name function should be able to access the format string');
        test.equal(moment([2011, 0, 1]).format('dd ddd dddd MMM MMMM'), 'date date date date date', 'format month name function should be able to access the moment object');
        test.equal(moment([2011, 0, 2]).format('dd ddd dddd MMM MMMM'), 'default default default default default', 'format month name function should be able to access the moment object');

        test.done();
    },

    'changing parts of a locale config' : function (test) {
        test.expect(2);

        moment.locale('partial-lang', {
            months : 'a b c d e f g h i j k l'.split(' ')
        });

        test.equal(moment([2011, 0, 1]).format('MMMM'), 'a', 'should be able to set locale values when creating the localeuage');

        moment.locale('partial-lang', {
            monthsShort : 'A B C D E F G H I J K L'.split(' ')
        });

        test.equal(moment([2011, 0, 1]).format('MMMM MMM'), 'a A', 'should be able to set locale values after creating the localeuage');

        test.done();
    },

    'start/endOf week feature for first-day-is-monday locales' : function (test) {
        test.expect(2);

        moment.locale('monday-lang', {
            week : {
                dow : 1 // Monday is the first day of the week
            }
        });

        moment.locale('monday-lang');
        test.equal(moment([2013, 0, 1]).startOf('week').day(), 1, 'for locale monday-lang first day of the week should be monday');
        test.equal(moment([2013, 0, 1]).endOf('week').day(), 0, 'for locale monday-lang last day of the week should be sunday');

        test.done();
    },

    'meridiem parsing' : function (test) {
        test.expect(2);

        moment.locale('meridiem-parsing', {
            meridiemParse : /[bd]/i,
            isPM : function (input) {
                return input === 'b';
            }
        });

        moment.locale('meridiem-parsing');
        test.equal(moment('2012-01-01 3b', 'YYYY-MM-DD ha').hour(), 15, 'Custom parsing of meridiem should work');
        test.equal(moment('2012-01-01 3d', 'YYYY-MM-DD ha').hour(), 3, 'Custom parsing of meridiem should work');

        test.done();
    },

    'invalid date formatting' : function (test) {
        moment.locale('has-invalid', {
            invalidDate: 'KHAAAAAAAAAAAN!'
        });

        test.equal(moment.invalid().format(), 'KHAAAAAAAAAAAN!');
        test.equal(moment.invalid().format('YYYY-MM-DD'), 'KHAAAAAAAAAAAN!');

        test.done();
    },

    'return locale name' : function (test) {
        test.expect(1);

        var registered = moment.locale('return-this', {});

        test.equal(registered, 'return-this', 'returns the locale configured');

        test.done();
    },

    'changing the global locale doesn\'t affect existing instances' : function (test) {
        var mom = moment();
        moment.locale('fr');
        test.equal('en', mom.locale());
        test.done();
    },

    'setting a language on instance returns the original moment for chaining' : function (test) {
        var mom = moment();

        test.equal(mom.lang('fr'), mom, 'setting the language (lang) returns the original moment for chaining');
        test.equal(mom.locale('it'), mom, 'setting the language (locale) returns the original moment for chaining');

        test.done();
    },

    'lang(key) changes the language of the instance' : function (test) {
        var m = moment().month(0);
        m.lang('fr');
        test.equal(m.locale(), 'fr', 'm.lang(key) changes instance locale');

        test.done();
    },

    'moment#locale(false) resets to global locale' : function (test) {
        var m = moment();

        moment.locale('fr');
        m.locale('it');

        test.equal(moment.locale(), 'fr', 'global locale is it');
        test.equal(m.locale(), 'it', 'instance locale is it');
        m.locale(false);
        test.equal(m.locale(), 'fr', 'instance locale reset to global locale');

        test.done();
    },

    'moment().locale with missing key doesn\'t change locale' : function (test) {
        test.equal(moment().locale('boo').localeData(), moment.localeData(),
                'preserve global locale in case of bad locale id');
        test.done();
    },

    'moment().lang with missing key doesn\'t change locale' : function (test) {
        test.equal(moment().lang('boo').localeData(), moment.localeData(),
                'preserve global locale in case of bad locale id');
        test.done();
    }
};
