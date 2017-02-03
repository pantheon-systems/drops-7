// Include gulp
var gulp = require('gulp');
var browserSync = require('browser-sync').create();
var config = require('./config.json');

// Include Our Plugins
var sass = require('gulp-sass');
var rename = require('gulp-rename');
var compass = require('gulp-compass');
var imagemin = require('gulp-imagemin');
var pngcrush = require('imagemin-pngcrush');
var livereload = require('gulp-livereload');
var shell = require('gulp-shell');
var gutil = require('gulp-util');
var plumber = require('gulp-plumber');

// Compress images
gulp.task('images', function () {
  return gulp.src('assets/images/**/*')
    .pipe(imagemin({
      progressive: true,
      svgoPlugins: [{ removeViewBox: false }],
      use: [pngcrush()]
    }))
    .pipe(gulp.dest('assets/images'));
});

// Static Server + watching scss files
gulp.task('serve', ['sass'], function() {
  browserSync.init({
    proxy: config.browserSyncProxy
  })

  gulp.watch('assets/sass/**/*.scss', ['sass']);
  gulp.watch('assets/stylesheets/**/*').on('change', browserSync.reload);
});

// Compile Our Sass with Bundle[d] Compass
gulp.task('sass', function() {
  return gulp.src('assets/sass/*.scss')
    .pipe(plumber({
      errorHandler: function (error) {
        console.log(error.message);
        this.emit('end');
      }}))
    .pipe(compass({
      config_file: 'config.rb',
      css: 'assets/stylesheets',
      sass: 'assets/sass',
      bundle_exec: true
    }))
    .pipe(gulp.dest('assets/stylesheets'));
});

// Run drush to clear the theme registry.
gulp.task('drush', shell.task([
  'drush cache-clear theme-registry'
]));

// Default Task
gulp.task('default', ['serve']);