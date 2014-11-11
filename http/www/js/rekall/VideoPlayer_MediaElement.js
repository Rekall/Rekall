/*
    This file is part of Rekall.
    Copyright (C) 2013-2014

    Project Manager: Clarisse Bardiot
    Development & interactive design: Guillaume Jacquemin & Guillaume Marais (http://www.buzzinglight.com)

    This file was written by Guillaume Jacquemin.

    Rekall is a free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

function VideoPlayer(dom, uniqueName, render) {
	this.render		  = render;
	this.dom          = dom;
	this.uniqueName   = uniqueName;
	this.thumbPath    = "";
	
	this.dom.append("<div class='videoItem invisible'><video width='100%' preload='metadata' id='" + this.uniqueName + "' class='video-js vjs-default-skin' controls></video></div>");
	//this.dom.append  ("<div class='videoItem'><video width='100%' preload='metadata' id='" + this.uniqueName + "' class='' controls='controls' src='/big_buck_bunny.mp4'></video></div>");
	this.activate();
	this.hide();
	this.autoplay = false;
}
VideoPlayer.audioHeight = 40;
VideoPlayer.prototype.loadLocal = function(tagOrDoc, play) {
	if(play == undefined)
		play = false;
	this.autoplay = play;
	if(this.tagOrDoc != tagOrDoc) {
		this.tagOrDoc = tagOrDoc;
		if(tagOrDoc != undefined) {
			if(tagOrDoc.isAudio())	$("#" + this.uniqueName).height(VideoPlayer.audioHeight);
			else					$("#" + this.uniqueName).height(220);
	
			$("#" + this.uniqueName).find(".vjs-custom").text(tagOrDoc.getMetadata("Rekall->Name"));
			this.url = Utils.getLocalFilePath(tagOrDoc);
			if(tagOrDoc.isVideo()) {
				this.thumbPath    = Utils.getPreviewPath(tagOrDoc);
				this.poster	      = this.thumbPath + "_1.jpg";
			}
			else {
				this.thumbPath = "";
				this.poster    = "";
			}
			
			//if(this.url.endsWith(".mov"))
			//	this.url += ".mp4";
			this.player.setPoster(this.poster);
			this.player.setSrc(this.url);
			this.player.load();
			this.show();
		}
	}
}

VideoPlayer.prototype.activate = function() {
	var thiss = this;
	this.player = new MediaElementPlayer("#" + this.uniqueName, {
	    pauseOtherPlayers: false,
	    enablePluginDebug: false,
	    plugins: ['flash','silverlight'],
	    pluginPath: '',
	    flashName: 'flashmediaelement.swf',
	    silverlightName: 'silverlightmediaelement.xap',
	    defaultVideoWidth: 480,
	    defaultVideoHeight: 270,

	    // if set, overrides <video width>
	    videoWidth: -1,
	    // if set, overrides <video height>
	    videoHeight: -1,
	    // width of audio player
	    audioWidth: 400,
	    // height of audio player
	    audioHeight: 30,
	    // initial volume when the player starts
	    startVolume: 0.8,
	    // useful for <audio> player loops
	    loop: false,
	    // enables Flash and Silverlight to resize to content size
	    enableAutosize: true,
	    // the order of controls you want on the control bar (and other plugins below)
	    features: ['playpause','progress','current','duration','tracks','volume','fullscreen'],
	    // Hide controls when playing and mouse is not over the video
	    alwaysShowControls: false,
	    // force iPad's native controls
	    iPadUseNativeControls: false,
	    // force iPhone's native controls
	    iPhoneUseNativeControls: false, 
	    // force Android's native controls
	    AndroidUseNativeControls: false,
	    // forces the hour marker (##:00:00)
	    alwaysShowHours: false,
	    // show framecount in timecode (##:00:00:00)
	    showTimecodeFrameCount: false,
	    // used when showTimecodeFrameCount is set to true
	    // framesPerSecond: 25,
	    // turns keyboard support on and off for this instance
	    // enableKeyboard: true,
	    // when this player starts, it will pause other players
	    // pauseOtherPlayers: true,
	    // array of keyboard commands
	    // keyActions: []
	    // specify to force MediaElement to use a particular video or audio type
	    //type: '',
	    // default if the <video width> is not specified
	    //defaultVideoWidth: 480,
	    // default if the <video height> is not specified     
	    // overrides <video width>
	    //pluginWidth: -1,
	    // overrides <video height>       
	    //pluginHeight: -1,
	    // rate in milliseconds for Flash and Silverlight to fire the timeupdate event
	    // larger number is less accurate, but less strain on plugin->JavaScript bridge
	    //timerRate: 250,
	    // method that fires when the Flash or Silverlight object is ready
	    success: function (mediaElement, domObject) {
	        mediaElement.addEventListener('timeupdate', function(e) {
	            //document.getElementById('current-time').innerHTML = mediaElement.currentTime;
	        }, false);
	        //mediaElement.play();
	    },
	    // fires when a problem is detected
	    error: function () { 

	    }
	});
	
	/*
	this.video = videojs(this.uniqueName);
	this.video.ready(function() {
		thiss.player = this;
		$("#" + thiss.uniqueName).find(".vjs-control-bar").append("<div class='vjs-time-controls vjs-control vjs-custom'></div>");
		thiss.semihide($("#" + thiss.uniqueName));
		
		thiss.player.on('seeking', function(e) {
		});
		
		thiss.player.on("loadedmetadata", function() {
			thiss.player.play();
			if(!thiss.autoplay)
				thiss.player.pause();
			thiss.semihide($("#" + thiss.uniqueName));
			thiss.player.userActive(true);
			
			thiss.video.thumbnails([]);
			if((thiss.thumbPath != undefined) && (thiss.thumbPath != "")) {
				$.ajax({
					url: 	thiss.poster,
					type: 	'HEAD',
					error: function() {
					},
					success: function() {
						var duration = thiss.player.duration();
						var thumbnails = new Object();
						var thumbEach = 5;
						for(var thumbIndex = 0 ; thumbIndex < ceil(duration / thumbEach) ; thumbIndex++) {
							thumbnails[thumbEach*thumbIndex] = new Object();
							thumbnails[thumbEach*thumbIndex].src = thiss.thumbPath + "_" + (thumbIndex+1) + ".jpg";
						}
						thiss.video.thumbnails(thumbnails);
					}
				});
			}
		});
	});
	
	$("#" + thiss.uniqueName).mouseover(function() {
		thiss.semishow($(this));
	});
	$("#" + thiss.uniqueName).mouseleave(function() {
		thiss.semihide($(this));
	});
	$("#" + thiss.uniqueName).click(function() {
	});
	$("#" + thiss.uniqueName).removeAttr("tabIndex");
	$("#" + thiss.uniqueName).find("*").removeAttr("tabIndex");
	*/
}

