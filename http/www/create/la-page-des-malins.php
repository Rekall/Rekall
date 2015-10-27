<?php 
	$listOfProjects = array();
	$listOfFolders = scandir(".");
	foreach($listOfFolders as $entry)
		if ($entry != "." && $entry != "..")
			if((is_dir($entry)) && ($entry != "php") && ($entry != "css") && ($entry != "js"))
				$listOfProjects[] = array("name" => $entry, "url" => "http://".$_SERVER['HTTP_HOST']."/".$entry."/?p=".file_get_contents($entry."/file/projectPassword.txt"));

	if(isset($_GET["id"])) {
		header('Content-Type: text/plain charset=utf-8');
		$countMin = $_GET["id"]; $countMax = 1000;
		$count = 0;
		if($handle = opendir('.')) {
	    	while (false !== ($entry = readdir($handle))) {
	        	if ($entry != "." && $entry != "..") {
					if((is_dir($entry)) && ($entry != "php") && ($entry != "css") && ($entry != "js")) {
						if(($countMin <= $count) && ($count < $countMax)) {
							echo "----- Update #".$count." de ".$entry." -----\n";
			            	echo "Copie de php/functions.php  ->  ".$entry."/php/functions.php\n";
							copy("php/functions.php", $entry."/php/functions.php");

			            	echo "Copie de php/project.php  ->  ".$entry."/php/project.php\n";
							copy("php/project.php", $entry."/php/project.php");

			            	echo "Copie de create.zip  ->  ".$entry."/php/create.zip\n";
							copy("create.zip", $entry."/php/create.zip");

							$url = "http://project.memorekall.fr/".$entry."/php/project.php?update=now";
							echo "Update de ".$url."\n";
							openUrl($url);

							echo "\n\n";
							exit(0);
						}

						$count++;
					}
				}
			}
			closedir($handle);
		}
		exit();
	}
	
	//Download un fichier
	function openUrl($url) {
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
		curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 120); 
		curl_setopt($ch, CURLOPT_TIMEOUT, 120);
		echo curl_exec($ch);
		curl_close($ch);
	}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-type" content="text/html; charset=UTF-8">
	<meta http-equiv="Content-Language" content="fr">
	<meta name="language" content="fr">
	<meta name="designer" content="buzzing light">  
	<meta name="copyright" content="buzzing light">
	<meta name="HandheldFriendly" content="true" />
	<meta name="viewport" content="width=device-width, user-scalable=no"><!--, minimum-scale=1.0, maximum-scale=1.0-->
	<meta name="apple-mobile-web-app-capable" content="yes">
	<meta name="apple-mobile-web-app-status-bar-style" content="black">

	<link rel="apple-touch-icon" href="favicon.png" />
	<link rel="stylesheet" type="text/css" href="css/reset.css" />

	<title>Rekall</title>
	                                                                          
    <script language="javascript" type='text/javascript' src='js/jquery.min.js'></script>     
	<script language="javascript" type='text/javascript' src='js/sha1.js'></script>
	<link rel="stylesheet" type="text/css" href="css/create-theme.css" />    
	
	<script language="javascript" type='text/javascript'>
		$(document).ready(function() {
			var id = 0;
			$("#GO").click(function(event) {
				event.stopImmediatePropagation();
				event.preventDefault();
				$("#GO").hide();
				go(id);
			});
			function go(id) {
				$.ajax("la-page-des-malins.php", {
					data: {id: id},
					async: false,
					success: function(data) {
						$("#log").prepend((data+"").replace(/(?:\r\n|\r|\n)/g, '<br />'));
						if(id <= <?=count($listOfProjects)?>)
							setTimeout(function() { go(++id); }, 100);
					}
				});
			}
		});
	</script>
</head>
<body id="createProjectBody">              
	<img id="createProjectLogo" src="css/images/logo.png" />
	<form action="#" method="post" id='formCreate'>
	    <input id="GO" class='submitButton' type="submit" value="Mettre à jour les <?=count($listOfProjects)?> projets" name="submit"><br/><br/><br/>
		<div id='log' style='font-family: Monaco; font-size: 10px;'></div><br/><br/><br/>
	</form>
	
	<form action="#" method="post" id='formCreate'>
		Accès rapide en <u>mode édition</u> aux projets<br/><br/>
<?php
	foreach($listOfProjects as $infos) {
?>
		<a target="_blank" href='<?=$infos["url"]?>'><?=$infos["name"]?></a><br/>
<?php
	}
?>
		
	</form>        
	
</body>
</html>