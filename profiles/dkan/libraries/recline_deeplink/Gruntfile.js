module.exports = function(grunt) {
  // Project configuration.
  grunt.initConfig({
    pkg: grunt.file.readJSON('package.json'),
    uglify: {
      options: {
        banner: '/*! <%= pkg.name %> <%= grunt.template.today("yyyy-mm-dd") %> */\n'
      },
      build: {
        src: 'src/<%= pkg.name %>.js',
        dest: 'dist/<%= pkg.name %>.min.js'
      }
    },
    livereload: {

    },
    express: {
      all: {
        options: {
          bases: ['./'],
          port: 8080,
          hostname: "0.0.0.0",
          livereload: true
        }
      }
    },
    watch: {
      all: {
        files: '**/*.html',
        options: {
          livereload: true
        }
      }
    },
    open: {
      all: {
        path: 'http://localhost:8080/examples/index.html'
      }
    },
    jshint: {
      all: ['Gruntfile.js', 'src/**/*.js']
    }
  });

  grunt.loadNpmTasks('grunt-contrib-jshint');
  grunt.loadNpmTasks('grunt-open');
  grunt.loadNpmTasks('grunt-express');
  grunt.loadNpmTasks('grunt-livereload');
  grunt.loadNpmTasks('grunt-contrib-uglify');
  // Default task(s).
  grunt.registerTask('default', [
    'express',
    'open',
    'watch'
  ]);

  grunt.registerTask('lint', ['jshint']);
};
