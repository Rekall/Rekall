//ADATPED sans analyse()
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

function Project(url) {
	this.sources = new Object();
	this.url = url;
	this.firstAnalysis = true;
}

Project.prototype.addDocument = function(key, document) {
	if(this.sources[key] == undefined)
		this.sources[key] = new Source();
	this.sources[key].addDocument(document);
}
Project.prototype.getDocument = function(path) {
	var retour = undefined;
	for (var key in this.sources) {
		if(retour == undefined)
			retour = this.sources[key].getDocument(path);
	}
	return retour;
}

Project.prototype.loadXML = function(xml) {
	this.sources["Files"] = new Source();
	/*
	this.sources["Files"].addMapping("file:///Users/guillaume/Documents/Rekall/Walden/Enregistrements/infinitespaces-test8H264.mov", "file:///Users/guillaume/Documents/Rekall/Walden/Test.txt");
	this.sources["Files"].addMapping("file:///Users/guillaume/Documents/Rekall/Walden/Test.txt", "file:///Users/guillaume/Documents/Rekall/Walden/Enregistrements/infinitespaces-test8H264.mov");
	this.sources["Files"].addMapping("file:///Users/guillaume/Documents/Rekall/Walden/Technique/trad auto.doc", "file:///Users/guillaume/Documents/Rekall/Walden/Enregistrements/infinitespaces-test8H264.mov");
	*/
	
	var thiss = this;
	var counts = {documents: 0, tags: 0, metadatas: 0};
	
	xml.find('document').each(function() {
		if(($(this).attr("remove") != undefined) && ($(this).attr("key") != undefined) && ($(this).attr("remove") == "true")) {
			var rekallDoc = thiss.sources["Files"].documents[$(this).attr("key")];
			if(rekallDoc != undefined) {
				for (var tagIndex in rekallDoc.tags)
					rekallDoc.tags[tagIndex].visuel.rect.remove();
				delete rekall.project.sources["Files"].documents[rekallDoc.key];
			}
		}
		else {
			var rekallDoc = new Document();
			counts.documents++;
			counts.tags++;
			$(this).find('meta').each(function() {
				var rekallDocMeta = new Metadata();
				rekallDocMeta.content 	  = $(this).attr('cnt');
				rekallDocMeta.metadataKey = $(this).attr('ctg');
				rekallDoc.setMetadata(rekallDocMeta);
				counts.metadatas++;
			});
			if($(this).attr("key") != undefined)
				rekallDoc.key = $(this).attr("key");
			thiss.addDocument("Files", rekallDoc);
		}
	});
	xml.find('edition').each(function() {
		var key = $(this).attr('key');
		if(thiss.sources["Files"].documents[key] != undefined) {
			var version       = $(this).attr('version');
			var metadataKey   = $(this).attr('metadataKey');
			var metadataValue = $(this).attr('metadataValue');
			thiss.sources["Files"].documents[key].setMetadata(metadataKey, metadataValue, version);
		}
	});
	xml.find('tag').each(function() {
		var key = $(this).attr('key');
		if(thiss.sources["Files"].documents[key] != undefined) {
			var version   = $(this).attr('version');
			var timeStart = parseFloat($(this).attr('timeStart')) + 0.;
			var timeEnd   = parseFloat($(this).attr('timeEnd'))   + 0.;
			for (var index in thiss.sources["Files"].documents[key].tags) {
				thiss.sources["Files"].documents[key].tags[index].setTimeStart(timeStart);
				thiss.sources["Files"].documents[key].tags[index].setTimeEnd(timeEnd);
			}
		}
	});
	
	console.log(counts.documents + " documents analysés, " + counts.metadatas + " métadonnées extraites et " + counts.tags + " tags affichés !");
	this.analyse();
}

Project.prototype.timelineUpdate = function() {
	
}

