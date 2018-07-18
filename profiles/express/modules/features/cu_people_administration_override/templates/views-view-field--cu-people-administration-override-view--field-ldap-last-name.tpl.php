<?php
  if (!empty($row->realname_realname)) {
    print $row->realname_realname;
  }
  else {
    print $row->users_name;
  }
?>
