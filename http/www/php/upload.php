<?php require_once("sql.php"); ?>
<?php require_once("project.php"); ?>
<?php
	if((isset($_FILES)) && (count($_FILES) > 0)) {
		header('Content-Type: text/plain charset=utf-8');
		
		if(!is_dir($uploadFolder))
			mkdir($uploadFolder);
		
		$retour = "";
		$status = false;
		openProject();
		
		//Clean
		$_FILES_OK = array();
		foreach($_FILES as $fileinfo => $file) {
			foreach($file as $fileKey => $fileValues) {
				if(is_array($fileValues)) {
					foreach($fileValues as $fileValueIndex => $fileValue)
						$_FILES_OK[$fileinfo.$fileValueIndex][$fileKey] = $fileValue;
				}
				else
					$_FILES_OK[$fileinfo][$fileKey] = $fileValues;
			}
		}
		$_FILES = $_FILES_OK;

		foreach($_FILES as $fileinfo => $file) {
			$date      = date("Y:m:d H:i:s");
			
			$filename = $_FILES[$fileinfo]['name'];
			$retour .= "{";

			if(isset($_POST["date"]))				$date = $_POST["date"];
			$metas = array(
				"Rekall->Author"					=> "",
				"File->File Access Date/Time"		=> $date,
				"File->File Creation Date/Time"		=> $date,
				"File->File Modification Date/Time"	=> $date,
				"Rekall->Date/Time"					=> $date,
				"Rekall->Import Date"				=> date("Y:m:d H:i:s"),
			);
			
			$fileTc = "";
			if(isset($_POST["tc"]))					$fileTc = $_POST["tc"];

			if(isset($_POST["author"]))				$metas["Rekall->Author"] = $_POST["author"];
			$metas["Rekall User Infos->User Name"]  = $metas["Rekall->Author"];

			if(isset($_POST["locationName"]))				$metas["Rekall->Location Name"] = $_POST["locationName"];
			if(isset($_POST["locationGps"]))				$metas["Rekall->Location GPS"]  = $_POST["locationGps"];
			$metas["Rekall User Infos->User IP"]    = $_SERVER['REMOTE_ADDR'];
			
			$infos = explode("|", $fileinfo);
			if(count($infos) > 1)
				$filename = str_replace("_", ".", $infos[1]);
			
			if(is_uploaded_file($_FILES[$fileinfo]['tmp_name'])) {
				if(file_exists($_FILES[$fileinfo]['tmp_name'])) {
					if(!file_exists($uploadFolder.$filename)) {
						if(is_writable($uploadFolder)) {
							//echo "Upload de ".$fileinfo."\t".$filename."...";
							if(move_uploaded_file($_FILES[$fileinfo]['tmp_name'], $uploadFolder.$filename)) {
								$metasAdded = addFileToProject($uploadFolder.$filename, $metas, $fileTc);
								$retour .= '"code":1, "tc":'.$fileTc.', "status":"OK ('.(filesize($uploadFolder.$filename)/1000.).' kB)", "metas":'.json_encode($metasAdded);
								$status |= true;
							}
							else
								$retour .= '"code":0, "error":"Erreur de copie dans '.$uploadFolder.$filename.'"';
						}
						else
							$retour .= '"code":-1, "error":"Dossier '.$uploadFolder.' n‘a pas les droits"';
					}
					else
						$retour .= '"code":-2, "error":"Fichier existant"';
				} 
				else
					$retour .= '"code":-3, "error":"Fichier uploadé temporaire introuvable"';
			}
			else
				$retour .= '"code":-4, "error":"Aucun fichier uploadé"';
			$retour .= "},";
		}
		$retour = rtrim($retour, ",");
		echo '{"code":'.(($status==true)?(1):(0)).', "files":['.$retour."]}";
		closeProject();

		exit();
	}
	else if((isset($_POST)) && (count($_POST) > 0)) {
		header('Content-Type: text/plain charset=utf-8');
		
		$retour = "{";
		$status = false;
		openProject();

		$date = date("Y:m:d H:i:s");
		if(isset($_POST["date"]))	$date = $_POST["date"];
		
		$metas = array(
			"Rekall->Author"					=> "",
			"Rekall->Date/Time"					=> $date,
			"Rekall->Import Date"				=> date("Y:m:d H:i:s"),
		);
		
		$fileTc = "";
		if(isset($_POST["tc"]))					$fileTc = $_POST["tc"];

		if(isset($_POST["author"]))				$metas["Rekall->Author"] = $_POST["author"];
		$metas["Rekall User Infos->User Name"]  = $metas["Rekall->Author"];

		if(isset($_POST["locationName"]))				$metas["Rekall->Location Name"] = $_POST["locationName"];
		if(isset($_POST["locationGps"]))				$metas["Rekall->Location GPS"]  = $_POST["locationGps"];
		$metas["Rekall User Infos->User IP"]    = $_SERVER['REMOTE_ADDR'];

		$metasAdded = addMarkerToProject($_POST["name"], $metas, $fileTc);
		$retour .= '"code":1, "tc":'.$fileTc.', "status":"OK", "metas":'.json_encode($metasAdded);
		$status |= true;

		$retour .= "},";
			
		$retour = rtrim($retour, ",");
		echo '{"code":'.(($status==true)?(1):(0)).', "files":['.$retour."]}";
		closeProject();

		exit();
	}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta http-equiv="Content-type" content="text/html; charset=UTF-8">
		<meta http-equiv="Content-Language" content="fr">
	</head>
	<body>
		<form action="upload.php" method="post" enctype="multipart/form-data">
		    Select image to upload:
		    <input type="file" name="fileToUpload[]" multiple="multiple">
		    
			<input type="text" name="tc"     value="234">
			<input type="text" name="author" value="Guillaume Jacquemin">
		    <input type="text" name="date" value="2010:01:28 08:10:32">
			
		    <input type="submit" value="Upload Image" name="submit">
		</form>
	</body>
</html>