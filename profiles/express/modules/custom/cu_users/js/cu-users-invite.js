(function( $ ){
  $(document).ready(function(){
    var elevatedRoles = Drupal.settings.elevatedRoles;
    var roleCheckCore = new Map();
    var roleCheckAddon = new Map();

    $('.form-item-core-rid input').attr('data-role-reset', 'true');

    $(elevatedRoles).each(function(){

      $('#edit-core-rid-' + this +':not(:checked)').attr('data-role-reset', 'false');
      $('#edit-core-rid-' + this +':not(:checked)').change(function(){
        var roleID = $(this).val();
        if (roleCheckCore.has(roleID)) {
          // roleCheckCore.delete(roleID);
        }
        else {
          roleCheckCore.set(roleID, roleID);
        }
      });
      $('#edit-addon-rids-' + this +':not(:checked)').change(function(){
        var roleID = $(this).val();
        if (roleCheckAddon.has(roleID)) {
          roleCheckAddon.delete(roleID);
        }
        else {
          roleCheckAddon.set(roleID, roleID);
        }
      });
    });
    $('[data-role-reset="true"]').change(function(){
      roleCheckCore = new Map();
    });

    $('#user-external-invite-form').submit(function( event ) {
      //alert( "Handler for .submit() called." );
      //console.log(roleCheck);


      if ( (roleCheckCore.size != 0) ||  (roleCheckAddon.size != 0) ) {
        if (confirm("You are adding roles with elevated permissions. Are you sure you want to do this?") == true) {
          // Submit form.
        } else {
          // Stop form submission.
          return false;
        }
      }
      else {
        console.log(roleCheckCore.size);
        console.log(roleCheckCore.values());
        console.log(roleCheckAddon.size);
        console.log(roleCheckAddon.values());
      }

      event.preventDefault();
    });
  });
})( jQuery );
