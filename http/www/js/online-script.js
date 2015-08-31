var videoPlayer = undefined;
var rekall = new Rekall();
var rekall_common = new Object();
$(document).ready(function() {
	rekall_common.isEditor = true;
	rekall_common.owner = {"author": "Guillaume Jacquemin", "locationGps": "", "locationName": ""};
	
	
	if((rekall_common.isEditor) && (navigator.geolocation))
		navigator.geolocation.getCurrentPosition(function(position) {
			rekall_common.owner.locationGps  = position.coords.latitude + ", " + position.coords.longitude;
			rekall_common.owner.locationName = rekall_common.owner.locationGps;
		});
	
	//Video
	videojs("video", {
		"controls": true,
		"autoplay": false,
		"preload": 	"auto",
		"loop": 	"false",
		"poster":   "http://video-js.zencoder.com/oceans-clip.png",
		"techOrder": ["youtube", "html5", "flash"], //youtube dailymotion vimeo
/*
		"src": 	 	"https://vimeo.com/45161598",
		"techOrder": [""], "src" : "",
		"techOrder": ["vimeo"], "src" : "",
		"techOrder": ["html5", "flash"], "src": "oceans-clip.mp4",
*/
		children: {
			controlBar: {
				children: {
					fullscreenToggle: 	false,
				}
			}
		}
		/*
		    PosterImage
		    TextTrackDisplay
		    LoadingSpinner
		    BigPlayButton
		    ControlBar
		        PlayToggle
		        FullscreenToggle
		        CurrentTimeDisplay
		        TimeDivider
		        DurationDisplay
		        RemainingTimeDisplay
		        ProgressControl
		            SeekBar
		              LoadProgressBar
		              PlayProgressBar
		              SeekHandle
		        VolumeControl
		            VolumeBar
		                VolumeLevel
		                VolumeHandle
		        MuteToggle
		*/
	}, function() {
		videoPlayer = this;
		//videoPlayer.volume(0);

		//videoPlayer.src([{type: "video/mp4", src: "http://video-js.zencoder.com/oceans-clip.mp4"}, {type: "video/webm", src: "http://video-js.zencoder.com/oceans-clip.webm"}, {type: "video/ogg", src: "http://video-js.zencoder.com/oceans-clip.ogv"}]);
		videoPlayer.src("http://www.youtube.com/watch?v=bnC9pu65pa0");
		//videoPlayer.src("http://www.dailymotion.com/video/xxvfw4_guillaume-jacquemin-soiree-di-zain-5-code-s-data-s_creation");
		//rekall.timeline.play();
		
		videoPlayer.on("durationchange", function(e) {
		});
		videoPlayer.on("ended", function(e) {
		});
		videoPlayer.on("error", function(e) {
		});
		videoPlayer.on("firstplay", function(e) {
		});
		videoPlayer.on("fullscreenchange", function(e) {
		});
		videoPlayer.on("loadedalldata", function(e) {
		});
		videoPlayer.on("loadeddata", function(e) {
		});
		videoPlayer.on("loadedmetadata", function(e) {
		});
		videoPlayer.on("loadstart", function(e) {
		});
		videoPlayer.on("pause", function(e) {
		});
		videoPlayer.on("play", function(e) {
		});
		videoPlayer.on("progress", function(e) {
		});
		videoPlayer.on("seeked", function(e) {
		});
		videoPlayer.on("seeking", function(e) {
		});
		videoPlayer.on("timeupdate", function(e) {
			rekall.timeline.update(videoPlayer.currentTime());
		});
		videoPlayer.on("volumechange", function(e) {
		});
		videoPlayer.on("waiting", function(e) {
		});
		videoPlayer.on("resize", function(e) {
		});
		$(window).trigger("resize");
	});
	
	
	//Drag&drop files
	$(document).on({
		dragenter: function(event) {
			event.stopImmediatePropagation();
			event.preventDefault();
		},
		dragleave: function(event) {
			event.stopImmediatePropagation();
			event.preventDefault();
		},
		dragover: function(event) {
			event.stopImmediatePropagation();
			event.preventDefault();
		},
		drop: function(event) {
			if(event.originalEvent.dataTransfer.files.length) {
				event.stopImmediatePropagation();
				event.preventDefault();
				uploadFiles(event.originalEvent.dataTransfer.files);
			}
		}
	});
		
	rekall.loadXMLFile();
});

var filesToUpload = [], fileIsUploading = false;
function uploadFiles(files) {
	$.each(files, function(index, file) {
		if(file.name != undefined) {
			var colorCategory = rekall.sortings["colors"].categories[Sorting.prefix + file.type];
			if(colorCategory != undefined) {
				//alert(colorCategory.color);
			}
			var fileType     = (file.type.split("/"))[0];
			var fileDateTime = moment(file.lastModifiedDate);
			//alert("Chargement de " + file.name + " (" + fileType + ", " + file.size + " octets, date du " + fileDateTime.format("YYYY:MM:DD HH:mm:ss") + ")");
			
			//Données du formulaire
			var formData = undefined;
			if($('form')[0] != undefined)
				formData = new FormData($('form')[0]); //à vérifier
			else {
				formData = new FormData();
				formData.append("fileToUpload", file);
			}
			formData.append(file.name + "_date", fileDateTime.format("YYYY:MM:DD HH:mm:ss"));
			formData.append("tc", rekall.timeline.timeCurrent);
			formData.append("author",       rekall_common.owner.author);
			formData.append("locationGps",  rekall_common.owner.locationGps);
			formData.append("locationName", rekall_common.owner.locationName);
			
			if(formData != undefined) {
				filesToUpload.push({
					url: 'php/upload.php',
					type: 'POST',
					xhr: function() {
						var myXhr = $.ajaxSettings.xhr();
						if(myXhr.upload) {
							myXhr.upload.addEventListener('progress', function(event) {
								window.document.title = "Téléchargement " + floor(event.loaded / event.total * 100) + "%";
							}, false);
						}
						return myXhr;
					},
					beforeSend: function(data) {
						//alert("beforeSend : " + data);
					},
					success:    function(data) {
						console.log(data);
						data = JSON.parse(data);
						alert(data.files[0].metas["Rekall->Name"] + " téléchargé");
						window.document.title = "Rekall Online";
						fileIsUploading = false;
						uploadFilesNext();
					},
					error:      function(data) {
						alert("Erreur de téléchargement");
						window.document.title = "Rekall Online";
						fileIsUploading = false;
						uploadFilesNext();
					},

					data:		 formData,
					cache:       false,
					contentType: false,
					processData: false
				});
				uploadFilesNext();
			}
		}
	});
}
function uploadFilesNext() {
	if((filesToUpload.length > 0) && (!fileIsUploading)) {
		fileIsUploading = true;
		$.ajax(filesToUpload[0]);
		filesToUpload.splice(0, 1);
		uploadFilesNext();
	}
}

function showInRuban(texte, time) {
	alert(texte);
}

$(window).resize(function(e) {
	if(videoPlayer != undefined) {
		videoPlayer.width (($("#container").width() - $("#flattentimeline").width() - 5) + "px");
		videoPlayer.height(($("#container").height()) + "px");
	}
});
	