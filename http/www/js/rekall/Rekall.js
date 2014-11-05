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

function Rekall() {
	this.sortings = new Object();
	this.sortings["colors"]     = new Sorting("Rekall->Type", false, "");
	this.sortings["groups"]     = new Sorting("Rekall->Date/Time|year", false, "");//Rekall->Date/Time|year
	this.sortings["vertical"]   = new Sorting("Rekall->Date/Time|month", false, "");//Rekall->Date/Time|month
	this.sortings["horizontal"] = new Sorting("Rekall->Author", true, "");//"Time");//File->File Size MB//EXIF->Aperture Value
	this.sortings["highlight"]  = new Sorting("File->MIME Type", false, "");//"Rekall->Extension", false, "max");
	this.sortings["search"]     = new Sorting("", false, "");
	this.sortings["authors"]    = new Sorting("Rekall->Author");
	this.sortings["types"]      = new Sorting("Rekall->Type");
	this.sortings["corpus"]     = new Sorting();
	this.sortings["keywords"]   = new Sorting();
	this.sortings["hashes"]   	= new Sorting("File->Hash");
	
	this.sortings["corpus"].analyse = false;
	
	this.shouldAnalyse		       = true;
	this.shouldAnalyseFull         = true;
	this.selectionId = 0;
	
	//Map
	this.map    = new Map();
	this.panner = new Panner();

	this.previewVideoPlayer = new VideoPlayer($("#previewVideo"), "previewVideoPlayer", false);
	this.captationVideoPlayers = new VideoPlayers();
	
	//Other events
	this.mousePressed = false;
	this.mousePressedPos = {x: 0, y: 0};
}

