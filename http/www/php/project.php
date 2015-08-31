<?php require_once("sql.php"); ?>
<?php
	$project = new DomDocument();
	$racine  = null;
	function openProject() {
		global $project;
		global $racine;
		$project->preserveWhiteSpace = false;
		$project->load("../file/project.xml");
		$project->formatOutput = true;
		$racine = $project->documentElement;
	}
	function addFileToProject($file, $metas, $tc) {
		global $project;
		global $racine;
		//echo $file."=".$tc;
		$tc          = floatval($tc);
		$document    = $project->createElement("document");
		
		$finfo       = finfo_open(FILEINFO_MIME_TYPE);

		$user        = "";
		if(fileowner($file) != FALSE) {
			$user = fileowner($file);
			$user = posix_getpwuid($user);
			$user = explode(",", $user["gecos"]);
			$user = $user[0];
		}
		
		//Création des métadatas
		$metasAdd = array(
	        "Rekall->Comments"					=> "",
	        "Rekall->Keywords"					=> "",
	        "Rekall->Group"						=> "",
	        "Rekall->Visibility"				=> "",
			
			"File->Hash"						=> strtoupper(sha1_file($file)),
			"Rekall->Flag"						=> "File",
			
			"File->Thumbnail"					=> "",
			"File->Owner"						=> $user,

			"File->MIME Type"					=> finfo_file($finfo, $file),
			"File->File Type"					=> finfo_file($finfo, $file),
	        "Rekall->Type"						=> finfo_file($finfo, $file),

			"File->File Name"					=> pathinfo($file, PATHINFO_BASENAME),
			"File->Extension"					=> pathinfo($file, PATHINFO_EXTENSION),
	        "File->Basename"					=> pathinfo($file, PATHINFO_FILENAME),
			"Rekall->Name"						=> pathinfo($file, PATHINFO_FILENAME),
			"Rekall->Extension"					=> strtoupper(pathinfo($file, PATHINFO_EXTENSION)),
			"Rekall->Folder"					=> "",

			"Rekall->File Size"					=> filesize($file),
			"Rekall->File Size (MB)"			=> filesize($file)/(1024.*1024.),
		); 
		$metas = array_merge($metas, $metasAdd);

		//Ajout des métadatas
		foreach($metas as $metaCategory => $metaContent) {
			$meta = $project->createElement("meta");
			$meta->setAttribute("ctg", $metaCategory);
			$meta->setAttribute("cnt", $metaContent);
			$document->appendChild($meta);
			$racine->appendChild($document);
		}
		
		//Tag de timeline
		$tag = $project->createElement("tag");
		$tag->setAttribute("key",       "/".$metas["Rekall->Folder"].$metas["File->File Name"]);
		$tag->setAttribute("timeStart", $tc);
		$tag->setAttribute("timeEnd",   $tc);
		$tag->setAttribute("version",   0);
		$racine->appendChild($tag);
		
		return $metas;
	}
	function closeProject() {
		global $project;
		global $racine;
		$project->save("../file/project.xml");
	}
?>