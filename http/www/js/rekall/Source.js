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

function Source(hash) {
	this.documents = new Object();
	this.mapping = new Object();
}

Source.prototype.addDocument = function(document) {
	var key = this.mapPath(Utils.getLocalFilePath(document, "", true));
	if(key == "")
		key = CryptoJS.SHA1(moment() + random() + "").toString();

	if(this.documents[key] == undefined) {
		if(document.tags.length == 0) {
			var tag = new Tag(document);
			document.addTag(tag);
		}
		document.metadatas[-1] = document.cloneMetadatas();
		
		this.documents[key] = document;
		document.key = key;
		if(document.getMetadata("Rekall->Media Function") == "Render")
			rekall.captationVideoPlayers.show(document);
	}
	else
		this.documents[key].addVersion(document);
}
Source.prototype.getDocument = function(path) {
	var retour = undefined;
	for (var keyDocument in this.documents) {
		for (var key in this.documents[keyDocument].tags) {
			if((retour == undefined) && (Utils.getLocalFilePath(this.documents[keyDocument].tags[key]).toLowerCase() == path.toLowerCase()))
				retour = this.documents[keyDocument].tags[key].document;
		}
	}
	return retour;
}

Source.prototype.mapPath = function(path) {
	var retour = path;
	while(this.mapping[retour] != undefined)
		retour = this.mapping[retour];
	return retour;
}
Source.prototype.addMapping = function(path1, path2) {
	var path1Exists = false, path2Exists = false;;
	for (var pathSrc in this.mapping) {
		var pathDst = this.mapping[pathSrc];
		if((pathSrc == path1) || (pathDst == path1))
			path1Exists = true;
		if((pathSrc == path2) || (pathDst == path2))
			path2Exists = true;
	}
	if(!((path1Exists) && (path2Exists))) {
		if((this.mapping[path1] != undefined) && (this.mapping[path2] == undefined))
			this.mapping[path2] = path1;
		else
			this.mapping[path1] = path2;
	}
}