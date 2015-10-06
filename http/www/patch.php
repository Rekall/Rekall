<?php 
	header('Content-Type: text/plain charset=utf-8');

	$countMin = 0; $countMax = 1000;
	$count = 0;
	if($handle = opendir('.')) {
    	while (false !== ($entry = readdir($handle))) {
        	if ($entry != "." && $entry != "..") {
				if((is_dir($entry)) && ($entry != "php")) {
					
					echo "----- Update #".$count." de ".$entry." -----\n";
					if(($countMin <= $count) && ($count < $countMax)) {
		            	echo "Copie de php/functions.php  ->  ".$entry."/php/functions.php\n";
						copy("php/functions.php", $entry."/php/functions.php");

		            	echo "Copie de php/project.php  ->  ".$entry."/php/project.php\n";
						copy("php/project.php", $entry."/php/project.php");

		            	echo "Copie de create.zip  ->  ".$entry."/php/create.zip\n";
						copy("create.zip", $entry."/php/create.zip");

						//$url = "http://project.memorekall.fr/".$entry."/php/project.php?update=now";
						//echo "Update de ".$url."\n";
						//openUrl($url);

						echo "\n\n";
					}

					$count++;
				}
			}
		}
		closedir($handle);
	}
	
	//Download un fichier
	function openUrl($url) {
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL, $url);
		curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
		curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
		curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, 120); 
		curl_setopt($ch, CURLOPT_TIMEOUT, 120);
		echo curl_exec($ch);
		curl_close($ch);
	}
?>