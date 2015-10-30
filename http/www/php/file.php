<?php require_once("sql.php"); ?>
<?php
	if(isset($_GET["r"])) {
		//header('Content-Type: text/plain charset=utf-8');
		$filename = realpath("../file/".$_GET["r"]);
		if(strpos(strtolower($_SERVER["HTTP_HOST"]), "memorekall") !== false) {
			if(!file_exists($filename))
				$filename = realpath("../file/".Normalizer::normalize($_GET["r"], Normalizer::NFC));
			if(!file_exists($filename))
				$filename = realpath("../file/".Normalizer::normalize($_GET["r"], Normalizer::NFKC));
			if(!file_exists($filename))
				$filename = realpath("../file/".Normalizer::normalize($_GET["r"], Normalizer::NFD));
			if(!file_exists($filename))
				$filename = realpath("../file/".Normalizer::normalize($_GET["r"], Normalizer::NFKD));
		}
		
		if((file_exists($filename)) && (startsWith($filename, realpath("../file/")))) {
			$mimeType = "";
			$finfo = finfo_open(FILEINFO_MIME_TYPE);
			$mimeType = finfo_file($finfo, $filename);
			finfo_close($finfo);
			if($mimeType === "")
				$mimeType = "application/octet-stream";
		
			header("Content-Type: ".$mimeType);
			readfile($filename);
		}
	}
?>
