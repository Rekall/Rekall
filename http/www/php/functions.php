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
	
	/**
	 * Function: sanitize
	 * Returns a sanitized string, typically for URLs.
	 *
	 * Parameters:
	 *     $string - The string to sanitize.
	 *     $force_lowercase - Force the string to lowercase?
	 *     $anal - If set to *true*, will remove all non-alphanumeric characters.
	 */
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
	
	// Returns a file size limit in bytes based on the PHP upload_max_filesize
	// and post_max_size
	function file_upload_max_size() {
	  static $max_size = -1;

	  if ($max_size < 0) {
	    // Start with post_max_size.
	    $max_size = parse_size(ini_get('post_max_size'));

	    // If upload_max_size is less, then reduce. Except if upload_max_size is
	    // zero, which indicates no limit.
	    $upload_max = parse_size(ini_get('upload_max_filesize'));
	    if ($upload_max > 0 && $upload_max < $max_size) {
	      $max_size = $upload_max;
	    }
	  }
	  return $max_size;
	}

	function parse_size($size) {
	  $unit = preg_replace('/[^bkmgtpezy]/i', '', $size); // Remove the non-unit characters from the size.
	  $size = preg_replace('/[^0-9\.]/', '', $size); // Remove the non-numeric characters from the size.
	  if ($unit) {
	    // Find the position of the unit in the ordered string which is the power of magnitude to multiply a kilobyte by.
	    return round($size * pow(1024, stripos('bkmgtpezy', $unit[0])));
	  }
	  else {
	    return round($size);
	  }
	}
	
	//Download un fichier
	function downloadFile($url, $path) {
		$fp = fopen($path, 'w+');
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, false);
		curl_setopt($ch, CURLOPT_BINARYTRANSFER, true);
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
		curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 5); 
		curl_setopt($ch, CURLOPT_TIMEOUT, 10);
		curl_setopt($ch, CURLOPT_FILE, $fp);
		curl_exec($ch);
		curl_close($ch);
		fclose($fp);
		if(filesize($path) == 0) {
			unlink($path);
			return $url." (error)";
		}
		return $url." => ".$path;
	}
	
	function SureRemoveDir($dir, $DeleteMe) {
	    if(!$dh = @opendir($dir)) return;
	    while (false !== ($obj = readdir($dh))) {
	        if($obj=='.' || $obj=='..') continue;
	        if (!@unlink($dir.'/'.$obj)) SureRemoveDir($dir.'/'.$obj, true);
	    }

	    closedir($dh);
	    if ($DeleteMe){
	        @rmdir($dir);
	    }
	}
?>
