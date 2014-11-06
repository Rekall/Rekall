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

function Map() {
	this.filtredTags = new Array();
	this.visible     = false; 


	var thiss = this;
	this.map = undefined;
	this.gpsPositions = new Array();
	
	try {
		this.myOptions = {
			mapTypeId: 	google.maps.MapTypeId.ROADMAP,
			zoom: 		8,
			center: 	new google.maps.LatLng(-34.397, 150.644),
		};
		this.map         = new google.maps.Map(document.getElementById("map-canvas"), this.myOptions);	
		this.infowindow  = new google.maps.InfoWindow(); 
		this.markers     = new Array();
	    this.bounds  	 = new google.maps.LatLngBounds();
		google.maps.event.addListener(thiss.map, 'bounds_changed', function(event) {
			if((this.getZoom() > 16) && (this.initialZoom == true)) {
				this.setZoom(16);
				this.initialZoom = false;
			}
		
			//Déselectionne les tags hors du bounds
			thiss.filtredTags = new Array();
			for (var index in thiss.markers) {
				var marker = thiss.markers[index];
				if ((thiss.map.getBounds()) && (thiss.map.getBounds().contains(marker.getPosition())))
					thiss.filtredTags.push(marker.rekall.tag);
				//marker.rekall.tag.update();
				rekall.analyse(false);
			}
		});	
		var homeControlDiv = document.createElement('div');
		var homeControl = new HomeControl(homeControlDiv, this.map);
		homeControlDiv.index = 1;
		this.map.controls[google.maps.ControlPosition.TOP_RIGHT].push(homeControlDiv);
		$("#mapTabNav").show();
	}
	catch(e) {
		console.log("No internet connection for maps");
	}
}


Map.prototype.isVisible = function() {
	return this.visible;
}
Map.prototype.resize = function(visible, bounds) {
	if(visible == true)	 {
		$("#map").show();
		$("#mapTabNav").addClass("active");
	}
	else if(visible == false) {
		$("#map").hide();
		$("#mapTabNav").removeClass("active");
	}

	if(this.map == undefined)
		return;

	this.visible = visible;
	if(this.visible == undefined)	this.visible = $("#map").is(":visible");
	else							this.firstDisplay = true;
	
	if(bounds == undefined)			bounds = false;
	if(this.firstDisplay)			bounds = true;
	
	google.maps.event.trigger(this.map, "resize");
	google.maps.event.trigger(this.map, "bounds_changed");	
	if(bounds) {
		this.map.initialZoom = true;
		this.map.fitBounds(this.bounds);
		this.firstDisplay = false;
	}
}
Map.prototype.show = function(markers, filter, bounds) {
	if(this.map == undefined)
		return;

	var thiss = this;
	var opacityFactor = 1;
	if(markers == undefined)
		markers = this.gpsPositions;
	if(markers.length == 0) {
		opacityFactor = 0.3;
		markers = this.gpsPositions;
		bounds = true;
	}
	
	//Clear les marqueurs
	for(var i = 0 ; i < this.markers.length ; i++)
	$.each(this.markers, function(index, marker) {
        thiss.markers[i].setMap(null);
	});
	this.markers = new Array();
	
	if(markers.length) {
	    this.bounds  = new google.maps.LatLngBounds();
		//Parcours les nouveaux marqueur à afficher
		$.each(markers, function(index, markerTmp) {
			var opacity = opacityFactor;
			if((filter != undefined) && (filter != markerTmp.tag))
				opacity = 0.3;
			
			//Ajoute le marqueur et fit à la map
			marker = new google.maps.Marker({
				map: 		thiss.map,
				position: 	new google.maps.LatLng(markerTmp.latitude, markerTmp.longitude),
				opacity: 	markerTmp.tag.colorRaw.getAlpha()*opacity,
				icon: 		new google.maps.MarkerImage("http://chart.apis.google.com/chart?chst=d_map_pin_letter&chld=%E2%80%A2|" + markerTmp.tag.colorRaw.toString("hex6").replace("#", ""),
								new google.maps.Size(21, 34),
				        		new google.maps.Point(0,0),
				        		new google.maps.Point(10, 34))
			});
			marker.rekall = markerTmp;
			thiss.markers.push(marker);
			thiss.bounds.extend(marker.position);
		
			//Ajoute un événement click sur le markqueur
			google.maps.event.addListener(marker, 'click', (function(marker, index) {
				return function() {
					Tags.addOne(marker.rekall.tag, false);
					Tag.displayMetadata();
				}
			})(marker, index));
			
			if((filter != undefined) && (filter == markerTmp.tag)) {
				thiss.infowindow.setContent(markerTmp.tag.getMetadata("Rekall->Name"));
				//thiss.infowindow.open(thiss.map, marker);
			}
		});
	}
	this.resize(undefined, bounds);
}






function HomeControl(controlDiv, map) {
	// Set CSS styles for the DIV containing the control
	// Setting padding to 5 px will offset the control
	// from the edge of the map
	controlDiv.style.padding = '5px';

	// Set CSS for the control border
	var controlUI = document.createElement('div');
	controlUI.style.backgroundColor = 'white';
	controlUI.style.borderStyle = 'solid';
	controlUI.style.borderWidth = '2px';
	controlUI.style.cursor = 'pointer';
	controlUI.style.textAlign = 'center';
	controlUI.title = 'Click to set the map to Home';
	controlDiv.appendChild(controlUI);

	// Set CSS for the control interior
	var controlText = document.createElement('div');
	controlText.style.fontFamily = 'Arial,sans-serif';
	controlText.style.fontSize = '12px';
	controlText.style.paddingLeft = '4px';
	controlText.style.paddingRight = '4px';
	controlText.innerHTML = '<b>Home</b>';
	controlUI.appendChild(controlText);

	// Setup the click event listeners: simply set the map to
	// Chicago
	google.maps.event.addDomListener(controlUI, 'click', function() {
		Tags.clear(true);
		$("#mapTabNav").trigger("click");
	});

}