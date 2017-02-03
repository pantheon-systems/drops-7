
Drupal.Imagecrop.cropUi = Drupal.Imagecrop.cropUi || {};

(function($) { 

$(function () {
  Drupal.Imagecrop.cropUi.initControls();
  Drupal.Imagecrop.cropUi.initScaling();
});  

Drupal.Imagecrop.imageCropWidthField = null;
Drupal.Imagecrop.imageCropHeightField = null;
Drupal.Imagecrop.imageCropXField = null;
Drupal.Imagecrop.imageCropYField = null;
Drupal.Imagecrop.imageCropScaleField = null;
Drupal.Imagecrop.resizeMe = null;

/**
 * Init the controls.
 */
Drupal.Imagecrop.cropUi.initControls = function() {
  
  // Store input fields
  var $imagecropform = $('#imagecrop-crop-settings-form');
  Drupal.Imagecrop.imageCropWidthField = $('input[name="image-crop-width"]', $imagecropform);
  Drupal.Imagecrop.imageCropHeightField = $('input[name="image-crop-height"]', $imagecropform);
  Drupal.Imagecrop.imageCropXField = $('input[name="image-crop-x"]', $imagecropform);
  Drupal.Imagecrop.imageCropYField = $('input[name="image-crop-y"]', $imagecropform);
  Drupal.Imagecrop.imageCropScaleField = $('input[name="image-crop-scale"]', $imagecropform);
  
  // Event listeners on input fields
  Drupal.Imagecrop.imageCropWidthField.change(Drupal.Imagecrop.cropUi.sizeListener);
  Drupal.Imagecrop.imageCropHeightField.change(Drupal.Imagecrop.cropUi.sizeListener);
  Drupal.Imagecrop.imageCropXField.change(Drupal.Imagecrop.cropUi.positionListener);
  Drupal.Imagecrop.imageCropYField.change(Drupal.Imagecrop.cropUi.positionListener);
  
  Drupal.Imagecrop.resizeMe = $('#resizeMe');
  Drupal.Imagecrop.cropUi.cropContainer = $('#image-crop-container');
  
  if (Drupal.Imagecrop.resizeMe.resizable) { 

    Drupal.Imagecrop.resizeMe.resizable({
      containment: Drupal.Imagecrop.cropUi.cropContainer,
      aspectRatio: Drupal.settings.imagecrop.resizeAspectRatio,
      autohide: true,
      handles: 'n, e, s, w, ne, se, sw, nw',
      resize: Drupal.Imagecrop.cropUi.resizeListener
    });
    
  }

  Drupal.Imagecrop.resizeMe.draggable({
    cursor: 'move',
    containment: Drupal.Imagecrop.cropUi.cropContainer,
    drag: Drupal.Imagecrop.cropUi.dragListener
  });
  
  Drupal.Imagecrop.cropUi.cropContainer.css({ opacity: 0.5 });
  Drupal.Imagecrop.resizeMe.css({ position : 'absolute' });
  
  var leftpos = Drupal.Imagecrop.imageCropXField.val();
  var toppos = Drupal.Imagecrop.imageCropYField.val();
  
  Drupal.Imagecrop.resizeMe.css({backgroundPosition: '-'+ leftpos + 'px -'+ toppos +'px'});
  Drupal.Imagecrop.resizeMe.width(Drupal.Imagecrop.imageCropWidthField.val() + 'px');
  Drupal.Imagecrop.resizeMe.height($('#edit-image-crop-height', '#imagecrop-crop-settings-form').val() + 'px');
  Drupal.Imagecrop.resizeMe.css({top: toppos +'px' });
  Drupal.Imagecrop.resizeMe.css({left: leftpos +'px' });
  
}

/**
 * Init the scaling dropdown.
 */
Drupal.Imagecrop.cropUi.initScaling = function() {
  
  Drupal.Imagecrop.fid = $('input[name="fid"]', '#imagecrop-crop-settings-form').val();
  Drupal.Imagecrop.style = $('input[name="style"]', '#imagecrop-crop-settings-form').val();
  Drupal.Imagecrop.cropFile = $('input[name="temp-style-destination"]', '#imagecrop-crop-settings-form').val();
  $('#edit-scaling', '#imagecrop-scale-settings-form').bind('change', Drupal.Imagecrop.cropUi.scaleImage);
  Drupal.Imagecrop.cropUi.cropWrapper = $('#imagecrop-crop-wrapper');
  
}

/**
 * Listener on the jquery ui resize plugin.
 */
Drupal.Imagecrop.cropUi.resizeListener = function(e, ui) {
  
  var curr_width = parseInt(Drupal.Imagecrop.resizeMe.width());
  var curr_height = parseInt(Drupal.Imagecrop.resizeMe.height());
  Drupal.Imagecrop.imageCropWidthField.val(curr_width);
  Drupal.Imagecrop.imageCropHeightField.val(curr_height);
  
  Drupal.Imagecrop.cropUi.validateSizeChanges(curr_width, curr_height, false);        
  
}

/**
 * Listener on the jquery ui draggable plugin.
 */
Drupal.Imagecrop.cropUi.dragListener = function(e, ui) {
  Drupal.Imagecrop.cropUi.setBackgroundPosition(ui.position.left, ui.position.top, true);  
}

/**
 * Listener on the X and Y field.
 */
Drupal.Imagecrop.cropUi.positionListener = function() {

  var x = parseInt(Drupal.Imagecrop.imageCropXField.val());
  var y = parseInt(Drupal.Imagecrop.imageCropYField.val());
  var changeInput = false;
  
  // Left must be integer
  if (isNaN(x)) {
    var position =Drupal.Imagecrop.resizeMe.position();
    Drupal.Imagecrop.imageCropXField.val(position.left);
    return;
  }

  // Top must be integer
  if (isNaN(y)) {
    var position = Drupal.Imagecrop.resizeMe.position();
    Drupal.Imagecrop.imageCropYField.val(position.top);
    return;
  }
  
  // X position can not be higher then width from container - width from cropping. 
  var max_x = Drupal.Imagecrop.cropUi.cropWrapper.width() - Drupal.Imagecrop.imageCropWidthField.val();
  if (x > max_x) {
    x = max_x;
    changeInput = true;
  }
  
  // Y position can not be higher then height from container - height from cropping. 
  var max_y = Drupal.Imagecrop.cropUi.cropWrapper.width() - Drupal.Imagecrop.imageCropWidthField.val();
  if (y > max_x) {
    y = max_y;
    changeInput = true;
  }

  Drupal.Imagecrop.resizeMe.css({ 'left' : x, 'top' : y});
  Drupal.Imagecrop.cropUi.setBackgroundPosition(x, y, changeInput);
  
}

/**
 * Set the current background position from the cropping area.
 */
Drupal.Imagecrop.cropUi.setBackgroundPosition = function(x, y, changeInput) {

  Drupal.Imagecrop.resizeMe.css({'background-position' : '-' + x + 'px -' + y + 'px'});
  if (changeInput) {
    Drupal.Imagecrop.imageCropXField.val(x);
    Drupal.Imagecrop.imageCropYField.val(y);    
  }  
  
}

/**
 * Event listener on the width / height field.
 */
Drupal.Imagecrop.cropUi.sizeListener = function() {

  var curr_height = parseInt(Drupal.Imagecrop.imageCropHeightField.val());
  var curr_width = parseInt(Drupal.Imagecrop.imageCropWidthField.val());
  
  // Height must be integer
  if (isNaN(curr_height)) {
    Drupal.Imagecrop.imageCropHeightField.val(Drupal.Imagecrop.resizeMe.height());
    return;
  }

  // Width must be integer
  if (isNaN(curr_width)) {
    Drupal.Imagecrop.imageCropWidthField.val(Drupal.Imagecrop.resizeMe.width());
    return;
  }  
  
  Drupal.Imagecrop.resizeMe.height(curr_height);
  Drupal.Imagecrop.resizeMe.width(curr_width);
  Drupal.Imagecrop.cropUi.validateSizeChanges(parseInt(curr_width), parseInt(curr_height), true);
  
}

/**
 * Validate the new width / height and update if needed.
 */
Drupal.Imagecrop.cropUi.validateSizeChanges = function(curr_width, curr_height, event) {

  var width_changed = false;
  var height_changed = false;
  
  if (curr_width < parseInt(Drupal.settings.imagecrop.minWidth)) {
    width_changed = true;
    curr_width = Drupal.settings.imagecrop.minWidth;
    if (Drupal.settings.imagecrop.resizeAspectRatio !== false) {
      height_changed = true;
      curr_height = Drupal.settings.imagecrop.minWidth / Drupal.settings.imagecrop.resizeAspectRatio;
    }
  }
  
  if (curr_height < parseInt(Drupal.settings.imagecrop.minHeight)) {
    curr_height = Drupal.settings.imagecrop.minHeight;
    height_changed = true;
    if (Drupal.settings.imagecrop.resizeAspectRatio !== false) {
      width_changed = true;
      curr_width = Drupal.settings.imagecrop.minHeight * Drupal.settings.imagecrop.resizeAspectRatio;
    }
  }  

  if (curr_height > Drupal.Imagecrop.cropUi.cropContainer.height()) {
    height_changed = true;
    Drupal.Imagecrop.resizeMe.css({top: '0' });
    if (Drupal.settings.imagecrop.resizeAspectRatio !== false) {
      width_changed = true;
      curr_width = Drupal.settings.imagecrop.minHeight * Drupal.settings.imagecrop.resizeAspectRatio;
    }    
    curr_height = Drupal.Imagecrop.cropUi.cropContainer.height();
  }

  if (curr_width > Drupal.Imagecrop.cropUi.cropContainer.width()) {
    width_changed = true;
    curr_width = Drupal.Imagecrop.cropUi.cropContainer.width();
    Drupal.Imagecrop.resizeMe.css({left: '0' });
    if (Drupal.settings.imagecrop.resizeAspectRatio !== false) {
      height_changed = true;
      curr_height = Drupal.settings.imagecrop.minWidth / Drupal.settings.imagecrop.resizeAspectRatio;
    }    
  }
  
  if (width_changed || event) {
    Drupal.Imagecrop.imageCropWidthField.val(curr_width);
    Drupal.Imagecrop.resizeMe.width(curr_width);
  }
  
  if (height_changed || event) {
    Drupal.Imagecrop.imageCropHeightField.val(curr_height);
    Drupal.Imagecrop.resizeMe.height(curr_height);
  }
  
  if (curr_width < Drupal.settings.imagecrop.startWidth || curr_height < Drupal.settings.imagecrop.startHeight ) {
    Drupal.Imagecrop.resizeMe.addClass('boxwarning');
  }
  else {
    Drupal.Imagecrop.resizeMe.removeClass('boxwarning');
  }  
 
  var pos = Drupal.Imagecrop.resizeMe.position();
  var left = (pos.left > 0) ? pos.left : 0;
  var top = (pos.top > 0) ? pos.top : 0;
  Drupal.Imagecrop.resizeMe.css({ backgroundPosition : ('-' + left + 'px -' + top + 'px')});
  Drupal.Imagecrop.imageCropXField.val(left);
  Drupal.Imagecrop.imageCropYField.val(top);  
  
}

/**
 * Scale the image to the selected width / height.
 */
Drupal.Imagecrop.cropUi.scaleImage = function() {
  
  var dimensions = $(this).val().split('x');
  if (dimensions.length != 2) {
    return false;
  }
  
  var imagecropData = {
    'fid' : Drupal.Imagecrop.fid,
    'style' : Drupal.Imagecrop.style,
    'scale' : dimensions[0]
  }
  
  $.ajax({
    url : Drupal.settings.imagecrop.manipulationUrl,
    data : imagecropData,
    type : 'post',
    success : function() {
      
      Drupal.Imagecrop.hasUnsavedChanges = true;
      
      // force new backgrounds and width / height
      var background = Drupal.Imagecrop.cropFile + '?time=' +  new Date().getTime();
      Drupal.Imagecrop.cropUi.cropContainer.css({
        'background-image' : 'url(' + background + ')',
        'width' : dimensions[0],
        'height' : dimensions[1]
      });

      Drupal.Imagecrop.cropUi.cropWrapper.css({
        'width' : dimensions[0],
        'height' : dimensions[1]
      });      

      // force background-size on resizeMe's background image as well.
      Drupal.Imagecrop.resizeMe.css({
        'background-size': dimensions[0] +'px '+ dimensions[1] +'px ',
        '-moz-background-size': dimensions[0] +'px '+ dimensions[1] +'px ',
        '-o-background-size': dimensions[0] +'px '+ dimensions[1] +'px ',
        '-webkit-background-size': dimensions[0] +'px '+ dimensions[1] +'px '
      });
      
      // make resize smaller when new image is smaller
      if (Drupal.Imagecrop.resizeMe.height() > dimensions[1]) {
        Drupal.Imagecrop.resizeMe.height(dimensions[1]);
      }
      if (Drupal.Imagecrop.resizeMe.width() > dimensions[0]) {
        Drupal.Imagecrop.resizeMe.width(dimensions[0]);
      }      
 
      Drupal.Imagecrop.resizeMe.css({'background-image' : 'url(' + background + ')'})
      Drupal.Imagecrop.imageCropScaleField.val(dimensions[0]);
      
    }
  })
  
}

})(jQuery); 
