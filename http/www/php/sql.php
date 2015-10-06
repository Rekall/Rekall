<?php require_once("functions.php"); ?>
<?php
	error_reporting(0);

	setlocale(LC_TIME, 'fr_FR.UTF-8', 'fra');
	session_start();
	
	$uploadFolder  = "../file/";
	ini_set('memory_limit', '64M');
/*
	if(strpos(strtolower($_SERVER["HTTP_HOST"]), "typomatic.fr") !== false) {
		header("Status: 301 Moved Permanently", false, 301);
		header("Location: http://www.typomatic.org");
		exit();
	}
	if(strpos(strtolower($_SERVER["HTTP_HOST"]), "typomatic") !== false) {
		$connection = mysql_connect('mysql5-4.bdb', 'buzzinglbase', 'Suze2805');
		$db = mysql_select_db("buzzinglbase");
		mysql_query("SET NAMES 'utf8'"); 
	}
	else {
		$connection = mysql_connect('localhost', 'root', 'root');
		$db = mysql_select_db("buzzinglbase");
		mysql_query("SET NAMES 'utf8'");
	}
	$rootUrl       = "http://www.typomatic.org/";
	$rootTitle     = "Typomatic";
	$resultsLimit  = 40;
	$dbTable       = "typomatic";
	$countTotal    = 0;
	$uploadFolder  = "upload/";
	$analyticsCode = "UA-1526504-10";
	$facebookAppId = "845224702230589";
	$urlPrefix     = "";
*/
?>