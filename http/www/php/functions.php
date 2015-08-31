<?php
	//Systèmes de mailings
	function genericMail($expediteur, $destinataire, $file, $vars) {
		if($destinataire == "")
			return "Aucun destinataire pour l'email";

		$fp = fopen($file, "r");
		if(!$fp)
			return "Template d'email ".$file." introuvable";

		$message = "";
		while(!feof($fp))
			$message .= fgets($fp, 4096);

		$headers = 'From: '.$expediteur."\n";
		$headers .= 'Content-Type: text/html; charset="utf-8"'."\n"; 
		$headers .= 'Content-Transfer-Encoding: 8bit';

		foreach($vars as $var => $substvar)
			$message = str_replace($var, $substvar, $message);

		$objetStart = strpos($message, "<title>") + 7;
		$objetEnd = strpos($message, "</title>");
		$objet = substr($message, $objetStart, $objetEnd - $objetStart);

		$ok = false;
		$ok = mail($destinataire, $objet, $message, $headers);
		return "Email envoyé à ".htmlspecialchars($destinataire)." depuis ".htmlspecialchars($expediteur)." avec pour objet <strong>".$objet."</strong> : succès = ".$ok."<br/><br/>".$message;
	}
	
	//Filesize
	function human_filesize($bytes, $decimals = 2) {
	    $size = array('B','kB','MB','GB','TB','PB','EB','ZB','YB');
	    $factor = floor((strlen($bytes) - 1) / 3);
	    return sprintf("%.{$decimals}f", $bytes / pow(1024, $factor)) . @$size[$factor];
	}
	
	//Créé une vignette à hauteur fixe
	function createThumb($filenameSrc, $filenameDst, $maxcote) {
		if(file_exists($filenameSrc)) {
			//Créé une vignette PNG
			if(strtolower(pathinfo($filenameSrc, PATHINFO_EXTENSION)) == "png") {
				$temp_image_file = imagecreatefrompng($filenameSrc);
				if($temp_image_file !== FALSE) {
					list($width,$height) = getimagesize($filenameSrc);
					if(($width > 10) && ($height > 10)) {
						$newwidth = $width * ($maxcote / $height);
						if($width > $height)
							$newwidth = $width * ($maxcote / $height);
						else if($width > $height)
							$maxcote = $height / ($maxcote / $width);

						$image_file = imagecreatetruecolor($newwidth, $maxcote);
						imagealphablending($image_file, false );
						imagesavealpha($image_file, true );
						imagecopyresampled($image_file, $temp_image_file, 0,0,0,0, $newwidth, $maxcote, $width, $height);
						imagejpeg($image_file, $filenameDst);
						imagedestroy($temp_image_file);
						imagedestroy($image_file);
					}
					else {
						unline($filenameSrc);
						unline($filenameDst);
					}
				}
				else {
					unline($filenameSrc);
					unline($filenameDst);
				}
			}
			//Créé une vignette JPG
			else if(strtolower(pathinfo($filenameSrc, PATHINFO_EXTENSION)) == "jpg") {
				$temp_image_file = imagecreatefromjpeg($filenameSrc);
				if($temp_image_file !== FALSE) {
					list($width,$height) = getimagesize($filenameSrc);
					if(($width > 10) && ($height > 10)) {
						$newwidth = $width * ($maxcote / $height);

						$image_file = imagecreatetruecolor($newwidth, $maxcote);
						imagecopyresampled($image_file, $temp_image_file, 0,0,0,0, $newwidth, $maxcote, $width, $height);
						imagejpeg($image_file, $filenameDst);
						imagedestroy($temp_image_file);
						imagedestroy($image_file);
					}
					else {
						unlink($filenameSrc);
						unlink($filenameDst);
					}
				}
				else {
					unlink($filenameSrc);
					unlink($filenameDst);
				}
			}
		}
	}
?>
