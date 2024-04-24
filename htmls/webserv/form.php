<?php
if ($argc < 2) {
	echo "No parameters received on our cgi :(";
	exit(1);
}

$arr = explode("&", $argv[1]);

echo "<!DOCTYPE html> \n<html lang=\"en\"> \n<head> \n<meta charset=\"UTF-8\"> \n<title>PHP Form</title> \n</head>\n<body>\n";

foreach ($arr as $param) {
	$keyval = explode("=", $param);
	echo "\t<h2>" . $keyval[0] . ": " . $keyval[1] . "</h2>\n";
}

echo "</body>\n</html>\n";
?>
