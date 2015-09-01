<?php require_once("sql.php"); ?>
<?php
	$project = new DomDocument();
	$racine  = null;
	
	//Ouverture / fermeture
	function openProject() {
		header('Content-Type: text/plain charset=utf-8');
		
		global $project;
		global $racine;
		$project->preserveWhiteSpace = false;
		$project->load("../file/project.xml");
		$project->formatOutput = true;
		$racine = $project->documentElement;
	}
	function closeProject() {
		global $project;
		global $racine;
		$project->save("../file/project.xml");
	}
	
	//Ajoute un fichier
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
	        "Rekall->Comments"			=> "",

			"File->Hash"				=> strtoupper(sha1_file($file)),
			"Rekall->Flag"				=> "File",
			
			"File->Thumbnail"			=> "",
			"File->Owner"				=> $user,

			"File->MIME Type"			=> finfo_file($finfo, $file),
			"File->File Type"			=> finfo_file($finfo, $file),
	        "Rekall->Type"				=> finfo_file($finfo, $file),

			"File->File Name"			=> pathinfo($file, PATHINFO_BASENAME),
			"File->Extension"			=> pathinfo($file, PATHINFO_EXTENSION),
	        "File->Basename"			=> pathinfo($file, PATHINFO_FILENAME),
			"Rekall->Name"				=> pathinfo($file, PATHINFO_FILENAME),
			"Rekall->Extension"			=> strtoupper(pathinfo($file, PATHINFO_EXTENSION)),
			"Rekall->Folder"			=> "",

			"Rekall->File Size"			=> filesize($file),
			"Rekall->File Size (MB)"	=> filesize($file)/(1024.*1024.),
		); 
		$metas = array_merge($metas, $metasAdd);
		$key = "/".$metas["Rekall->Folder"].$metas["File->File Name"];
		
		//Génère une vignette
		$fileDestBasename = strtoupper(sha1($metas["Rekall->Folder"])."-".$metas["File->Hash"]);
		$fileDest = "../file/rekall_cache/".$fileDestBasename.".jpg";
		createThumb($file, $fileDest, 160);
		if(file_exists($fileDest))
			$metas["File->Thumbnail"] = $fileDestBasename;

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
		$tag->setAttribute("key",       $key);
		$tag->setAttribute("timeStart", $tc);
		$tag->setAttribute("timeEnd",   $tc);
		$tag->setAttribute("version",   0);
		$racine->appendChild($tag);
		
		return $metas;
	}

	
	//Ajoute un marker
	function addMarkerToProject($name, $metas, $tc) {
		global $project;
		global $racine;

		$tc          = floatval($tc);
		$document    = $project->createElement("document");

		//Création des métadatas
		$metasAdd = array(
	        "Rekall->Comments"					=> "",
	        "Rekall->Type"						=> "rekall/marker",
	        "Rekall->Date/Time"					=> date("Y:m:d H:i:s"),
	        "Rekall->Import Date"				=> date("Y:m:d H:i:s"),
			"Rekall->Flag"						=> "Marker",
			"Rekall->Name"						=> $name,
		); 
		$metas = array_merge($metas, $metasAdd);
		$key = "marker-".sha1(mktime().rand()."");
		$document->setAttribute("key", $key);

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
		$tag->setAttribute("key",       $key);
		$tag->setAttribute("timeStart", $tc);
		$tag->setAttribute("timeEnd",   $tc);
		$tag->setAttribute("version",   0);
		$racine->appendChild($tag);
		
		return $metas;
	}
	
	
	//Supprime un fichier
	function removeFileFromProject($folder, $file) {
		global $project;
		global $racine;
		$key = "/".$folder.$file;
		$retours = array("document" => 0, "tag" => 0, "edition" => 0, "file" => 0);
		$thumbnail = "";
		
		//Recherche dans les documents
		$documents = $racine->getElementsByTagName("document");
		foreach($documents as $document) {
			$metas = $document->getElementsByTagName("meta");
			$searchFile   = "";
			$searchFolder = "";
			foreach($metas as $meta) {
				if     ($meta->getAttribute("ctg") == "File->File Name")
					$searchFile = $meta->getAttribute("cnt");
				else if($meta->getAttribute("ctg") == "Rekall->Folder")
					$searchFolder = $meta->getAttribute("cnt");
				else if($meta->getAttribute("ctg") == "File->Thumbnail")
					$thumbnail = $meta->getAttribute("cnt");
			}
			$searchKey = "/".$searchFolder.$searchFile;
			if($searchKey == $key) {
				$retours["document"]++;
				$racine->removeChild($document);
				break;
			}
			else
				$thumbnail = "";
		}
		
		//Recherche dans les tags
		$tags = $racine->getElementsByTagName("tag");
		foreach($tags as $tag) {
			$searchKey = $tag->getAttribute("key");
			if($searchKey == $key) {
				$retours["tag"]++;
				$racine->removeChild($tag);
			}
		}
		
		//Recherche dans les editions
		$editions = $racine->getElementsByTagName("edition");
		foreach($editions as $edition) {
			$searchKey = $edition->getAttribute("key");
			if($searchKey == $key) {
				$retours["edition"]++;
				$racine->removeChild($edition);
			}
		}
		
		//Supprime le fichier
		$fileToRemove = "../file".$key;
		if(file_exists($fileToRemove)) {
			$retours["file"]++;
			unlink($fileToRemove);
		}
		//et sa vignette
		$fileToRemove = "../file/rekall_cache/".$thumbnail.".jpg";
		if(file_exists($fileToRemove)) {
			$retours["file"]++;
			unlink($fileToRemove);
		}
		
		
		
		echo json_encode($retours);
	}
	
	
	//Supprime un fichier
	function removeMarkerFromProject($key) {
		global $project;
		global $racine;
		$retours = array("document" => 0, "tag" => 0, "edition" => 0);
		
		//Recherche dans les documents
		$documents = $racine->getElementsByTagName("document");
		foreach($documents as $document) {
			$searchKey = $document->getAttribute("key");
			if($searchKey == $key) {
				$retours["document"]++;
				$racine->removeChild($document);
			}
		}
		
		//Recherche dans les tags
		$tags = $racine->getElementsByTagName("tag");
		foreach($tags as $tag) {
			$searchKey = $tag->getAttribute("key");
			if($searchKey == $key) {
				$retours["tag"]++;
				$racine->removeChild($tag);
			}
		}
		
		//Recherche dans les editions
		$editions = $racine->getElementsByTagName("edition");
		foreach($editions as $edition) {
			$searchKey = $edition->getAttribute("key");
			if($searchKey == $key) {
				$retours["edition"]++;
				$racine->removeChild($edition);
			}
		}
		
		echo json_encode($retours);
	}
	
	//Change le TC
	function editTc($key, $tcIn, $tcOut) {
		global $project;
		global $racine;
		$retours = array("success" => 1);
		
		//Tag de timeline
		$tag = $project->createElement("tag");
		$tag->setAttribute("key",       $key);
		$tag->setAttribute("timeStart", $tcIn);
		$tag->setAttribute("timeEnd",   $tcOut);
		$tag->setAttribute("version",   0);
		$racine->appendChild($tag);

		echo json_encode($retours);
	}
	
	//Change les metadonnées
	function editMetadata($key, $metadataKey, $metadataValue) {
		global $project;
		global $racine;
		$retours = array("success" => 1);

		//Tag de timeline
		$edition = $project->createElement("edition");
		$edition->setAttribute("key",           $key);
		$edition->setAttribute("metadataKey",   $metadataKey);
		$edition->setAttribute("metadataValue", $metadataValue);
		$edition->setAttribute("version",   0);
		$racine->appendChild($edition);

		echo json_encode($retours);
	}

	//API
	if(true) {
		//Opérations sur les fichiers
		if((isset($_GET["folder"])) && (isset($_GET["file"]))) {
			if(isset($_GET["remove"])) {
				openProject();
				removeFileFromProject($_GET["folder"], $_GET["file"]);
				closeProject();
			}
			else if((isset($_GET["tcIn"])) && (isset($_GET["tcOut"]))) {
				openProject();
				editTc("/".$_GET["folder"].$_GET["file"], $_GET["tcIn"], $_GET["tcOut"]);
				closeProject();
			}
			else if((isset($_GET["metadataKey"])) && (isset($_GET["metadataValue"]))) {
				openProject();
				editMetadata("/".$_GET["folder"].$_GET["file"], $_GET["metadataKey"], $_GET["metadataValue"]);
				closeProject();
			}
		}
		else if(isset($_GET["key"])) {
			if(isset($_GET["remove"])) {
				openProject();
				removeMarkerFromProject($_GET["key"]);
				closeProject();
			}
			else if((isset($_GET["tcIn"])) && (isset($_GET["tcOut"]))) {
				openProject();
				editTc($_GET["key"], $_GET["tcIn"], $_GET["tcOut"]);
				closeProject();
			}
			else if((isset($_GET["metadataKey"])) && (isset($_GET["metadataValue"]))) {
				openProject();
				editMetadata($_GET["key"], $_GET["metadataKey"], $_GET["metadataValue"]);
				closeProject();
			}
		}
	}
?>