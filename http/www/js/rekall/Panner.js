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

function Panner() {
	this.filtredTags = new Array();
	this.visible     = false; 
	this.recreateGallery = true;
	$("#panner .gallery").click(function(event) {
		event.stopPropagation();
		Tags.clear(true);
	});
	$("#panzoom-close").click(function(event) {
		event.stopPropagation();
		Tags.clear(true);
	});
	$("#panner .panzoom").panzoom({
		$zoomRange: 	$("#panner .zoom-range"),
        startTransform: 'scale(1)',
        increment: 		0.1,
        minScale: 		0.1,
        maxScale: 		10,
        contain: 		'invert'
	});
	$("#panner .panzoom").mousewheel(function(event) {
		event.preventDefault();
		$("#panner .panzoom").panzoom("pan", -event.deltaX, event.deltaY, { relative: true });
	});
	$("#panner #panzoom-slider").slider({
		min: 	0,
		max: 	4,
		step: 	0.01,
		slide: function(event, ui) {
			var pt = {clientX: $("#panner").width()*0.66, clientY: $("#panner").height()*0.66};
			$("#panner .panzoom").panzoom('zoom', ui.value, {focal: pt});
		}
	});
	$("#panner .panzoom").load(function(event) {
		$(this).css({"width": "auto", "height": "auto"});
		var maxWidth = $(this).parent().width();
		var maxHeight = $(this).parent().height();
		var ratio = 0;
		var width  = $(this).width();
		var height = $(this).height();

		if(false) {
			// Check if the current width is larger than the max
			if(width > maxWidth){
				ratio = maxWidth / width;
				$(this).css({"width": maxWidth, "height": height * ratio});
				height = height * ratio;
				width = width * ratio;
			}

			// Check if current height is larger than max
			if(height > maxHeight) {
				ratio = maxHeight / height;
				$(this).css({"height": maxHeight, "width": width * ratio});
				width = width * ratio;
				height = height * ratio;
			}
		}
		else if(false) {
			var ratio = Math.max(maxWidth / width, maxHeight / height);
			$(this).css({"width": width*ratio, "height": height*ratio});
		}
		$("#panner .panzoom").show();
		$("#panner .panzoom").panzoom('resetDimensions');
		$("#panner .panzoom").panzoom('resetPan');
		$("#panner .panzoom").panzoom('resetZoom');
		var zoom = Math.max(maxWidth / width, maxHeight / height);
		$("#panner #panzoom-slider").slider({value: zoom});
		$("#panner .panzoom").panzoom('zoom', zoom);
	});
}


Panner.prototype.isVisible = function() {
	return this.visible;
}
Panner.prototype.resize = function(visible, bounds) {
	if(visible == true)	 {
		$("#panner").show();
		$("#pannerTabNav").addClass("active");
	}
	else if(visible == false) {
		$("#panner").hide();
		$("#pannerTabNav").removeClass("active");
	}

	this.visible = visible;
	if(this.visible == undefined)	this.visible = $("#panner").is(":visible");
	else							this.firstDisplay = true;

	$("#panner .panzoom").trigger("load");
}
Panner.prototype.show = function(filter, bounds) {
	var recreateGallery = this.recreateGallery;
	this.recreateGallery = true;
	
	//TODO
	//return;
	
	var thiss = this;
	var gallery1 = $("#panner-gallery1");
	var gallery2 = $("#panner-gallery2");
	
	if(recreateGallery) {
		this.filtredTags = new Array();
		var parent1 = gallery1.parent();
		var parent2 = gallery2.parent();
		gallery1.detach();
		gallery2.detach();
		gallery1.html("");
		gallery2.html("");
		this.markers = new Array();
	}
	var scrollTo = undefined;
	for (var category in this.thumbnails) {
		var contents = this.thumbnails[category];
		if((recreateGallery) && (contents.thumbnails.length)) {
			var target = gallery1;
			var categoryVerbose = contents.category.categoryVerbose;
			if(contents.category.category == Sorting.prefix) {
				target          = gallery2;
				categoryVerbose = "Not specified";
			}
			target.append("<h1 style='color: " + contents.category.color + "'>" + categoryVerbose + "</h1>");
		}
		
		/*for (var index in contents.thumbnails) {
			var thumbnail = contents.thumbnails[index];*/
		$.each(contents.thumbnails, function(index, thumbnail) {
			if(recreateGallery) {
				thiss.filtredTags.push(thumbnail.tag);		
				target.append(function() {
					if(thumbnail.url != undefined)	thumbnail.dom = $("<div draggable=true class='thumbnail'><img src='" + thumbnail.url + "' style='border-color: " + thumbnail.tag.color + ";'/><br/>" + Utils.elide(thumbnail.tag.getMetadata("Rekall->Name"), 20) + "</div>'");
					else							thumbnail.dom = $("<div draggable=true class='nothumbnail'>" + Utils.elide(thumbnail.tag.getMetadata("Rekall->Name"), 20) + "</div>'");

					thumbnail.dom.mouseenter(function(event) {
						if(!Tags.isStrong) {
							thiss.recreateGallery = false;
							Tags.addOne(thumbnail.tag, false);
							Tag.displayMetadata();
						}
					});
					thumbnail.dom.click(function(event) {
						thiss.recreateGallery = false;
						event.stopPropagation();
						Tags.addOne(thumbnail.tag, true);
						Tag.displayMetadata();
					});
					thumbnail.dom.on({
						dragstart: function(event) {
				            event.dataTransfer.setData("key", 	  thumbnail.tag.document.key);
				            event.dataTransfer.setData("version", thumbnail.tag.version);
						}
					});
					return thumbnail.dom;
				});
			}
			if((thumbnail.dom != undefined) && (filter != undefined)) {
				var opacity = 1, add = true;
			 	if($.inArray(thumbnail.tag, filter) !== -1) {
					opacity = 1;
					scrollTo = thumbnail;
				}
				else if(Tags.isStrong) {
					opacity = 0.5;
					if(filter.length > 1)
						add = false;
				}
				else {
					opacity = 0.8;
					border = 'dotted';
				}
				if(add)	thumbnail.dom.show().css("opacity", opacity);
				else	thumbnail.dom.hide().css("opacity", opacity);
			}
		});
	}
	if(recreateGallery) {
		parent1.prepend(gallery1);
		parent2.prepend(gallery2);
	}
	
	
	if(filter == undefined)
		this.hidePhoto();
	else if((filter.length == 1) && (Tags.isStrong)) {
		if((filter[0].isImage()) || (filter[0].getMetadata("Rekall->Type").endsWith("pdf")))
			rekall.panner.showPhoto(filter[0]);
	}
	else
		this.hidePhoto();
		
	if((scrollTo) && (recreateGallery) && ($("#panner .gallery").is(":visible")))
		$("#panner").scrollTop(scrollTo.dom.position().top + $("#panner").scrollTop());
}
Panner.prototype.showPhoto = function(tag) {
	$("#panner .gallery").hide();
	$("#panner .panzoom-container").show();
	if($("#panner .panzoom").attr("src") != Utils.getLocalFilePath(tag)) {
		$("#panner .panzoom").hide();
		$("#panner .panzoom").attr("src", Utils.getLocalFilePath(tag));
	}
}
Panner.prototype.hidePhoto = function() {
	$("#panner .gallery").show();
	$("#panner .panzoom-container").hide();
}
