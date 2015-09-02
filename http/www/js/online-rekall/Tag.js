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

function Tag(document) {
	this.document         = document;
	this.documentOriginal = this.document;
	this.version          = document.currentVersion;
	this.versionOriginal  = this.version;
	this.timeStart = 0;
	this.timeEnd   = 1;
	this.color     = "#FFFFFF";
}
Tag.keyToOpenAfterLoading = undefined;


Tag.prototype.getMetadata = function(metadataKey, metadataValue) {
	return this.document.getMetadata(metadataKey, this.version);
}
Tag.prototype.setMetadata = function(metadataKey, metadataValue) {
	return this.document.setMetadata(metadataKey, metadataValue, this.version);
}
Tag.prototype.getMetadatas = function() {
	return this.document.getMetadatas(this.version);
}
Tag.prototype.isVideoOrAudio = function() {
	return this.document.isVideoOrAudio(this.version); 
}
Tag.prototype.isAudio = function() {
	return this.document.isAudio(this.version); 
}
Tag.prototype.isVideo = function() {
	return this.document.isVideo(this.version); 
}
Tag.prototype.isMarker = function() {
	return this.document.isMarker(this.version); 
}
Tag.prototype.isImage = function() {
	return this.document.isImage(this.version); 
}


Tag.prototype.getTimeStart = function() {
	return this.timeStart;
}
Tag.prototype.getTimeEnd = function() {
	return this.timeEnd;
}
Tag.prototype.setTimeStart = function(val) {
	if(!isNaN(val))
		this.timeStart = val;
}
Tag.prototype.setTimeEnd = function(val) {
	if(!isNaN(val))
		this.timeEnd = val;
}



Tag.prototype.isGoodVersion = function() {
	return this.version == this.document.goodVersion;
}


Tag.prototype.openFile = function() {
	return this.document.openFile();
}
Tag.prototype.openFinder = function() {
	return this.document.openFinder();
}
Tag.prototype.openQuickLook = function() {
	return this.document.openQuickLook();
}
Tag.prototype.downloadFile = function() {
	return this.document.downloadFile();
}
Tag.prototype.openBrowser = function() {
	return this.document.openBrowser();
}   
Tag.prototype.openPopupEdit = function() {
	fillPopupEdit(this);  
}

Tag.prototype.update = function(color, strong) {
	if(color != undefined) {
		this.colorRaw  = color;
		this.color     = color.toString();
		
		this.isMarkerCache = this.isMarker();
		/*
		if(this.isMarkerCache) {
			this.visuel.rect.setCornerRadius(0);
		}
		else {
			this.visuel.rect.setCornerRadius(Tag.tagHeight/3);
		}
		*/
	}
	if(this.color == undefined)
		this.color = "#000000";

	var fillColor   = '';
	var strokeColor = '';
	var strokeWidth = 0;
	var opacity = 1;
		
	if(this.isGoodVersion()) {
		fillColor   = this.color;
		strokeWidth = 0;
		strokeColor = "";
	}
	else {
		fillColor   = tinycolor(this.colorRaw.toString()).setAlpha(0.1).toString();
		strokeColor = this.color;
		strokeWidth = 0.8;
	}
	
	/*
	if((this.selected) || (this.document.selected > 0)) {
		strokeColor = '#FFFFFF';
		if(this.selected) {
			if(strong)		strokeWidth = 2;
			else			strokeWidth = 1.5;
		}
		else
			strokeWidth       = 0.8;
	}
	
	if(this.isMarkerCache) {
		if(strokeColor == "")
			this.visuel.rect.setStroke(fillColor);	
		else
			this.visuel.rect.setStroke(strokeColor);
		this.visuel.rect.setStrokeWidth(max(1, strokeWidth));
	}
	else {
		this.visuel.rect.setFill       (fillColor);
		this.visuel.rect.setStroke     (strokeColor);	
		this.visuel.rect.setStrokeWidth(strokeWidth);
	}
	this.visuel.rect.setOpacity(opacity);
	*/
}