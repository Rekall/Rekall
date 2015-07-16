var videoPlayer = undefined;
var rekall = new Rekall();
$(document).ready(function() {
	//Video
	videojs("video", {
		"controls": true,
		"autoplay": false,
		"preload": 	"auto",
		"loop": 	"false",
		/*
		"techOrder": ["html5", "flash"],
		*/
		//"techOrder": ["youtube", "html5", "flash"],
		"techOrder": ["youtube"],
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
		children: {
			controlBar: {
				children: {
					fullscreenToggle: 	false,
				}
			}
		}
	}, function() {
		videoPlayer = this;
		/*
		videoPlayer.src([
			{ type: "video/mp4",  src: "http://video-js.zencoder.com/oceans-clip.mp4" },
			{ type: "video/webm", src: "http://video-js.zencoder.com/oceans-clip.webm" },
			{ type: "video/ogg",  src: "http://video-js.zencoder.com/oceans-clip.ogv" },
		]);
		*/
		//videoPlayer.poster("http://video-js.zencoder.com/oceans-clip.png");
		videoPlayer.src("http://www.youtube.com/watch?v=bnC9pu65pa0");
		//videoPlayer.volume(0);
		rekall.timeline.play();
		
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
	
	rekall.loadXMLFile();
});

function showInRuban(texte, time) {
	alert(texte);
}

$(window).resize(function(e) {
	if(videoPlayer != undefined) {
		videoPlayer.width (($("#container").width() - $("#flattentimeline").width() - 5) + "px");
		videoPlayer.height(($("#container").height()) + "px");
	}
});
	