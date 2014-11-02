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

function Sorting(metadataKey, valCanBeFloats, metadataSearch) {
	this.setCriterias(metadataKey, valCanBeFloats, metadataSearch, false);
	this.categories = new Object();
	this.tabVisible = true;
}

Sorting.prototype.setCriterias = function(metadataConfigStr, valCanBeFloats, metadataSearch, analyse) {
	this.metadataConfigStr  = metadataConfigStr;
	this.metadataKey        = undefined;
	this.metadataKeyPortion = {left: -1, length:-1, date: '', dateFormat: ''};
	this.metadataFormat     = "";
	if(metadataSearch != undefined)
		this.metadataSearch	= metadataSearch;
	if(metadataConfigStr != undefined) {
		var metadataConfigs = this.metadataConfigStr.split("|");
		this.metadataKey = metadataConfigs[0].trim();
		if(metadataConfigs.length > 1) {
			var metadataKeyPortionFormula = metadataConfigs[1].trim().toLowerCase();
			this.metadataFormat = metadataKeyPortionFormula;
			/*
			if     (this.metadataFormat == "year")			metadataKeyPortionFormula = "0,4";
			else if(this.metadataFormat == "month")			metadataKeyPortionFormula = "0,6";
			else if(this.metadataFormat == "day")			metadataKeyPortionFormula = "0,8";
			else if(this.metadataFormat == "hour")			metadataKeyPortionFormula = "0,11";
			else if(this.metadataFormat == "minute")		metadataKeyPortionFormula = "0,13";
			else if(this.metadataFormat == "second")		metadataKeyPortionFormula = "0,15";
			else if(this.metadataFormat == "year_only")		metadataKeyPortionFormula = "0,4";
			else if(this.metadataFormat == "month_only")	metadataKeyPortionFormula = "4,2";
			else if(this.metadataFormat == "day_only")		metadataKeyPortionFormula = "6,2";
			else if(this.metadataFormat == "hour_only")		metadataKeyPortionFormula = "9,2";
			else if(this.metadataFormat == "minute_only")	metadataKeyPortionFormula = "11,2";
			else if(this.metadataFormat == "second_only")	metadataKeyPortionFormula = "13,2";
			*/	
			if     (this.metadataFormat == "year")			metadataKeyPortionFormula = "-1,-1,YYYY,YYYY";
			else if(this.metadataFormat == "month")			metadataKeyPortionFormula = "-1,-1,YYYY:MM,MMMM YYYY";
			else if(this.metadataFormat == "day")			metadataKeyPortionFormula = "-1,-1,YYYY:MM:DD,ddd LL";
			else if(this.metadataFormat == "hour")			metadataKeyPortionFormula = "-1,-1,YYYY:MM:DD HH,ddd LL HH[h]";
			else if(this.metadataFormat == "minute")		metadataKeyPortionFormula = "-1,-1,YYYY:MM:DD HH:mm,ddd LL HH:mm";
			else if(this.metadataFormat == "second")		metadataKeyPortionFormula = "-1,-1,YYYY:MM:DD HH:mm:ss,ddd LL HH:mm:ss";
			else if(this.metadataFormat == "year_only")		metadataKeyPortionFormula = "-1,-1,YYYY,YYYY";
			else if(this.metadataFormat == "month_only")	metadataKeyPortionFormula = "-1,-1,MM,MMMM";
			else if(this.metadataFormat == "day_only")		metadataKeyPortionFormula = "-1,-1,DD,D";
			else if(this.metadataFormat == "hour_only")		metadataKeyPortionFormula = "-1,-1,HH,HH[ h]";
			else if(this.metadataFormat == "minute_only")	metadataKeyPortionFormula = "-1,-1,mm,m";
			else if(this.metadataFormat == "second_only")	metadataKeyPortionFormula = "-1,-1,ss,s";
			else if(this.metadataFormat == "quarter")		metadataKeyPortionFormula = "-1,-1,Q,Q";
			else if(this.metadataFormat == "dayofyear")		metadataKeyPortionFormula = "-1,-1,DDD,DDD";
			else if(this.metadataFormat == "dayofweek")		metadataKeyPortionFormula = "-1,-1,e,dddd";
			else if(this.metadataFormat == "weekofyear")	metadataKeyPortionFormula = "-1,-1,W,Wo";
			
			
			var portions = metadataKeyPortionFormula.split(",");
			this.metadataKeyPortion    .left   = parseInt(portions[0]);
			if(portions.length > 1)		this.metadataKeyPortion.length     = parseInt(portions[1]);
			if(portions.length > 2)		this.metadataKeyPortion.date       = portions[2];
			if(portions.length > 3)		this.metadataKeyPortion.dateFormat = portions[3];
			if(isNaN(this.metadataKeyPortion.left))		this.metadataKeyPortion.left   = -1;
			if(isNaN(this.metadataKeyPortion.length))	this.metadataKeyPortion.length = -1;
		}
	}
	if(valCanBeFloats == undefined)
		valCanBeFloats = false;
	this.valCanBeFloats = valCanBeFloats;
	if(analyse != false)
		rekall.analyse();
}


