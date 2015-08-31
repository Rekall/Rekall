<?php require_once("sql.php"); ?>
<?php
	if((isset($_FILES)) && (count($_FILES) > 0)) {
		header('Content-Type: text/plain charset=utf-8');
		
		if(!is_dir($uploadFolder))
			mkdir($uploadFolder);
		
		$retour = "";
		$status = false;
		foreach($_FILES as $fileinfo => $file) {
			$filename = $_FILES[$fileinfo]['name'];
			$retour .= "{";
			
			$infos = explode("|", $fileinfo);
			if(count($infos) > 1)
				$filename = str_replace("_", ".", $infos[1]);
			
			if(is_uploaded_file($_FILES[$fileinfo]['tmp_name'])) {
				if(file_exists($_FILES[$fileinfo]['tmp_name'])) {
					if(is_writable($uploadFolder)) {
						//echo "Upload de ".$fileinfo."\t".$filename."...";
						if(move_uploaded_file($_FILES[$fileinfo]['tmp_name'], $uploadFolder.$filename)) {
							$retour .= '"code":1, "status":"OK ('.(filesize($uploadFolder.$filename)/1000.).' kB)"';
							$status |= true;
						}
						else
							$retour .= '"code":0, "error":"Erreur de copie dans '.$uploadFolder.$filename.'"';
					}
					else
						$retour .= '"code":0, "error":"Dossier '.$uploadFolder.' n‘a pas les droits"';
				} 
				else
					$retour .= '"code":0, "error":"Fichier uploadé temporaire introuvable"';
			}
			else
				$retour .= '"code":0, "error":"Aucun fichier uploadé"';
			$retour .= "},";
		}
		$retour = rtrim($retour, ",");
		echo '{"code": '.(($status==true)?(1):(0)).', "files":['.$retour."]}";

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
		    <input type="file" name="fileToUpload" id="fileToUpload">
		    <input type="submit" value="Upload Image" name="submit">
		</form>
	</body>
</html>