VideoPlayer.prototype.show = function() {
	this.dom.find("#" + this.uniqueName).parent().show();
}
VideoPlayer.prototype.hide = function() {
	this.dom.find("#" + this.uniqueName).parent().hide();
	this.tagOrDoc = undefined;
	this.player.pause();
	this.autoplay = false;
}

VideoPlayer.prototype.play = function(timeCurrentOffset) {
	if(this.player.media.paused)
		this.player.play();
	//this.player.setCurrentTime(timeCurrentOffset);
}
VideoPlayer.prototype.pause = function() {
	if(!this.player.media.paused)
		this.player.pause();
	this.autoplay = false;
}
VideoPlayer.prototype.rewind = function(timeCurrentOffset) {
	//this.player.setCurrentTime(timeCurrentOffset);
	this.pause();
	this.autoplay = false;
}


VideoPlayer.renderControlBarItems = ["vjs-play-control", "vjs-time-divider", "vjs-duration", "vjs-current-time", "vjs-volume-control", "vjs-fullscreen-control", "vjs-mute-control"]
VideoPlayer.previewControlBarItems = ["vjs-volume-control", "vjs-fullscreen-control", "vjs-mute-control"]
VideoPlayer.prototype.semishow = function(obj) {
	if(this.render) {
		$.each(VideoPlayer.renderControlBarItems, function(index, controlBarItem) {
			obj.find("." + controlBarItem).fadeTo(100, 1.);
		});
	}
	else {
		$.each(VideoPlayer.previewControlBarItems, function(index, controlBarItem) {
			obj.find("." + controlBarItem).fadeTo(100, 1.);
		});
	}
}
VideoPlayer.prototype.semihide = function(obj) {
	if(this.render) {
		$.each(VideoPlayer.renderControlBarItems, function(index, controlBarItem) {
			obj.find("." + controlBarItem).fadeTo(100, 0.);
		});
	}
	else {
		$.each(VideoPlayer.previewControlBarItems, function(index, controlBarItem) {
			obj.find("." + controlBarItem).fadeTo(100, 0.);
		});
	}
}






