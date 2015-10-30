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

function Tags() {
}

//HideFlattenTimeline	Tags.flattenTimelineTags = new Array();
Tags.selectedTags = new Array();
Tags.hoveredTag = undefined;
Tags.isStrong = false;
Tags.clear = function(displayMeta) {
	Tags.selectedTags = [];
	Tags.isStrong = false;	

	//Update graphique générale
	rekall.analyse(false);
	if(displayMeta == true)
		Tag.displayMetadata();		
}

Tags.add = function(tag, strong) {
	Tags.isStrong |= strong;
	Tags.selectedTags.push(tag);

	//Update graphique générale
	rekall.analyse(false);
}
Tags.toggle = function(tag, strong) {
	Tags.isStrong |= strong;
	var index = $.inArray(tag, Tags.selectedTags);
	if(index === -1)
		Tags.selectedTags.push(tag);
	else
		Tags.selectedTags.splice(index, 1);

	//Update graphique générale
	rekall.analyse(false);
}

Tags.addOne = function(tag, strong) {
	Tags.clear();
	Tags.add(tag, strong);
}


Tags.count = function() {
	return Tags.selectedTags.length;
}

Tags.unique = function() {
	return Tags.selectedTags[0];
}
