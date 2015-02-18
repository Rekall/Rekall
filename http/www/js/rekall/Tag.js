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

Tag.tagHeight = 6;

function Tag(document) {
	this.document         = document;
	this.documentOriginal = this.document;
	this.version          = document.currentVersion;
	this.versionOriginal  = this.version;
	this.timeStart = 0;
	this.timeEnd   = 1;
	
	this.isSelectable = true;
	this.selected = false;
	this.rect = new Rect();

	this.visuel = new Object();
	this.visuel.rect = new Kinetic.Rect({
		transformsEnabled: 'position',
		listening: 			false,
	});
	rekall.timeline.tagLayer.group.add(this.visuel.rect);
	this.visuel.tag = this;
}
Tag.prototype.updatePosititon = function() {
	this.visuel.rect.setPosition(this.rect.getPosition());
	this.visuel.rect.setSize(this.rect.getSize());
	this.visuel.rect.setVisible(true);
	//this.visuel.rect.setVisible(false);
}


Tag.prototype.intersectsAll = function(tags) {
	for (var key in tags)
		if((this != tags[key]) && (this.rect.intersects(tags[key].rect)))
			return true;
	return false;
}

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



Tag.prototype.setVisible = function(val) {
	this.visuel.rect.setVisible(val);
}
Tag.prototype.isVisible = function() {
	return this.visuel.rect.getVisible();
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


Tag.prototype.setSelected = function(val, strong) {
	this.selected = val;
	if(this.selected)
		this.document.selected = rekall.selectionId;
	else if(this.document.selected != rekall.selectionId)
		this.document.selected = 0;

	for (var key in this.document.tags)
		this.document.tags[key].update(undefined, false);
	this.update(undefined, strong);
}
Tag.prototype.update = function(color, strong) {
	if(color != undefined) {
		this.colorRaw  = color;
		this.color     = color.toString();
		
		this.isMarkerCache = this.isMarker();
		if(this.isMarkerCache) {
			this.visuel.rect.setCornerRadius(0);
		}
		else {
			this.visuel.rect.setCornerRadius(Tag.tagHeight/3);
		}
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
	
	if((rekall.map.isVisible()) && ($.inArray(this, rekall.map.filtredTags) === -1)) {
		opacity *= 0.5;
	}
	if((rekall.panner.isVisible()) && ($.inArray(this, rekall.panner.filtredTags) === -1)) {
		opacity *= 0.5;
	}
	
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
}
Tag.prototype.isSelected = function() {
	return this.selected;
}




Tag.metadataSorting        = new Sorting();
Tag.metadataSortingVisible = {"Rekall":{'visible': true}};
Tag.metadataCancel 		   = false;
Tag.metadataEditionKey     = [];//"Rekall->Comments"
Tag.displayMetadataEditing = "";
Tag.prototype.displayMetadata = function() {
	if((!Tags.isStrong) && (this.isSelectable)) {
		Tags.addOne(this);
		Tag.displayMetadata();
	}
}
Tag.displayMetadata = function() {	
	//Restaure un truc propre
	if(Tags.count() == 0) {
		$("#metadatas").hide();  
		$("#metadatas_tools").hide();  
		$("#flattentimeline").show();
	}
	else {
		$("#metadatas").show(); 
		$("#metadatas_tools").show(); 
		$("#flattentimeline").hide();
	}

		
	//Titre
	if(Tags.count() == 1) {
		//Preview
		if(Tags.unique().isAudio()) {
			rekall.previewVideoPlayer.loadLocal(Tags.unique(), $("#inspecteur").is(":visible") && (Tags.isStrong));
			$("#previewImage").hide();
			$("#previewVideo").show();
			$("#preview_menu_bar").hide();
		}
		else {
			if((Tags.unique().thumbnail != undefined) && (Tags.unique().thumbnail.url != undefined)) {
				if(($("#previewImage img").attr("src") != Tags.unique().thumbnail.url) && (Tags.unique().thumbnail.url != undefined))
					$("#previewImage").html("<img src=\"" + Tags.unique().thumbnail.url + "\">");
				$("#previewImage").show();
				//$("#preview_menu_bar").show();
				$("#preview_menu_bar").hide();
			}
			else {
				$("#previewImage").hide();
				$("#preview_menu_bar").hide();
			}
			rekall.previewVideoPlayer.hide();
		}
		

		$("#metadatas_title").text(Tags.unique().getMetadata("Rekall->Name"));
		var version = "";
		if(Tags.unique().document.currentVersion > 0) {
			version += "version " + (Tags.unique().version+1);
			if(!Tags.unique().isGoodVersion())
				version += "<br/><span>set this version as best one</span>";
			
			$("#metadatas_version").slider({
				value: 	Tags.unique().version,
				min: 	0,
				max: 	Tags.unique().document.currentVersion,
				step: 	1,
				slide: function(event, ui) {
					for (var index in Tags.unique().document.tags) {
						var tag = Tags.unique().document.tags[index];
						if(tag.version == ui.value) {
							Tags.addOne(tag, true);
							Tag.displayMetadata();
							return;
						}
					}
				}
			});
			$("#metadatas_version .ui-slider-handle").show();
		}
		else {
			$("#metadatas_version").slider();
			$("#metadatas_version .ui-slider-handle").hide();
		}
		if((Tags.unique().getMetadata(rekall.sortings["colors"].metadataKey) != "") && (version != ""))
			$("#metadatas_subtitle").html(Tags.unique().getMetadata(rekall.sortings["colors"].metadataKey) + " — " + version);
		else if(Tags.unique().getMetadata(rekall.sortings["colors"].metadataKey) != "")
			$("#metadatas_subtitle").html(Tags.unique().getMetadata(rekall.sortings["colors"].metadataKey));
		else if(version != "")
			$("#metadatas_subtitle").html(version);
		else
			$("#metadatas_subtitle").html("");
		$("#inspecteur")      .css("background-color", Tags.unique().color);
		$("#inspecteurTabNav").css("background-color", Tags.unique().color);
		if(Tags.unique().getMetadata("File->File Name") != undefined) {
			if(rekall_common.isLocal) {
				$("#metadatas_menu_bar_open").show();
				$("#metadatas_menu_bar_finder").show();
				$("#metadatas_menu_bar_quicklook").show();
				$("#metadatas_menu_bar_open_browser").hide();
				$("#metadatas_menu_bar_download").hide();
			}
			else {
				$("#metadatas_menu_bar_open").hide();
				$("#metadatas_menu_bar_finder").hide();
				$("#metadatas_menu_bar_quicklook").hide();
				$("#metadatas_menu_bar_open_browser").show();
				$("#metadatas_menu_bar_download").show();

				$("#metadatas_menu_bar_open_browser a").attr("href", Utils.getLocalFilePath(Tags.unique(), "file"));
				$("#metadatas_menu_bar_download a")	   .attr("href", Utils.getLocalFilePath(Tags.unique(), "download"));
			}
		}
		else {
			$("#metadatas_menu_bar_open").hide();
			$("#metadatas_menu_bar_finder").hide();
			$("#metadatas_menu_bar_quicklook").hide();
			$("#metadatas_menu_bar_open_browser").hide();
			$("#metadatas_menu_bar_download").hide();
		}
		
		$("#metadatas_subtitle span").click(function() {
			Tags.unique().document.goodVersion = Tags.unique().version;
			rekall.analyse(true);
			Tag.displayMetadata();
			rekall.projectChanged("<edition key=\"" + Utils.escapeHtml(Tags.unique().document.key) + "\" version=\"" + Tags.unique().version + "\" goodVersionIs=\"" + Tags.unique().document.goodVersion + "\" />\n");
		});
		$("#metadatas_menu_bar_open").click(function() {
			Tags.unique().openFile();
		});
		$("#metadatas_menu_bar_finder").click(function() {
			Tags.unique().openFinder();
		});
		$("#metadatas_menu_bar_quicklook").click(function() {
			Tags.unique().openQuickLook();
		});
		var originalMetadatas = Tags.unique().document.compareMetadatas();
	}
	else if(Tags.count() > 1) {
		$("#metadatas_title")   .text("MULTIPLE SELECTION");
		$("#metadatas_subtitle").text("");
		$("#inspecteur")      .css("background-color", "#2C3032");
		$("#inspecteurTabNav").css("background-color", "#2C3032");
	}
	if(Tags.count() != 1) {
		$("#metadatas_version").slider();
		$("#metadatas_version .ui-slider-handle").hide();
		$("#metadatas_menu_bar_open").hide();
		$("#metadatas_menu_bar_finder").hide();
		$("#metadatas_menu_bar_quicklook").hide();
		$("#previewImage").hide();
		$("#previewVideo").hide();
		$("#preview_menu_bar").hide();
		$("#inspecteur")      .css("background-color", "#2C3032");
		$("#inspecteurTabNav").css("background-color", "#EC8960");
		rekall.previewVideoPlayer.hide();
	}
	
	//Position GPS
	var gpsPositions = new Array();
	for (var index in Tags.selectedTags)
		if(Tags.selectedTags[index].gpsPosition != undefined)
			gpsPositions.push(Tags.selectedTags[index].gpsPosition);
	if((gpsPositions.length > 0) && ($("#mapTabNav").is(":visible"))) {
		$("#metadatas_menu_bar_map").show();
		$("#metadatas_menu_bar_map").click(function() {
			$("#mapTabNav").trigger("click");
		});
	}
	else
		$("#metadatas_menu_bar_map").hide();
		
	if((Tags.count() == 1) && (Tags.unique().isImage())) {
		$("#metadatas_menu_bar_zoom").show();
		$("#metadatas_menu_bar_zoom").click(function() {
			$("#pannerTabNav").trigger("click");
		});
	}
	else
		$("#metadatas_menu_bar_zoom").hide();
		
	if(Tags.count() == 0) {
		rekall.map.show();
		rekall.panner.show();
	}
	else if(!Tags.isStrong) {
		rekall.map.show(undefined, Tags.unique());
		rekall.panner.show([Tags.unique()]);
	}
	else {
		rekall.map.show(gpsPositions, undefined, true);
		rekall.panner.show(Tags.selectedTags);
	}

	
	//Metadatas du groupe
	Tag.metadataSorting.analyseStart();
	for (var index in Tags.selectedTags)
		for (var metadataKeysStr in Tags.selectedTags[index].document.getMetadatas())
			Tag.metadataSorting.analyseAdd(Tags.selectedTags[index], metadataKeysStr);
	Tag.metadataSorting.analyseEnd();

	//Sortie HTML
	var html = "<div id='metadatas_table_smalltitle'>— Metadata —</div>";
	var htmls = new Object();
	$("#metadatas_table").html(html);
	for (var key in Tag.metadataSorting.categories) {
		var metadataSortingCategory = Tag.metadataSorting.categories[key];

		var metadataKeys = metadataSortingCategory.categoryVerbose.split("->");
		if(htmls[metadataKeys[0]] == undefined) {
			htmls[metadataKeys[0]] = "";
			htmls[metadataKeys[0]] += "<div class='metadatas_table_category'>";
			htmls[metadataKeys[0]] += "		<div class='metadatas_table_category_title'>" + metadataKeys[0] + "</div>";
			var display = "invisible";
			if((Tag.metadataSortingVisible[metadataKeys[0]] != undefined) && (Tag.metadataSortingVisible[metadataKeys[0]].visible))
				display = "visible";
			htmls[metadataKeys[0]] += "		<div class='metadatas_table_category_elements " + display + "'>";
		}
		var metadataKey = metadataSortingCategory.category.substr(1);

		//Analyse des valeurs
		var value = undefined;
		var elementClass = "";
		for (var key in metadataSortingCategory.tags) {
			var newValue = metadataSortingCategory.tags[key].getMetadata(metadataKey);
			if(value == undefined)
				value = newValue;
			else if((newValue != undefined) && (newValue != value)) {
				elementClass = "differentValues";
				continue;
			}
		}

		//Clef
		var key    = metadataKeys[1];
		var keyTip = (key).replace(/["']/g, "");
		if((originalMetadatas != undefined) && (originalMetadatas[metadataKey] != undefined)) {
			key += "*";
			keyTip += " (different from original)";
		}

		//Préparation de l'affichage
		if(value == "")			value = "&nbsp;";
		if(elementClass != "")	value = "Multiple values";
		if(value != undefined) {
			htmls[metadataKeys[0]] += "				<div draggable=true class='metadatas_table_element selectable " + elementClass + "'>";
			htmls[metadataKeys[0]] += "					<div class='metadatas_table_element_category invisible'>" + metadataKey + "</div>";
			htmls[metadataKeys[0]] += "					<div class='metadatas_table_element_key'   title='" + keyTip + "'>" + key + "</div>";
			htmls[metadataKeys[0]] += "					<div class='metadatas_table_element_value' title='" + value.replace(/'/g, "\\'") + "'>" + value + "</div>";
			htmls[metadataKeys[0]] += "					<div class='metadatas_table_element_value_editor unselectable invisible'></div>";
			htmls[metadataKeys[0]] += "				</div>";
		}
	}
	for (var key in htmls)
		html += htmls[key] + "</div></div></div>";
	$("#metadatas_table").html(html);

	//Clic pour déploiement d'une section
	$("#metadatas .metadatas_table_category_title").click(function() {
		var category = $(this).text();
		if(Tag.metadataSortingVisible[category] == undefined)
			Tag.metadataSortingVisible[category] = new Object();
		Tag.metadataSortingVisible[category].visible = !$(this).parent().find('.metadatas_table_category_elements').is(":visible");
		if(Tag.metadataSortingVisible[category].visible)
			$(this).parent().find('.metadatas_table_category_elements').show();
		else
			$(this).parent().find('.metadatas_table_category_elements').hide();
	});
	
	
	//Fonction temporaire de validation de l'édition d'un élément
	function changeValueTo(obj, metadataKey, metadataValue) {
		var projectChangedXml = "";
		if((obj != undefined) && (!Tag.metadataCancel)) {
			var changed = false;
			var reopen  = false;
			if(obj.hasClass("one_selection")) {
				if(metadataValue == undefined)
					metadataValue = obj.parent().find(".selected").text();
				if(metadataValue == "—")
					metadataValue = "";
				if(metadataKey.toLowerCase().indexOf("date/time") !== -1) {
					var metadataValueTmp = moment(metadataValue);
					if(metadataValueTmp.isValid())
						metadataValue = metadataValueTmp.format("YYYY:MM:DD HH:mm:ss");
				}
				
				for (var index in Tags.selectedTags) {
					var tag = Tags.selectedTags[index];
					var hasChanged = tag.setMetadata(metadataKey, metadataValue.trim());
					changed |= hasChanged;
					if(hasChanged)
						projectChangedXml += "<edition key=\"" + Utils.escapeHtml(tag.document.key) + "\" version=\"" + tag.version + "\" metadataKey=\"" + Utils.escapeHtml(metadataKey) + "\" metadataValue=\"" + Utils.escapeHtml(metadataValue.trim()) + "\" />\n";
				}
			}
			else {
				if(metadataValue)
					reopen = true;
					
				//Parsing des valeurs à ajouter et à virer
				var keywordsToAdd = new Object(), keywordsToRemove = new Object();
				if(!metadataValue)
					metadataValue = obj.parent().find("input, textarea").val();
				var splits = Utils.splitKeywords(metadataValue);
				for (var index in splits)
					keywordsToAdd[splits[index]] = true;
				obj.parent().find(".added").each(function() {
					keywordsToAdd[$(this).text().trim()] = true;
				});
				obj.parent().find(".selected").each(function() {
					keywordsToAdd[$(this).text().trim()] = true;
				});
				obj.parent().find(".removed").each(function() {
					keywordsToRemove[$(this).text().trim()] = true;
				});
				
				//Applique les changements
				for (var index in Tags.selectedTags) {
					var tag = Tags.selectedTags[index];
					var keywords = Utils.splitKeywords(tag.getMetadata(metadataKey));
					for (var keyword in keywordsToAdd) {
						if($.inArray(keyword, keywords) === -1)
							keywords.push(keyword);
					}
					for (var keyword in keywordsToRemove) {
						if($.inArray(keyword, keywords) !== -1)
							keywords.splice(keywords.indexOf(keyword), 1);
					}
					var hasChanged = tag.setMetadata(metadataKey, Utils.joinKeywords(keywords));
					changed |= hasChanged;
					if(hasChanged)
						projectChangedXml += "<edition key=\"" + Utils.escapeHtml(tag.document.key) + "\" version=\"" + tag.version + "\" metadataKey=\"" + Utils.escapeHtml(metadataKey) + "\" metadataValue=\"" + Utils.escapeHtml(Utils.joinKeywords(keywords)) + "\" />\n";
				}
			}
		}

		//Ferme tous les éditeurs
		$("#metadatas_table").find(".metadatas_table_element_value")       .show();
		$("#metadatas_table").find(".metadatas_table_element_value_editor").hide();
		$("#metadatas_table").find(".metadatas_table_element_value_editor").html("");

		if(!reopen)
			Tag.metadataEditionKey = [/*"Rekall->Comments"*/];
		if(changed) {
			rekall.analyse();
			rekall.projectChanged(projectChangedXml);
		}
	}


	//Affichage de l'éditeur
	$("#metadatas .metadatas_table_element").click(function(event) {
		event.stopPropagation();
		var metadataKey = $(this).find(".metadatas_table_element_category").text();
		
		if((!($(this).find(".metadatas_table_element_value_editor").is(":visible"))) && (!metadataKey.startsWith("File->"))) {
			//Ferme tous les autres éditeurs au cas où…
			$("#metadatas_table").find(".metadatas_table_element_value")       .show();
			$("#metadatas_table").find(".metadatas_table_element_value_editor").hide();
			$("#metadatas_table").find(".metadatas_table_element_value_editor").html("");
			
			//Classe
			var valueClass = "one_selection";
			var shouldSplit = false;
			if((metadataKey == "Rekall->Keywords") || (metadataKey == "Rekall User Infos->Applications running")) {
				shouldSplit = true;
				valueClass = "";
			}
			
			//Autocompletion possible
			var availableTags = new Array();
			if(metadataKey != "Rekall->Comments") {
				var availableTagsCount = new Object();
				if(metadataKey == "Rekall->Media Function") {
					availableTagsCount["Contextual"] = {count: 1};
					availableTagsCount["Render"]	 = {count: 1};
				}
				if(metadataKey == "Rekall->Visibility") {
					availableTagsCount["Visible on timeline"] = {count: 1};
					availableTagsCount["Hidden on timeline"]  = {count: 1};
				}
				for (var keySource in rekall.project.sources) {
					for (var keyDocument in rekall.project.sources[keySource].documents) {
						for (var key in rekall.project.sources[keySource].documents[keyDocument].tags) {
							var splits = Utils.splitKeywords(rekall.project.sources[keySource].documents[keyDocument].tags[key].getMetadata(metadataKey), undefined, shouldSplit);
							for (var index in splits) {
								var val = splits[index];
								if(val.trim() != "") {
									if(availableTagsCount[val] == undefined) {
										availableTagsCount[val] = new Object();
										availableTagsCount[val].count = 0;
									}
									availableTagsCount[val].count++;
								}
							}
						}
					}
				}
				for (var i in availableTagsCount)
					availableTags.push(i);
				availableTags.sort(function(a, b) {
					if (availableTagsCount[a].count > availableTagsCount[b].count)	return -1;
					if (availableTagsCount[a].count < availableTagsCount[b].count)	return 1;
			  		return 0;
				});
				availableTags = availableTags.slice(0, 30);
			}


			//Analyse des valeurs
			var values = new Object();
			var value  = undefined;
			var isDifferentValues = false;
			for (var key in Tag.metadataSorting.categories[Sorting.prefix + metadataKey].tags) {
				var newValue = Tag.metadataSorting.categories[Sorting.prefix + metadataKey].tags[key].getMetadata(metadataKey);
				var splits = Utils.splitKeywords(newValue, undefined, shouldSplit);
				for (var index in splits) {
					var newValue = splits[index];
					if(values[newValue] == undefined) {
						values[newValue] = new Object()
						values[newValue].count = 0;
					}
					values[newValue].count++;
				}
				if(value == undefined)
					value = newValue;
				else if((newValue != undefined) && (newValue != value))
					isDifferentValues = true;
			}


			//Editeur
			var editor = "";

			if(metadataKey == "Rekall->Comments")
				editor = "<textarea class='" + valueClass + "'>" + value + "</textarea><p style='clear: left;'>Press Enter to validate</p>";
			else {
				editor = "<div class='metadatas_table_element_value_editor_bubbles'>";
				
				//Valeurs déjà existantes
				var count = 0;
				for (var value in values) {
					var valueClass2 = "";
					if(values[value].count == Tags.count())
						valueClass2 = "single";
					if(value != "") {
						editor += "<div title='Value of one document in the multiple selection' class='values " + valueClass + " " + valueClass2 + "'>" + value + "</div>";
						count++;
					}
				}

				//Valeurs proposées
				for (var index in availableTags) {
					if(values[availableTags[index]] == undefined) {
						editor += "<div title='Suggested value' class='autocompleteValues " + valueClass + "'>" + availableTags[index] + "</div>";				
						count++;
					}
				}
				
				if((valueClass != "") && (originalMetadatas != undefined) && (originalMetadatas[metadataKey] != undefined)) {
					var originalValue = originalMetadatas[metadataKey];
					var originalValueTip = "Original value";
					if(originalValue == "") {
						originalValue = "—";
						originalValueTip += " was empty";
					}
					editor += "<div title='" + originalValueTip + "' class='originalValues " + valueClass + "'>" + originalValue + "</div>";				
				}

				//Edition manuelle
				editor += "			<input type='text' class='" + valueClass + "' placeholder='" + ((count)?("or type a new value"):("Type a value")) + "'/>";
				if(valueClass != "one_selection")
					editor += "		<span class='metadatas_table_element_value_editor_bubbles_ok " + valueClass + "'>Save&nbsp;changes</span>";
				editor += "</div><p style='clear: left;'>&nbsp;</p>";
			}
				
			//HTML
			$(this).find(".metadatas_table_element_value_editor").html(editor);
			$(this).find(".metadatas_table_element_value").hide();
			$(this).find(".metadatas_table_element_value_editor").show();
			Tag.metadataCancel     = false;
			if($.inArray(metadataKey, Tag.metadataEditionKey) === -1)
				Tag.metadataEditionKey.push(metadataKey);

			$(this).find(".metadatas_table_element_value_editor_bubbles_ok").click(function(event) {
				event.stopPropagation();
				changeValueTo($(this), metadataKey);
			});
			$(this).find(".metadatas_table_element_value_editor_bubbles div").click(function(event) {
				event.stopPropagation();
				var thiss = $(this);
				if($(this).hasClass("one_selection")) {
					$(this).addClass("selected");
					setTimeout(function() { changeValueTo(thiss, metadataKey); }, 200);
				}
				else if($(this).hasClass("autocompleteValues")) {
					if($(this).hasClass("selected"))
						$(this).removeClass("selected");
					else
						$(this).addClass("selected");
				}
				else if($(this).hasClass("single")) {
					if($(this).hasClass("removed")) {
						$(this).removeClass("removed");
					}
					else {
						$(this).addClass("removed");
					}
				}
				else if($(this).hasClass("values")) {
					if($(this).hasClass("removed")) {
						$(this).removeClass("removed");
					}
					else if($(this).hasClass("added")) {
						$(this).removeClass("added");
						$(this).addClass("removed");
					}
					else {
						$(this).addClass("added");
					}
				}
			});
			function inputBlur(obj) {
				changeValueTo(obj, metadataKey, obj.val());
			}
			$(this).find("input, textarea").focus();
			$(this).find("input, textarea").select();
			$(this).find("input, textarea").keyup(function(event) {
				event.stopPropagation();
			  	event.preventDefault();
			    if(event.keyCode == 13)
					inputBlur($(this));
				else if(event.keyCode == 27) {
					Tag.metadataCancel = true;
					inputBlur($(this));
				}
			});

			//Initialisation de l'autocomplétion
			function extractLast(term, really) {
				if(really)	return Utils.splitKeywords(term, undefined, shouldSplit).pop();
				else 		return term;
			}
			/*
			var normalize = function(term) {
				var ret = "";
				var accentMap = {
					"á": "a",
					"ö": "o"
				};
				for(var i = 0; i < term.length; i++)
					ret += accentMap[ term.charAt(i) ] || term.charAt(i);
				return ret;
			};

			$( "#developer" ).autocomplete({
				source: function( request, response ) {
					var matcher = new RegExp( $.ui.autocomplete.escapeRegex( request.term ), "i" );
					response( $.grep( names, function( value ) {
						value = value.label || value.value || value;
						return matcher.test( value ) || matcher.test( normalize( value ) );
						}) );
					}
			});*/			
			$(this).find("input, textarea").bind("keydown", function(event) {
				if(event.keyCode === $.ui.keyCode.TAB && $(this).autocomplete("instance").menu.active)
					event.preventDefault();
			})
			$(this).find("input").autocomplete({
				minLength: 0,
				source: function(request, response) {
					response($.ui.autocomplete.filter(availableTags, extractLast(request.term, false)));
				},
			});
			$(this).find("textarea").autocomplete({
				minLength: 0,
				source: function(request, response) {
					response($.ui.autocomplete.filter(availableTags, extractLast(request.term, true)));
				},
				focus: function() {
					return false;
				},
				select: function( event, ui ) {
					var terms = Utils.splitKeywords(this.value, undefined, shouldSplit);
					terms.pop();
					terms.push(ui.item.value);
					terms.push("");
					this.value = terms.join(", ");
					return false;
				}
			});
			$(this).find(".metadatas_table_element_key").click(function(event) {
				if($(this).parent().find(".metadatas_table_element_value_editor").is(":visible")) {
					Tag.metadataCancel = true;
					event.stopPropagation();
					changeValueTo();
				}
			});
			$(this).find("input, textarea").trigger("autocompleteselect");
		}
	});
	$("#metadatas .metadatas_table_element").on({
		dragstart: function(event) {
            event.dataTransfer.setData("metadataKey", $(this).find(".metadatas_table_element_category").text());
			$("#navigateur .navigateur_tab .dropable").addClass("dragStart");
		},
		dragend: function(event) {
			$("#navigateur .navigateur_tab .dropable").removeClass("dragStart");
			$("#navigateur .navigateur_tab .dropable").removeClass("dragEnter");
			$("#navigateur .navigateur_tab .dropable").removeClass("dragOver");
		}
	});
	$("#metadatas .metadatas_table_element .metadatas_table_element_category").filter(function() { return $.inArray($(this).text(), Tag.metadataEditionKey) !== -1; }).trigger("click");
	
	rekall.analyse(false);
}
