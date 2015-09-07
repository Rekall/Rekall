<?php require_once("php/sql.php"); ?>
<?php
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
	
	if((isset($_POST["create"])) && (isset($_POST["video"])) && (isset($_POST["password"]))) {
		createProject($_POST["create"], $_POST["video"], $_POST["password"]);
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

	<title>Rekall Online</title>
	
    <script language="javascript" type='text/javascript' src='js/libs/jquery.min.js'></script>     
    <script language="javascript" type='text/javascript' src='js/libs/jquery-migrate.js'></script>	             
	<script language="javascript" type='text/javascript' src='js/libs/sha1.js'></script>
	<link rel="stylesheet" type="text/css" href="css/online-theme.css" />
	
	<script language="javascript" type='text/javascript'>
		$(document).ready(function() {
			$("#formCreate").submit(function(event) {
				event.stopPropagation();
				if(($("input[name=create]").val() != "") && ($("input[name=video]").val() != "") && ($("input[name=password]").val() != "")) {
					var passwordRaw = $("input[name=password]").val();
					var password = (CryptoJS.SHA1(passwordRaw) + "").toUpperCase();
					$.ajax("create.php", {
						type: "POST",
						dataType: "json",
						data: {"create": $("input[name=create]").val(), "video": $("input[name=video]").val(), "password": password},
						success: function(retour) {
							console.log(retour);
							if(retour.success == 1) {
								window.document.location = document.URL.substr(0,document.URL.lastIndexOf('/')) + "/" + retour.value + "?password=" + passwordRaw;
							}
							else
								alert(retour.error)
						},
						error: function(retour) {
						}
					});	
				}
				else
					alert("Please fill all the form before submit!");
				return false;
			});			
		});
	</script>
</head>
<body>         
	<body>
		<form action="create.php" method="post" id='formCreate'>
			<label>Project name</label>
			<input type="text" name="create"   placeholder="my-project-name" size="30"><br/>
			<label>Youtube video URL</label>
		    <input type="text" name="video"    placeholder="https://www.youtube.com/watch?v=4Rcw5GCXJnQ" size="50"><br/>
			<label>Edition password</label>
		    <input type="password" name="password" placeholder="Password" size="10"><br/>

		    <input type="submit" value="Create project" name="submit">
		</form>
	</body>
</body>
</html>