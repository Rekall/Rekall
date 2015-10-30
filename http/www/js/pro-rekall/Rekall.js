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

function Rekall() {
	this.sortings = new Object();
	this.sortings["colors"]     = new Sorting("Rekall->Type", false, "");
	this.sortings["groups"]     = new Sorting("Rekall->Date/Time|year", false, "");//Rekall->Date/Time|year
	this.sortings["vertical"]   = new Sorting("Rekall->Date/Time|month", false, "");//Rekall->Date/Time|month
	this.sortings["horizontal"] = new Sorting("Rekall->Author", true, "");//"Time");//File->File Size MB//EXIF->Aperture Value
	this.sortings["highlight"]  = new Sorting("Rekall->Type", false, "");//"Rekall->Extension", false, "max");
	this.sortings["authors"]    = new Sorting("Rekall->Author");
	this.sortings["types"]      = new Sorting("Rekall->Type");

	this.sortings["search"]     = new Sorting("", false, "");	
	this.sortings["corpus"]     = new Sorting("Rekall->Folder");
	this.sortings["hashes"]   	= new Sorting("File->Hash");
	this.sortings["keywords"]   = new Sorting();
	
	this.sortings["corpus"].analyse = false;
	
	this.shouldAnalyse		         = true;
	this.shouldAnalyseFull           = true;
	this.shouldAnalyseIsCheckbox     = false;
//HideFlattenTimeline	this.shouldUpdateFlattenTimeline = true;
	this.selectionId = 0;
	
	//Map
	this.map    = new Map();
	this.panner = new Panner();

	this.previewVideoPlayer = new VideoPlayer($("#previewVideo"), "previewVideoPlayer", false);
	this.captationVideoPlayers = new VideoPlayers();
	
	//Other events
	this.mousePressed = false;
	this.mousePressedPos = {x: 0, y: 0};
	this.isScrolling = false;
}

