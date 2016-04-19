module.exports = function(grunt) {
  'use strict';
  // Project configuration.
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    concat: {
      options: {
        separator: ';'
      },
      core: {
        src: ['src/*.js'],
        dest: 'dist/<%= pkg.name %>.min.js'
      },
      controls: {
        src: ['src/controls/*.js'],
        dest: 'dist/<%= pkg.name %>.controls.min.js'
      },
      backends: {
        src: ['src/backends/*.js'],
        dest: 'dist/recline.backends.min.js'
      }
    },
    uglify: {
      options: {
        banner: '/*! <%= pkg.name %> v0.3.0 */\n'
      },
      core: {
        src: ['src/*.js'],
        dest: 'dist/<%= pkg.name %>.min.js'
      },
      controls: {
        src: ['src/controls/*.js'],
        dest: 'dist/<%= pkg.name %>.controls.min.js'
      },
      backends: {
        src: ['src/backends/*.js'],
        dest: 'dist/recline.backends.min.js'
      }
    },
    express: {
      all: {
        options: {
          bases: ['./'],
          port: 8080,
          hostname: '0.0.0.0',
          livereload: true
        }
      }
    },
    jshint: {
      files: ['Gruntfile.js', 'src/**/*.js', 'examples/*.js' ],
      options: {
        jshintrc: true
      }
    },
    watch: {
      files:  ['<%= jshint.files %>'],
      tasks: ['jshint', 'concat', 'uglify'],
      options: {
        livereload: true
      }
    },
    open: {
      all: {
        path: 'http://localhost:8080/examples/index.html'
      }
    }
  });

  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-open');
  grunt.loadNpmTasks('grunt-express');
  grunt.loadNpmTasks('grunt-livereload');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  grunt.loadNpmTasks('grunt-contrib-concat');

  // Default task(s).
  grunt.registerTask('default', [
    'express',
    'watch',
  ]);

  grunt.registerTask('build', [
    'jshint',
    'concat'
  ]);

  grunt.registerTask('lint', ['jshint']);
};
