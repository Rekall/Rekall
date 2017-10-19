<?php require_once("functions.php"); ?>
<?php
	error_reporting(0);

	setlocale(LC_TIME, 'fr_FR.UTF-8', 'fra');
	session_start();
	
	$uploadFolder  = "../file/";
	ini_set('memory_limit', '64M');

?>