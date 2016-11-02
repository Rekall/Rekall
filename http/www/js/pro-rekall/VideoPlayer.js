/*
    This file is part of Rekall.
    Copyright (C) 2013-2015

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
	this.webbasedPlayer = true;
	if(dom == undefined)
		this.webbasedPlayer = false;
	this.render		  = render;
	this.uniqueName   = uniqueName;
	this.thumbPath    = "";
	this.poster       = "";
	this.dom          = dom;
	if(this.webbasedPlayer)
		this.dom.append("<div class='videoItem invisible'><video width='100%' preload='metadata' id='" + this.uniqueName + "' class='video-js vjs-default-skin' controls></video></div>");
	this.activate();
	this.hide();
	this.autoplay    = false;
}
VideoPlayer.audioHeight = 40;
VideoPlayer.renderControlBarItems = ["vjs-play-control", "vjs-time-divider", "vjs-duration", "vjs-current-time", "vjs-volume-control", "vjs-fullscreen-control", "vjs-mute-control"]
VideoPlayer.previewControlBarItems = ["vjs-volume-control", "vjs-fullscreen-control", "vjs-mute-control"]
VideoPlayer.prototype.loadLocal = function(tagOrDoc, play) {
	if(this.webbasedPlayer) {
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
				this.player.poster(this.poster);
				this.player.src(this.url);
				this.show();
			}
		}
		else if(this.autoplay)
			this.play();
	}
	else {
		if(this.tagOrDoc != tagOrDoc) {
			this.tagOrDoc = tagOrDoc;
			this.url = Utils.getLocalFilePath(this.tagOrDoc);
			this.show();
		}
	}
}

VideoPlayer.prototype.activate = function() {
	if(this.webbasedPlayer) {
		var thiss = this;
		videojs(this.uniqueName, {
			"techOrder": ["html5"],
			"controls": true,
			"autoplay": false,
			"loop": 	"false",
			"preload": 	"auto"
		}, function() {
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
			
				//thiss.video.thumbnails([]);
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
			
			thiss.player.on("durationchange", function(e) {
			});
			thiss.player.on("ended", function(e) {
			});
			thiss.player.on("error", function(e) {
			});
			thiss.player.on("firstplay", function(e) {
			});
			thiss.player.on("fullscreenchange", function(e) {
			});
			thiss.player.on("loadedalldata", function(e) {
			});
			thiss.player.on("loadeddata", function(e) {
			});
			thiss.player.on("loadedmetadata", function(e) {
			});
			thiss.player.on("loadstart", function(e) {
			});
			thiss.player.on("pause", function(e) {
			});
			thiss.player.on("play", function(e) {
			});
			thiss.player.on("progress", function(e) {
			});
			thiss.player.on("seeked", function(e) {
			});
			thiss.player.on("seeking", function(e) {
			});
			thiss.player.on("timeupdate", function(e) {
			});
			thiss.player.on("volumechange", function(e) {
			});
			thiss.player.on("waiting", function(e) {
			});
			thiss.player.on("resize", function(e) {
			});
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
			
				//thiss.video.thumbnails([]);
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
		*/
	
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
	}
}

VideoPlayer.prototype.show = function() {
	if(this.webbasedPlayer) {
		this.dom.find("#" + this.uniqueName).parent().show();
	}
	else if(this.tagOrDoc != undefined) {
		$.ajax(Utils.getLocalFilePath(this.tagOrDoc, "video/show") + "?friendlyName=" + this.tagOrDoc.getMetadata("Rekall->Name"));
	}
}
VideoPlayer.prototype.hide = function() {
	if(this.webbasedPlayer) {
		this.dom.find("#" + this.uniqueName).parent().hide();
		this.tagOrDoc = undefined;
		if(this.player)
			this.player.pause();
		this.autoplay = false;
	}
	else if(this.tagOrDoc != undefined) {
		$.ajax(Utils.getLocalFilePath(this.tagOrDoc, "video/hide"));
	}
}