function VideoPlayers() {
	this.visible = false;
	
	this.videos = new Object();
	this.videosCount = 0;
	this.audios = new Object();
	this.audiosCount = 0;
	this.players = new Object();
}


VideoPlayers.prototype.isVisible = function() {
	return this.visible;
}
VideoPlayers.prototype.resize = function(visible) {
	if(visible == true)	 {
		$("#renders").show();
		$("#rendersTabNav").addClass("active");
	}
	else if(visible == false) {
		$("#renders").hide();
		$("#rendersTabNav").removeClass("active");
	}

	this.visible = visible;
	if(this.visible == undefined)	this.visible = $("#renders").is(":visible");
	else							this.firstDisplay = true;
	
	var disposition = "grid";
	if(disposition == "matrix") {
		var audiosHeight = ceil(this.audiosCount/2) * VideoPlayer.audioHeight;
		var videosHeight = $("#renders").height() - audiosHeight;
		var width  = $("#renders").width() / constrain(1, this.videosCount, 2);
		var height = videosHeight / ceil(this.videosCount/2);
		var playerIndex = 0;
		var yOffset = 0;
		$.each(this.videos, function(playerId, player) {
			var y = floor(player.index/2) * height;
			yOffset = max(yOffset, y);
			$("#" + playerId).parent().parent().parent().parent().css("top",  y).css("left", (player.index%2) * width).width(width).height(height).css("z-index", 1);
		});
		yOffset += height;
		var width  = $("#renders").width() / constrain(1, this.audiosCount, 2);
		var height = VideoPlayer.audioHeight;
		$.each(this.audios, function(playerId, player) {
			$("#" + playerId).parent().parent().parent().parent().width(width).height(height).css("top",  floor(player.index/2) * height + yOffset).css("left", (player.index%2) * width).css("z-index", 1);
		});
	}
	else if((disposition == "grid") || (disposition == "pip")) {
		var mainPlayerId = "789D3365279144A9049B6FF118C1CC2CB3362CC6";
		var thumbWidth = 200;
		var mainWidth  = $("#renders").width(), mainHeight = $("#renders").height();
		if(disposition == "grid")
			mainWidth -= thumbWidth;

		var audiothumbsHeight = VideoPlayer.audioHeight;
		var videothumbsHeight = (mainHeight - audiothumbsHeight) / this.videosCount;
		var thumbX = mainWidth, thumbY = 0;
		$.each(this.videos, function(playerId, player) {
			if(playerId == mainPlayerId)
				$("#" + playerId).parent().parent().parent().parent().width(mainWidth).height(mainHeight).css("top",  0).css("left", 0).css("z-index", 1);
			else {
				$("#" + playerId).parent().parent().parent().parent().width(thumbWidth).height(videothumbsHeight).css("top",  thumbY).css("left", thumbX).css("z-index", 1000);
				thumbY += videothumbsHeight;
			}
		});
		$.each(this.audios, function(playerId, player) {
			if(playerId == mainPlayerId)
				$("#" + playerId).parent().parent().parent().parent().width(mainWidth).height(mainHeight).css("top",  0).css("left", 0).css("z-index", 1);
			else {
				$("#" + playerId).parent().parent().parent().parent().width(thumbWidth).height(audiothumbsHeight).css("top",  thumbY).css("left", thumbX).css("z-index", 1000);
				thumbY += audiothumbsHeight;
			}
		});
	}
}
VideoPlayers.prototype.show = function(document) {
	var playerId = document.getMetadata("File->Hash");
	if(document.isAudio()) {
		this.audios[playerId] = new VideoPlayer($("#renders"), playerId, true);
		this.players[playerId] = this.audios[playerId];
		this.players[playerId].loadLocal(document);
		this.players[playerId].index = this.audiosCount;
		this.audiosCount++;
	}
	else {
		this.videos[playerId] = new VideoPlayer($("#renders"), playerId, true);
		this.players[playerId] = this.videos[playerId];
		this.players[playerId].loadLocal(document);
		this.players[playerId].index = this.videosCount;
		this.videosCount++;
	}
	this.resize();
}

VideoPlayers.prototype.play = function(timeCurrentOffset) {
	$.each(this.players, function(playerId, player) {	player.play(timeCurrentOffset);	});
}
VideoPlayers.prototype.pause = function() {
	$.each(this.players, function(playerId, player) {	player.pause();					});
}
VideoPlayers.prototype.rewind = function(timeCurrentOffset) {
	$.each(this.players, function(playerId, player) {	player.rewind(timeCurrentOffset);	});
}
