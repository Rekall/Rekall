//ADAPTED
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

function Timeline() {
	this.timeStart         = moment();
	this.state             = 0;
	this.timeCurrentOffset = 0;
	this.timeCurrent       = 0;
}

Timeline.prototype.play = function(timeCurrentOffset) {
	rekall.videoPlayer.play();
}
Timeline.prototype.pause = function() {
	rekall.videoPlayer.pause();
}
Timeline.prototype.isPaused = function() {
	return rekall.videoPlayer.paused();
}
Timeline.prototype.stop = function() {
	rekall.videoPlayer.stop();
}
Timeline.prototype.rewind = function(timeCurrentOffset) {
	rekall.videoPlayer.rewind();
}

Timeline.prototype.toggle = function() {
	if(this.state)
		this.stop();
	else
		this.play();
}
Timeline.prototype.update = function(_timeCurrent) {
	this.timeCurrent = _timeCurrent;
	this.updateFlattenTimeline();
}
Timeline.prototype.updateFlattenTimeline = function() {
	for (var keySource in rekall.project.sources) {      
		for (var keyDocument in rekall.project.sources[keySource].documents) {
			for (var key in rekall.project.sources[keySource].documents[keyDocument].tags) {
				var tag = rekall.project.sources[keySource].documents[keyDocument].tags[key];
				var progress = 0;
				var timeEndExtended = tag.timeStart + max(2, tag.timeEnd - tag.timeStart);

				if(this.timeCurrent <= tag.timeStart)
					progress = this.timeCurrent - tag.timeStart;
				else if((tag.timeStart < this.timeCurrent) && (this.timeCurrent < timeEndExtended))
					progress = (this.timeCurrent - tag.timeStart) / (timeEndExtended - tag.timeStart);
				else
					progress = undefined;
				
				var dom = tag.flattenTimelineDom;
				if(dom != undefined) {
					if(progress == undefined) {         
						//dom.slideUp();  
						dom.removeClass("docTocome").removeClass("docLive").removeClass("docFaraway");
						dom.find(".flattentimeline_counter").hide();
					}
					else {
						if(timeEndExtended == tag.timeEnd)
							dom.find(".flattentimeline_opacifiant").css("width", constrain(progress, 0, 1)*100 + "%");
						else
							dom.find(".flattentimeline_opacifiant").css("width", "100%");

						if((0 <= progress) && (progress < 1))  
							dom.removeClass("docTocome").removeClass("docFaraway").addClass("docLive");//.slideDown();//("opacity", 1.0).slideDown();   
							//dom.css("opacity", 1.0).slideDown();
						else if((-5 <= progress) && (progress <= 0))
							dom.removeClass("docLive").removeClass("docFaraway").addClass("docTocome");//.slideDown();
							//dom.css("opacity", 0.5).slideDown();
						else if(-9999999 < progress)
							dom.removeClass("docTocome").removeClass("docLive").addClass("docFaraway");//.slideDown();
							//dom.css("opacity", 0.1).slideDown();
						else         
							dom.removeClass("docTocome").removeClass("docLive").removeClass("docFaraway");//.hide();
							//dom.css("opacity", 0.1).hide();
					}
				}
			}
		}
	}
}
