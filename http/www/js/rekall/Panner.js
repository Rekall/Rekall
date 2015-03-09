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

function Panner() {
	this.filtredTags = new Array();
	this.visible     = false; 
	this.recreateGallery = true;
   /* $("#panner .gallery").click(function(event) {
		event.stopPropagation();
		Tags.clear(true);
	});*/                       
	
	$(".panzoom").click(function(event) {
		event.stopPropagation(); 
	});
	$(".panzoom-parent").click(function(event) {
		event.stopPropagation();
		Tags.clear(true);
	});
	$("#closePhotoBtn").click(function(event) {
		event.stopPropagation();
		Tags.clear(true);
	});
    $("#panner .panzoom").panzoom({
		$zoomRange: 	$("#panner .zoom-range"),
        startTransform: 'scale(1)',
        increment: 		0.1,
        minScale: 		0.5,
        maxScale: 		4
	});  
	$("#panner .panzoom").mousewheel(function(event) {
		event.preventDefault();
		$("#panner .panzoom").panzoom("pan", -event.deltaX, event.deltaY, { relative: true });
	});
	$("#panner #panzoom-slider").slider({
		min: 	0.5,
		max: 	4,
		step: 	0.1,
		slide: function(event, ui) { 
			var focusX = $("#panner .panzoom").width()*0.5;
			var focusY = $("#panner .panzoom").height()*0.5;                  
			var pt = {clientX: focusX, clientY: focusY}; 
  //  		alert(ui.value+" / "+focusX+" / "+focusY);
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
        var top = 0;
		if(true) {   
			                              
			
			// Check if current height is larger than max
			if(height > maxHeight) {
				ratio = maxHeight / height;   
				width = width * ratio;
				height = height * ratio;        
		  //  	top = (maxHeight-height)/2;
				//$(this).css({"height": height, "width": width}); 
	   // 		alert("2 - width = "+width+" / maxWidth = "+maxWidth+" / height = "+height+" / maxHeight = "+maxHeight);  
			}             
			// Check if the current width is larger than the max
			if(width > maxWidth){
				ratio = maxWidth / width;        
				height = height * ratio;
				width = width * ratio;                    
		  //  	top = (maxHeight-height)/2;
				//$(this).css({"width": width, "height": height});   
	 //   		alert("1 - width = "+width+" / maxWidth = "+maxWidth+" / height = "+height+" / maxHeight = "+maxHeight); 
			}
			   
			top = (maxHeight-height)/2;   
			$(this).css({"height": height, "width": width, "marginTop": top});                          
		}
		else if(false) {
			var ratio = Math.max(maxWidth / width, maxHeight / height);
			$(this).css({"width": width*ratio, "height": height*ratio});     
		}                                 
	    $("#panner .panzoom").show();
	    $("#panner .panzoom").panzoom('resetDimensions');    
	    $("#panner .panzoom").panzoom('resetPan');
	    $("#panner .panzoom").panzoom('resetZoom');        
		                        
   // 	alert("3 - width = "+width+" / maxWidth = "+maxWidth+" / height = "+height+" / maxHeight = "+maxHeight);
		var zoom = 1;//Math.floor(maxWidth / width);
		//alert(zoom);
		$("#panner #panzoom-slider").slider({value: zoom});
		//$("#panner .panzoom").panzoom('zoom', zoom);   
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
	/*var legende  = $("#thumbLgd"); */
	
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
	var countCat = 0;
	for (var category in this.thumbnails) {
		var contents = this.thumbnails[category];
		if((recreateGallery) && (contents.thumbnails.length)) {
			var target = gallery1;
			var categoryVerbose = contents.category.categoryVerbose;
			if(contents.category.category == Sorting.prefix) {
				target          = gallery2;
				categoryVerbose = "Not specified";
			}                
			//target.append("<h1 style='color: " + contents.category.color + "'>" + categoryVerbose + "</h1>");
			//target.append("<h1><div class='thumbnailPastille' style='background-color:"+contents.category.color+";'></div>" + categoryVerbose + "</h1>");        
			var categoryVerboseTable = categoryVerbose.split("/");                                                                                                        
			var categoryVerboseTitle = categoryVerboseTable[categoryVerboseTable.length-1];  
			if((!categoryVerboseTitle)&&(categoryVerboseTable.length>=2)) categoryVerboseTitle = categoryVerboseTable[categoryVerboseTable.length-2];  
			if(!categoryVerboseTitle) categoryVerboseTitle = categoryVerbose;                                                                                    
			     
			var rgb = contents.category.color.toString(); 
			rgb = rgb.split('(');  
			if(rgb.length>1) {
				rgb = rgb[1].split(')');
				if(rgb.length>1) { 
				    rgb = rgb[0];
				}
			}            
			var rgbtmp = rgb.split(',');
			if(rgbtmp.length>3) {
				//rgb = rgbtmp[0]+","+rgbtmp[1]+","+rgbtmp[2];        
				//background-image: -webkit-linear-gradient(top left, rgba("+rgb+",.75) 0%, rgba(255,255,255,.5) 100%);
				target.append("<div class='panner-gallery-group' style='background-color: rgba("+rgb+");' title='"+categoryVerbose+"'><h1 style='color: rgba("+rgb+");'>" + categoryVerboseTitle.toUpperCase() + "</h1></div>");
			}                        
			//else target.append("<div class='panner-gallery-group' style='background-color: rgba("+rgb+",.25); border:2px solid rgba("+rgb+",.75);' title='"+categoryVerbose+"'><h1 style='color: rgb("+rgb+");'>" + categoryVerboseTitle.toUpperCase() + "</h1></div>");  
			else target.append("<div class='panner-gallery-group' style='background-image: -webkit-linear-gradient(bottom right, rgba("+rgb+",.7) 0%, rgba("+rgb+",.4) 100%);   ' title='"+categoryVerbose+"'><h1 style='color: rgb("+rgb+");'>" + categoryVerboseTitle.toUpperCase() + "</h1></div>");  
/*			if(countCat==0) {
			    legende.html(""); 
				var tmpHeight = legende.parent().parent().css("height");   
				tmpHeight = tmpHeight.substr(0,tmpHeight.length-2)-70;
				legende.css("maxHeight",tmpHeight+"px");
				//alert(tmpHeight);   
			}
			legende.append("<div class='panner-gallery-lgd' style='background-color: rgb("+rgb+");' title='"+categoryVerbose+"'></div>");      */
		    countCat++;   

		}           
		//if(countCat>15) legende.html(""); 
		
		//for (var index in contents.thumbnails) {
		//	var thumbnail = contents.thumbnails[index];
		$.each(contents.thumbnails, function(index, thumbnail) {
			if(recreateGallery) {
				thiss.filtredTags.push(thumbnail.tag);		

				target.children().last().append(function() {
					if(thumbnail.url != undefined)	thumbnail.dom = $("<div draggable=true class='thumbnail'><div class='thumbnailImage' style='background-image:url("+thumbnail.url +");'></div><div class='thumbnailTxt' style='backgrouncolor:"+thumbnail.tag.color+";'>" + Utils.elide2lines(thumbnail.tag.getMetadata("Rekall->Name"), 13) + "</div></div>'");
					else							thumbnail.dom = $("<div draggable=true class='nothumbnail'><div class='thumbnailImage'></div><div class='thumbnailTxt' style='backgrouncolor:"+thumbnail.tag.color+";'>" + Utils.elide2lines(thumbnail.tag.getMetadata("Rekall->Name"), 13) + "</div></div>'");      


				   /* thumbnail.dom.mouseenter(function(event) {
						if(!Tags.isStrong) {
							thiss.recreateGallery = false;
							Tags.addOne(thumbnail.tag, false);
							Tag.displayMetadata();
						}
					});  */
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
					opacity = 0.5;
					border = 'dotted';
				}
			
				
				if(add)	thumbnail.dom.css("opacity", opacity).css("display", "inline-block");
				else	thumbnail.dom.hide().css("opacity", opacity);
			}
		});
	}
	if(recreateGallery) {          
		parent2.prepend(gallery2);
		parent1.prepend(gallery1);  
//		parent1.prepend(legende);
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