Sorting.prefix = "_";
Sorting.prototype.parseMeta = function(metadata) {
	if(metadata != undefined) {
		var prefix = Sorting.prefix;
		if(this.metadataKeyPortion.date != '') {
			metadata = moment(metadata, "YYYY:MM:DD HH:mm:ss", true).format(this.metadataKeyPortion.date);
			if(metadata.toLowerCase() == "invalid date") {
				metadata = "Undated";
				prefix = "0";
			}
		}
		metadata = (""+metadata).replace(/:/g, "");
		if(this.metadataKeyPortion.left > -1) {
			if(this.metadataKeyPortion.length > -1)		return  prefix + metadata.substr(this.metadataKeyPortion.left, this.metadataKeyPortion.length);
			else										return  prefix + metadata.substr(this.metadataKeyPortion.left);
		}
		return prefix + metadata;
	}
	return metadata;
}


Sorting.prototype.analyseStart = function(tags) {
	this.valMin       = 999999;
	this.valMax       = 0;
	this.valAreFloats = true;
	this.valAreDates  = true;
	if(this.metadataKey == "Time")
		this.valMin = 0;
	else {
		this.categoriesIndex = 0;
		$.each(this.categories, function(key, category) {
			category.shoudBeRemoved = true;
			category.tags = new Array();
		});
	}
}
Sorting.prototype.analyseAdd = function(tag, metadata, alwaysIncludeTags, useThisInsteadOfValue) {
	if(alwaysIncludeTags == undefined)
		alwaysIncludeTags = false;

	if(this.metadataKey == "Time") {
		this.valMin = min(this.valMin, tag.getTimeStart());
		this.valMax = max(40, max(this.valMax, tag.getTimeEnd()));
		return true;
	}
	else {
		if(metadata == undefined)
	 		metadata = tag.getMetadata(this.metadataKey);
		var metadataRaw = metadata;
//		if(metadataRaw == undefined) 
//			metadataRaw = "Unknown";
		if(metadataRaw == undefined)
			return false;
				
		metadata = this.parseMeta(metadataRaw);
		
		/*
		if((metadata == Sorting.prefix) && (this.metadataKey != undefined) && (this.metadataKey != ""))
			return false;
		*/
 		if((this.metadataSearch != undefined) && (this.metadataSearch != "") && (metadata.toLowerCase().indexOf(this.metadataSearch.toLowerCase()) < 0))
			return false;
		
		if(useThisInsteadOfValue != undefined)
			metadata = useThisInsteadOfValue;
			
		if(this.categories[metadata] == undefined) {
			this.categories[metadata]                = new Object();
			this.categories[metadata].tags           = new Array();
			this.categories[metadata].checked        = true;
			this.categories[metadata].visible        = true;
			this.categories[metadata].shoudBeRemoved = true;
		}
		if(this.categories[metadata].shoudBeRemoved)
			this.categories[metadata].index = this.categoriesIndex++;
		this.categories[metadata].shoudBeRemoved = false;
		this.categories[metadata].category        = metadata;
		this.categories[metadata].categoryRaw     = metadataRaw;
		this.categories[metadata].categoryVerbose = metadata.substr(1);
		var isValable = ((this.categories[metadata].checked) && (this.categories[metadata].visible));
		
		if((isValable) || (alwaysIncludeTags))
			this.categories[metadata].tags.push(tag);
		
		if(this.valAreFloats) {
			var valFloat = parseFloat(metadata.substr(1));
			if(!isNaN(valFloat)) {
				this.valMin = min(this.valMin, valFloat);
				this.valMax = max(this.valMax, valFloat);
			}
			else
				this.valAreFloats = false;
		}
		if(this.valAreDates) {
			var valDate = moment(metadataRaw, "YYYY:MM:DD HH:mm:ss", true);
			this.valAreDates = valDate.isValid();
		}
		return isValable;
	}
}
Sorting.prototype.analyseEnd = function(minTagCount) {
	var thiss = this;
	this.categoriesIndex = 0;
	$.each(this.categories, function(key, category) {
		if((minTagCount != undefined) && (category.tags.length <= minTagCount))
			category.shoudBeRemoved = true;
		if(category.shoudBeRemoved)
			delete thiss.categories[key];
		else
			thiss.categoriesIndex++;
    });


	//Echelle Linéaire
	if((this.valCanBeFloats) && (this.valAreFloats)) {
		//Echelle linéaire
		if(this.metadataKey == "Time")
			this.valMax = ceil(this.valMax/10)*10;
		var index = 0;
		var minMaxDiff = this.valMax - this.valMin;
		var decimals = 1;
		if     (minMaxDiff < 1)			decimals = 1;
		else if(minMaxDiff < 0.1)		decimals = 2;
		else if(minMaxDiff < 0.01)		decimals = 3;
		else if(minMaxDiff < 0.001)		decimals = 4;
		else if(minMaxDiff < 0.0001)	decimals = 5;
		
		var categoriesTmp = new Object();
		var valStep = minMaxDiff / Sorting.timeSteps;
		if(this.metadataKey == "Time")
			valStep = Sorting.timeSteps;
		for(var val = this.valMin ; val <= this.valMax ; val += valStep) {
			var metadata = index;
			categoriesTmp[metadata]                = new Object();
			categoriesTmp[metadata].tags           = new Array();
			categoriesTmp[metadata].checked        = true;
			categoriesTmp[metadata].visible        = true;
			categoriesTmp[metadata].shoudBeRemoved = false;
			categoriesTmp[metadata].index 		   = index++;
			categoriesTmp[metadata].size 		   = 0;
			categoriesTmp[metadata].category 	   = val;
			if(this.metadataKey == "Time")
				categoriesTmp[metadata].categoryVerbose = (val+"").toHHMMSS();
			else
				categoriesTmp[metadata].categoryVerbose = round(val, decimals);
			
			if((this.metadataKey != undefined) && (this.metadataKey != "Time")) {
				$.each(this.categories, function(key, category) {
					$.each(category.tags, function(index, tag) {
						var valCompare = parseFloat(thiss.parseMeta(tag.getMetadata(thiss.metadataKey)).substr(1));
						if((val <= valCompare) && (valCompare < (val+valStep)))
							categoriesTmp[metadata].tags.push(tag);
					});
			    });
			}
		}
		this.categories = categoriesTmp;
	}
	else {
		this.categories = Utils.sortObj(this.categories, this.valAreFloats);
		if(this.analyse) {
			console.log("----------------------------------------------------------------");
			$.each(this.categories, function(key, category) {
				console.log(key);
			});
		}
		
		
		var categoryNewIndex = 0;
		var categoryTagsCount = 0;
		$.each(this.categories, function(key, category) {
			categoryTagsCount += category.tags.length;
	    });
	    $.each(this.categories, function(key, category) {
			category.index = categoryNewIndex++;
			category.size  = thiss.categoriesIndex;
			if(key == Sorting.prefix)
				category.color = Sorting.defaultColor;
			else
				category.color = Utils.getColor(category.index / (category.size-1));
			category.tagsSize = categoryTagsCount;
			if((!category.checked) || (!category.visible))
				category.color = category.color.setAlpha(0.2);
			if(thiss.valAreDates)
				category.categoryVerbose = moment(category.categoryRaw, "YYYY:MM:DD HH:mm:ss", true).format(thiss.metadataKeyPortion.dateFormat);
		});
	}
}