Project.prototype.analyse = function() {
	$('#flattentimeline').html("");

	//Analyse
	Tags.flattenTimelineTags = [];
	var filtredTags = new Array();
	rekall.sortings["horizontal"].analyseStart();
	rekall.sortings["colors"]    .analyseStart();
	for (var keySource in this.sources) {      
		for (var keyDocument in this.sources[keySource].documents) {
			for (var key in this.sources[keySource].documents[keyDocument].tags) {
				var tag = this.sources[keySource].documents[keyDocument].tags[key];
				rekall.sortings["horizontal"].analyseAdd(tag);
				rekall.sortings["colors"]    .analyseAdd(tag);
				Tags.flattenTimelineTags.push(tag);
			}
		}
	}
	rekall.sortings["horizontal"].analyseEnd();
	rekall.sortings["colors"]    .analyseEnd();
	Tags.flattenTimelineTags.sort(function(a, b) {
		if(a.timeStart < b.timeStart) return -1;
		if(a.timeStart > b.timeStart) return 1;
		return 0;
	});

	//Affichage
	var categories = rekall.sortings["horizontal"].categories;
	if(rekall.sortings["horizontal"].metadataKey == "Time")
		categories = {time: {tags: Tags.flattenTimelineTags}};
	
	//Affectation des couleurs
	for (var key in rekall.sortings["colors"].categories) {
		var colorSortingCategory = rekall.sortings["colors"].categories[key];
		for (var key in colorSortingCategory.tags) {
			var tag = colorSortingCategory.tags[key];
			tag.update(colorSortingCategory.color);
			tag.isSelectable = colorSortingCategory.checked;

			//Analyse de vignettes
			/*
			if(true) {
				var thumbUrl = undefined
				if((tag.getMetadata("File->Thumbnail") != undefined) && (tag.getMetadata("File->Thumbnail") != "")) {
					var thumbUrl = Utils.getPreviewPath(tag);

					if(tag.isVideo())	thumbUrl += "_1.jpg";
					else				thumbUrl +=  ".jpg";
				}
				tag.thumbnail = {url: thumbUrl, tag: tag};

				if(rekall.panner.thumbnails[colorSortingCategory.category] == undefined)
					rekall.panner.thumbnails[colorSortingCategory.category] = {category: colorSortingCategory, thumbnails: [], documents: []};
				rekall.panner.thumbnails[colorSortingCategory.category].thumbnails.push(tag.thumbnail);
			}
			*/
		}
	}

	//Tags / catégories
	for (var key in categories) {
		$.each(categories[key].tags, function(index, tag) {
			//Analyse de vignettes
			if(true) {
				var thumbUrl = undefined
				if((tag.getMetadata("File->Thumbnail") != undefined) && (tag.getMetadata("File->Thumbnail") != "")) {
					var thumbUrl = Utils.getPreviewPath(tag);

					if(tag.isVideo())	thumbUrl += "_1.jpg";
					else				thumbUrl +=  ".jpg";
				}
				tag.thumbnail = {url: thumbUrl, tag: tag};
			}
			
			//Dom
			$('#flattentimeline').append(function() {
				var styleColor = "background-color: " + tag.color + ";";
				var textColor = "color: " + tag.color + ";";

				/*var styleColor = "background-image: -webkit-linear-gradient(left, #000 0%, " + tag.color + " 100%);";*/
				var styleColor2 = styleColor;//"background-color: #3EA8B1;";
				var styleImage = "";
				if(tag.thumbnail.url != undefined) {
					styleImage = "background-image: url(" + tag.thumbnail.url + ");";//" opacity: 0.5;";
					/*styleColor += "opacity: 0.25;"; */
				} else styleImage = "background-color: rgba(0,0,0,.9)";

				var icnType = "";
				var tmpType = tag.getMetadata("Rekall->Type");
				if(tmpType.indexOf("application/msword") >=0 ) 		icnType = "background-image:url(css/images/icn-word.png);";
				else if(tmpType.indexOf("application/pdf") >=0 ) 	icnType = "background-image:url(css/images/icn-pdf.png);";
				else if(tmpType.indexOf("application/") >=0 ) 		icnType = "background-image:url(css/images/icn-document.png);";
				else if(tmpType.indexOf("audio/") >=0 ) 			icnType = "background-image:url(css/images/icn-music.png);";
				else if(tmpType.indexOf("image/") >=0 ) 			icnType = "background-image:url(css/images/icn-image.png);";
				else if(tmpType.indexOf("text/x-vcard") >=0 ) 		icnType = "background-image:url(css/images/icn-user.png);";
				else if(tmpType.indexOf("text/") >=0 ) 				icnType = "background-image:url(css/images/icn-document.png);";
				else if(tmpType.indexOf("video/") >=0 ) 			icnType = "background-image:url(css/images/icn-video.png);";

				//alert(icnType);

				var html = "<div draggable=true class='flattentimeline_item'>";
				html 	+= "<div class='flattentimeline_image'      style='" + styleImage + "'></div>";
				/*html 	+= "<div class='flattentimeline_color'      style='" + styleColor + "'></div>";*/

				/*html 	+= "<div class='flattentimeline_bar'        style=''></div>";*/

				html 	+= "<div class='flattentimeline_opacifiant' style='" + styleColor2 + "'></div>";

				/*html 	+= "<div class='flattentimeline_counter'      style='" + styleColor2 + "'></div>";*/

				html 	+= "<div class='flattentimeline_type'		style='" + icnType +"' title='" + tmpType + "'></div>";
			/*	html 	+= "<div class='flattentimeline_color'      style='" + styleColor + "' title='" + tag.getMetadata(rekall.sortings["colors"].metadataKey) + "'></div>";
				html 	+= "<div class='flattentimeline_type'		style='" + styleColor + "' title='" + tag.getMetadata(rekall.sortings["colors"].metadataKey) + "'></div>";*/
				html 	+= "<div class='flattentimeline_title' title='" + tag.getMetadata("Rekall->Name") + "'>" + tag.getMetadata("Rekall->Name") + "</div>";
				/*html 	+= "<div class='flattentimeline_subtitle'>" + tag.getMetadata(rekall.sortings["colors"].metadataKey) + "</div>";*/
				html    += "</div>";

				tag.flattenTimelineDom = $(html);
				tag.flattenTimelineDom.click(function(event) {
					//Tags.addOne(tag, true);
					//Tag.displayMetadata();
					//alert(Utils.getLocalFilePath(this, "file"));	return;
					console.log(tag);
					tag.openBrowser();
				});
				tag.flattenTimelineDom.on({
					dragstart: function(event) {
			            event.dataTransfer.setData("key", 	  tag.document.key);
			            event.dataTransfer.setData("version", tag.version);
					}
				});
				return tag.flattenTimelineDom;
			});
		});
	}
}