Rekall.prototype.start = function() {
	this.timeline = new Kinetic.Stage({
		listening: 			true,
		container: 			'timeline',
		transformsEnabled: 	'none',
   	});
	this.timeline.shouldRedraw     = true;
	this.timeline.shouldRedrawFull = true;

	//Time Layer
	this.timeline.timeLayer = new Kinetic.Layer({
		listening: 			true,
		transformsEnabled: 'position',
	});
	this.timeline.add(this.timeline.timeLayer);
	this.timeline.timeLayer.rect = new Kinetic.Rect({
		/*fill: '#373C3F',*/
		fill: '#1D2021', //26292B',
		listening: 			true,
		transformsEnabled: 'none',
	});
	this.timeline.timeLayer.rect.on("click", function(e) {
		if(rekall.sortings["horizontal"].metadataKey == "Time")
			rekall.timeline.bar.rewind(Sorting.timeForPosition(Sorting.unmapPosition(rekall.timeline.getPointerPosition().x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x())));
	})
	this.timeline.timeLayer.add(this.timeline.timeLayer.rect);
	this.timeline.timeLayer.group = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.timeLayer.add(this.timeline.timeLayer.group);
	
	//Timeline bar
	this.timeline.bar = new Timeline();
	this.timeline.barLayer = new Kinetic.Layer({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.add(this.timeline.barLayer);	
	this.timeline.barLayer.group = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.barLayer.add(this.timeline.barLayer.group);
	this.timeline.bar.line = new Kinetic.Line({
		points: 		[NaN, 0, NaN, 0],
		stroke: 		'#2DCAE1',
		strokeWidth: 	2,
		listening: false, 
		transformsEnabled: 'none',
	});
	this.timeline.barLayer.group.add(this.timeline.bar.line);
	this.timeline.bar.text = new Kinetic.Text({
		y: 			0,
		fontSize: 	10,
		fontFamily: 'OpenSans',  
		fontWeight: 400,
		fill: 		'#2DCAE1',
		text: 		"",
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.barLayer.group.add(this.timeline.bar.text);

	//Grid Layer
	this.timeline.gridLayer = new Kinetic.Layer({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.add(this.timeline.gridLayer);
	this.timeline.gridLayer.rect = new Kinetic.Rect({
		/*fill: '#373C3F',*/
		fill: '#161A1A', //26292B',
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.gridLayer.add(this.timeline.gridLayer.rect);
	this.timeline.gridLayer.group = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.gridLayer.add(this.timeline.gridLayer.group);
	
	//Data Layer
	this.timeline.tagLayer = new Kinetic.Layer({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.add(this.timeline.tagLayer);
	this.timeline.tagLayer.groupUnderlay = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'position',
	});
	this.timeline.tagLayer.add(this.timeline.tagLayer.groupUnderlay);
	this.timeline.tagLayer.group = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'position',		
	});
	this.timeline.tagLayer.add(this.timeline.tagLayer.group);
	
	//Selection layer
	this.timeline.selectionLayer = new Kinetic.Layer({
		listening: false, 
		transformsEnabled: 'position',		
	});
	this.timeline.add(this.timeline.selectionLayer);
	this.timeline.selectionLayer.group = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'position',		
	});
	this.timeline.selectionLayer.add(this.timeline.selectionLayer.group);
	this.timeline.selectionLayer.path = new Kinetic.Path({
		stroke:  			'#828382',
		dash: 				[5, 5],
		cornerRadius: 		Tag.tagHeight/3,
		listening: false, 
		transformsEnabled: 'position',		
	});
	this.timeline.selectionLayer.group.add(this.timeline.selectionLayer.path);
	this.timeline.selectionLayer.path.polygon = new Polygon();
	
	//Scrollbars
	var stage = this.timeline;
	this.timeline.tagLayer.scrollbars = new Kinetic.Group({
		listening: true, 
		transformsEnabled: 'none',		
	});
	this.timeline.tagLayer.scrollbars.bounds = {x: 1000, y: 0};
	this.timeline.tagLayer.scrollbars.aeras = new Kinetic.Group({
		listening: false, 
		transformsEnabled: 'none',		
	});
	this.timeline.tagLayer.scrollbars.layer = new Kinetic.Layer({
		listening: true, 
		transformsEnabled: 'position',		
	});
	this.timeline.tagLayer.scrollbars.hscrollArea = new Kinetic.Rect({
		fill:   'white',
		opacity: 0.1,
		listening: false,
		transformsEnabled: 'position',		
	});
	this.timeline.tagLayer.scrollbars.hscroll = new Kinetic.Rect({
		fill:   'white',
		cornerRadius: 7,
		opacity: 0.2,
		draggable: true,
		transformsEnabled: 'position',		
		dragBoundFunc: function(pos) {
			return {
				x: constrain(pos.x, rekall.timeline.tagLayer.scrollbars.hscrollArea.x(), rekall.timeline.tagLayer.scrollbars.hscrollArea.width()+rekall.timeline.tagLayer.scrollbars.hscrollArea.x()-rekall.timeline.tagLayer.scrollbars.hscroll.width()),
				y: this.getAbsolutePosition().y
			}
		},
	});
	this.timeline.tagLayer.scrollbars.vscrollArea = new Kinetic.Rect({
		fill: 	'white',
		opacity: this.timeline.tagLayer.scrollbars.hscrollArea.opacity(),
		listening: false,
		transformsEnabled: 'position',		
	});
	this.timeline.tagLayer.scrollbars.vscroll = new Kinetic.Rect({
		fill:   'white',
		cornerRadius: this.timeline.tagLayer.scrollbars.hscroll.cornerRadius(),
		opacity: this.timeline.tagLayer.scrollbars.hscroll.opacity(),
		draggable: true,
		transformsEnabled: 'position',		
		dragBoundFunc: function(pos) {
			return {
				x: this.getAbsolutePosition().x,
				y: constrain(pos.y, rekall.timeline.tagLayer.scrollbars.vscrollArea.y(), rekall.timeline.tagLayer.scrollbars.vscrollArea.height()+rekall.timeline.tagLayer.scrollbars.vscrollArea.y()-rekall.timeline.tagLayer.scrollbars.vscroll.height())
			}
		},
	});
	this.timeline.tagLayer.scrollbars.on('mouseover', function() {
		document.body.style.cursor = 'pointer';
	});
	this.timeline.tagLayer.scrollbars.on('mouseout', function() {
		document.body.style.cursor = 'default';
	});
	this.timeline.tagLayer.scrollbars.aeras.add(this.timeline.tagLayer.scrollbars.hscrollArea);
	this.timeline.tagLayer.scrollbars.aeras.add(this.timeline.tagLayer.scrollbars.vscrollArea);
	this.timeline.tagLayer.scrollbars.add(this.timeline.tagLayer.scrollbars.hscroll);
	this.timeline.tagLayer.scrollbars.add(this.timeline.tagLayer.scrollbars.vscroll);
	this.timeline.tagLayer.scrollbars.layer.add(this.timeline.tagLayer.scrollbars.aeras);
	this.timeline.tagLayer.scrollbars.layer.add(this.timeline.tagLayer.scrollbars);
	this.timeline.add(this.timeline.tagLayer.scrollbars.layer);
	this.timeline.tagLayer.scrollbars.hscroll.on('dragmove', function() { rekall.isScrolling = true; rekall.mousePressed = false; rekall.redraw(); });
	this.timeline.tagLayer.scrollbars.vscroll.on('dragmove', function() { rekall.isScrolling = true; rekall.mousePressed = false; rekall.redraw(); });
	this.timeline.tagLayer.scrollbars.hscroll.on('dragend',  function() { rekall.isScrolling = false; rekall.mousePressed = false; rekall.redraw(); });
	this.timeline.tagLayer.scrollbars.vscroll.on('dragend',  function() { rekall.isScrolling = false; rekall.mousePressed = false; rekall.redraw(); });

	this.resize();

	//Animation	
	var globalTime = 0;
	function refresh() {
		if(rekall != undefined) {
			fastdom.write(function() {
				if(rekall.timeline.bar.update())
					rekall.timeline.barLayer.draw();
			
				if((rekall.timeline.shouldRedraw) && (rekall.timeline != undefined)) {
					var shouldRedrawFull = rekall.timeline.shouldRedrawFull;
					rekall.timeline.shouldRedraw     = false;
					rekall.timeline.shouldRedrawFull = false;
					rekall.timeline.tagLayer.draw();
					rekall.timeline.gridLayer.draw();
					if(shouldRedrawFull != false) {
						rekall.timeline.timeLayer.draw();
						rekall.timeline.barLayer.draw();
						rekall.timeline.tagLayer.scrollbars.layer.draw();
					}
				}
			
				if((rekall.shouldAnalyse) && (rekall.project != undefined)) {
					var shouldAnalyseFull       = rekall.shouldAnalyseFull;
					var shouldAnalyseIsCheckbox = rekall.shouldAnalyseIsCheckbox;
					rekall.shouldAnalyse     = false;
					rekall.shouldAnalyseFull = false;
					rekall.shouldAnalyseIsCheckbox = false;
					rekall.project.analyse(shouldAnalyseFull, shouldAnalyseIsCheckbox);
				}
			
//HideFlattenTimeline
/*				if(rekall.shouldUpdateFlattenTimeline) {
					rekall.shouldUpdateFlattenTimeline = false;
					rekall.timeline.bar.updateFlattenTimeline();
				}   */
			});
		}
		//setTimeout(refresh, 50);
	}
	//refresh();
	setInterval(refresh, 50);
	
	
	$("#rendersTabNav").click(function() {
		rekall.map.resize(false);
		rekall.panner.resize(false);
		rekall.captationVideoPlayers.resize(true);
		rekall.analyse(false);
	});
	$("#mapTabNav").click(function() {
		rekall.map.resize(true);
		rekall.panner.resize(false);
		rekall.captationVideoPlayers.resize(false);
		rekall.analyse(false);
	});
	$("#pannerTabNav").click(function() {
		rekall.map.resize(false);
		rekall.panner.resize(true);
		rekall.captationVideoPlayers.resize(false);
		rekall.analyse(false);
	});

	$("#navigateurTabNav").click(function() {
	    var tmpWidth = $("#left_menu_item_open").width();
		if(tmpWidth>0) {
			$("#left_menu_item_open").attr("tool","").width(0);//hide();
			$(".left_menu_item_checklist").hide();
			$(".left_menu_item_tab_search").hide();
			$("#navigateurTabNav").css("backgroundColor","#559299");
			$(".left_menu_item").removeClass("selected");
		} else {
			$("#left_menu").parent().toggle();
			if($("#left_menu").parent().is(":visible"))	$("#navigateurTabNav").html("&times;");
			else										$("#navigateurTabNav").html("Menu");
			rekall.map.resize();
			rekall.panner.resize();
			rekall.captationVideoPlayers.resize();
			rekall.resize();
			rekall.analyse(true);
		}    
	});
	$("#inspecteurTabNav").click(function() {
		$("#inspecteur").parent().toggle();
		if($("#inspecteur").parent().is(":visible"))	$("#inspecteurTabNav").html("&times;");
		else											$("#inspecteurTabNav").html("METADATA");
		rekall.map.resize();
		rekall.panner.resize();
		rekall.captationVideoPlayers.resize();
		rekall.resize();
		rekall.analyse(true);                        
	});                                                      
	  
	$("#central").css("height", $(window).height() * 0.50);
	
	rekall.map.resize();
	rekall.panner.resize();
	rekall.captationVideoPlayers.resize();
	rekall.resize();
	rekall.analyse(true);                       
	
	$("#pannerTabNav").trigger("click");
	                  
	var nowWidth = $("#central").width(); 
	//var nowWidth = $("#central").width(); 
	//alert(nowWidth);
	$("#central").resizable({ 
		  /*alsoResize: "#thumbLgd", */
		  handles: "s", 
		  maxHeight: "90%",
	      maxWidth: nowWidth,
	      minHeight: "50",
	      minWidth: nowWidth,
		  stop: function( event, ui ) { rekall.panner.resize(); }
	});
	//$("#timeline").resizable();  
	//$(".layout_cell").resizable();
	
	//Déploiement d'une section à gauche
	$(".tab_title.openable").click(function() {
		if($(this).parent().find('.tab_content').is(":visible")) {
			$(this).parent().find('.tab_content').slideUp();
			$(this).find("span").html("&#x25B8;");
		}
		else {
			$(this).parent().find('.tab_content').slideDown();
			$(this).find("span").html("&#x25BE;");
		}
	});

	//Affichage des scroll bars
	$("#timeline").mouseover(function(event){
		rekall.timeline.tagLayer.scrollbars.layer.setVisible(true);
		//mouseRelease(event);
	});
	$("#timeline").mouseleave(function(event){
		rekall.timeline.tagLayer.scrollbars.layer.setVisible(false);
		rekall.mousePressed = false;
	});


	//Zoom sur la timeline
	$("#timeline").mousewheel(function(event){
		event.preventDefault();
		rekall.timeline.tagLayer.scrollbars.hscroll.setPosition({
			x: constrain(rekall.timeline.tagLayer.scrollbars.hscroll.x() + event.deltaX, rekall.timeline.tagLayer.scrollbars.hscrollArea.x(), rekall.timeline.tagLayer.scrollbars.hscrollArea.width()+rekall.timeline.tagLayer.scrollbars.hscrollArea.x()-rekall.timeline.tagLayer.scrollbars.hscroll.width()),
			y: rekall.timeline.tagLayer.scrollbars.hscroll.getAbsolutePosition().y
		});
		rekall.timeline.tagLayer.scrollbars.vscroll.setPosition({
			x: rekall.timeline.tagLayer.scrollbars.vscroll.getAbsolutePosition().x,
			y: constrain(rekall.timeline.tagLayer.scrollbars.vscroll.y() - event.deltaY, rekall.timeline.tagLayer.scrollbars.vscrollArea.y(), rekall.timeline.tagLayer.scrollbars.vscrollArea.height()+rekall.timeline.tagLayer.scrollbars.vscrollArea.y()-rekall.timeline.tagLayer.scrollbars.vscroll.height())
		});
		rekall.redraw();
	});

	//Double click
	$("#timeline").dblclick(function(event) {
		if(Tags.hoveredTag != undefined) {
			if(rekall_common.isLocal)	Tags.hoveredTag.openFile();
			else				Tags.hoveredTag.openBrowser();
		}
	});
	
	//Marqueurs
	$(document).keyup(function(e) { 
		//if((e.keyCode == 77) alert(e.keyCode); 
		//if(e.altKey && e.keyCode == 77) alert("Alt+m");   
		if( !$(document.activeElement).is('input') ) {
			if((e.keyCode == 8) || (e.keyCode == 46)) {
				if(rekall.sortings["horizontal"].metadataKey == "Time") {
					if(Tags.count()) {
						var phrase = "Are-you sure to remove this document from timeline view?";
						if(Tags.count() > 1)
							phrase = "Are-you sure to remove theses " + Tags.count() + " documents from timeline view?";
						var sur = confirm(phrase);
						if(sur == true) {
							var projectChangedXml = "";
							for (var index in Tags.selectedTags) {
								var tag = Tags.selectedTags[index];
								var metadataKey   = "Rekall->Visibility"
								var metadataValue = "Hidden on timeline";
								tag.setMetadata(metadataKey, metadataValue);
								projectChangedXml += "<edition key=\"" + Utils.escapeHtml(tag.document.key) + "\" version=\"" + tag.version + "\" metadataKey=\"" + Utils.escapeHtml(metadataKey) + "\" metadataValue=\"" + Utils.escapeHtml(metadataValue.trim()) + "\" />\n";
							}
							rekall.projectChanged(projectChangedXml);
							rekall.analyse();
						}
					}
				}
				else {
					var markers = [];
					for (var index in Tags.selectedTags)
						if(Tags.selectedTags[index].isMarker())
							markers.push(Tags.selectedTags[index].document)

					if(markers.length) {
						var phrase = "Are-you sure to remove this marker?";
						if(Tags.count() > 1)
							phrase = "Are-you sure to remove theses " + markers.length + " markers?";
						var sur = confirm(phrase);
						if(sur == true) {
							var projectChangedXml = "";
							for (var markerIndex in markers) {
								for (var tagIndex in markers[markerIndex].tags)
									markers[markerIndex].tags[tagIndex].visuel.rect.remove();
								projectChangedXml += "<document key=\"" + markers[markerIndex].key + "\" remove=\"true\" />\n";
								delete rekall.project.sources["Files"].documents[markers[markerIndex].key];
							}
							rekall.projectChanged(projectChangedXml);
							rekall.analyse();
						}
					}
				}
			}
		    else if(e.keyCode == 77) {
				var currentDate = moment().format("YYYY:MM:DD HH:mm:ss");

				var state = rekall.timeline.bar.state;
				if(state)
					rekall.timeline.bar.stop();
				var friendlyName = prompt("Marker name", "Marker");
				if(friendlyName != "") {
					var marker = new Document();
					var tag = new Tag(marker);
					tag.setTimeStart(rekall.timeline.bar.timeCurrent);
					tag.setTimeEnd  (rekall.timeline.bar.timeCurrent + 4);
					tag.setMetadata("Rekall->Comments",    "");
					tag.setMetadata("Rekall->Date/Time",   currentDate);
					tag.setMetadata("Rekall->Flag",        "Marker");
					tag.setMetadata("Rekall->Group",       "");
					tag.setMetadata("Rekall->Import Date", currentDate);
					tag.setMetadata("Rekall->Keywords",    "");
					tag.setMetadata("Rekall->Name",        friendlyName);
					tag.setMetadata("Rekall->Type",        "rekall/marker");

					if(rekall_common.owner != undefined) {
						tag.setMetadata("Rekall->Author",      			    rekall_common.owner.author);
						tag.setMetadata("Rekall User Infos->User Name",     rekall_common.owner.author);
						tag.setMetadata("Rekall User Infos->Location Name", rekall_common.owner.locationName);
					}
					marker.addTag(tag);

					rekall.project.addDocument("Files", marker);

					var projectChangedXml = "<document key=\"" + marker.key + "\">\n";
					var metadatas = marker.tags[0].getMetadatas();
					for (var key in metadatas) {
						var meta = metadatas[key];
						projectChangedXml += "<meta cnt=\"" + Utils.escapeHtml(meta) + "\" ctg=\"" + Utils.escapeHtml(key) + "\" />\n";
					}
					projectChangedXml += "</document>\n";
					projectChangedXml += "<tag key=\"" + Utils.escapeHtml(tag.document.key) + "\" version=\"" + tag.version + "\" timeStart=\"" + tag.timeStart + "\" timeEnd=\"" + tag.timeEnd + "\"/>\n";
					rekall.projectChanged(projectChangedXml);

					rekall.analyse();
				}
				if(state)
					rekall.timeline.bar.play();
			}
			else if(e.keyCode == 82) {
				for (var keySource in rekall.project.sources) {
					for (var keyDocument in rekall.project.sources[keySource].documents) {
						for (var key in rekall.project.sources[keySource].documents[keyDocument].tags) {
							var tag = rekall.project.sources[keySource].documents[keyDocument].tags[key];
							tag.timeStart = random(0, 100);
							tag.timeEnd   = tag.timeStart + random(0, 20);
						}
				    }
				}
				rekall.analyse(true);
			}  
		}   
	});
	
	
	$('#timeline').on({
		dragenter: function(event) {
			//alert("dragenter");
			event.stopImmediatePropagation();
			event.preventDefault();
		},
		dragover: function(event) {
			//alert("dragover");
			event.stopImmediatePropagation();
			event.preventDefault();
		},
		drop: function(event) {
			//alert("drop");
			var tag = undefined;
			if((event.dataTransfer.getData("key") != undefined) && (event.dataTransfer.getData("version") != undefined)) {
				for(var index in rekall.project.sources["Files"].documents[event.dataTransfer.getData("key")].tags) {
					var tagTmp = rekall.project.sources["Files"].documents[event.dataTransfer.getData("key")].tags[index];
					if(tagTmp.version == event.dataTransfer.getData("version")) {
						tag = tagTmp;
						break;
					}
				}
			}
			if(tag != undefined) {
				event.stopImmediatePropagation();
				event.preventDefault();
				
				console.log(event.pageX);
				rekall.mouseMoveTime = Sorting.timeForPosition(Sorting.unmapPosition(rekall.timeline.getPointerPosition().x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x()));
				console.log(rekall.mouseMoveTime);
				console.log(tag);
			}
		},
		/*
		dragleave: function(event) {
			//alert("dragleave");
			event.preventDefault();
		},
		dragend: function(event) {
			//alert("dragend");
			event.preventDefault();
		},*/
	});
	
	
	//Zoom
	$("#timeline-unzoom").click(function() {
		Sorting.size -= 3;
		rekall.analyse(false);
		rekall.redraw(true);
	});
	$("#timeline-zoom").click(function() {
		Sorting.size += 3;
		rekall.analyse(false);
		rekall.redraw(true);
	});
	$("#timeline-ff").click(function() {
		rekall.timeline.bar.rewind();
	});
	$("#timeline-play").click(function() {
		rekall.timeline.bar.toggle();
	});
	$("#timeline-pause").click(function() {
		rekall.timeline.bar.toggle();
	});
	
//HideFlattenTimeline
/*	$("#flattentimeline div:first span").click(function() {
		rekall.analyse(true);
	});
*/
	//Survols et sélections
	$("#timeline").mousedown(function(event){
		if(rekall.isScrolling != true) {
			//Déplacement
			rekall.mousePressedPos  = rekall.timeline.getPointerPosition();
			rekall.mousePressedTime = Sorting.timeForPosition(Sorting.unmapPosition(rekall.mousePressedPos.x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x()));
		
			for (var index in Tags.selectedTags) {
				var tag = Tags.selectedTags[index];
				tag.timeStartMouse = tag.timeStart;
				tag.timeEndMouse   = tag.timeEnd;
			}

			rekall.mousePressedMode = "";
			if(Tags.count())
				rekall.mousePressedMode = "move";
			for (var index in Tags.selectedTags) {
				var tag = Tags.selectedTags[index];
				var tagCenter = tag.getTimeStart() + (tag.getTimeEnd() - tag.getTimeStart())/2;
				var tagDurationTolerence = max((tag.getTimeEnd() - tag.getTimeStart()) * 0.15, 0.2);
				if(((tagCenter - tagDurationTolerence/2) <= rekall.mousePressedTime) && (rekall.mousePressedTime <= (tagCenter + tagDurationTolerence/2)))
					rekall.mousePressedMode = "move";
				
				else if (((tag.getTimeStart() - tagDurationTolerence) <= rekall.mousePressedTime) && (rekall.mousePressedTime <= (tag.getTimeStart() + tagDurationTolerence)))
					rekall.mousePressedMode = "resizeL";
				else if(((tag.getTimeEnd()   - tagDurationTolerence) <= rekall.mousePressedTime) && (rekall.mousePressedTime <= (tag.getTimeEnd()   + tagDurationTolerence)))
					rekall.mousePressedMode = "resizeR";
			}

			rekall.mousePressed = true;
		
			rekall.timeline.selectionLayer.path.polygon.clear();
			if(Tags.hoveredTag == undefined)
				drawSelection(event);
		}
	});
	$("#timeline").mouseup(function(event){
		if(rekall.isScrolling != true)
			mouseRelease(event);
	});
	$("#timeline").mousemove(function(event){
		if(rekall.isScrolling != true) {
			//Position
			var pos = {
				x: event.pageX - $(this).offset().left - rekall.timeline.tagLayer.x() - rekall.timeline.tagLayer.group.x(),
				y: event.pageY - $(this).offset().top  - rekall.timeline.tagLayer.y() - rekall.timeline.tagLayer.group.y(),
			};

			//Survol
			var hoveredTagOld = Tags.hoveredTag;
			Tags.hoveredTag = undefined;
			if((rekall.project) && (rekall.timeline.selectionLayer.path.polygon.points.length == 0)) {
				for (var keySource in rekall.project.sources) {
					if(Tags.hoveredTag == undefined) {
						for (var keyDocument in rekall.project.sources[keySource].documents) {
							if(Tags.hoveredTag == undefined) {
								for (var key in rekall.project.sources[keySource].documents[keyDocument].tags) {
									if((Tags.hoveredTag == undefined) && (rekall.project.sources[keySource].documents[keyDocument].tags[key].isVisible()) && (rekall.project.sources[keySource].documents[keyDocument].tags[key].rect.contains(pos))) {
										Tags.hoveredTag = rekall.project.sources[keySource].documents[keyDocument].tags[key];
										break;
									}
								}
							}
						}
					}
				}
			}
		
			if((rekall.mousePressed) && (Tags.count()) && (rekall.sortings["horizontal"].metadataKey == "Time")) {
				//Déplacement
				rekall.mouseMoveTime = Sorting.timeForPosition(Sorting.unmapPosition(rekall.timeline.getPointerPosition().x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x()));

				for (var index in Tags.selectedTags) {
					var tag = Tags.selectedTags[index];
					if     (rekall.mousePressedMode == "resizeL")
						tag.setTimeStart(constrain(0, tag.timeStartMouse + (rekall.mouseMoveTime - rekall.mousePressedTime), tag.timeEnd));
					else if(rekall.mousePressedMode == "resizeR")
						tag.setTimeEnd(max(tag.timeStart, tag.timeEndMouse + (rekall.mouseMoveTime - rekall.mousePressedTime)));
					else if(rekall.mousePressedMode == "move") {
						var duration = tag.timeEndMouse - tag.timeStartMouse;
						tag.setTimeStart(max(0, tag.timeStartMouse + (rekall.mouseMoveTime - rekall.mousePressedTime)));
						tag.setTimeEnd  (tag.timeStart + duration);
					}
					rekall.doNotChangeSelection = true;

					//Update
					tag.projectChangedXml = "<tag key=\"" + Utils.escapeHtml(tag.document.key) + "\" version=\"" + tag.version + "\" timeStart=\"" + tag.timeStart + "\" timeEnd=\"" + tag.timeEnd + "\"/>";
				}
				rekall.analyse(false);
				rekall.timeline.tagLayer.draw();
			}
			else if((rekall.mousePressed) && (Tags.hoveredTag == undefined)) {
				rekall.timeline.selectionLayer.path.polygon.addPoint(pos);
				drawSelection(event);
			}
			else if(Tags.hoveredTag != undefined) {
				if(hoveredTagOld != Tags.hoveredTag)
					Tags.hoveredTag.displayMetadata();
			}
			
			/*if(((rekall.mousePressed) || (Tags.hoveredTag != undefined)) && (rekall.mousePressedMode == "move"))
				window.document.body.style.cursor = 'move';
			else if(((rekall.mousePressed) || (Tags.hoveredTag != undefined)) && (rekall.mousePressedMode.startsWith("resize")))
				window.document.body.style.cursor = 'col-resize';
			else*/if((rekall.mousePressed) && (Tags.hoveredTag == undefined))
				window.document.body.style.cursor = 'crosshair';
			else if(Tags.hoveredTag != undefined)
				window.document.body.style.cursor = 'pointer';
			else
				window.document.body.style.cursor = 'default';
		}
	});
	function mouseRelease(event) {
		if(rekall.mousePressed){
			rekall.mousePressed = false;
			
			if(rekall.doNotChangeSelection != true) {
				if((!event.shiftKey) && (!event.ctrlKey))
					Tags.clear();
				if((Tags.hoveredTag != undefined) && (rekall.timeline.selectionLayer.path.polygon.points.length == 0)) 
					Tags.toggle(Tags.hoveredTag, true);
				else if(rekall.timeline.selectionLayer.path.polygon.points.length > 0) {
					for (var keySource in rekall.project.sources)
						for (var keyDocument in rekall.project.sources[keySource].documents)
							for (var key in rekall.project.sources[keySource].documents[keyDocument].tags)
								if((rekall.project.sources[keySource].documents[keyDocument].tags[key].isVisible()) && (rekall.project.sources[keySource].documents[keyDocument].tags[key].isSelectable) && (rekall.timeline.selectionLayer.path.polygon.contains(rekall.project.sources[keySource].documents[keyDocument].tags[key].rect.getPosition()))) 
									Tags.toggle(rekall.project.sources[keySource].documents[keyDocument].tags[key], true);
				}
			}
			else {
				var projectChangedXml = "";
				for (var index in Tags.selectedTags) {
					if((Tags.selectedTags[index].projectChangedXml) && (Tags.selectedTags[index].projectChangedXml.length)) {
						projectChangedXml += Tags.selectedTags[index].projectChangedXml + "\n";
						Tags.selectedTags[index].projectChangedXml = undefined;
					}
				}
				if(projectChangedXml.length)
					rekall.projectChanged(projectChangedXml);
			}
			rekall.doNotChangeSelection = false;
			rekall.timeline.selectionLayer.path.polygon.clear();
			drawSelection(event);
			Tag.displayMetadata();
		}
	}
	function drawSelection() {
		rekall.timeline.selectionLayer.path.setData(rekall.timeline.selectionLayer.path.polygon.toPath());
		rekall.timeline.selectionLayer.draw();
	}
}



Rekall.prototype.loadXMLFile = function() {
	var oldURL = window.document.location + "";
	var index = 0;
	var url = oldURL;
	url = url.replace("pro-index.html", "");
	url = url.replace("online-index.html", "");
	url = url.replace("index.html", "");
	index = oldURL.indexOf('?');
	if(index == -1)
	    index = oldURL.indexOf('#');
	if(index != -1)
	    url = oldURL.substring(0, index);
	if(!url.endsWith("/"))
		url = url + "/";
	this.baseUrl = url;
	
	var thiss = this;
	$.ajax(rekall.baseUrl + "project", {
		type: "GET",
		dataType: "json",
		success: function(infos) {
			thiss.infos = infos;
			window.document.title = "Rekall — " + rekall.infos.friendlyName;
		}
	});

	$.ajax(rekall.baseUrl + "xml", {
		type: "GET",
		dataType: "xml",
		success: function(xml) {
			if((xml == null) || (xml == undefined))
				openAlert("Your project is unreachable. Did you move the folder to an other location? Did you plug your external hard drive or your network drive?", 60);
			else {
				$(xml).find('project').each(function() {
					if($(this).find('document').length == 0)
						openAlert("Your project is empty. Rekall is probably analysing all your files for the first time (it can take a while).", 60);
					else {
						if(rekall.project == undefined)
							rekall.project = new Project(url);
						rekall.project.loadXML($(this));
					}
				});
			}
		}
	});
}
Rekall.prototype.projectChanged = function(xml) {
	window.document.title = "Rekall — " + rekall.infos.friendlyName + " (save in progress)";
	xml = "<!DOCTYPE rekall>\n<changes>\n" + xml + "</changes>";
	rekall_common.enableRubanNextTime = false;
	$.ajax(rekall.baseUrl + "xml", {
		type: "POST",
		data: {change: xml},
		success: function(infos) {
			rekall_common.enableRubanNextTime = false;
			window.document.title = "Rekall — " + rekall.infos.friendlyName;
		}
	});
}


Rekall.prototype.analyse = function(full, isCheckbox) {
	this.shouldAnalyse = true;
	if(this.shouldAnalyseFull == false)
		this.shouldAnalyseFull = full;
	if(this.shouldAnalyseIsCheckbox == false)
		this.shouldAnalyseIsCheckbox = isCheckbox;
}
Rekall.prototype.updateFlattenTimeline = function() {
//HideFlattenTimeline	this.shouldUpdateFlattenTimeline = true;
}

Rekall.prototype.resize = function() {
	this.timeline.setPosition({
		x: 0,
		y: 0,
	});
	this.timeline.setSize({
		width: 	$("#timeline").innerWidth(),
		height: window.innerHeight - $("#timeline").position().top
	});

	this.timeline.gridLayer.rect.setSize({
		width:  150,
		height: this.timeline.height(),
	});
	this.timeline.timeLayer.setPosition({
		x: this.timeline.gridLayer.rect.width(),
		y: 0,
	});
	this.timeline.timeLayer.rect.setSize({
		width:  this.timeline.width() - this.timeline.gridLayer.rect.x(),
		height: 20,
	});
	this.timeline.gridLayer.setPosition({
		x: 0,
		y: this.timeline.timeLayer.rect.height(),
	});
	
	this.timeline.tagLayer.setPosition({
		x: this.timeline.gridLayer.rect.width(),
		y: this.timeline.timeLayer.rect.height(),
	});
	this.timeline.tagLayer.setSize({
		width:  this.timeline.width() - this.timeline.tagLayer.x(),
		height: this.timeline.height() - this.timeline.timeLayer.rect.height(),
	});

	this.timeline.selectionLayer.setPosition({
		x: this.timeline.tagLayer.x(),
		y: this.timeline.tagLayer.y(),
	});

	this.timeline.tagLayer.scrollbars.hscrollArea.setPosition({
		x: 		this.timeline.tagLayer.x() + 5,
		y:   	this.timeline.getHeight() - 15,
	});
	this.timeline.tagLayer.scrollbars.hscrollArea.setSize({
		width:  this.timeline.tagLayer.getWidth() - 20,
		height: 10,
	});
	this.timeline.tagLayer.scrollbars.vscrollArea.setPosition({
		x: 		this.timeline.getWidth() - 15,
		y: 		this.timeline.tagLayer.y() + 5,
	});
	this.timeline.tagLayer.scrollbars.vscrollArea.setSize({
		width: 	10,
		height: this.timeline.tagLayer.getHeight() - 20,
	});

	this.timeline.tagLayer.scrollbars.hscroll.setPosition({
		x: 		this.timeline.tagLayer.scrollbars.hscrollArea.x(),
		y: 		this.timeline.tagLayer.scrollbars.hscrollArea.y(),
	});
	this.timeline.tagLayer.scrollbars.hscroll.setSize({
		width: 	20,
		height: this.timeline.tagLayer.scrollbars.hscrollArea.height(),
	});
	this.timeline.tagLayer.scrollbars.vscroll.setPosition({
		x: 		this.timeline.tagLayer.scrollbars.vscrollArea.x(),
		y: 		this.timeline.tagLayer.scrollbars.vscrollArea.y(),
	});
	this.timeline.tagLayer.scrollbars.vscroll.setSize({
		width: 	this.timeline.tagLayer.scrollbars.vscrollArea.width(),
		height: 20,
	});
}


Rekall.prototype.redraw = function(full) {
	this.timeline.tagLayer.groupUnderlay.setPosition({
		x: map(this.timeline.tagLayer.scrollbars.hscroll.getPosition().x, this.timeline.tagLayer.scrollbars.hscrollArea.x(), this.timeline.tagLayer.scrollbars.hscrollArea.x()+this.timeline.tagLayer.scrollbars.hscrollArea.width()+this.timeline.tagLayer.scrollbars.hscroll.width(),  0, -rekall.timeline.tagLayer.scrollbars.bounds.x),
		y: map(this.timeline.tagLayer.scrollbars.vscroll.getPosition().y, this.timeline.tagLayer.scrollbars.vscrollArea.y(), this.timeline.tagLayer.scrollbars.vscrollArea.y()+this.timeline.tagLayer.scrollbars.vscrollArea.height()-this.timeline.tagLayer.scrollbars.vscroll.height(), 0, -rekall.timeline.tagLayer.scrollbars.bounds.y),
	});
	this.timeline.tagLayer.group.setPosition(this.timeline.tagLayer.groupUnderlay.position());
	this.timeline.tagLayer.setClip({
		x: 0,
		y: 0,
		width: this.timeline.width(),
		height: this.timeline.height(),
	});

	this.timeline.selectionLayer.group.setPosition({
		x: this.timeline.tagLayer.group.x(),
		y: this.timeline.tagLayer.group.y()
	});
	this.timeline.selectionLayer.setClip({
		x: 0,
		y: 0,
		width: this.timeline.width(),
		height: this.timeline.height(),
	});

	this.timeline.gridLayer.group.setPosition({
		x: 0,
		y: map(this.timeline.tagLayer.scrollbars.vscroll.getPosition().y, this.timeline.tagLayer.scrollbars.vscrollArea.y(), this.timeline.tagLayer.scrollbars.vscrollArea.y()+this.timeline.tagLayer.scrollbars.vscrollArea.height()-this.timeline.tagLayer.scrollbars.vscroll.height(), 0, -rekall.timeline.tagLayer.scrollbars.bounds.y),
	});
	this.timeline.gridLayer.setClip({
		x: 0,
		y: 0,
		width: this.timeline.width(),
		height: this.timeline.height(),
	});

	this.timeline.timeLayer.group.setPosition({
		x: map(this.timeline.tagLayer.scrollbars.hscroll.getPosition().x, this.timeline.tagLayer.scrollbars.hscrollArea.x(), this.timeline.tagLayer.scrollbars.hscrollArea.x()+this.timeline.tagLayer.scrollbars.hscrollArea.width()+this.timeline.tagLayer.scrollbars.hscroll.width(),  0, -rekall.timeline.tagLayer.scrollbars.bounds.x),
		y: 0,
	});
	this.timeline.timeLayer.setClip({
		x: 0,
		y: 0,
		width:  this.timeline.width(),
		height: this.timeline.height(),
	});
	
	this.timeline.barLayer.setPosition({
		x: this.timeline.timeLayer.x(),
		y: 0,
	});
	this.timeline.barLayer.group.setPosition({
		x: this.timeline.timeLayer.group.x(),
		y: 0,
	});
	this.timeline.barLayer.setClip({
		x: 0,
		y: 0,
		width:  this.timeline.width(),
		height: this.timeline.height(),
	});

	var timeBarPoints = this.timeline.bar.line.points();
	timeBarPoints[3] = this.timeline.height();
	this.timeline.bar.line.setPoints(timeBarPoints);

	this.timeline.shouldRedraw     = true;
	this.timeline.shouldRedrawFull = full;
}


window.onbeforeunload = function() {
    return "Are-you sure you want to leave your Rekall project?";
};