Rekall.prototype.start = function() {
	this.timeline = new Kinetic.Stage({
		container: 			'timeline',
		transformsEnabled: 	'none',
   	});
	this.timeline.shouldRedraw     = true;
	this.timeline.shouldRedrawFull = true;

	//Time Layer
	this.timeline.timeLayer = new Kinetic.Layer();
	this.timeline.add(this.timeline.timeLayer);
	this.timeline.timeLayer.rect = new Kinetic.Rect({ fill: '#373C3F' });
	this.timeline.timeLayer.rect.on("click", function(e) {
		if(rekall.sortings["horizontal"].metadataKey == "Time")
			rekall.timeline.bar.rewind(Sorting.timeForPosition(Sorting.unmapPosition(rekall.timeline.getPointerPosition().x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x())));
	})
	this.timeline.timeLayer.add(this.timeline.timeLayer.rect);
	this.timeline.timeLayer.group = new Kinetic.Group();
	this.timeline.timeLayer.add(this.timeline.timeLayer.group);
	
	//Timeline bar
	this.timeline.bar = new Timeline();
	this.timeline.barLayer = new Kinetic.Layer();
	this.timeline.add(this.timeline.barLayer);	
	this.timeline.barLayer.group = new Kinetic.Group();
	this.timeline.barLayer.add(this.timeline.barLayer.group);
	this.timeline.bar.line = new Kinetic.Line({
		points: 		[NaN, 0, NaN, 0],
		stroke: 		'#2DCAE1',
		strokeWidth: 	2,
		listening: 		false,
	});
	this.timeline.barLayer.group.add(this.timeline.bar.line);
	this.timeline.bar.text = new Kinetic.Text({
		y: 			0,
		fontSize: 	10,
		fontFamily: 'open_sansregular',
		fill: 		'#2DCAE1',
		text: 		"",
		listening: 	false,
	});
	this.timeline.barLayer.group.add(this.timeline.bar.text);

	//Grid Layer
	this.timeline.gridLayer = new Kinetic.Layer();
	this.timeline.add(this.timeline.gridLayer);
	this.timeline.gridLayer.rect = new Kinetic.Rect({ fill: '#373C3F' });
	this.timeline.gridLayer.add(this.timeline.gridLayer.rect);
	this.timeline.gridLayer.group = new Kinetic.Group();
	this.timeline.gridLayer.add(this.timeline.gridLayer.group);
	
	//Data Layer
	this.timeline.tagLayer = new Kinetic.Layer();
	this.timeline.add(this.timeline.tagLayer);
	this.timeline.tagLayer.groupUnderlay = new Kinetic.Group();
	this.timeline.tagLayer.add(this.timeline.tagLayer.groupUnderlay);
	this.timeline.tagLayer.group = new Kinetic.Group();
	this.timeline.tagLayer.add(this.timeline.tagLayer.group);
	
	//Selection layer
	this.timeline.selectionLayer = new Kinetic.Layer();
	this.timeline.add(this.timeline.selectionLayer);
	this.timeline.selectionLayer.group = new Kinetic.Group();
	this.timeline.selectionLayer.add(this.timeline.selectionLayer.group);
	this.timeline.selectionLayer.path = new Kinetic.Path({
		stroke:  			'#828382',
		listening: 			true,
		dash: 				[5, 5],
		cornerRadius: 		Tag.tagHeight/3,
	});
	this.timeline.selectionLayer.group.add(this.timeline.selectionLayer.path);
	this.timeline.selectionLayer.path.polygon = new Polygon();
	
	//Scrollbars
	var stage = this.timeline;
	this.timeline.tagLayer.scrollbars = new Kinetic.Group();
	this.timeline.tagLayer.scrollbars.bounds = {x: 1000, y: 0};
	this.timeline.tagLayer.scrollbars.aeras = new Kinetic.Group();
	this.timeline.tagLayer.scrollbars.layer = new Kinetic.Layer();
	this.timeline.tagLayer.scrollbars.hscrollArea = new Kinetic.Rect({
		fill:   'white',
		opacity: 0.1,
		listening: false,
	});
	this.timeline.tagLayer.scrollbars.hscroll = new Kinetic.Rect({
		fill:   'white',
		cornerRadius: 7,
		opacity: 0.2,
		draggable: true,
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
	});
	this.timeline.tagLayer.scrollbars.vscroll = new Kinetic.Rect({
		fill:   'white',
		cornerRadius: this.timeline.tagLayer.scrollbars.hscroll.cornerRadius(),
		opacity: this.timeline.tagLayer.scrollbars.hscroll.opacity(),
		draggable: true,
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
	this.timeline.tagLayer.scrollbars.hscroll.on('dragmove', function() { rekall.redraw(); });
	this.timeline.tagLayer.scrollbars.vscroll.on('dragmove', function() { rekall.redraw(); });
	this.timeline.tagLayer.scrollbars.hscroll.on('dragend',  function() { rekall.redraw(); });
	this.timeline.tagLayer.scrollbars.vscroll.on('dragend',  function() { rekall.redraw(); });

	this.resize();

	//Animation	
	var globalTime = 0;
	setInterval(function() {
		if(rekall != undefined) {
			if(rekall.timeline.bar.update())
				rekall.timeline.barLayer.draw();
			
			if((rekall.timeline.shouldRedraw) && (rekall.timeline != undefined)) {
				rekall.timeline.tagLayer.draw();
				rekall.timeline.gridLayer.draw();
				if(rekall.timeline.shouldRedrawFull != false) {
					rekall.timeline.timeLayer.draw();
					rekall.timeline.barLayer.draw();
					rekall.timeline.tagLayer.scrollbars.layer.draw();
				}
				rekall.timeline.shouldRedraw = false;
			}
			
			if((rekall.shouldAnalyse) && (rekall.project != undefined)) {
				rekall.project.analyse(rekall.shouldAnalyseFull);
				rekall.shouldAnalyse = false;
				rekall.shouldAnalyseFull = false;
			}
		}
	}, 30);
	
	
	
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
		$("#navigateur").parent().toggle();
		if($("#navigateur").parent().is(":visible"))	$("#navigateurTabNav").html("&times;");
		else											$("#navigateurTabNav").html("Filters");
		rekall.map.resize();
		rekall.panner.resize();
		rekall.captationVideoPlayers.resize();
		rekall.resize();
		rekall.analyse(true);
	});
	$("#inspecteurTabNav").click(function() {
		$("#inspecteur").parent().toggle();
		if($("#inspecteur").parent().is(":visible"))	$("#inspecteurTabNav").html("&times;");
		else											$("#inspecteurTabNav").html("Inspector");
		rekall.map.resize();
		rekall.panner.resize();
		rekall.captationVideoPlayers.resize();
		rekall.resize();
		rekall.analyse(true);
	});
	$("#maximizeTabNav").click(function() {
		if((rekall.isMaximized == true) || (rekall.isMaximized == undefined)) {
			rekall.isMaximized = false;
			$("#central").css("height", $(window).height() * 0.50);	
			$("#maximizeTabNav").html("&#9634;");		
		}
		else {
			rekall.isMaximized = true;
			$("#central").css("height", $(window).height() * 0.90);
			$("#maximizeTabNav").html("_");		
		}
		rekall.map.resize();
		rekall.panner.resize();
		rekall.captationVideoPlayers.resize();
		rekall.resize();
		rekall.analyse(true);
	});
	$("#maximizeTabNav").trigger("click");
	$("#rendersTabNav").trigger("click");
	
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
		if(e.keyCode == 77) {
			window.document.title = rekall.timeline.bar.timeCurrent;
		}
		else if(e.keyCode == 82) {
			$.each(rekall.project.sources, function(key, source) {
				$.each(source.documents, function(key, document) {
			  	  $.each(document.tags, function(key, tag) {
						tag.timeStart = random(0, 100);
						tag.timeEnd   = tag.timeStart + random(0, 20);
					});
			    });
			});
			rekall.analyse(true);
		}
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
	
	$("#flattentimeline div:first span").click(function() {
		rekall.analyse(true);
	});

	//Survols et sélections
	$("#timeline").mousedown(function(event){
		//Déplacement
		rekall.mousePressedPos  = rekall.timeline.getPointerPosition();
		rekall.mousePressedTime = Sorting.timeForPosition(Sorting.unmapPosition(rekall.mousePressedPos.x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x()));
		
		$.each(Tags.selectedTags, function(index, tag) {
			tag.timeStartMouse = tag.timeStart;
			tag.timeEndMouse   = tag.timeEnd;
		});

		rekall.mousePressedMode = "";
		if(Tags.selectedTags.length)
			rekall.mousePressedMode = "move";
		$.each(Tags.selectedTags, function(index, tag) {
			var tagCenter = tag.getTimeStart() + (tag.getTimeEnd() - tag.getTimeStart())/2;
			var tagDurationTolerence = max((tag.getTimeEnd() - tag.getTimeStart()) * 0.15, 0.2);
			if(((tagCenter - tagDurationTolerence/2) <= rekall.mousePressedTime) && (rekall.mousePressedTime <= (tagCenter + tagDurationTolerence/2)))
				rekall.mousePressedMode = "move";
				
			else if (((tag.getTimeStart() - tagDurationTolerence) <= rekall.mousePressedTime) && (rekall.mousePressedTime <= (tag.getTimeStart() + tagDurationTolerence)))
				rekall.mousePressedMode = "resizeL";
			else if(((tag.getTimeEnd()   - tagDurationTolerence) <= rekall.mousePressedTime) && (rekall.mousePressedTime <= (tag.getTimeEnd()   + tagDurationTolerence)))
				rekall.mousePressedMode = "resizeR";
		});

		rekall.mousePressed = true;
		
		rekall.timeline.selectionLayer.path.polygon.clear();
		if(Tags.hoveredTag == undefined)
			drawSelection(event);
	});
	$("#timeline").mouseup(function(event){
		mouseRelease(event);
	});
	$("#timeline").mousemove(function(event){
		//Position
		var pos = {
			x: event.pageX - $(this).offset().left - rekall.timeline.tagLayer.x() - rekall.timeline.tagLayer.group.x(),
			y: event.pageY - $(this).offset().top  - rekall.timeline.tagLayer.y() - rekall.timeline.tagLayer.group.y(),
		};

		//Survol
		var hoveredTagOld = Tags.hoveredTag;
		Tags.hoveredTag = undefined;
		if((rekall.project) && (rekall.timeline.selectionLayer.path.polygon.points.length == 0)) {
			$.each(rekall.project.sources, function(key, source) {
				$.each(source.documents, function(key, document) {
			  	  $.each(document.tags, function(key, tag) {
						if((Tags.hoveredTag == undefined) && (tag.isVisible()) && (tag.rect.contains(pos))) {
							Tags.hoveredTag = tag;
						}
					});
			    });
			});
		}
		
		if((rekall.mousePressed) && (Tags.selectedTags.length) && (rekall.sortings["horizontal"].metadataKey == "Time")) {
			//Déplacement
			rekall.mouseMoveTime = Sorting.timeForPosition(Sorting.unmapPosition(rekall.timeline.getPointerPosition().x - rekall.timeline.timeLayer.x() - rekall.timeline.timeLayer.group.x()));

			$.each(Tags.selectedTags, function(index, tag) {
				if     (rekall.mousePressedMode == "resizeL")
					tag.setTimeStart(constrain(0, tag.timeStartMouse + (rekall.mouseMoveTime - rekall.mousePressedTime), tag.timeEnd));
				else if(rekall.mousePressedMode == "resizeR")
					tag.setTimeEnd(max(tag.timeStart, tag.timeEndMouse + (rekall.mouseMoveTime - rekall.mousePressedTime)));
				else if(rekall.mousePressedMode == "move") {
					tag.setTimeStart(max(0, tag.timeStartMouse + (rekall.mouseMoveTime - rekall.mousePressedTime)));
					tag.setTimeEnd  (max(tag.timeStart, tag.timeEndMouse + (rekall.mouseMoveTime - rekall.mousePressedTime)));
				}
				rekall.doNotChangeSelection = true;

				//Update
				tag.projectChangedXml = "<tag key=\"" + tag.document.key + "\" version=\"" + tag.version + "\" timeStart=\"" + tag.timeStart + "\" timeEnd=\"" + tag.timeEnd + "\"/>";
			});
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
	});
	function mouseRelease(event) {
		if(rekall.mousePressed){
			rekall.mousePressed = false;
			
			if(rekall.doNotChangeSelection != true) {
				Tags.clear();
				if((Tags.hoveredTag != undefined) && (rekall.timeline.selectionLayer.path.polygon.points.length == 0)) {
					Tags.addOne(Tags.hoveredTag, true);
				}
				else if(rekall.timeline.selectionLayer.path.polygon.points.length > 0) {
					$.each(rekall.project.sources, function(key, source) {
						$.each(source.documents, function(key, document) {
					    	$.each(document.tags, function(key, tag) {
								if((tag.isVisible()) && (tag.isSelectable) && (rekall.timeline.selectionLayer.path.polygon.contains(tag.rect.getPosition())))
									Tags.add(tag, true);
								});
				    		});
						});
				}
			}
			else {
				var projectChangedXml = "";
				$.each(Tags.selectedTags, function(index, tag) {
					if((tag.projectChangedXml) && (tag.projectChangedXml.length)) {
						projectChangedXml += tag.projectChangedXml + "\n";
						tag.projectChangedXml = undefined;
					}
				});
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
			$(xml).find('project').each(function() {
				if(rekall.project == undefined)
					rekall.project = new Project(url);
				rekall.project.loadXML($(this));
			});
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


Rekall.prototype.analyse = function(full) {
	this.shouldAnalyse     = true;
	if(this.shouldAnalyseFull == false)
		this.shouldAnalyseFull = full;
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
