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

function Polygon() {
	this.points = new Array();
}
Polygon.prototype.addPoint = function(point) {
	this.points.push(point)
}
Polygon.prototype.clear = function() {
	this.points = [];
}
Polygon.prototype.toPath = function() {
	var path = "";
	for (var index in this.points) {
		if(index == 0)  path += Utils.movePath(this.points[index]);
		else			path += Utils.linePath(this.points[index]);
	}
	return path;
}
Polygon.prototype.contains = function(pt) {
	var fillRule = 1
	if(this.points.length == 0)
		return false;

	var winding_number = 0;

	var last_pt = this.points[0];
	var last_start = this.points[0];
	for (var i = 1; i < this.points.length; ++i) {
		var e = this.points[i];
		winding_number = Polygon.isect_line(last_pt, e, pt, winding_number);
		last_pt = e;
	}

	//implicitly close last subpath
	if (last_pt != last_start)
		winding_number = Polygon.isect_line(last_pt, last_start, pt, winding_number);
	

	return (fillRule == 1
		? (winding_number != 0)
		: ((winding_number % 2) != 0));
	/*
	var first = true, direction = true, directionOk = false;
    for(var i = 0 ; i < this.points.length ; i++) {
        var pt0 = this.points[i], pt1 = this.points[(i+1)%this.points.length];
        var val = (pt.y - pt0.y) * (pt1.x - pt0.x) - (pt.x - pt0.x) * (pt1.y - pt0.y);
        if(val > 0) {
            if((!direction) && (directionOk) && (!first))
                return false;
            direction   = true;
            directionOk = true;
        }
        else if(val < 0) {
            if((direction) && (directionOk) && (!first))
                return false;
            direction = false;
            directionOk = true;
        }
        first = false;
	}
    return true;
*/
}

Polygon.isect_line = function(p1, p2, pos, winding) {
    var x1 = p1.x;
    var y1 = p1.y;
    var x2 = p2.x;
    var y2 = p2.y;
    var y = pos.y;
 
    var dir = 1;
 
    if (y1 == y2) {
        // ignore horizontal lines according to scan conversion rule
        return winding;
    } else if (y2 < y1) {
        var x_tmp = x2; x2 = x1; x1 = x_tmp;
        var y_tmp = y2; y2 = y1; y1 = y_tmp;
        dir = -1;
    }
 
    if (y >= y1 && y < y2) {
        var x = x1 + ((x2 - x1) / (y2 - y1)) * (y - y1);
 
        // count up the winding number if we're
        if (x <= pos.x) {
            winding += dir;
        }
    }
	return winding;
}
