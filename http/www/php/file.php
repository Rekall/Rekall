<?php require_once("sql.php"); ?>
<?php
	if(isset($_GET["r"])) {
		$filename = realpath("../file/".$_GET["r"]);
		
		if(startsWith($filename, realpath("../file/"))) {
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
