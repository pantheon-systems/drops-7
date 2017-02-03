// karma.conf.js
module.exports = function(config) {
  'use strict';

  config.set({
    frameworks: ['mocha', 'expect'],
    files: [
      // Put dependencies here.
      'test/test.js'
    ],
    client: {
      mocha: {
        timeout: 5000
      }
    }
  });
};