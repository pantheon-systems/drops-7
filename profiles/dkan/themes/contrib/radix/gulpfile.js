// Include gulp.
var gulp = require('gulp');

// Include plugins.
var sassLint = require('gulp-sass-lint');
var jshint = require('gulp-jshint');

// SCSS Linting.
gulp.task('scss-lint', function() {
  return gulp.src(['./assets/scss/**/*.scss'])
    .pipe(sassLint())
    .pipe(sassLint.format())
    .pipe(sassLint.failOnError());
});

// JS Linting.
gulp.task('js-lint', function() {
  return gulp.src('./assets/js/*.js')
    .pipe(jshint())
    .pipe(jshint.reporter('default'));
});

// Default Task
gulp.task('default', ['js-lint', 'scss-lint']);
