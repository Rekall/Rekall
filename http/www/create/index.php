<?php
	function sanitize($string, $force_lowercase = true, $anal = false) {
	    $strip = array("~", "`", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "=", "+", "[", "{", "]",
	                   "}", "\\", "|", ";", ":", "\"", "'", "&#8216;", "&#8217;", "&#8220;", "&#8221;", "&#8211;", "&#8212;",
	                   "â€”", "â€“", ",", "<", ".", ">", "/", "?");
	    $clean = trim(str_replace($strip, "", strip_tags($string)));
	    $clean = preg_replace('/\s+/', "-", $clean);
	    $clean = ($anal) ? preg_replace("/[^a-zA-Z0-9]/", "", $clean) : $clean ;
	    return ($force_lowercase) ?
	        (function_exists('mb_strtolower')) ?
	            mb_strtolower($clean, 'UTF-8') :
	            strtolower($clean) :
	        $clean;
	}
	
	//Créé un projet Rekall
	function createProject($name, $videoUrl, $sha1password) {
		$retours = array("success" => 0, "error" => "", "value" => "");
		$name = sanitize($name);

		if($name == "")
			$name = sha1(rand());
	
		if(!file_exists($name)) {
			$zip = new ZipArchive;
			$res = $zip->open("create.zip");
			if ($res === TRUE) {
				$zip->extractTo($name);
				$zip->close();
				file_put_contents($name."/file/project.xml", str_replace("__video__", $videoUrl, file_get_contents($name."/file/project.xml")));
				file_put_contents($name."/file/projectPassword.txt", $sha1password);				
				$retours["success"] = 1;
			} else {
				$retours["success"] = -1;
				$retours["error"] = "No seed found";
			}
		}
		else {
			$retours["success"] = 0;
			$retours["error"] = "Project already exists";
		}
		$retours["value"] = $name;

		echo json_encode($retours);
	}
	
	if((isset($_POST["create"])) && (isset($_POST["video"])) && (isset($_POST["p"]))) {
		createProject($_POST["create"], $_POST["video"], $_POST["p"]);
		exit();
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
			$("#formCreate").submit(function(event) {
				event.stopPropagation();
				if(($("input[name=create]").val() != "") && ($("input[name=video]").val() != "") && ($("input[name=password]").val() != "")) {
					var passwordRaw = $("input[name=p]").val();
					var password = (CryptoJS.SHA1(passwordRaw) + "").toUpperCase();
					$("#submitCreate").hide();
					$.ajax("index.php", {
						type: "POST",
						dataType: "json",
						data: {"create": $("input[name=create]").val(), "video": $("input[name=video]").val(), "p": password},
						success: function(retour) {
							console.log(retour);
							if(retour.success == 1) {
								window.document.location = document.URL.substr(0,document.URL.lastIndexOf('/')) + "/" + retour.value + "?p=" + password;
							}
							else {
								alert(retour.error)
								$("#submitCreate").show();
							}
						},
						error: function(retour) {
							$("#submitCreate").show();
						}
					});	
				}
				else
					alert("Merci de saisir tous les champs");
				return false;
			});			
		});
	</script>
</head>
<body id="createProjectBody">              
	<img id="createProjectLogo" src="css/images/logo.png" />
	<form action="index.php" method="post" id='formCreate'>
		<label>Project name *</label>
		<input type="text" name="create"   placeholder="my-project-name" size="30"><br/>
		<label>YouTube video URL *</label>
	    <input type="text" name="video"    placeholder="http://www.youtube.com/watch?v=bnC9pu65pa0" size="30"><br/> 
		<label>Edition password *</label>
	    <input type="password" name="p" placeholder="password" size="30"><br/>

	    <input id="submitCreate" type="submit" value="Create a project" name="submit">
	</form>        
</body>
</html>