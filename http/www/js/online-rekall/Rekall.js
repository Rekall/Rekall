//ADAPTED
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
	this.sortings["horizontal"] = new Sorting("Time");
	this.sortings["colors"]     = new Sorting("Rekall->Type", false, "");
	this.timeline = new Timeline();
	this.videoPlayer = undefined;
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
	$.ajax("file/project.xml", {
		type: "GET",
		data: {"rand": random()},
		dataType: "xml",
		success: function(xml) {
			if((xml == null) || (xml == undefined))
				showInRuban("Your project is unreachable. Did you move the folder to an other location?", 60);
			else {
				$(xml).find('project').each(function() {
					if($(this).find('document').length == 0)
						showInRuban("Start by addind files to your project.", 60);
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