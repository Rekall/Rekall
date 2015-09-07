<?php require_once("../php/sql.php"); ?>
<?php
	//Créé un projet Rekall
	function createProject($name, $videoUrl) {
		$retours = array("success" => 0, "error" => "", "value" => "");
		$name = sanitize($name);

		if($name == "")
			$name = sha1(rand());
	
		if(!file_exists("../".$name)) {
			$zip = new ZipArchive;
			$res = $zip->open("seed.zip");
			if ($res === TRUE) {
				$zip->extractTo("../".$name);
				$zip->close();
				file_put_contents("../".$name."/file/project.xml", str_replace("__video__", $videoUrl, file_get_contents("../".$name."/file/project.xml")));
				$retours["success"] = 1;
			} else {
				$retours["success"] = -1;
				$retours["error"] = "No seed found";
			}
		}
		else {
			$retours["success"] = 0;
			$retours["error"] = "Project exists";
		}
		$retours["value"] = $name;

		echo json_encode($retours);
	}
	
	if((isset($_POST["create"])) && (isset($_POST["video"])))
		createProject($_POST["create"], $_POST["video"]);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta http-equiv="Content-type" content="text/html; charset=UTF-8">
		<meta http-equiv="Content-Language" content="fr">
	</head>
	<body>
		<form action="index.php" method="post" enctype="multipart/form-data">
			<input type="text" name="create" value="Guillaume Jacquemin">
		    <input type="text" name="video"  value="https://www.youtube.com/watch?v=4Rcw5GCXJnQ">

		    <input type="submit" value="Create" name="submit">
		</form>
	</body>
</html>