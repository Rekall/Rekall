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
	$.each(this.sources, function(key, source) {
		if(retour == undefined)
			retour = source.getDocument(path);
	});
	return retour;
}

Project.prototype.loadXML = function(xml) {
	this.sources["Files"] = new Source();
	this.sources["Files"].addMapping("file:///Users/guillaume/Documents/Rekall/Walden/Enregistrements/infinitespaces-test8H264.mov", "file:///Users/guillaume/Documents/Rekall/Walden/Test.txt");
	this.sources["Files"].addMapping("file:///Users/guillaume/Documents/Rekall/Walden/Test.txt", "file:///Users/guillaume/Documents/Rekall/Walden/Enregistrements/infinitespaces-test8H264.mov");
	this.sources["Files"].addMapping("file:///Users/guillaume/Documents/Rekall/Walden/Technique/trad auto.doc", "file:///Users/guillaume/Documents/Rekall/Walden/Enregistrements/infinitespaces-test8H264.mov");
	
	var thiss = this;
	var counts = {documents: 0, tags: 0, metadatas: 0};
	xml.find('document').each(function() {
		var rekallDoc = new Document();
		counts.documents++;
		/*
		$(this).find('tags').each(function() {
			$(this).find('tag').each(function() {
				var rekallDocTag = new Tag(rekallDoc);
				rekallDocTag.timeStart 		 = $(this).attr('timeStart');
				rekallDocTag.timeEnd 		 = $(this).attr('timeEnd');
				rekallDoc.addTag(rekallDocTag);
				counts.tags++;
			});
		});
		*/
		$(this).find('meta').each(function() {
			var rekallDocMeta = new Metadata();
			rekallDocMeta.content 	  = $(this).attr('cnt');
			rekallDocMeta.metadataKey = $(this).attr('ctg');
			rekallDoc.setMetadata(rekallDocMeta);
			counts.metadatas++;
		});
		thiss.addDocument("Files", rekallDoc);
	});
	xml.find('edition').each(function() {
		var key           = $(this).attr('key');
		var version       = $(this).attr('version');
		var metadataKey   = $(this).attr('metadataKey');
		var metadataValue = $(this).attr('metadataValue');
		thiss.sources["Files"].documents[key].setMetadata(metadataKey, metadataValue, version);
	});
	xml.find('tag').each(function() {
		var key       = $(this).attr('key');
		var version   = $(this).attr('version');
		var timeStart = $(this).attr('timeStart') + 0.;
		var timeEnd   = $(this).attr('timeEnd')   + 0.;
		$.each(thiss.sources["Files"].documents[key].tags, function(index, tag) {
			tag.setTimeStart(parseFloat(timeStart));
			tag.setTimeEnd(parseFloat(timeEnd));
		});
	});
	/*
	xml.find('event').each(function() {
	});
	*/
	console.log(counts.documents + " documents analysés, " + counts.metadatas + " métadonnées extraites et " + counts.tags + " tags affichés !");
	rekall.analyse();
}



