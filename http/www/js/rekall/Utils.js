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

function Utils() {
}


Utils.sortObj = function(arr, valAreFloats) {
	// Setup Arrays
	var sortedKeys = new Array();
	var sortedObj = {};

	// Separate keys and sort them
	for (var i in arr)
		sortedKeys.push(i);

	sortedKeys.sort(function(a, b) {
		if(valAreFloats) {
			a = parseFloat(a.substr(1));
			b = parseFloat(b.substr(1));
		}
		else {
			a = a.toLowerCase();
			b = b.toLowerCase();
		}
		if (a < b)	return -1;
		if (a > b)	return 1;
  		return 0;
	});

	//Reconstruct sorted obj based on keys
	for (var i in sortedKeys)
		sortedObj[sortedKeys[i]] = arr[sortedKeys[i]];
	return sortedObj;
}

Utils.cubicPathLine = function(pt1, pt2, factor, offset) {
	if(factor == undefined)
		factor = 0.5;
	if(offset == undefined)
		offset = 0;
	return " C" + pt1.x + "," + (pt1.y + (pt2.y-pt1.y)*factor + offset) + " " + pt2.x + "," + (pt1.y + (pt2.y-pt1.y)*(1-factor) + offset) + " " + pt2.x + "," + pt2.y;
}
Utils.toPath = function(pt) {
	return pt.x + "," + pt.y;
}
Utils.linePath = function(pt) {
	return " L" + Utils.toPath(pt);
}
Utils.movePath = function(pt) {
	return " M" + Utils.toPath(pt);
}
Utils.closePath = function(pt) {
	return " Z";
}



Utils.elide = function(text, length) {
	if(text.length < length)
		return text;
	else
		return text.substr(0, length) + "…";
}

Utils.getPreviewPath = function(tagOrDoc) {
	if(tagOrDoc.getMetadata("File->Thumbnail") != undefined)
		return rekall.project.url + "file/rekall_cache/" + tagOrDoc.getMetadata("File->Thumbnail");
	else
		return undefined;
}
Utils.getLocalFilePath = function(tagOrDoc, action, prefix) {
	var fileName      = tagOrDoc.getMetadata("File->File Name");
	var fileDirectory = tagOrDoc.getMetadata("Rekall->Folder");
	if((fileName != undefined) && (fileDirectory != undefined)) {
		var path = "/" + fileDirectory + fileName;
		if(action == undefined)
			action = "file";
		if(prefix != true)
			path = rekall.project.url + action + path;
		return path;
	}
	else
		return "";
}

Utils.getColor = function(percent) {
	if(percent != percent)
		percent = 0;
	var colors = [[229,149,205],  [123,144,206],  [74,201,159],  [166,204,91],  /*[255,234,136],*/  [255,147,102],  [181,134,118],  [255,84,79]];
	var indexFloat = percent * (colors.length-1);
	var indexLow   = floor(indexFloat), indexSup = min(indexLow+1, colors.length-1); indexFloat = indexFloat - indexLow;
	return tinycolor("rgb(" + round(colors[indexLow][0]*(1-indexFloat) + colors[indexSup][0]*(indexFloat)) + ", " + round(colors[indexLow][1]*(1-indexFloat) + colors[indexSup][1]*(indexFloat)) + ", " + round(colors[indexLow][2]*(1-indexFloat) + colors[indexSup][2]*(indexFloat)) + ")");
}

Utils.splitKeywords = function(val, strong, really) {
	if(val == undefined)
		return [];
	if(really == false)
		return [val];
	if(strong == undefined)
		strong = true;

	var vals = val.split(/,\s*/);
	if(!strong)
		return vals;

	var retour = new Array();
	for (var index in vals) {
		var val = vals[index].trim();
		if((val != "") && ($.inArray(val, retour) === -1))
			retour.push(val);
	}
	return retour;
}
Utils.joinKeywords = function(vals) {
	var retour = "";
	for (var index in vals) {
		var val = vals[index].trim();
		if(val != "")
			retour += val + ", ";
	}
	retour = retour.substr(0, retour.length - 2);
	return retour;
}
Utils.inArray = function(array, check) {
	var retour = false;
	for (var variable in array) {
		if(variable == check) {
			retour = true;
			return retour;
		}
	}
	return retour;
}


String.prototype.toHHMMSS = function (moduloHours, forceHours) {
    var sec_num = parseInt(this, 10); // don't forget the second param
	var minus = "";
	if(sec_num < 0) {
		sec_num = -sec_num;
		minus = "-";
	}
    var hours   = Math.floor(sec_num / 3600);
    var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
    var seconds = sec_num - (hours * 3600) - (minutes * 60);

    var time    = minus;
	if((forceHours == true) || (hours > 0)) {
		if(moduloHours)	hours = hours % 24;
	    if (hours < 10)	time += "0"+hours + ":";
		else			time += hours + ":";
	}
    if (minutes < 10)	time += "0"+minutes + ":";
	else				time += minutes + ":";
    if (seconds < 10)	time += "0"+seconds;
	else				time += seconds;
    return time;
}
String.prototype.toHHMMSSmmm = function (forceHours) {
    var sec_num  = parseInt(this); // don't forget the second param
	var minus = "";
	if(sec_num < 0) {
		sec_num = -sec_num;
		minus = "-";
	}
    var hours    = Math.floor(sec_num / 3600);
    var minutes  = Math.floor((sec_num - (hours * 3600)) / 60);
    var seconds  = sec_num - (hours * 3600) - (minutes * 60);
    var mseconds = round((parseFloat(this) - floor(parseFloat(this))) * 1000);

    var time    = minus;
	if((forceHours == true) || (hours > 0)) {
		if(moduloHours)	hours = hours % 24;
	    if (hours < 10)	time += "0"+hours + ":";
		else			time += hours + ":";
	}
    if (minutes < 10)	time += "0"+minutes + ":";
	else				time += minutes + ":";
    if (seconds < 10)	time += "0"+seconds + ":";
	else				time += seconds + ":";
    if 	    (mseconds < 10)  time += "00"+mseconds;
    else if (mseconds < 100) time += "0"+mseconds;
	else					 time += mseconds;
	
    return time;
}
String.prototype.tommm = function () {
    var sec_num  = parseInt(this); // don't forget the second param
    var hours    = Math.floor(sec_num / 3600);
    var minutes  = Math.floor((sec_num - (hours * 3600)) / 60);
    var seconds  = sec_num - (hours * 3600) - (minutes * 60);
    var mseconds = round((parseFloat(this) - floor(parseFloat(this))) * 1000);

    var time    = "";
    if 	    (mseconds < 10)  time += "00"+mseconds;
    else if (mseconds < 100) time += "0"+mseconds;
	else					 time += mseconds;
	
    return time;
}