VideoPlayer.prototype.play = function(timeCurrentOffset) {
	if(this.webbasedPlayer) {
		if(this.player.paused())
			this.player.play();
		this.player.currentTime(timeCurrentOffset);
	}
	else {
		$.ajax(rekall.project.url + "video/play?timecode=" + round(timeCurrentOffset*1000));
	}
}
VideoPlayer.prototype.pause = function() {
	if(this.webbasedPlayer) {
		if(!this.player.paused())
			this.player.pause();
		this.autoplay = false;
	}
	else {
		$.ajax(rekall.project.url + "video/pause");
	}
}
VideoPlayer.prototype.rewind = function(timeCurrentOffset) {
	if(this.webbasedPlayer) {
		this.player.currentTime(timeCurrentOffset);
		this.pause();
		this.autoplay = false;
	}
	else {
		$.ajax(rekall.project.url + "video/rewind?timecode=" + round(timeCurrentOffset*1000));
	}
}
VideoPlayer.prototype.semishow = function(obj) {
	if(this.webbasedPlayer) {	
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
}
VideoPlayer.prototype.semihide = function(obj) {
	if(this.webbasedPlayer) {	
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
			$("#" + playerId).css("top",  y).css("left", (player.index%2) * width).width(width).height(height).css("z-index", 1);
		});
		yOffset += height;
		var width  = $("#renders").width() / constrain(1, this.audiosCount, 2);
		var height = VideoPlayer.audioHeight;
		$.each(this.audios, function(playerId, player) {
			$("#" + playerId).width(width).height(height).css("top",  floor(player.index/2) * height + yOffset).css("left", (player.index%2) * width).css("z-index", 1);
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
				$("#" + playerId).width(mainWidth).height(mainHeight).css("top",  0).css("left", 0).css("z-index", 1);
			else {
				$("#" + playerId).width(thumbWidth).height(videothumbsHeight).css("top",  thumbY).css("left", thumbX).css("z-index", 1000);
				thumbY += videothumbsHeight;
			}
		});
		$.each(this.audios, function(playerId, player) {
			if(playerId == mainPlayerId)
				$("#" + playerId).width(mainWidth).height(mainHeight).css("top",  0).css("left", 0).css("z-index", 1);
			else {
				$("#" + playerId).width(thumbWidth).height(audiothumbsHeight).css("top",  thumbY).css("left", thumbX).css("z-index", 1000);
				thumbY += audiothumbsHeight;
			}
		});
	}
}
VideoPlayers.prototype.show = function(document, visibility) {
	return;
	
	var dom = $("#renders");
	//Local usage of video
	dom = undefined;
	var playerId = document.getMetadata("File->Hash");
	if((this.players[playerId] == undefined) && (visibility)) {
		if(document.isAudio()) {
			this.audios[playerId] = new VideoPlayer(dom, playerId, true);
			this.players[playerId] = this.audios[playerId];
			this.players[playerId].loadLocal(document);
			this.players[playerId].index = this.audiosCount;
			this.audiosCount++;
		}
		else {
			this.videos[playerId] = new VideoPlayer(dom, playerId, true);
			this.players[playerId] = this.videos[playerId];
			this.players[playerId].loadLocal(document);
			this.players[playerId].index = this.videosCount;
			this.videosCount++;
		}
	}
	else if((this.players[playerId] != undefined) && (!visibility)) {
		this.players[playerId].hide();
		if(document.isAudio())
			this.audios[playerId] = this.players[playerId] = undefined;
		else
			this.videos[playerId] = this.players[playerId] = undefined;
	}
	if(dom != undefined)
		this.resize();
}


VideoPlayers.prototype.play = function(timeCurrentOffset) {
	$.each(this.players, function(playerId, player) {	if(player != undefined)	player.play(timeCurrentOffset);	});
}
VideoPlayers.prototype.pause = function() {
	$.each(this.players, function(playerId, player) {	if(player != undefined)	player.pause();					});
}
VideoPlayers.prototype.rewind = function(timeCurrentOffset) {
	$.each(this.players, function(playerId, player) {	if(player != undefined)	player.rewind(timeCurrentOffset);	});
}
