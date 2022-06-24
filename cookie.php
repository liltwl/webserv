<?php
$cookie_name = "random";
$cookie_value = "ali";
setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
?>
