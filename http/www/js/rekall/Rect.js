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

function Rect() {
	this.x      = 0;
	this.y      = 0;
	this.width  = 0;
	this.height = 0;
}
Rect.prototype.adjusted = function(val) {
	var newRect = new Rect();
	newRect.x      = this.x      - val;
	newRect.y      = this.y      - val;
	newRect.width  = this.width  + 2*val;
	newRect.height = this.height + 2*val;
	return newRect;
}
Rect.prototype.getPosition = function() {
	return {x: this.x, y: this.y};	
}
Rect.prototype.getTopLeft = function() {
	return {x: this.x, y: this.y};	
}
Rect.prototype.getTopRight = function() {
	return {x: this.x+this.width, y: this.y};	
}
Rect.prototype.getBottomRight = function() {
	return {x: this.x+this.width, y: this.y+this.height};	
}
Rect.prototype.getBottomLeft = function() {
	return {x: this.x, y: this.y+this.height};	
}
Rect.prototype.getCenter = function() {
	return {x: this.x+this.width/2, y: this.y+this.height/2};	
}
Rect.prototype.getSize = function() {
	return {width: this.width, height: this.height};	
}
Rect.prototype.contains = function(pt) {
	return (this.x <= pt.x) && (pt.x <= (this.x+this.width)) && (this.y <= pt.y) && (pt.y <= (this.y+this.height));
}
Rect.prototype.intersects = function(rect) {
	var l1 = this.x;
    var r1 = this.x;
    if (this.width < 0)
        l1 += this.width;
    else
        r1 += this.width;
    if (l1 == r1) // null rect
        return false;

    var l2 = rect.x;
    var r2 = rect.x;
    if (rect.width < 0)
        l2 += rect.width;
    else
        r2 += rect.width;
    if (l2 == r2) // null rect
        return false;

    if (l1 >= r2 || l2 >= r1)
        return false;

    var t1 = this.y;
    var b1 = this.y;
    if (this.height < 0)
        t1 += this.height;
    else
        b1 += this.height;
    if (t1 == b1) // null rect
        return false;

    var t2 = rect.y;
    var b2 = rect.y;
    if (rect.height < 0)
        t2 += rect.height;
    else
        b2 += rect.height;
    if (t2 == b2) // null rect
        return false;

    if (t1 >= b2 || t2 >= b1)
        return false;

    return true;
}
