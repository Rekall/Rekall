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

function Document() {
	this.tags      		   = new Array();
	this.metadatas		   = new Array();
	this.currentVersion    = 0;
	this.goodVersion       = 0;
	this.setMetadata("Rekall->Group", "");
	this.setMetadata("Rekall->Visibility", "");
}
Document.prototype.addTag = function(tag) {
	this.tags.push(tag);
}

Document.prototype.initMetadatas = function(version) {
	if(version == undefined)
		version = this.currentVersion;
	if(this.metadatas[version] == undefined)
		this.metadatas[version] = new Metadata();
	return version;
}
Document.prototype.getMetadatas = function(version) {
	version = this.initMetadatas(version);
	return this.metadatas[version];
}
Document.prototype.setMetadatas = function(metadatas, version) {
	version = this.initMetadatas();
	this.metadatas[version] = metadatas;
}
Document.prototype.isVideoOrAudio = function(version) {
	return this.isAudio(version) || this.isVideo(version); 
}
Document.prototype.isAudio = function(version) {
	var type = this.getMetadata("Rekall->Type", version);
	if(type == undefined)
		return false;
	else
		return type.startsWith("audio"); 
}
Document.prototype.isVideo = function(version) {
	var type = this.getMetadata("Rekall->Type", version);
	if(type == undefined)
		return false;
	else
		return type.startsWith("video"); 
}
Document.prototype.isMarker = function(version) {
	var type = this.getMetadata("Rekall->Type", version);
	if(type == undefined)
		return true;
	else
		return (type == "rekall/marker"); 
}
Document.prototype.isImage = function(version) {
	var type = this.getMetadata("Rekall->Type", version);
	if(type == undefined)
		return false;
	else
		return type.startsWith("image"); 
}
Document.prototype.isLink = function(version) {
	var type = this.getMetadata("Rekall->Type", version);
	if(type == undefined)
		return false;
	else
		return type.startsWith("rekall/link"); 
}


Document.availableMetadataKeys = new Object();
Document.prototype.getMetadata = function(metadataKey, version) {
	if(metadataKey == "")
		return "";
	if(this.getMetadatas(version)[metadataKey] != undefined)
		return this.getMetadatas(version)[metadataKey];
	else
		return undefined;
}
Document.prototype.setMetadata = function(metadataKey, metadataValue, version) {
	var currentValue = "";
	if(metadataKey instanceof Metadata) {
		metadataValue = metadataKey.content + "";
		metadataKey   = metadataKey.metadataKey + "";
	}
	if((!metadataKey.toLowerCase().startsWith("exiftool")) && (metadataValue.toLowerCase().indexOf("use -b option to extract") === -1)) {
		currentValue = this.getMetadatas(version)[metadataKey];
		this.getMetadatas(version)[metadataKey] = metadataValue;
	
		if(Document.availableMetadataKeys[metadataKey] == undefined)
			Document.availableMetadataKeys[metadataKey] = {count: 0};
		Document.availableMetadataKeys[metadataKey].count++;
	}
	return (currentValue != metadataValue);
}

Document.prototype.compareMetadatas = function(metadatas) {
	if(metadatas == undefined)
		metadatas = this.getMetadatas(-1);
		
	var metadatasCompared = new Metadata();
	var metadatasSource   = this.getMetadatas();
	for (var metadataKey in metadatas) {
		if(metadatasSource[metadataKey] != metadatas[metadataKey])
			metadatasCompared[metadataKey] = metadatas[metadataKey];
	}
	return metadatasCompared;
}
Document.prototype.cloneMetadatas = function(version) {
	var metadatasCloned = new Metadata();
	for (var metadataKey in this.getMetadatas(version))
		metadatasCloned[metadataKey] = this.getMetadatas(version)[metadataKey];
	return metadatasCloned;
}


Document.prototype.addVersion = function(document) {
	var thisVersion = this.currentVersion;
	this.currentVersion++;
	this.goodVersion++;
	this.setMetadatas(document.getMetadatas(), this.currentVersion);
	if(document.tags.length == 0) {
		for (var index in this.tags) {
			var tag = this.tags[index];
			if(tag.version == thisVersion) {
				var newTag = new Tag(this);
				newTag.timeStart = tag.getTimeStart();
				newTag.timeEnd   = tag.getTimeEnd();
				this.addTag(newTag);
			}
		}
	}
	else {
		for (var index in document.tags) {
			var tag = document.tags[index];
			tag.version = this.currentVersion;
			tag.document = this;
			this.tags.push(tag);
		}
	}
}

Document.prototype.checkRender = function() {
	rekall.captationVideoPlayers.show(this, (this.getMetadata("Rekall->Media Function") == "Render"));
}


Document.prototype.openFile = function() {
	$.ajax(Utils.getLocalFilePath(this, "open"));
}
Document.prototype.openFinder = function() {
	$.ajax(Utils.getLocalFilePath(this, "finder"));
}
Document.prototype.openQuickLook = function() {
	$.ajax(Utils.getLocalFilePath(this, "quicklook"));
}
Document.prototype.openBrowser = function() {
	window.open(Utils.getLocalFilePath(this, "file"), "_blank");
}
Document.prototype.downloadFile = function() {
	window.open(Utils.getLocalFilePath(this, "download"), "_blank");
}

function Metadata() {
}