Project.prototype.analyse = function(full) {
	if(full != false)	console.log("Analyse complète des fichiers…");
	else 				console.log("Analyse partielle des fichiers…");
	
	//Filtrage des tags
	if(full != false) {
		var filtredTags = new Array();
		rekall.sortings["groups"]    .analyseStart();
		rekall.sortings["authors"]   .analyseStart();
		rekall.sortings["types"]     .analyseStart();
		rekall.sortings["keywords"]  .analyseStart();
		rekall.sortings["search"]    .analyseStart();
		rekall.sortings["corpus"]    .analyseStart();
		rekall.sortings["horizontal"].analyseStart();
		rekall.sortings["vertical"].analyseStart();
		Tags.byTime = [];
	    $.each(this.sources, function(key, source) {
		    $.each(source.documents, function(key, document) {
			    $.each(document.tags, function(key, tag) {
					Tags.byTime.push(tag);
					
					//Filtrage
					var isOk = true;
					if((false) && (!tag.isGoodVersion()))
						isOk = false;
				
					//Recherche forcée
					if(isOk) {
						var isOkTmp = false;
						$.each(tag.document.getMetadatas(), function(metadataKey, metadataValue) {
							isOkTmp |= rekall.sortings["search"].analyseAdd(tag, metadataValue, false, Sorting.prefix + metadataKey.replace("->", " / "));
							if((metadataValue != "") && (metadataValue != undefined))
								isOkTmp |= rekall.sortings["search"].analyseAdd(tag, metadataKey, false, Sorting.prefix + metadataKey.replace("->", " / "));
						});
						isOk &= isOkTmp;
					}

					//Filtres par défault
					if(isOk) {
						//Listing de mots-clefs
						var keywords = Utils.splitKeywords(tag.getMetadata("Rekall->Keywords"), false);
						var isOkTmp = false;
						$.each(keywords, function(index, keyword) {
							isOkTmp |= rekall.sortings["keywords"].analyseAdd(tag, keyword);
						});
						isOk &= isOkTmp;

						isOk &= rekall.sortings["authors"] .analyseAdd(tag);
						isOk &= rekall.sortings["types"]   .analyseAdd(tag);
						if((tag.getMetadata("Rekall->Folder") != undefined) && (tag.getMetadata("File->File Name") != undefined))
							isOk &= rekall.sortings["corpus"]  .analyseAdd(tag, tag.getMetadata("Rekall->Folder") + tag.getMetadata("File->File Name"));
						else if(tag.getMetadata("Rekall->File Name") != undefined)
							isOk &= rekall.sortings["corpus"]  .analyseAdd(tag, tag.getMetadata("File->File Name"));

						if(isOk)	isOk &= rekall.sortings["horizontal"].analyseAdd(tag);
						if(isOk)	isOk &= rekall.sortings["groups"]    .analyseAdd(tag);
						if(isOk)	isOk &= rekall.sortings["vertical"]  .analyseAdd(tag);
					}


					//Réinit graphique
					tag.update(Sorting.defaultColor);
					if(isOk) {
						filtredTags.push(tag);
						tag.setVisible(true);
					}
					else
						tag.setVisible(false);
				});
			});
		});
		rekall.sortings["vertical"]  .analyseEnd();
		rekall.sortings["groups"]    .analyseEnd();
		rekall.sortings["authors"]   .analyseEnd();
		rekall.sortings["types"]     .analyseEnd();
		rekall.sortings["corpus"]    .analyseEnd();
		rekall.sortings["horizontal"].analyseEnd();
		rekall.sortings["keywords"]  .analyseEnd();
		rekall.sortings["search"]    .analyseEnd();

	
		//Extractions d'infos sur le GPS, les vignettes
		rekall.sortings["colors"]   .analyseStart();
		rekall.sortings["highlight"].analyseStart();
		rekall.sortings["hashes"]   .analyseStart();
		rekall.map.gpsPositions = new Array();
		rekall.panner.thumbnails = new Array();
	    $.each(filtredTags, function(key, tag) {
			//Analyse GPS
			var gpsPosition = {latitude: NaN, longitude: NaN, tag: undefined};
			$.each(tag.getMetadatas(), function(key, meta) {
				if((key.toLowerCase().indexOf("gps") > -1) && (meta.indexOf(",") > -1)) {
					var metaPos = meta.split(",");
					gpsPosition.latitude  = parseFloat(metaPos[0]);
					gpsPosition.longitude = parseFloat(metaPos[1]);
					gpsPosition.tag	      = tag;
				}
			});
			if((!isNaN(gpsPosition.latitude)) && (!isNaN(gpsPosition.longitude))) {
				tag.gpsPosition = gpsPosition;
				rekall.map.gpsPositions.push(tag.gpsPosition);
			}
			else
				tag.gpsPosition = undefined;

			//Analyse de vignettes
			if((tag.getMetadata("File->Thumbnail") != undefined) && (tag.getMetadata("File->Thumbnail") != "")) {
				if(!tag.isVideoOrAudio()) {
					tag.thumbnail = {url: Utils.getPreviewPath(tag) + ".jpg", tag: tag};
					if(tag.isImage())
						rekall.panner.thumbnails.push(tag.thumbnail);
				}
			}
			else
				tag.thumbnail = undefined
									
			var isOk = true;
			rekall.sortings["colors"].analyseAdd(tag, undefined, true);
			rekall.sortings["hashes"].analyseAdd(tag);
			if(rekall.sortings["highlight"].metadataSearch != "")
				rekall.sortings["highlight"].analyseAdd(tag);

			if(isOk) {
				filtredTags.push(tag);
				tag.setVisible(true);
			}
			else
				tag.setVisible(false);

		});
		rekall.sortings["colors"]   .analyseEnd();
		rekall.sortings["highlight"].analyseEnd();
		rekall.sortings["hashes"]   .analyseEnd(1);
	}
	

	//Cases à cocher ont changées
	if(full != false) {
		/*
		var availableMetadatas = new Array();
		for (var i in Document.availableMetadataKeys)
			availableMetadatas.push(i);
		availableMetadatas.sort(function(a, b) {
			if (Document.availableMetadataKeys[a].count > Document.availableMetadataKeys[b].count)	return -1;
			if (Document.availableMetadataKeys[a].count < Document.availableMetadataKeys[b].count)	return 1;
	  		return 0;
		});
		//availableMetadatas = availableMetadatas.slice(0, 30);
		console.log(availableMetadatas);
		*/
		var availableMetadatas = new Array();
		for (var i in Document.availableMetadataKeys)
			availableMetadatas.push(i);
		availableMetadatas.sort();
		
		var availableMetadatas2 = new Array();
		$.each(availableMetadatas, function(index, metadata) {
			if(metadata.startsWith("Rekall->"))
				availableMetadatas2.push(metadata);
		});
		$.each(availableMetadatas, function(index, metadata) {
			if(!metadata.startsWith("Rekall->"))
				availableMetadatas2.push(metadata);
		});
		
		var availableMetadatasSorted = new Object();
		$.each(availableMetadatas2, function(index, metadata) {
			var metadataSplit = metadata.split("->");
			if(availableMetadatasSorted[metadataSplit[0]] == undefined)
				availableMetadatasSorted[metadataSplit[0]] = new Array();
			availableMetadatasSorted[metadataSplit[0]].push(metadataSplit[1]);
		});

		var availableMetadatasHtml = "";
		$.each(availableMetadatasSorted, function(category, metadatas) {
			if(category == "Rekall")
				availableMetadatasHtml += "<li><b>" + category + "</b>";
			else
				availableMetadatasHtml += "<li>" + category + "s";
			availableMetadatasHtml += "<ul>";
			$.each(metadatas, function(index, metadata) {
				var metadataKey = category + "->" + metadata;
				availableMetadatasHtml += "<li metadataKey='" + metadataKey + "'>" + metadata + "</li>";
			});
			availableMetadatasHtml += "</ul>";
			availableMetadatasHtml += "</li>";
		});
		
		$.each(rekall.sortings, function(keyword, sorting) {
			var extraChoice = "";
			if(keyword == "horizontal")
				extraChoice = "<li metadataKey='Time'>Time</li>"
			htmlDom  = $("#" + keyword + "Tab .tab_list");
			var metadataConfigFormated = sorting.metadataConfigStr;
			if(metadataConfigFormated != undefined) {
				metadataConfigFormated = metadataConfigFormated.replace("->", "&nbsp;&#x25B8;&nbsp;");
				metadataConfigFormated = metadataConfigFormated.replace("|",  "&nbsp;&#x25B9;&nbsp;");
			}
			$("#" + keyword + "Tab .tab_choice").html("<div metadataKey='" + sorting.metadataConfigStr + "' class='tab_choice_toggle'>" + metadataConfigFormated + " <span class='invisible'>(change)</span></div><ul class='tag_metadatas_menu invisible' id='" + keyword + "Menu' sorting='" + keyword + "'>" + extraChoice + availableMetadatasHtml + "</ul>");
			
			sortingVerbose = keyword;
			if(htmlDom.get(0)) {
				var html = "", postHtml = "";
				htmlDom.html(html + postHtml);
				if((sorting != rekall.sortings["search"]) || ((sorting == rekall.sortings["search"]) && (rekall.sortings["search"].metadataSearch != ""))) {
					$.each(sorting.categories, function(key, category) {
						var value        = category.category;
						var percentage   = category.tags.length / category.tagsSize
						var valueVerbose = category.categoryVerbose;
						var valueDisplayed = "<span>" + valueVerbose + "</span>";

						var percentageDiv = "";
						if(percentage >= 0.01)
							percentageDiv = "<div class='tab_list_item_percentage' style='color:rgba(255, 255, 255, " + map(percentage, 0, 1, 0.1, 1) + ")'>" + round(100*percentage) + "%</div>";
						else if(percentage > 0.001)
							percentageDiv = "<div class='tab_list_item_percentage' style='color:rgba(255, 255, 255, " + map(percentage, 0, 1, 0.1, 1) + ")'>-</div>";

						if(value == Sorting.prefix) {
							valueDisplayed = "Not specified";
							postHtml += "<div class='tab_list_item " + ((!category.visible)?("invisible'"):("visible")) + "'><label><input class='tab_list_item_check' type='checkbox'" + ((category.checked)?("checked"):("")) + "/><span></span>" + valueDisplayed + "</label>" + percentageDiv + "<div class='tab_list_item_category invisible'>" + value + "</div></div>";
						}
						else {
							if(sorting == rekall.sortings["colors"])
								valueDisplayed = "<span><span style='background-color: " + category.color + ";'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>&nbsp;" + valueVerbose + "</span>"
							else if((sorting == rekall.sortings["corpus"]) || (sorting == rekall.sortings["search"])) {
								var values = valueVerbose.split("/");
								valueDisplayed = "";
								if(sorting == rekall.sortings["corpus"])
									percentageDiv = "";
									/*
								if(values.length == 0)
									valueDisplayed += "<span>" + values[valuesIndex] + "</span>";
									*/
								for(var valuesIndex = 0 ; valuesIndex < values.length ; valuesIndex++) {
									if(valuesIndex == values.length-1)
										valueDisplayed += "<span>" + values[valuesIndex] + "</span>";
									else
										valueDisplayed += values[valuesIndex] + "/";
								}
							}
							html += "<div class='tab_list_item " + ((!category.visible)?("invisible'"):("visible")) + "'><label><input class='tab_list_item_check' type='checkbox'" + ((category.checked)?("checked"):("")) + "/><span></span>" + valueDisplayed + "</label>" + percentageDiv + "<div class='tab_list_item_category invisible'>" + value + "</div></div>";
						}
					});
				}
				htmlDom.html(html + postHtml);
			}
		});
	
		$(".tag_metadatas_menu").menu();
		$(".tab_choice_toggle").click(function() {
			$(this).next().toggle();
			if($(this).attr("metadataKey")) {
				var dom = $(this).next().find("li[metadataKey='" + $(this).attr("metadataKey") + "']");
				dom.parent().parent().trigger("click");
				dom.parent().find("li").removeClass("ui-state-focus");
				dom.addClass("ui-state-focus");
			}
			
		});
		$(".tag_metadatas_menu li.ui-menu-item").click(function() {
			var metadataKey = $(this).attr("metadataKey");
			if(metadataKey != "Time") {
				var sorting = $(this).parent().parent().parent().attr("sorting");
				if((metadataKey != undefined) && (metadataKey != "") && (sorting != undefined))
					rekall.sortings[sorting].setCriterias(metadataKey, rekall.sortings[sorting].valCanBeFloats, undefined, true);
			}
			else {
				var sorting = $(this).parent().attr("sorting");
				rekall.sortings[sorting].setCriterias(metadataKey, rekall.sortings[sorting].valCanBeFloats, undefined, true);
			}
		});
		if(this.firstAnalysis) {
			$("#tag_workspaces_save").click(function() {
				var friendlyName = prompt("Please type a name for this preset", "My preset");
				if((friendlyName != null) && (friendlyName != "")) {
					$("#tag_workspaces_menu").append("<li action='horizontal=" + rekall.sortings["horizontal"].metadataConfigStr + " ; vertical=" + rekall.sortings["vertical"].metadataConfigStr + " ; groups=" + rekall.sortings["groups"].metadataConfigStr + "'>" + friendlyName + "</li>");
					$("#tag_workspaces_menu").menu("refresh");
				}
			});
			$("#tag_workspaces_menu").menu();
			$("#tag_workspaces_menu li").first().trigger("click");
		}
		
		$("#tag_workspaces_menu li").unbind("click");
		$("#tag_workspaces_menu li").click(function() {
			var actions = $(this).attr("action");
			$("#tag_workspaces_menu").find("li").removeClass("selected");
			$(this).addClass("selected");
			if(actions != undefined) {
				var actionsList = actions.split(";");
				$.each(actionsList, function(index, action) {
					var actionList = action.split("=");
					if(action.length > 1) {
						var sorting = actionList[0].trim();
						var metadataKey = actionList[1].trim();
						rekall.sortings[sorting].setCriterias(metadataKey, rekall.sortings[sorting].valCanBeFloats, undefined, true);
					}
				});
			}
		});
		$("#tag_workspaces_menu li").unbind("dblclick");
		$("#tag_workspaces_menu li").dblclick(function() {
			var friendlyName = prompt("Please type a name for this preset", $(this).text());
			if((friendlyName != null) && (friendlyName != "")) {
				$(this).text(friendlyName);
			}
		});

		//Actions sur le cochage
		$(".tab_list_item input").click(function(event) {
			var sorting  = $(this).parent().parent().parent().parent().parent().attr("id").replace("Tab", "");
			sorting = rekall.sortings[sorting]
			var category = $(this).parent().parent().find(".tab_list_item_category").text();
			var value = $(this).prop('checked'), valueBefore = !value;
			if(event.shiftKey) {
				$.each(sorting.categories, function(key, sortingCategory) {
					if(sortingCategory.category == category)	sortingCategory.checked = value;
					else										sortingCategory.checked = !value;
				});
			}
			else		
				sorting.categories[category].checked = value;
			rekall.analyse();
		});
		
		//Action sur la recherche
		$(".tab_search input").keyup(function(event) {
			event.stopPropagation();
			var searchText = $(this).val().toLowerCase();
			var sorting    = $(this).parent().parent().parent().attr("id").replace("Tab", "");
			if((sorting == "search") || (sorting == "highlight"))
				rekall.sortings[sorting].setCriterias(rekall.sortings[sorting].metadataConfigStr, rekall.sortings[sorting].valCanBeFloats, searchText, false);
			else {
				sorting = rekall.sortings[sorting];
				$(this).parent().parent().parent().find('.tab_list_item').each(function(index) {
					var category = $(this).find(".tab_list_item_category").text();
					if((searchText == "") || ($(this).text().toLowerCase().indexOf(searchText) > -1)) {
						sorting.categories[category].visible = true;
						$(this).show();
					}
					else {
						sorting.categories[category].visible = false;
						$(this).hide();
					}
				});
			}
			rekall.analyse();
		});
	}

		
	//Etiquettes horizontales et verticales à refaire
	if(full != false) {
		rekall.timeline.gridLayer.group.destroyChildren();
		rekall.timeline.timeLayer.group.destroyChildren();
	}
	//Étiquettes horizontales
	var alternate = 0;
	$.each(rekall.sortings["horizontal"].categories, function(key, horizontalSortingCategory) {
		var horizontalSortingCategoryPos = rekall.sortings["horizontal"].positionFor(undefined, horizontalSortingCategory.index);
		if(horizontalSortingCategory.rectAlternate == undefined) {
			horizontalSortingCategory.rectAlternate = new Kinetic.Rect({
				fill: 		'#4D5355',
				listening: 	false,
			});
		}
		if(full != false)
			rekall.timeline.timeLayer.group.add(horizontalSortingCategory.rectAlternate);
		horizontalSortingCategory.rectAlternate.setPosition({x: horizontalSortingCategoryPos.x, y: y});
		horizontalSortingCategory.rectAlternate.setSize({width: horizontalSortingCategoryPos.xMax - horizontalSortingCategoryPos.x, height: rekall.timeline.height()});
		horizontalSortingCategory.rectAlternate.setOpacity((((alternate++)%2)==0)?(0.):(0.2));
		
		if(horizontalSortingCategory.categoryText == undefined) {
			horizontalSortingCategory.categoryText = new Kinetic.Text({
				fill: 		'#F5F8EE',
				fontSize: 	9,
				fontFamily: 'open_sansregular',
				listening: 	false,
			});
		}
		if(full != false)
			rekall.timeline.timeLayer.group.add(horizontalSortingCategory.categoryText);
		horizontalSortingCategory.categoryText.setPosition({x: horizontalSortingCategoryPos.x+5, y: 9});
		horizontalSortingCategory.categoryText.setText(horizontalSortingCategory.categoryVerbose);
	});


	//Disposition des tags + étiquettes
	rekall.selectionId++;
	var bounds = {x: 0, y: 0};
	var xMax = 0, y = 0, alternate = 0;
	var yMaxGroupSortingCategory = 0;
	$.each(rekall.sortings["groups"].categories, function(key, groupSortingCategory) {
		if(full != false) {
			if(groupSortingCategory.verticalSorting == undefined)
				groupSortingCategory.verticalSorting = new Sorting();
			groupSortingCategory.verticalSorting.setCriterias(rekall.sortings["vertical"].metadataConfigStr, rekall.sortings["vertical"].valCanBeFloats, rekall.sortings["vertical"].metadataSearch, false);
	
			if(full != false) {
				groupSortingCategory.verticalSorting.analyseStart();
				$.each(groupSortingCategory.tags, function(key, tag) {
					groupSortingCategory.verticalSorting.analyseAdd(tag);
				});
				groupSortingCategory.verticalSorting.analyseEnd();
			}
		}
	
		$.each(groupSortingCategory.verticalSorting.categories, function(key, verticalSortingCategory) {
			$.each(verticalSortingCategory.tags, function(key, tag) {
				var dimensions = rekall.sortings["horizontal"].positionFor(tag);
				tag.rect.x      = dimensions.x;
				tag.rect.y      = 0;
				tag.rect.width  = dimensions.width;
				tag.rect.height = Tag.tagHeight;
			});
		});
	
		//Label de groupe
		if((groupSortingCategory.rect == undefined) || (full != false)) {
			groupSortingCategory.rect = new Kinetic.Rect({
				x: 			0,
				y: 			y,
				width: 		15,
				height: 	10,
				fill: 		'#4D5355',
				listening: 	false,
			});
			rekall.timeline.gridLayer.group.add(groupSortingCategory.rect);
		}
		if((groupSortingCategory.text == undefined) || (full != false)) {
			groupSortingCategory.text = new Kinetic.Text({
				x: 			2,
				y: 			groupSortingCategory.rect.y(),
				text: 		groupSortingCategory.categoryVerbose,
				
				fill: 		'#F5F8EE',
				fontSize: 	10,
				fontFamily: 'open_sansregular',
				rotation: 	-90,
				listening: 	false,
			});
			rekall.timeline.gridLayer.group.add(groupSortingCategory.text);
		}
	

		//Affectation du placement
		var yMaxVerticalSortingCategory = 0;
		$.each(groupSortingCategory.verticalSorting.categories, function(key, verticalSortingCategory) {
			//Label de catégorie
			if((verticalSortingCategory.rect == undefined) || (full != false)) {
				verticalSortingCategory.rect = new Kinetic.Rect({
					x: 			0,
					y: 			y,
					width: 		rekall.timeline.gridLayer.rect.width(),
					height: 	10,
					fill: 		'#4D5355',
					listening: 	false,
				});
				rekall.timeline.gridLayer.group.add(verticalSortingCategory.rect);
			}
			if((verticalSortingCategory.rectAlternate == undefined) || (full != false)) {
				verticalSortingCategory.rectAlternate = new Kinetic.Rect({
					x: 			rekall.timeline.gridLayer.rect.width(),
					y: 			y,
					width: 		rekall.timeline.tagLayer.width(),
					height: 	10,
					fill: 		'#4D5355',
					opacity:    (((alternate++)%2)==0)?(0):(0.2),
					listening: 	false,
				});
				rekall.timeline.gridLayer.group.add(verticalSortingCategory.rectAlternate);
			}
			if((verticalSortingCategory.text == undefined) || (full != false)) {
				verticalSortingCategory.text = new Kinetic.Text({
					x: 			verticalSortingCategory.rect.x() + 20,
					y: 			verticalSortingCategory.rect.y(),
					fill: 		'#F5F8EE',
					text: 		verticalSortingCategory.categoryVerbose,
					fontSize: 	10,
					fontFamily: 'open_sansregular',
					listening: 	false,
				});
				rekall.timeline.gridLayer.group.add(verticalSortingCategory.text);
			}
			verticalSortingCategory.text.setY(verticalSortingCategory.rect.y());


			var linearScale = (groupSortingCategory.verticalSorting.valCanBeFloats) && (groupSortingCategory.verticalSorting.valAreFloats);
			if(linearScale) {
				var dimensions = groupSortingCategory.verticalSorting.positionFor(undefined, key);
				//y						    = dimensions.x/5;
				//yMaxVerticalSortingCategory = dimensions.x/5+dimensions.width/5;
			}
			var tagsAdded = new Object();
			$.each(verticalSortingCategory.tags, function(key, tag) {
				var iterationInCaseOfInfiniteLoop = 100;
				var dimensions = rekall.sortings["horizontal"].positionFor(tag);

				var previousTag = tagsAdded[tag.document.key];
				if((previousTag != undefined) && (!previousTag.isGoodVersion()) && (!tag.isGoodVersion())) {
					documentWasAdded = true;
					tag.rect.x = tagsAdded[tag.document.key].rect.x;
					tag.rect.y = tagsAdded[tag.document.key].rect.y;
				}
				else {
					tag.rect.x = dimensions.x;
					tag.rect.y = y + Tag.tagHeight/2;
					while((tag.intersectsAll(verticalSortingCategory.tags)) && (iterationInCaseOfInfiniteLoop-- >= 0)) {
						tag.rect.x += tag.rect.width;
						if((tag.rect.x+tag.rect.width) > dimensions.xMax) {
							tag.rect.x = dimensions.x;
							tag.rect.y += tag.rect.height + Tag.tagHeight/2;
						}
						else {
						}
					}
					tagsAdded[tag.document.key] = tag;
				}
				yMaxVerticalSortingCategory = max(yMaxVerticalSortingCategory, tag.rect.y);
				xMax = max(xMax, tag.rect.x + tag.rect.width);
				tag.updatePosititon();
				
				//Bounds et sélection
				bounds.x = max(bounds.x, tag.rect.x+tag.rect.width);
				bounds.y = max(bounds.y, tag.rect.y+tag.rect.height);
				if(($.inArray(tag, Tags.selectedTags) !== -1) && (rekall.sortings["groups"].getCategory(tag) != undefined)) {
					rekall.sortings["groups"].getCategory(tag).text.setFill("#2DCAE1");
					if(rekall.sortings["groups"].getCategory(tag).verticalSorting.getCategory(tag) != undefined)
						rekall.sortings["groups"].getCategory(tag).verticalSorting.getCategory(tag).text.setFill(rekall.sortings["groups"].getCategory(tag).text.getFill());
					tag.setSelected(true, Tags.isStrong);
				}
				else
					tag.setSelected(false, Tags.isStrong);		
				
			});
			y = yMaxVerticalSortingCategory + Tag.tagHeight + Tag.tagHeight/2;
			verticalSortingCategory.rect.setHeight(y - verticalSortingCategory.rect.y());
			verticalSortingCategory.rectAlternate.setHeight(verticalSortingCategory.rect.height());
			verticalSortingCategory.text.setY(verticalSortingCategory.text.y() + verticalSortingCategory.rect.height()/2 - verticalSortingCategory.text.height()/2);

			yMaxGroupSortingCategory = max(yMaxGroupSortingCategory, y);
			y += 2;
		});
	
		y = yMaxGroupSortingCategory;
		groupSortingCategory.rect.setHeight(y - groupSortingCategory.rect.y());
		groupSortingCategory.text.setY(groupSortingCategory.rect.y() + groupSortingCategory.text.width() + 2);
		groupSortingCategory.rect.moveToTop();
		groupSortingCategory.text.moveToTop();

		y += Tag.tagHeight*2;
	});
	rekall.timeline.tagLayer.scrollbars.bounds = bounds;

	
	//Affectation des couleurs
	if(full != false) {
		$.each(rekall.sortings["colors"].categories, function(key, colorSortingCategory) {
			$.each(colorSortingCategory.tags, function(key, tag) {
				tag.update(colorSortingCategory.color);
				tag.isSelectable = colorSortingCategory.checked;
			});
		});
	}
	
	//Hightlights graphiques au survol ou sélections
	$.each(rekall.sortings["groups"].categories, function(key, groupSortingCategory) {
		groupSortingCategory.text.setFill("#F5F8EE");
		$.each(groupSortingCategory.verticalSorting.categories, function(key, verticalSortingCategory) {
			verticalSortingCategory.text.setFill("#F5F8EE");
		});
	});

	
	//Display meta
	if(full != false)
		Tag.displayMetadata();


	//Highlight / chemins de hightlight
	rekall.timeline.tagLayer.groupUnderlay.removeChildren();
	$.each(rekall.sortings["highlight"].categories, function(key, category) {
		if(category.category != Sorting.prefix) {
			if((category.visible) && (category.checked)) {
				category.path = new Kinetic.Path({
					fill: 		'#FFFFFF',
					stroke: 	'#FFFFFF',
					strokeWidth: 0,
					/*
					lineJoin: 	'round',
					lineCap: 	'round',
					*/
					opacity: 	0.2,
					listening: 	false,
					transformsEnabled: 	'none',
				});
				rekall.timeline.tagLayer.groupUnderlay.add(category.path);
				var areas = new Object();

				var selected = false;
				$.each(category.tags, function(key, tag) {
					selected |= tag.isSelected();
					var key = Sorting.prefix + tag.rect.y;
					if(areas[key] == undefined) {
						areas[key] = new Object();
						areas[key].rects = new Array();
					}
					areas[key].rects.push(tag.rect.adjusted(2));
				});
				areas = Utils.sortObj(areas, true);
				if(selected)
					category.path.setOpacity(0.4);
		
				var path = "";
				var aeraPrev = undefined;
				$.each(areas, function(key, area) {
					$.each(area.rects, function(index, rect) {
						if(aeraPrev == undefined) {
							path += Utils.movePath(rect.getBottomLeft());
							path += Utils.linePath(rect.getTopLeft());
							path += Utils.linePath(rect.getTopRight());
							path += Utils.linePath(rect.getBottomRight());
							path += Utils.closePath();
						}
						if(aeraPrev) {
							$.each(aeraPrev.rects, function(key, rectPrev) {
								path += Utils.movePath(rectPrev.getBottomLeft());
								path += Utils.cubicPathLine(rectPrev.getBottomLeft(), rect.getTopLeft());
								path += Utils.linePath(rect.getTopRight());
								path += Utils.cubicPathLine(rect.getTopRight(), rectPrev.getBottomRight());
								path += Utils.closePath();
							});
							path += Utils.movePath(rect.getBottomLeft());
							path += Utils.linePath(rect.getTopLeft());
							path += Utils.linePath(rect.getTopRight());
							path += Utils.linePath(rect.getBottomRight());
							path += Utils.closePath();
						}
						path += " M" + (rect.getBottomLeft().x-5)  + "," + (rect.getCenter().y);
						path += " L" + (rect.getBottomRight().x+5) + "," + (rect.getCenter().y);
					});
					aeraPrev = area;
				});
				path += Utils.closePath();
				category.path.setData(path);
			}
		}
	});	
	
	//Duplicates
	if(true) {
		$.each(rekall.sortings["hashes"].categories, function(key, category) {
			if((category.visible) && (category.checked)) {
				category.path = new Kinetic.Path({
					stroke: 			'#FFFFFF',
					strokeWidth: 		1,
					opacity: 			0.5,
					listening: 			false,
					transformsEnabled: 	'none',
					dash: 				[2, 2],
				});
				rekall.timeline.tagLayer.groupUnderlay.add(category.path);
			
				var path = "";
				var selected = category.tags[0].isSelected();
				for(var i = 1 ; i < category.tags.length ; i++) {
					selected |= category.tags[i].isSelected();
					path += Utils.movePath(category.tags[0].rect.getCenter());
					var offset = 0;
					if(category.tags[0].rect.getCenter().y == category.tags[i].rect.getCenter().y)
						offset = -Tag.tagHeight*2;
					path += Utils.cubicPathLine(category.tags[0].rect.getCenter(), category.tags[i].rect.getCenter(), undefined, offset);
				}
				category.path.setData(path);
			
				if(selected)
					category.path.setOpacity(1);
			}
		});	
	}
	
	
	//Timeline applatie
	if(full != false) {
		Tags.byTime.sort(function(a, b) {
			if(a.timeStart < b.timeStart) return -1;
			if(a.timeStart > b.timeStart) return 1;
			return 0;
		});
		$("#flattentimeline_items").html("");
    	$.each(Tags.byTime, function(key, tag) {
			$('#flattentimeline_items').append(function() {
				var styleColor = "background-color: " + tag.color + ";";
				if(tag.getMetadata("File->Thumbnail") != undefined) {
					var thumbUrl = Utils.getPreviewPath(tag);
					if(tag.isVideo())	thumbUrl += "_1.jpg";
					else				thumbUrl +=  ".jpg";
					styleImage = "background-image: url(" + thumbUrl + ");";
					styleColor += "opacity: 0.75;";
				}
				
				var html = "<div class='flattentimeline_item'>";
				html 	+= "<div class='flattentimeline_image' style='" + styleImage + "'></div>";
				html 	+= "<div class='flattentimeline_color' style='" + styleColor + "'></div>";
				html 	+= "<div class='flattentimeline_bar'   style=''></div>";
				html 	+= "<div class='flattentimeline_counter'></div>";
				html 	+= "<div class='flattentimeline_text'>" + tag.getMetadata("Rekall->Name") + "</div>";
				html    += "</div>";
				
				tag.flattenTimelineDom = $(html);
				return tag.flattenTimelineDom;
			});
		});
	}
	
	rekall.redraw(full);
	rekall.timeline.bar.updateFlattenTimeline();
	this.firstAnalysis = false;
}