Sorting.defaultColor = tinycolor("rgb(114, 124, 126)");
Sorting.timeSteps = 10;
Sorting.catFactor = 10;
Sorting.size      = 11;

Sorting.prototype.positionFor = function(tag, index) {
	var position = {x: -15, xMax: -15, width: 0};
	
	if((index != undefined) && (this.metadataKey == "Time"))
		position = {x: Sorting.positionForTime(index*Sorting.timeSteps), xMax: Sorting.positionForTime((index+1)*Sorting.timeSteps), width: 0};
	else if(index != undefined)
		position = {x: index * Sorting.catFactor, xMax: (index+1) * Sorting.catFactor, width: 0};
	else if(tag != undefined) {
		var metadata = this.parseMeta(tag.getMetadata(this.metadataKey));
		if(this.metadataKey == "Time")
			position = {x: Sorting.positionForTime(tag.getTimeStart()), xMax: Sorting.positionForTime(tag.getTimeStart()), width: Sorting.positionForTime(tag.getTimeEnd()-tag.getTimeStart())};
		else if((this.valCanBeFloats) && (this.valAreFloats) && (metadata != undefined)) {
			var val = parseFloat(metadata.substr(1));
			position = {x: Sorting.positionForTime(val), xMax: Sorting.positionForTime(val), width: 0};
		}
		else if((this.categories[metadata] != undefined))
			position = {x: this.categories[metadata].index*Sorting.catFactor, xMax: (this.categories[metadata].index+1)*Sorting.catFactor, width: 0};
	}
	return Sorting.mapPosition(position);
}

Sorting.positionForTime = function(time, sorting) {
	if(sorting == undefined)
		sorting = rekall.sortings["horizontal"];
	return map(time, sorting.valMin, sorting.valMax, 0, Sorting.catFactor*Sorting.timeSteps);
}
Sorting.timeForPosition = function(x, sorting) {
	if(sorting == undefined)
		sorting = rekall.sortings["horizontal"];
	return map(x, 0, Sorting.catFactor*Sorting.timeSteps, sorting.valMin, sorting.valMax);
}
Sorting.mapPosition = function(position) {
	return {x: position.x * Sorting.size, xMax: position.xMax * Sorting.size, width: max(position.width * Sorting.size, Tag.tagHeight)};
}
Sorting.unmapPosition = function(x) {
	return x / Sorting.size;
}


Sorting.prototype.getCategory = function(tag) {
	var metadata = this.parseMeta(tag.getMetadata(this.metadataKey));
	return this.categories[metadata];
}