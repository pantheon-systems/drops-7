<?php
  if (!empty($row->users_node__realname_realname)) {
    print $row->users_node__realname_realname;
  }
  else {
    print $row->users_node_name;
  }
?>
